/*
 * uart_communication.c
 *
 *  Created on: 13 thg 11, 2026
 *      Author: cuong
 */

/*
 * uart_communication.c
 */
#include "uart_communication.h"
#include "command_parser.h"
#include <stdio.h>

typedef enum {
    UART_IDLE,      /* waiting for !RST# */
    UART_WAIT_OK    /* packet sent, waiting for !OK# (3 s timeout) */
} UartState_t;

static UartState_t uart_state = UART_IDLE;
static uint32_t adc_value = 0;
static char     packet[20];
static uint16_t packet_len = 0;
static uint32_t t_sent = 0;

static uint32_t read_adc(void)
{
    uint32_t v = adc_value;
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
        v = HAL_ADC_GetValue(&hadc1);
    }
    HAL_ADC_Stop(&hadc1);
    return v;
}

static void send_packet(void)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)packet, packet_len, 1000);
    t_sent = HAL_GetTick();
}

void uart_communication_fsm(void)
{
    switch (uart_state) {

    case UART_IDLE:
        if (command_flag) {
            command_flag = 0;
            if (command_data == CMD_RST) {
                adc_value  = read_adc();
                packet_len = sprintf(packet, "!ADC=%lu#\r\n", (unsigned long)adc_value);
                HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n", 2, 100);
                send_packet();
                uart_state = UART_WAIT_OK;
            }
            /* !OK# while idle is ignored */
        }
        break;

    case UART_WAIT_OK:
        if (command_flag) {
            command_flag = 0;
            if (command_data == CMD_OK) {
                uart_state = UART_IDLE;         /* communication finished */
                break;
            }
            /* any other command is ignored while waiting */
        }
        if (HAL_GetTick() - t_sent >= UART_TIMEOUT_MS) {
            send_packet();                      /* resend previous packet (same value) */
        }
        break;

    default:
        uart_state = UART_IDLE;
        break;
    }
}
