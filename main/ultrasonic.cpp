#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

#include "src/pins.h"
#include "src/util.h"
#include "src/ultrasonic.h"
#include "src/timer.h"

void us_sensor_init(){
    cli();
    DDRB |= ((OUTPUT<<US1_TRIG)|(OUTPUT<<US2_TRIG)); //Set TRIG pin to Output
    DIGITAL_WRITE_LOW(PORTB, US1_TRIG);
    DIGITAL_WRITE_LOW(PORTB, US2_TRIG);
    DDRB &= ~((1)<<US1_ECHO); 
    DDRD &= ~((1)<<US2_ECHO);
    sei();
}

uint16_t us_read(uint8_t target_us){
    uint16_t distance_cm = 0;
    if(target_us == 1 || target_us == 2){
        //Setup Registers
        uint8_t us_echo = (target_us == 1) ? US1_ECHO : US2_ECHO;
        uint8_t us_trig = (target_us == 1) ? US1_TRIG : US2_TRIG;
        volatile uint8_t* us_echo_port = (target_us == 1) ? &PINB : &PIND;
        volatile uint8_t* us_trig_port = &PORTB;
        uint16_t pulse_width_ticks;

        //Start Ranging
        DIGITAL_WRITE_HIGH(*us_trig_port, us_trig);
        delay_micro(10);
        DIGITAL_WRITE_LOW(*us_trig_port, us_trig);

        //Get Echo pulse
        pulse_width_ticks = read_pulse(us_echo_port, us_echo, HIGH, TICKS_60MS_256_PRESCALE);

        if(pulse_width_ticks != 0){
            uint16_t time_micro = pulse_width_ticks * US_PER_TICK_256_PRESCALE;
            distance_cm = (time_micro <=116) ? 2 : time_micro / (uint16_t)58; //Minimum distance is 2cm
        }
    }

    else{
        //Throw Error through UART
    }

    return distance_cm;
}