#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "src/util.h"
#include "src/timer.h"
#include "src/pins.h"
#include "src/global.h"

#ifdef DEBUGGING
static char debug_message[64] = {0};
#endif

static volatile uint32_t ms_ticks = 0;

ISR(TIMER0_COMPB_vect){ //Triggers every 1ms (250 ticks * 4 us at prescaler 64)
    OCR0B += 250;
    ms_ticks++;
}

uint32_t timer_millis(){
    uint8_t sreg = SREG;
    cli();                  // 32-bit read is not atomic on AVR
    uint32_t ms = ms_ticks;
    SREG = sreg;
    return ms;
}


void timer_init(){
    //Timer 0
    //ISR
    //Stop Timer
    TCCR0B &= 0xF8;
    OCR0B = 250;
    //clear stale flags
    TIFR0 = ~1;
    //Interrupt on OCR2B Match
    TIMSK0 = (1 << OCIE0B);

    //Normal Mode
    TCCR0A = 0;
    TIFR0 = ~0; // Clear all interrupt flags

    //Note: If you modify this you'll also have to modify delay_one_ms() and ISR
    //Timer0 Prescaler of 64
    TCCR0B = 0x03;

    //Timer 1
    //PWM for Servo Motor
    TCCR1A = (1 << COM1A1) | (1 << WGM11); 
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);

    ICR1 = 39999;
    OCR1A = 3000;

    //Timer 2 uses PWM on OCR2A and OCR2B
    // stop Timer2
    TCCR2B &= 0xF8;

    DIGITAL_WRITE_HIGH(DDRB, PB5);       // PB5 output
    /*
    Timer2 PWM
    Fast PWM Mode with TOP = 0xFF
    Set OC2B on compare match (inverting mode), OC2B at BOTTOM
    */
    TCCR2A |= (1 << COM2B1) | (1 << COM2B0) |
                (1 << WGM21) | (1 << WGM20);
    TCNT2  = 0;

    //Note: If you modify this you'll also have to modify the ISR in servo.cpp
    // OCR2A = 20;

    //Timer2 Prescaler of 8      
    TCCR2B |= (1 << CS21);
}