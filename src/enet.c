#include "MK64f12.h"
#include "enet.h"
#include "phy.h"
#include <string.h>


//**
//Frame presented above is ICMP echo reply
//**
uint8_t ping [74] = {0x00,0x26,0x82,0xed,0xd5,0xd5,0x00,0x14,0x4f,0x9a,0xe1,0x0c,0x08,0x00,0x45,0x00,
0x00,0x3c,0x55,0xe0,0x00,0x00,0x34,0x01,0xaa,0xac,192,168,10,10,192,168,10,2,0x00,0x00,0x53,0x76,0x01,0x00,0x00,0xe6,0x61,0x62,0x63,0x64,0x65,0x66,
0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,
0x77,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69};


 //TX buffer
static uint8_t txBuffer[MK64_ETH_TX_BUFFER_COUNT][MK64_ETH_TX_BUFFER_SIZE]
__attribute__((aligned(4)));
//RX buffer
static uint8_t rxBuffer[MK64_ETH_RX_BUFFER_COUNT][MK64_ETH_RX_BUFFER_SIZE]
__attribute__((aligned(4)));
//TX buffer descriptors
static uint16_t txBufferDesc[MK64_ETH_TX_BUFFER_COUNT][16]
__attribute__((aligned(16)));
//RX buffer descriptors
static uint16_t rxBufferDesc[MK64_ETH_RX_BUFFER_COUNT][16]
__attribute__((aligned(16)));

//TX buffer index
static uint8_t txBufferIndex = 0;
//RX buffer index
static uint8_t rxBufferIndex;

void enet_init()
{
	
MPU->CESR &= ~MPU_CESR_VLD_MASK;			// FSL: allow concurrent access to MPU controller.  
																			//Example: ENET uDMA to SRAM, otherwise bus error*/

OSC->CR |= OSC_CR_ERCLKEN_MASK;					
	
SIM_Init();
GPIO_Init();
	
ENET->ECR = ENET_ECR_RESET_MASK;			//Reset ENET module
while(ENET->ECR & ENET_ECR_RESET_MASK);//Wait for the reset to complete
	
//Reveive control register
ENET->RCR = ENET_RCR_MAX_FL(1518) | ENET_RCR_RMII_MODE_MASK | ENET_RCR_MII_MODE_MASK;
ENET->TCR = 0;		//Transmit control register

enet_start_mii();
MAC_SetAddress();
	
/* Clear the Individual and Group Address Hash registers */
ENET_IALR = 0;
ENET_IAUR = 0;
ENET_GALR = 0;
ENET_GAUR = 0;

 //Disable transmit accelerator functions
ENET->TACC = 0;
//Disable receive accelerator functions
ENET->RACC = 0;

//Use enhanced buffer descriptors
ENET->ECR = ENET_ECR_EN1588_MASK;
//Clear MIC counters
ENET->MIBC = ENET_MIBC_MIB_CLEAR_MASK;

MAC_DescInit();

//Clear any pending interrupts
ENET->EIR = 0xFFFFFFFF;
//Enable desired interrupts
ENET->EIMR = ENET_EIMR_TXF_MASK | ENET_EIMR_RXF_MASK | ENET_EIMR_EBERR_MASK;

ENET_ECR |= ENET_ECR_ETHEREN_MASK;
/* Indicate that there have been empty receive buffers produced */
ENET_RDAR = ENET_RDAR_RDAR_MASK;

NVIC_EnableIRQ(ENET_Transmit_IRQn);
NVIC_EnableIRQ(ENET_Receive_IRQn);
NVIC_EnableIRQ(ENET_Error_IRQn);
}

//*************************************************************

void MAC_SetAddress(void/*uint8_t *ucMACAddress*/){
	
uint32_t value;
	
const uint8_t ucMACAddress[6] =
{
configMAC_ADDR0,
configMAC_ADDR0,
configMAC_ADDR0,
configMAC_ADDR0,
configMAC_ADDR0,
configMAC_ADDR0
};

value = ucMACAddress[5];
value |= (ucMACAddress[4] << 8);
ENET->PAUR = ENET_PAUR_PADDR2(value) | ENET_PAUR_TYPE(0x8808); //Set 16 upper bits

value = ucMACAddress[3];
value |= (ucMACAddress[2] << 8);
value |= (ucMACAddress[1] << 16);
value |= (ucMACAddress[0] << 24);
ENET->PALR = ENET_PALR_PADDR1(value);
}

