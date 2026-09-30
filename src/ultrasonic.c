#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

#include "include/pins.h"
#include "include/util.h"
#include "include/ultrasonic.h"

void us_sensor_init(){
    cli();
    DDRB |= ((OUTPUT<<US1_TRIG)|(OUTPUT<<US2_TRIG)); //Set TRIG pin to Output
    DIGITAL_WRITE_LOW(PORTB, US1_TRIG);
    DIGITAL_WRITE_LOW(PORTB, US2_TRIG);
    DDRB &= ~((~INPUT)<<US1_ECHO); 
    DDRD &= ~((~INPUT)<<US2_ECHO);
    sei();
}