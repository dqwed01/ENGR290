#define F_CPU 16000000UL

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "src/pins.h"
#include "src/util.h"
#include "src/ultrasonic.h"
#include "src/timer.h"

void us_sensor_init(){
    cli();
    DDRB |= ((OUTPUT<<US1_TRIG)|(OUTPUT<<US2_TRIG) | (OUTPUT<<PB5)); //Set TRIG pin to Output
    DIGITAL_WRITE_LOW(PORTB, US1_TRIG);
    DIGITAL_WRITE_LOW(PORTB, US2_TRIG);
    DDRB &= ~((1)<<US1_ECHO); 
    DDRD &= ~((1)<<US2_ECHO);
    sei();
}

float us_read(uint8_t target_us){
    float distance_cm = 0;
    if(target_us == 1 || target_us == 2){
        //Setup Registers
        uint8_t us_echo = US1_ECHO;
        uint8_t us_trig = US1_TRIG;
        volatile uint8_t* us_echo_port = &PINB;
        volatile uint8_t* us_trig_port = &PORTB;
        if(target_us == 2){
            us_echo = US2_ECHO;
            us_trig = US2_TRIG;
            us_echo_port = &PIND;
        }
        uint32_t pulse_width_ticks;

        //Start Ranging
        DIGITAL_WRITE_HIGH(*us_trig_port, us_trig);
        _delay_us(10);
        DIGITAL_WRITE_LOW(*us_trig_port, us_trig);

        //Get Echo pulse
        pulse_width_ticks = read_pulse(us_echo_port, us_echo, HIGH, TICKS_60MS_256_PRESCALE);

        if(pulse_width_ticks != 0){
            uint32_t time_micro = pulse_width_ticks * US_PER_TICK_256_PRESCALE;
            distance_cm = (time_micro <=116) ? 2 : time_micro / (uint16_t)58; //Minimum distance is 2cm
        }
    }

    else{
        //Throw Error through UART
    }

    return distance_cm;
}