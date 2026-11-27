#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

#include "src/timer.h"
#include "src/servo.h"

static volatile uint32_t ms_ticks = 0;

ISR(TIMER2_COMPB_vect){ //Triggers every 1ms (250 ticks * 4 us at prescaler 64)
    OCR2B += TICKS_1MS_64_PRESCALE;
    ms_ticks++;
}

uint32_t timer_millis(){
    uint8_t sreg = SREG;
    cli();                  // 32-bit read is not atomic on AVR
    uint32_t ms = ms_ticks;
    SREG = sreg;
    return ms;
}

// µs since timer_init(). 4 µs resolution (prescaler 64). Wraps after ~71 min.
uint32_t timer_micros(){
    uint8_t sreg = SREG;
    cli();
    uint32_t ms   = ms_ticks;
    uint8_t  t    = TCNT2;
    uint8_t  next = OCR2B;              // next compare point
    uint8_t  since = t - next + TICKS_1MS_64_PRESCALE;    // ticks since last compare, 0..249

    // Compare matched but the ISR hasn't run yet (we're inside cli()).
    // (t - next) < 128 means TCNT2 is past the match, not just before it.
    if ((TIFR2 & (1 << OCF2B)) && (uint8_t)(t - next) < 128) {
        ms++;
        since = t - next;
    }
    SREG = sreg;
    return ms * 1000UL + since * 4U;
}

void timer_init(){
    // ------ Timer 2 (1ms Interrupt) ------
    // Uses the above ISR

    // Stop Timer2
    TCCR2B = 0;

    // Normal Mode
    TCCR2A = 0;

    // Set Compare Match Register for 1ms increments
    OCR2B = TICKS_1MS_64_PRESCALE;

    // Interrupt on OCR2B Match
    TIMSK2 = (1 << OCIE2B);

    // Clear all interrupt flags
    TIFR2 = ~0;

    // Prescaler of 64 (CS22:20 = 100 on Timer2), starts timer
    TCCR2B = (1 << CS22);

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

    // ------ Timer 0 (PWM for Lift and Thrust Fans) ------
    // Stop Timer0
    TCCR0B = 0;

    // WGM02:00 = 001: Phase Correct PWM with TOP = 0xFF
    // 16 MHz / (255*2) = 31.4 kHz, close to the 25 kHz we want.
    // Non-inverting on PD6 (OC0A) and PD5 (OC0B)
    TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM00);

    OCR0A = OCR0B = 0; // start with 0% duty cycle

    // No Timer0 interrupts
    TIMSK0 = 0;

    // Timer0 Prescaler of 1
    TCCR0B = (1 << CS00);
}
