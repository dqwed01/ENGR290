#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#define TIMER0_MAX_TICK 256
#define TICKS_1MS_64_PRESCALE 250
#define TICKS_1US_8_PRESCALE 2
#define TICKS_60MS_256_PRESCALE 3750
#define US_PER_TICK_256_PRESCALE 16

#define TIMER1_MAX_TICK 65536


void timer_init();
// Timer2 supplies a 1 ms clock alongside Timer0 lift PWM and Timer1 servo PWM.
// Enable global interrupts after initialization.
void timer_clock_init();
void delay_one_ms();
void delay_ms(uint16_t);

// Milliseconds from the selected clock, reset by timer_clock_init().
// Wraps after ~49.7 days; compare with (uint32_t)(now - then).
uint32_t timer_millis();

#endif
