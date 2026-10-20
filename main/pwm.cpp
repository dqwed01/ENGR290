#include <avr/io.h>

#include "src/pins.h"
#include "src/util.h"
#include "src/pwm.h"
#include "src/global.h"

void pwm_init(void){
    DIGITAL_WRITE_HIGH(DDRB, PWM0);       // PB1 output
    DIGITAL_WRITE_LOW(PORTB, PWM0);       // idle low
}

void pwm_set_duty(uint8_t percent){
    if(percent == 0){
        OCR2A = 0;
        DIGITAL_WRITE_LOW(PORTB, PWM0);
        return;
    }
    if(percent >= 100){
        OCR2A = 255;              // compare never matches -> constant high
    } else {
        OCR2A = (uint8_t)((255 * percent) / 100);
    }
}