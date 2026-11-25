#ifndef PWM_H
#define PWM_H

#include <stdint.h>

#define FAN_PWM_TOP 255   // Timer2 Phase Correct mode with TOP = 255: 16 MHz / (2*255) = 31.4 kHz

void pwm_init(void);
void pwm_set_duty(volatile uint8_t* OCRx, uint8_t top, uint8_t percent);

#endif
