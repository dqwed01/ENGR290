#include <avr/io.h>

#include "src/pins.h"
#include "src/pwm.h"

void pwm_init() {
    TCCR0B = 0;
    TCCR0A = 0;
    PORTD &= ~(1 << LIFT_FAN_CTRL);
    DDRD |= (1 << LIFT_FAN_CTRL);
    TIMSK0 = 0;  // Timer0 generates lift PWM without interrupts.
    TCNT0 = 0;
    OCR0A = 0;
    TCCR0A = (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS01) | (1 << CS00);  // 16 MHz / 64 / 256 = 976.6 Hz.
}

void pwm_set_duty(uint8_t percent) {
    if (percent == 0 || percent >= 100) {
        if (percent == 0) {
            PORTD &= ~(1 << LIFT_FAN_CTRL);
        } else {
            PORTD |= (1 << LIFT_FAN_CTRL);
        }
        TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0));
    } else {
        OCR0A = (static_cast<uint16_t>(percent) * 256UL + 50UL) / 100UL - 1;
        TCCR0A |= (1 << COM0A1);
    }
}
