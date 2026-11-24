#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include "uart.h"

//Constant Definitions
#define INPUT 0
#define OUTPUT 1
#define HIGH 1
#define LOW 0

//Register Operations
#define DIGITAL_WRITE_HIGH(x,y) (x) |= (1U<<(y))
#define DIGITAL_WRITE_LOW(x,y) (x) &= ~(1U<<(y))
#define DIGITAL_TOGGLE(x,y) (x) ^= (1U<<(y))
#define DIGITAL_READ(x,y) (((x) >> (y)) & 1U)


//Return the length of how long a pulse lasted
uint16_t read_pulse(volatile uint8_t*, uint8_t, uint8_t, uint16_t);
uint16_t adc_read(uint8_t);
#endif
