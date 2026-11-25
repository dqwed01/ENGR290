#include <avr/io.h>

#include "src/pins.h"
#include "src/util.h"
#include "src/pwm.h"
#include "src/global.h"

void pwm_init(void){
    DIGITAL_WRITE_HIGH(DDRB, LIFT_FAN_PWM);    // PB3 output
    DIGITAL_WRITE_LOW(PORTB, LIFT_FAN_PWM);    // idle low

    DIGITAL_WRITE_HIGH(DDRD, THRUST_FAN_PWM);  // PD3 output
    DIGITAL_WRITE_LOW(PORTD, THRUST_FAN_PWM);  // idle low
}

void pwm_set_duty(volatile uint8_t* OCRx, uint8_t top, uint8_t percent){
    if(percent > 100) percent = 100;

    // Thankfully, Phase Correct mode does not cause issues when the OCR is set to 0 or 255
    *OCRx = (uint8_t)((top * percent) / 100);
}