#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "queue.h"
#include "task.h"

#include "tasks.h"
#include "uart.h"

#define MAX_INPUT_LENGTH    50
#define MAX_OUTPUT_LENGTH   100


extern QueueHandle_t UartQueue;

void vStreamTask( void *pvParameters )
{

BaseType_t xMoreDataToFollow;
RxStruct ReceivedStream;
	char pcWriteBuffer[MAX_OUTPUT_LENGTH];

    for( ;; )
    {
        /* This implementation reads a single character at a time.  Wait in the
        Blocked state until a character is received. */
       // FreeRTOS_read( xConsole, &cRxedChar, sizeof( cRxedChar ) );

        if( xQueueReceive( UartQueue, &ReceivedStream,0 ) == pdTRUE )
        {
						/* wysylac stream przez ethernet */
           
						vTaskSuspend(  NULL );
        }
      
    }
}        
		