/* FreeRTOS includes. */
#include "FreeRTOS.h"

/* FreeRTOS+CLI includes. */
#include "FreeRTOS_CLI.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "uart.h"


/*
 * Defines a command that can take a variable number of parameters.  Each
 * parameter is echoes back one at a time.
 */
static BaseType_t prvTransmitPing( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );
static BaseType_t prvTransmitOwnPacket( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString );


/* Structure that defines the "echo_parameters" command line command.  This
takes a variable number of parameters that the command simply echos back one at
a time. */
static const CLI_Command_Definition_t xPingICMP =
{
	"send_ping",
	"\r\nSend ICMP ping frame\r\n\r\n",
	prvTransmitPing, /* The function to run. */
	2 /* The user can enter any number of commands. */
};


static const CLI_Command_Definition_t xOwnPacket =
{
	"send_own_packet",
	"\r\nSend own Ethernet packet\r\n\r\n",
	prvTransmitOwnPacket, /* The function to run. */
	1 /* The user can enter any number of commands. */
};


void vRegisterCLICommands( void )
{
	/* Register all the command line commands defined immediately above. */
	FreeRTOS_CLIRegisterCommand( &xPingICMP );
	FreeRTOS_CLIRegisterCommand( &xOwnPacket );
}
/*-----------------------------------------------------------*/




static BaseType_t prvTransmitPing( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
extern QueueHandle_t UartQueue;
extern TaskHandle_t	PingTransmitTaskHandler;
	
	
const char *pcParameter1,*pcParameter2;
BaseType_t lParameterStringLength, xReturn;
static BaseType_t lParameterNumber = 0;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

		/* Obtain the parameter string. */
		
	pcParameter1 = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							1,		/* Return the next parameter. */
							&lParameterStringLength	/* Store the parameter string length. */
						);
	
	pcParameter2 = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							2,		/* Return the next parameter. */
							&lParameterStringLength	/* Store the parameter string length. */
						);

		if( pcParameter1 != NULL )
		{
			/* Pierwszy parametr to IP, pierwsze 4 bajty to adres IP */
				memset( pcWriteBuffer, 0x00, xWriteBufferLen );
				pcWriteBuffer[0] =  atoi(strtok( (char*)pcParameter1,"."));
				pcWriteBuffer[1] = 	atoi(strtok(NULL,"."));
				pcWriteBuffer[2] = 	atoi(strtok(NULL,"."));
				pcWriteBuffer[3] = 	atoi(strtok(NULL,"."));
		}
		

		if( pcParameter2 != NULL )
		{
				/* pozyskanie adresu MAC, separator adresu '-' */
				pcWriteBuffer[5] =  atoi(strtok( (char*)pcParameter2,"-"));
				pcWriteBuffer[6] =  atoi(strtok( NULL,"-"));
				pcWriteBuffer[7] =  atoi(strtok( NULL,"-"));
				pcWriteBuffer[8] =  atoi(strtok( NULL,"-"));
				pcWriteBuffer[9] =  atoi(strtok( NULL,"-"));
				pcWriteBuffer[10] =  atoi(strtok( NULL,"-"));
				
		}	
		RxStruct tmp;
		tmp.size=11;
		tmp.tx_buff = (uint8_t*)pcWriteBuffer;
		
		xQueueSend( UartQueue, &tmp,NULL );
		vTaskResume( PingTransmitTaskHandler );
		xReturn = pdFALSE;

	return xReturn;
}
/*-----------------------------------------------------------*/



static BaseType_t prvTransmitOwnPacket( char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString )
{
extern bool is_stream;
extern RxStruct RxSTREAM;
const char *pcParameter1;
BaseType_t lParameterStringLength, xReturn;
static BaseType_t lParameterNumber = 0;

	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );

		/* Obtain the parameter string. */
		
	pcParameter1 = FreeRTOS_CLIGetParameter
						(
							pcCommandString,		/* The command string itself. */
							1,		/* Return the next parameter. */
							&lParameterStringLength	/* Store the parameter string length. */
						);
	
	

		if( pcParameter1 != NULL )
		{
			
				memset( pcWriteBuffer, 0x00, xWriteBufferLen );
				RxSTREAM.stream_size =  atoi((char*)pcParameter1);
				//xSemaphoreTake(DataStreamSemaphore,0 );
				is_stream = true;
				
		}
			
		xReturn = pdFALSE;

	return xReturn;
}
/*-----------------------------------------------------------*/
