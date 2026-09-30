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

//Return the length of how long a pulse lasted
uint32_t readPulse(uint8_t, uint8_t);

#endif