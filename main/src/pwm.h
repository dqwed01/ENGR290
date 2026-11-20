#ifndef PWM_H
#define PWM_H

#include <stdint.h>

// P4 fan output, PD6/OC0A. Timer0 belongs to lift PWM in this bench test.
void pwm_init();
void pwm_set_duty(uint8_t percent);

#endif
