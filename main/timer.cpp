#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "src/timer.h"
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
    //Interrupt on OCR0B Match
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
}
