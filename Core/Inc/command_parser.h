/*
 * common.parser.h
 *
 *  Created on: 13 thg 11, 2026
 *      Author: cuong
 */

/*
 * command_parser.h
 * Receives bytes from USART2 (interrupt) into a ring buffer and extracts
 * commands of the form !RST# and !OK# with a small FSM.
 */
#ifndef INC_COMMAND_PARSER_H_
#define INC_COMMAND_PARSER_H_

#include "main.h"

#define MAX_BUFFER_SIZE 30
#define MAX_CMD_SIZE    10

/* values of command_data */
#define CMD_NONE 0
#define CMD_RST  1
#define CMD_OK   2

extern UART_HandleTypeDef huart2;

extern uint8_t temp;                    /* 1-byte RX holder for HAL_UART_Receive_IT */
extern volatile uint8_t buffer_head;    /* written by ISR  */
extern volatile uint8_t buffer_tail;    /* written by main */
extern uint8_t command_flag;            /* 1 = a new command is ready */
extern uint8_t command_data;            /* CMD_RST or CMD_OK */

void command_parser_fsm(void);

#endif /* INC_COMMAND_PARSER_H_ */
