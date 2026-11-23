/*
 * uart_communication.h
 *
 *  Created on: 13 thg 11, 2026
 *      Author: cuong
 */

#ifndef INC_UART_COMMUNICATION_H_
#define INC_UART_COMMUNICATION_H_

#include "main.h"

#define UART_TIMEOUT_MS 3000

extern UART_HandleTypeDef huart2;
extern ADC_HandleTypeDef hadc1;

void uart_communication_fsm(void);

#endif /* INC_UART_COMMUNICATION_H_ */
