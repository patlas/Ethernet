
#include "MK64f12.h"



void test(){
	
	while(1){
//		vTaskDelay( 1000 );
		PTB->PTOR |= GPIO_PTOR_PTTO(21);
	}
	
}


int main(void)
{
	
	/*Simple diode output setting */ //B21 -> blue led
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; //enable PORTB clock
	PORTB->PCR[21] |= PORT_PCR_MUX(1); //set as GPIO
	PTB->PDDR |= GPIO_PDDR_PDD(21); // set as output
	PTB->PSOR |= 1<<21;
	test();
	while(1){};
}