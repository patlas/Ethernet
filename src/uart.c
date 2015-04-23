
#include "uart.h"

RxStruct RxDATA;
RxStruct RxSTREAM;

uint8_t RxBuff[100];
uint8_t RxStreamBuff[100];


volatile bool is_stream = false;

extern QueueHandle_t UartQueue;
extern TaskHandle_t	CliTaskHandler;
extern TaskHandle_t	StreamReceivedTaskHandler;

bool UART_Init( uart_settings *settings){
	
	UART_Type *UARTx;
	
	uint16_t SBR;
	uint8_t BRFA;
	uint32_t UART_module_clock;
	float sbr_brfd;
	
	//bd = UART_module_clock / (16 * SBR + BRFD)   // BRFD = BRFDval * 1/32
	
	//jezelu uart0/1 to system core clock inaczej to bus clock
	if(settings->Instance == INSTANCE_UART0 || settings->Instance == INSTANCE_UART1)
		UART_module_clock = SystemCoreClock;
	else //Bus clock frequency
		UART_module_clock = SystemCoreClock / (1+ ((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK)>>SIM_CLKDIV1_OUTDIV2_SHIFT));
	
	/* BaudRate calculation */
	sbr_brfd =  ((float)UART_module_clock)/(16*settings->BaudRate);
	
	SBR = (uint16_t) sbr_brfd;
	BRFA = (uint8_t)((sbr_brfd - SBR) * 32);
	
	/* Add more UARTs if necessary */
	switch(settings->Instance){
		
		case INSTANCE_UART0:
			UARTx = UART0;	
			SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
		break;
		case INSTANCE_UART1:
			UARTx = UART1;
			SIM->SCGC4 |= SIM_SCGC4_UART1_MASK;
		break;
		default:
			UARTx = UART0;
			SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
		break;	
	};
	
	
	
	if(settings->StopBits == 1)
		UARTx->BDH &= ~UART_BDH_SBNS_MASK;
	else
		UARTx->BDH |= UART_BDH_SBNS_MASK;
		
	UARTx->BDH |= UART_BDH_SBR( ((SBR>>8)& 0x1F));
	UARTx->BDL |= (uint8_t) SBR;
	UARTx->C4	 |= UART_C4_BRFA(BRFA);
	
	if(settings->DataBits8_9 == 8)
		UARTx->C1 &= ~UART_C1_M_MASK;
	else
		UARTx->C1 |= UART_C1_M_MASK;
	
	if(settings->ParityType != PARITY_NONE)
	{
		UARTx->C1 |= UART_C1_PE_MASK;
		
		if(settings->ParityType == PARITY_EVEN)
			UARTx->C1 &= ~UART_C1_PT_MASK;
		else
			UARTx->C1 |= UART_C1_PT_MASK;
	}
	else
		UARTx->C1 &= ~UART_C1_PE_MASK;
	
	//PTB16/17 alt 3
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	PORTB->PCR[16] |= PORT_PCR_MUX(3);
	PORTB->PCR[17] |= PORT_PCR_MUX(3);	
	
	UARTx->C2 |= UART_C2_TE_MASK;
	UARTx->C2 |= UART_C2_RE_MASK;
	
	UARTx->C2 |= UART_C2_RIE_MASK;
	NVIC_EnableIRQ (UART0_RX_TX_IRQn);
	NVIC_SetPriority(UART0_RX_TX_IRQn,2);
	
	//init queue element
	RxDATA.size = 0;
	RxDATA.tx_buff = RxBuff;
	
	RxSTREAM.size = 0;
	RxSTREAM.tx_buff = RxStreamBuff;
	
	
	
	return true;
}




void UART0_RX_TX_IRQHandler(void){
	
	uint8_t tmp = UART0->D;
	if((UART0_S1 & UART_S1_RDRF_MASK)){
		if(is_stream == false){
				if(tmp != '\n'){
					RxDATA.tx_buff[RxDATA.size] = tmp;
					//buf[a++] = tmp; //usunac
					RxDATA.size++;
					//UART0->D = tmp;
				}
				else{
					xQueueSendFromISR ( UartQueue, &RxDATA,NULL );
					RxDATA.size=0;
					xTaskResumeFromISR( CliTaskHandler );
				}
		}
		else{
			RxSTREAM.tx_buff[RxSTREAM.size] = tmp;
			RxSTREAM.size++;
			if( (RxSTREAM.stream_size--)<2 ){ // sprawdzic czy <1 czy moze 2??
				is_stream=false;
				xQueueSendFromISR ( UartQueue, &RxSTREAM,NULL );
				RxSTREAM.size=0;
				RxSTREAM.stream_size=0;
				xTaskResumeFromISR( StreamReceivedTaskHandler );
			}
		}
			
	}
	
	NVIC_ClearPendingIRQ(UART0_RX_TX_IRQn);
	
}
