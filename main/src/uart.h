#ifndef UART_H
#define UART_H

#include <stdint.h>

#define UART_BAUD_RATE 115200UL
#define UBRR_VAL ((F_CPU + 4UL * UART_BAUD_RATE) / (8UL * UART_BAUD_RATE) - 1)

void UART_init();
void UART_transmit_char(unsigned char data);
void UART_transmit(const char* message);
unsigned char UART_receive();

bool uart_try_receive(char* command);
// Remember the latest command values and send reports incrementally in uart_poll().
void uart_report_controls(uint8_t lift_percent, int8_t servo_angle);
void uart_poll();

#endif
