#include "phy.h"
#include "enet.h"

int main(void){
	
struct packet_fields packet;
	
uint64_t payload = 0xF81234142;
uint64_t dest_add = 0x88AE1DDAF29D;
uint64_t sour_add = 0xAEAEAEAEAEAB;
uint16_t length = 0x8; //Size of payload in bytes

packet.dest_add = &dest_add;
packet.source_add = &sour_add;
packet.length = &length;
packet.payload_ptr = &payload;

enet_init();
	
SendPacket (&packet);

}
