#define F_CPU 16000000UL

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "src/pins.h"
#include "src/util.h"
#include "src/ultrasonic.h"
#include "src/timer.h"
#include "src/global.h"

#ifdef DEBUGGING
static char debug_message[64] = {0};
#endif

void us_sensor_init(){
    DDRB |= ((OUTPUT<<US1_TRIG) | (OUTPUT<<PB5)); //Set TRIG pin to Output
    DIGITAL_WRITE_LOW(PORTB, US1_TRIG);
    DDRB &= ~((1)<<US1_ECHO); 
}

float us_read(){
    float distance_cm = 0;
    //Setup Registers
    uint8_t us_echo = US1_ECHO;
    uint8_t us_trig = US1_TRIG;
    volatile uint8_t* us_echo_port = &PINB;
    volatile uint8_t* us_trig_port = &PORTB;
    uint16_t pulse_width_ticks;

    //Start Ranging
    DIGITAL_WRITE_HIGH(*us_trig_port, us_trig);
    _delay_us(10);
    DIGITAL_WRITE_LOW(*us_trig_port, us_trig);

    //Get Echo pulse
    pulse_width_ticks = read_pulse(us_echo_port, us_echo, HIGH, TICKS_60MS_256_PRESCALE);

    if(pulse_width_ticks != 0){
        uint32_t time_micro = pulse_width_ticks * US_PER_TICK_256_PRESCALE;
        distance_cm = (time_micro <=116) ? 2 : ((float) time_micro) / 58.0; //Minimum distance is 2cm
    }
#ifdef DEBUGGING
    char num[7];
    dtostrf(distance_cm, 1, 2, num);
    sprintf(debug_message, "us_read: %s cm", num);
#endif
    return distance_cm;
}