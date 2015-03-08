
#include "MK64f12.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

void test(void *param){
	
	while(1){
		vTaskDelay( 1000 );
		PTB->PTOR |= GPIO_PTOR_PTTO(21);
	}
	
}


int main(void)
{
	
	/*Simple diode output setting */ //B21 -> blue led
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK; //enable PORTB clock
	PORTB->PCR[21] |= PORT_PCR_MUX(1); //set as GPIO
	PTB->PDDR |= GPIO_PDDR_PDD(21); // set as output
	/************************************************************/
	
	xTaskCreate(test,"test", configMINIMAL_STACK_SIZE, (void*)NULL, tskIDLE_PRIORITY, NULL );
	
	/* Start the scheduler. */
	vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	to be created.  See the memory management section on the FreeRTOS web site
	for more details. */
	for( ;; );
}
