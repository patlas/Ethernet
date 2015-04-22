#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "queue.h"
#include "task.h"

#include "tasks.h"
#include "uart.h"
#include "enet.h"
#include "phy.h"



extern QueueHandle_t UartQueue;

/*uint8_t pings [74] = {0x00,0x26,0x82,0xed,0xd5,0xd5,0x00,0x14,0x4f,0x9a,0xe1,0x0c,0x08,0x00,0x45,0x00,
0x00,0x3c,0x55,0xe0,0x00,0x00,0x34,0x01,0xaa,0xac,192,168,10,10,192,168,10,2,0x00,0x00,0x53,0x76,0x01,0x00,0x00,0xe6,0x61,0x62,0x63,0x64,0x65,0x66,
0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,
0x77,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69};
*/
void vStreamTask( void *pvParameters )
{

	RxStruct ReceivedStream;

    for( ;; )
    {
        /* This implementation reads a single character at a time.  Wait in the
        Blocked state until a character is received. */
       // FreeRTOS_read( xConsole, &cRxedChar, sizeof( cRxedChar ) );

        if( xQueueReceive( UartQueue, &ReceivedStream,0 ) == pdTRUE )
        {
						/* wysylac stream przez ethernet */
					taskENTER_CRITICAL();
					SendRaw(ReceivedStream.tx_buff, ReceivedStream.size);
					taskEXIT_CRITICAL();
					
					
           //SendRawData(ReceivedStream.tx_buff, ReceivedStream.size);
					//SendRawData(pings,74);
						vTaskSuspend(  NULL );
        }
      
    }
}        
		