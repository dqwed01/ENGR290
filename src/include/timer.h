#ifndef TIMER_H
#define TIMER_H

#define TIMER0_MAX_TICK 256
#define TICKS_1MS_64_PRESCALE 250

void timer_init();
void delay_one_ms();
void delay_ms(uint16_t);

#endif
