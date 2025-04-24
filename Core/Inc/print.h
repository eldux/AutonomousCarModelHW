/*
 * print.h
 *
 *  Created on: Dec 19, 2022
 *      Author: Eldar
 */

#ifndef INC_PRINT_H_
#define INC_PRINT_H_

#include <stdint.h>
#include "stm32f3xx_hal.h"

#define DEBUG_ENABLED (1)

typedef struct {
	uint16_t length;
	uint8_t message[60];
} print_queue_unit_t;

int print(char *fmt, ...);
void printTask();

#endif /* INC_PRINT_H_ */
