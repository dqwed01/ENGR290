#include <avr/io.h>

#include "src/pins.h"
#include "src/util.h"
#include "src/pwm.h"
#include "src/global.h"

#ifdef DEBUGGING
static char debug_message[64] = {0};
#endif

void pwm_init(void){
    DIGITAL_WRITE_HIGH(DDRB, SERVO_PWM);     // PB1 output
    DIGITAL_WRITE_LOW(PORTB, SERVO_PWM);     // idle low

    DIGITAL_WRITE_HIGH(DDRD, FAN_PWM);       // PD3 output
    DIGITAL_WRITE_LOW(PORTD, FAN_PWM);       // idle low
}

void pwm_set_servo_angle(float angle){
    if(angle > 80.0) angle = 80.0;
    if(angle < -80.0) angle = -80.0;

    OCR1A = (uint16_t)(SERVO_PWM_CENTER + angle * 20.0);
}

void pwm_set_fan_speed(uint8_t percent){
    if(percent == 0){
        // OCR2B = 0 would still give a 1-tick pulse (1/80 = 1.25%), so disconnect
        // OC2B instead: PD3 falls back to PORTD, which pwm_init() holds low.
        TCCR2A &= ~((1 << COM2B1) | (1 << COM2B0));
        return;
    }

    if(percent > 100) percent = 100;

    OCR2B = (uint8_t)((FAN_PWM_TOP * percent) / 100);

    // Non-inverting on OC2B; reconnects the pin if 0% disconnected it.
    TCCR2A |= (1 << COM2B1);
}