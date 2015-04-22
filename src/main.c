
#include "MK64f12.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"

#include "uart.h"
#include "tasks.h"
#include "enet.h"
#include "phy.h"

void abc(void *param){
	
	while(1){
		vTaskDelay( 2000 );
		PTB->PTOR |= GPIO_PTOR_PTTO(1<<22);
	}
	
}

void test2(void *param){
	
	while(1){
		vTaskDelay( 1000 );
		PTB->PTOR |= GPIO_PTOR_PTTO(1<<21);
		UART0->D = 0x55;
	}
	
}

void vRegisterCLICommands(void);

QueueHandle_t UartQueue;
TaskHandle_t	CliTaskHandler, StreamReceivedTaskHandler;
SemaphoreHandle_t DataStreamSemaphore;

int main(void)
{
	
	/*Simple diode output setting */ //B21 -> blue led
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; //enable PORTB clock
	PORTB->PCR[21] |= PORT_PCR_MUX(1); //set as GPIO
	PTB->PDDR |= GPIO_PDDR_PDD(1<<21); // set as output
	PORTB->PCR[22] |= PORT_PCR_MUX(1); //set as GPIO
	PTB->PDDR |= GPIO_PDDR_PDD(1<<22); // set as output
	/************************************************************/
	
	/* Starting SysTick with interrupt flag to make RTOS scheduler works */
	//NVIC_EnableIRQ(SysTick_IRQn);
	//NVIC_ClearPendingIRQ(SysTick_IRQn);
	//SysTick_Config(SystemCoreClock / 1000/*configTICK_RATE_HZ*/);
	//SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
	//__enable_irq();
	//__enable_irq();
	

	uart_settings uart;
	uart.Instance = 0;
	uart.BaudRate = 9600;
	uart.DataBits8_9 = 8;
	uart.ParityType = PARITY_NONE;
	uart.StopBits = 1;
	
	UART_Init(&uart);
	enet_init();
						
					/*	struct packet_fields packet;
									
uint64_t payload = 0xF81234142;
uint64_t dest_add = 0x88AE1DDAF29D;
uint64_t sour_add = 0xAEAEAEAEAEAB;
uint8_t length = 0x8; //Size of payload in bytes

packet.dest_add = &dest_add;
packet.source_add = &sour_add;
packet.length = length;
packet.payload_ptr = &payload;
//					SendRaw(ping,74); 
	
		struct ping_frame exmpl;
		exmpl.dest_mac_add = 0x001122334455;
		exmpl.dest_ip_add = 0x08080404;
		//SendPing(&exmpl);
		
		*/
	
	UartQueue = xQueueCreate( 1, sizeof( RxStruct ) );
	DataStreamSemaphore = xSemaphoreCreateBinary();
	
	xTaskCreate(abc,"abc", configMINIMAL_STACK_SIZE, (void*)NULL, tskIDLE_PRIORITY+1, NULL );
	
	xTaskCreate(test2,"test2", configMINIMAL_STACK_SIZE, (void*)NULL, tskIDLE_PRIORITY+1, NULL );
	
	xTaskCreate(vCommandConsoleTask,"CLI", configMINIMAL_STACK_SIZE, (void*)NULL, tskIDLE_PRIORITY+2, &CliTaskHandler );
	xTaskCreate(vStreamTask,"StreamData", configMINIMAL_STACK_SIZE, (void*)NULL, tskIDLE_PRIORITY+1, &StreamReceivedTaskHandler );

	vTaskSuspend(  CliTaskHandler );
	vTaskSuspend(  StreamReceivedTaskHandler );
	
	vRegisterCLICommands();
	
	/* Start the scheduler. */
	vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	to be created.  See the memory management section on the FreeRTOS web site
	for more details. */
	for( ;; );
}

