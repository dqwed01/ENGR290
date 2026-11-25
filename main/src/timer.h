#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

void timer_init();

// Milliseconds since timer_init(), counted by the Timer0 compare-B ISR.
// Wraps after ~49.7 days; compare with (uint32_t)(now - then).
uint32_t timer_millis();
uint32_t timer_micros();

#endif