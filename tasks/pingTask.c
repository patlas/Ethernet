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


void vPingTask( void *pvParameters )
{

	RxStruct ReceivedStream;
	struct ping_frame frame;

    for( ;; )
    {
        /* This implementation reads a single character at a time.  Wait in the
        Blocked state until a character is received. */
       // FreeRTOS_read( xConsole, &cRxedChar, sizeof( cRxedChar ) );

        if( xQueueReceive( UartQueue, &ReceivedStream,0 ) == pdTRUE )
        {
						/* wysylac stream przez ethernet */
					frame.dest_ip_add =   (ReceivedStream.tx_buff[0] << 24);
					frame.dest_ip_add |=  (ReceivedStream.tx_buff[1] << 16);
					frame.dest_ip_add |=  (ReceivedStream.tx_buff[2] << 8);
					frame.dest_ip_add |=  (ReceivedStream.tx_buff[3]);
					
					frame.dest_mac_add = 	(ReceivedStream.tx_buff[5] << 40);
					frame.dest_mac_add |= 	(ReceivedStream.tx_buff[6] << 32);
					frame.dest_mac_add |= 	(ReceivedStream.tx_buff[7] << 24);
					frame.dest_mac_add |= 	(ReceivedStream.tx_buff[8] << 16);
					frame.dest_mac_add |= 	(ReceivedStream.tx_buff[9] << 8);
					frame.dest_mac_add |= 	(ReceivedStream.tx_buff[10]);
					
					taskENTER_CRITICAL();
					//tutaj wysylanie pinga
					SendPing(&frame);
					taskEXIT_CRITICAL();
					
					
           //SendRawData(ReceivedStream.tx_buff, ReceivedStream.size);
					//SendRawData(pings,74);
						vTaskSuspend(  NULL );
        }
      
    }
}        
		