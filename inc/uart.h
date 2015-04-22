#ifndef UART_H
#define UART_H

#include "MK64f12.h"
#include <stdbool.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#define INSTANCE_UART0 0
#define INSTANCE_UART1 1
#define INSTANCE_UART2 2

#define ONE_STOP_BIT  1
#define TWO_STOP_BITS 2

#define PARITY_NONE 	0
#define PARITY_ODD		1
#define PARITY_EVEN		0

typedef struct{
	
	uint8_t Instance;
	uint16_t BaudRate;
	uint8_t StopBits;
	uint8_t DataBits8_9;
	uint8_t ParityType;
	
	
} uart_settings;


#define MAX_RxStruct_BUFF_SIZE 50
typedef struct {
	uint8_t *tx_buff;
	uint8_t size;
	uint8_t stream_size;
} RxStruct;


bool UART_Init( uart_settings *settings);


#endif /* UART_H */

