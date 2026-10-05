#ifndef PWM_H
#define PWM_H

#define PWM_FREQ_HZ   1000UL
#define PWM_PRESCALER 8UL                                   // CS11
#define PWM_TOP       ((F_CPU / (PWM_PRESCALER * PWM_FREQ_HZ)) - 1)   // 1999 -> 1 kHz

void pwm_init(void);
void pwm_set_duty(uint8_t percent);   // 0-100

#endif
