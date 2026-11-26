#include <avr/io.h>

#include "src/pins.h"
#include "src/util.h"
#include "src/servo.h"

void servo_init(void){
    DIGITAL_WRITE_HIGH(DDRB, SERVO_PWM);   // PB1 (OC1A) output
    DIGITAL_WRITE_LOW(PORTB, SERVO_PWM);   // idle low
}

void servo_set_angle(float angle){
    if(angle > SERVO_MAX_ANGLE)  angle = SERVO_MAX_ANGLE;
    if(angle < -SERVO_MAX_ANGLE) angle = -SERVO_MAX_ANGLE;

    OCR1A = (uint16_t)(SERVO_PWM_CENTER + angle * SERVO_TICKS_PER_DEG);
}
