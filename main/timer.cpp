#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

#include "src/timer.h"

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
    // ------ Timer 0 (1ms Interrupt) ------
    // Uses the above ISR

    // Stop Timer0
    TCCR0B &= 0xF8;

    // Normal Mode
    TCCR0A = 0;

    // Set Compare Match Register for 1ms increments
    OCR0B = 250;

    //Interrupt on OCR0B Match
    TIMSK0 = (1 << OCIE0B);

    // Clear all interrupt flags
    TIFR0 = ~0;

    // Prescaler of 64, starts timer
    TCCR0B = 0x03;

    // ------ Timer 1 (PWM for Servo Motor) ------
    // Stop Timer1
    TCCR1B &= 0xF8;

    TCCR1A = (1 << COM1A1) | (1 << WGM11); 
    // COM1A1: Non-inverting on PB1
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);
    // WGM13:0 = 1110: Fast PWM with TOP = ICR1.
    // CS11: prescaler 8: 16 MHz / 8 = 2 MHz, so one tick = 0.5 µs.

    // Period: (39999 + 1) × 0.5 µs = 20 ms = 50 Hz
    ICR1 = 39999;
    // Pulse: 3000 × 0.5 µs = 1.5 ms
    OCR1A = 3000;

    // ------ Timer 2 (PWM for Lift and Thrust Fans) ------
    // Stop Timer2
    TCCR2B &= 0xF8;

    // Set Fast PWM Mode with TOP = 0xFF
    // Set OC2B on compare match (inverting mode), OC2B at BOTTOM
    TCCR2A |= (1 << COM2B1) | (1 << COM2B0) |
                (1 << WGM21) | (1 << WGM20);
    TCNT2  = 0;

    // Timer2 Prescaler of 8      
    TCCR2B |= (1 << CS21);
}