void MAC_DescInit(){
uint32_t i;
uint32_t address;
//Clear TX and RX buffer descriptors
memset(txBufferDesc, 0, sizeof(txBufferDesc));
memset(rxBufferDesc, 0, sizeof(rxBufferDesc));

//Initialize TX buffer descriptors
for(i = 0; i < MK64_ETH_TX_BUFFER_COUNT; i++)
{
	
	address = (uint32_t) txBuffer[i];		//Calculate the address of the current TX buffer
	//Transmit buffer address
  txBufferDesc[i][2] = HTOBE16(address >> 16);
  txBufferDesc[i][3] = HTOBE16(address & 0xFFFF);
  //Generate interrupts
  txBufferDesc[i][4] = HTOBE16(ENET_TBD4_INT);
}

//Mark the last descriptor entry with the wrap flag
txBufferDesc[i - 1][0] |= HTOBE16(ENET_TBD0_W);
//Initialize TX buffer index
txBufferIndex = 0;

//Initialize RX buffer descriptors
for(i = 0; i < MK64_ETH_RX_BUFFER_COUNT; i++){
  //Calculate the address of the current RX buffer
  address = (uint32_t) rxBuffer[i];
  //The descriptor is initially owned by the DMA
  rxBufferDesc[i][0] = HTOBE16(ENET_RBD0_E);
 //Receive buffer address
 rxBufferDesc[i][2] = HTOBE16(address >> 16);
 rxBufferDesc[i][3] = HTOBE16(address & 0xFFFF);
 //Generate interrupts
 rxBufferDesc[i][4] = HTOBE16(ENET_RBD4_INT);
}

//Mark the last descriptor entry with the wrap flag
rxBufferDesc[i - 1][0] |= HTOBE16(ENET_RBD0_W);
//Initialize RX buffer index
rxBufferIndex = 0;

//Start location of the TX descriptor list
ENET->TDSR = (uint32_t) txBufferDesc;
//Start location of the RX descriptor list
ENET->RDSR = (uint32_t) rxBufferDesc;
//Maximum receive buffer size
ENET->MRBR = MK64_ETH_RX_BUFFER_SIZE;
}

void GPIO_Init(void){
	
PORTA_PCR14 = PORT_PCR_MUX(4);//RMII0_CRS_DV/MII0_RXDV
PORTA_PCR5 = PORT_PCR_MUX(4);//RMII0_RXER/MII0_RXER
PORTA_PCR12 = PORT_PCR_MUX(4);//RMII0_RXD1/MII0_RXD1
PORTA_PCR13 = PORT_PCR_MUX(4);//RMII0_RXD0/MII0_RXD0
PORTA_PCR15 = PORT_PCR_MUX(4);//RMII0_TXEN/MII0_TXEN
PORTA_PCR16 = PORT_PCR_MUX(4);//RMII0_TXD0/MII0_TXD0
PORTA_PCR17 = PORT_PCR_MUX(4);//RMII0_TXD1/MII0_TXD1
	
}

void SIM_Init(void){
	
	
SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTA_MASK;		//Clock for GPIO
SIM_SCGC2 |= SIM_SCGC2_ENET_MASK; 		//Clock for ENET
	
}


uint8_t SendPacket(struct packet_fields *frame){

char lol[*(frame->length)];
uint16_t tmp_length = HTOBE16(*(frame->length));
uint16_t *tmp_ptr = &tmp_length;
	
memcpy(lol,frame->dest_add,6);
memcpy(lol+6,frame->source_add,6);
memcpy(lol+12,tmp_ptr,2);
memcpy(lol+14,frame->payload_ptr,*(frame->length));

//Copy user data to the transmit buffer
memcpy(txBuffer[txBufferIndex],lol, 14 + *(frame->length));

//Set frame length
txBufferDesc[txBufferIndex][1] = HTOBE16(14 + *(frame->length));
//Clear BDU flag
txBufferDesc[txBufferIndex][8] = 0;

txBufferDesc[txBufferIndex][0] = HTOBE16(ENET_TBD0_R | ENET_TBD0_L | ENET_TBD0_TC);
txBufferIndex++;

ENET->TDAR = ENET_TDAR_TDAR_MASK;

return 1;
}
