/*
 * print.c
 *
 *  Created on: Dec 19, 2022
 *      Author: Eldar
 */

/*
 * dbox_log.c
 *
 *  Created on: 2021-06-21
 *      Author: Eldar
 */

#include "print.h"
#include "cmsis_os.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern osSemaphoreId usart2TXAvailableHandle;

osMailQDef (object_pool_q_log, 5, print_queue_unit_t);  // Declare mail queue
osMailQId  (object_pool_q_log_id);

static void print_log(uint8_t * msg, uint16_t length);

// This implements thread-safe log printing
void printTask()
{
	osEvent event_log;
	object_pool_q_log_id = osMailCreate(osMailQ(object_pool_q_log), NULL);

	print("Autonomous Car V0.1 build date: %s\n", __DATE__);

	while(1)
	{
		event_log = osMailGet(object_pool_q_log_id, osWaitForever);
		if (event_log.status == osEventMail)
		{
			print_queue_unit_t *received = (print_queue_unit_t *)event_log.value.p;       // ".p" indicates that the message is a pointer
			print_log((uint8_t*)received->message, received->length);
			osMailFree(object_pool_q_log_id, received);
		}
		osDelay(1);
	}
}

int print(char *fmt, ...)
{
#if DEBUG_ENABLED
	va_list args;
	va_start(args, fmt);
	print_queue_unit_t *object_data = NULL;
	object_data = (print_queue_unit_t *) osMailAlloc(object_pool_q_log_id, 100);
	if (object_data != NULL)
	{
		int len = vsnprintf((char*)object_data->message, 60, fmt, args);
		object_data->length = len;
		va_end(args);
		osMailPut(object_pool_q_log_id, object_data);
		return len;
	}
	else return 0;
#else
	return 0;
#endif
}

static void print_log(uint8_t * msg, uint16_t length)
{
	osSemaphoreWait(usart2TXAvailableHandle, osWaitForever);
	HAL_UART_Transmit_DMA(&huart2, msg, length);
	osSemaphoreWait(usart2TXAvailableHandle, osWaitForever);
	osSemaphoreRelease(usart2TXAvailableHandle);
}

/**
  * @brief Tx Transfer completed callback.
  * @param huart UART handle.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
   if(huart->Instance==USART2)
   {
	   osSemaphoreRelease(usart2TXAvailableHandle);
   }
}

