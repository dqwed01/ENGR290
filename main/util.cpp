#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "src/util.h"

uint32_t readPulse(uint8_t pin, uint8_t value, uint16_t timeout){
    uint32_t timeElapsed = 0;
    //TODO
    return timeElapsed;
}