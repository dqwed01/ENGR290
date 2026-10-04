#ifndef UTIL_H
#define UTIL_H

//Constant Definitions
#define INPUT 0
#define OUTPUT 1
#define HIGH 1
#define LOW 0

//Register Opeartions
#define DIGITAL_WRITE_HIGH(x,y) (x) |= (1U<<(y))
#define DIGITAL_WRITE_LOW(x,y) (x) &= ~(1U<<(y))
#define DIGITAL_READ(x,y) (x) & (1U<<(y))
#define UART_BAUD_RATE 9600
#define UBRR_VAL ((F_CPU / (16UL * UART_BAUD_RATE)) - 1)


//Return the length of how long a pulse lasted
uint32_t read_pulse(volatile uint8_t*, uint8_t, uint8_t, uint16_t);
void UART_init(void);
void UART_transmit_char(unsigned char);
unsigned char UART_receive(void);
#endif