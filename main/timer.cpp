#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

#include "src/timer.h"
#include "src/pwm.h"

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

    // COM1A1: Non-inverting on PB1
    // WGM11: low half of the mode bits (WGM13:12 are set in TCCR1B below)
    TCCR1A = (1 << COM1A1) | (1 << WGM11);

    // Period: (39999 + 1) × 0.5 µs = 20 ms
    ICR1 = SERVO_PWM_TOP;
    // Pulse: 3000 × 0.5 µs = 1.5 ms
    OCR1A = SERVO_PWM_CENTER;

    // WGM13:10 = 1110: Fast PWM with TOP = ICR1.
    // CS11: prescaler 8: 16 MHz / 8 = 2 MHz, so one tick = 0.5 µs.
    // Written last so Timer1 only starts once ICR1 and OCR1A are set
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);

    // ------ Timer 2 (PWM for Lift and Thrust Fans) ------
    // Stop Timer2
    TCCR2B &= 0xF8;

    // WGM22:20 = 111: Set Fast PWM Mode with TOP = OCR2A
    // set_fan_duty() sets COM2B1: Non-inverting on PD3
    TCCR2A |= (1 << WGM21) | (1 << WGM20);

    // 25 kHz PWM frequency: (16 MHz / 8) / (79 + 1) = 25 kHz
    OCR2A = FAN_PWM_TOP;
    
    // Set initial duty cycle to 0% (0/79)
    // OCR2B = 0; // not actually necessary since the pin is disconnected when duty cycle is 0%

    // Timer2 Prescaler of 8
    TCCR2B |= (1 << WGM22) | (1 << CS21);
}