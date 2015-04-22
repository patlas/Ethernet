#include "MK64f12.h"
#include <stdlib.h>

#define configMAC_ADDR0 0xAB
#define 	MK64_ETH_TX_BUFFER_COUNT   3
#define 	MK64_ETH_TX_BUFFER_SIZE   1536
#define 	MK64_ETH_RX_BUFFER_COUNT   6
#define 	MK64_ETH_RX_BUFFER_SIZE   1536

#define 	ENET_TBD0_R   0x8000
#define 	ENET_TBD0_L   0x0800
#define 	ENET_TBD0_TC   0x0400
#define 	ENET_TBD4_INT   0x4000
#define 	ENET_TBD0_W   0x2000
#define 	ENET_RBD0_W   0x2000
#define 	ENET_RBD4_INT   0x0080
#define 	ENET_RBD0_E   0x8000

#define HTOBE16(value) (((value) & 0x00FF) << 8 ) | (((value) & 0xFF00) >> 8 ) //????
#define HTOBE8(value) (((value) & 0x0F) << 4) | (((value) & 0xF0) >>4)

struct packet_fields{

	uint64_t *dest_add;
	uint64_t *source_add;
	uint8_t *length; // length of payload in bytes
	uint64_t *payload_ptr;
	
};


void MAC_DescInit(void);
void GPIO_Init(void);
void SIM_Init(void);
void MAC_SetAddress(void);
void enet_init(void);
uint8_t SendPacket(struct packet_fields *frame);
uint8_t SendRaw(uint8_t *data, uint8_t bytecount);
