#ifndef TIMER_H
#define TIMER_H

#define TIMER0_MAX_TICK 256
#define TICKS_1MS_64_PRESCALE 250
#define TICKS_1US_8_PRESCALE 2
#define TICKS_60MS_256_PRESCALE 3750
#define US_PER_TICK_256_PRESCALE 16

#define TIMER1_MAX_TICK 65536


void timer_init();
void delay_one_ms();
void delay_ms(uint16_t);
void delay_one_micro();
void delay_micro(uint16_t);

#endif
