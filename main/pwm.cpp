#include <avr/io.h>

#include "src/pins.h"
#include "src/util.h"
#include "src/pwm.h"
#include "src/global.h"

void pwm_init(void){
    DIGITAL_WRITE_HIGH(DDRB, SERVO_PWM);       // PB1 output
    DIGITAL_WRITE_LOW(PORTB, SERVO_PWM);       // idle low

    DIGITAL_WRITE_HIGH(DDRB, LIFT_FAN_PWM);    // PB3 output
    DIGITAL_WRITE_LOW(PORTB, LIFT_FAN_PWM);    // idle low

    DIGITAL_WRITE_HIGH(DDRD, THRUST_FAN_PWM);  // PD3 output
    DIGITAL_WRITE_LOW(PORTD, THRUST_FAN_PWM);  // idle low
}

void pwm_set_servo_angle(float angle){
    if(angle > 80.0) angle = 80.0;
    if(angle < -80.0) angle = -80.0;

    OCR1A = (uint16_t)(SERVO_PWM_CENTER + angle * 20.0);
}

void pwm_set_fan_speed(volatile uint8_t* OCR2x, uint8_t percent){
    if(percent > 100) percent = 100;

    // Thankfully, Phase Correct mode does not cause issues when OCR2x is set to 0 or 255
    *OCR2x = (uint8_t)((FAN_PWM_TOP * percent) / 100);
}