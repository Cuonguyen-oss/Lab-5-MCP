/*
 * common_parser.c
 *
 *  Created on: 13 thg 11, 2026
 *      Author: cuong
 */
/*
 * command_parser.c
 */
#include "command_parser.h"
#include <string.h>

uint8_t temp = 0;
static uint8_t buffer[MAX_BUFFER_SIZE];
volatile uint8_t buffer_head = 0;
volatile uint8_t buffer_tail = 0;
uint8_t command_flag = 0;
uint8_t command_data = CMD_NONE;

/* Parser states */
typedef enum {
    PARSER_WAIT_START,   /* waiting for '!' */
    PARSER_READ_CMD      /* collecting characters until '#' */
} ParserState_t;

static ParserState_t parser_state = PARSER_WAIT_START;
static uint8_t cmd_buf[MAX_CMD_SIZE];
static uint8_t cmd_idx = 0;

/* ISR: store the received byte in the ring buffer, then re-arm reception */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) {
    	HAL_UART_Transmit(&huart2, &temp, 1, 50);
        uint8_t next = (buffer_head + 1) % MAX_BUFFER_SIZE;
        if (next != buffer_tail) {          /* drop byte if buffer is full */
            buffer[buffer_head] = temp;
            buffer_head = next;
        }
        HAL_UART_Receive_IT(&huart2, &temp, 1);
    }
}

/* Called from the main loop: consume every byte currently in the buffer */
void command_parser_fsm(void)
{
    while (buffer_tail != buffer_head) {
        uint8_t c = buffer[buffer_tail];
        buffer_tail = (buffer_tail + 1) % MAX_BUFFER_SIZE;

        switch (parser_state) {
        case PARSER_WAIT_START:
            if (c == '!') {
                cmd_idx = 0;
                parser_state = PARSER_READ_CMD;
            }
            break;

        case PARSER_READ_CMD:
            if (c == '#') {
                cmd_buf[cmd_idx] = '\0';
                if (strcmp((char *)cmd_buf, "RST") == 0) {
                    command_data = CMD_RST;
                    command_flag = 1;
                } else if (strcmp((char *)cmd_buf, "OK") == 0) {
                    command_data = CMD_OK;
                    command_flag = 1;
                }
                parser_state = PARSER_WAIT_START;
            } else if (c == '!') {
                cmd_idx = 0;                /* restart on a new '!' */
            } else if (cmd_idx < MAX_CMD_SIZE - 1) {
                cmd_buf[cmd_idx++] = c;
            } else {
                parser_state = PARSER_WAIT_START;   /* too long -> invalid */
            }
            break;

        default:
            parser_state = PARSER_WAIT_START;
            break;
        }
    }
}

