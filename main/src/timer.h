#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#define TICKS_1MS_64_PRESCALE 250   // Timer2 ticks per 1 ms: 16 MHz / 64 = 250 kHz

void timer_init();

// Milliseconds since timer_init(), counted by the Timer2 compare-B ISR.
// Wraps after ~49.7 days; compare with (uint32_t)(now - then).
uint32_t timer_millis();
uint32_t timer_micros();

#endif