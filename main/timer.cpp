#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "src/util.h"
#include "src/timer.h"
#include "src/pins.h"
#include "src/global.h"

uint16_t isr_period = 0;

ISR(TIMER2_COMPB_vect){ //Triggers every 125us
    OCR2B += 251;
    if(blink_enabled){
        if(++isr_period >= 6000){
            isr_period = 0;
            DIGITAL_TOGGLE(PORTB, PB5);
        }
    }
    else{
        isr_period = 0; //Prevent shorter first toggle
    }
}


void timer_init(){
    TCCR0B = 0; //Stop Timer
    TIMSK0 = 0; //No Interrupt Timers
    TCCR0A = 0; //Disconnect OC0x pins and use Normal Waveform Generator
    TIFR0 = ~0; // Clear all interrupt flags

    //Timer 2 uses PWM on OCR2A and ISR on OCR2B
    // stop Timer2
    TCCR2B &= 0xF8;

    DIGITAL_WRITE_HIGH(DDRB, PB5);       // PB5 output
    //Timer2 PWM
    //Fast PWM Mode with TOP = 0xFF
    //Clear OC2A on compare match, set OC2A at BOTTOM
    TCCR2A |= (1 << COM2A1) | (1 << COM2A0) | (1 << WGM21) | (1 << WGM20);
    TCNT2  = 0;

    //Timer2 ISR
    OCR2B  = 250;
    TIFR2  = (1 << OCF2B); // clear stale flag
    TIMSK2 = (1 << OCIE2B); // enable compare-match interrupt

    //Note: If you modify this you'll also have to modify the ISR ms value
    //Timer2 Prescaler of 8       
    TCCR2B |= (1 << CS21);
}

void delay_one_ms(){
    TCNT0 = (TIMER0_MAX_TICK) - (TICKS_1MS_64_PRESCALE);
    TCCR0B = 0x03; //Set Prescaler to 64

    while((TIFR0 & (1 << TOV0)) == 0);

    TCCR0B = 0; //Stop Timer upon overflow
    DIGITAL_WRITE_HIGH(TIFR0, TOV0); //Clear Timer Overflow flag
}

void delay_ms(uint16_t time_ms){
    for(uint16_t i = 0; i < time_ms; i++){
        delay_one_ms();
    }
}
