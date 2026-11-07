#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "src/util.h"
#include "src/timer.h"
#include "src/pins.h"
#include "src/global.h"

#ifdef DEBUGGING
static char debug_message[64] = {0};
#endif

uint16_t servo_count = 0;
uint16_t isr_count = 0;

void servo_init(){
    // Servo output
    DIGITAL_WRITE_HIGH(DDRB, SERVO1_CTRL);     
    // idle low
    DIGITAL_WRITE_LOW(PORTB, SERVO1_CTRL);        
}

void servo_set_angle(float angle){
    if(angle > 80.0) angle = 80.0;
    if(angle < -80.0) angle = -80.0;

    OCR1A = (uint16_t)(3000.0 + angle * 20.0);
}