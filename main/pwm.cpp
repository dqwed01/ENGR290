#include <avr/io.h>

#include "src/pins.h"
#include "src/util.h"
#include "src/pwm.h"

void pwm_init(void){
    DIGITAL_WRITE_HIGH(DDRB, PWM0);       // PB1 output
    DIGITAL_WRITE_LOW(PORTB, PWM0);       // idle low

    TIMER1_CTR_REG_B = 0;                           // stop timer
    TIMER1_CTR_REG_A = (1 << WGM11);                // mode 14: fast PWM, TOP = ICR1
    ICR1   = PWM_TOP;
    OCR1A  = 0;
    TIMER1_CNTR  = 0;
    TIMER1_INT_FLAG_REG  = ~0;
    TIMER1_CTR_REG_B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);   // prescaler 8, starts timer
    // OC1A stays disconnected until a non-zero duty is set
}

void pwm_set_duty(uint8_t percent){
    if(percent == 0){
        TIMER1_CTR_REG_A &= ~(1 << COM1A1);         // disconnect OC1A so the pin is truly low
        DIGITAL_WRITE_LOW(PORTB, PWM0);
        return;
    }
    if(percent >= 100){
        OCR1A = PWM_TOP + 1;              // compare never matches -> constant high
    } else {
        OCR1A = (uint16_t)(((uint32_t)(PWM_TOP + 1) * percent) / 100);
    }
    TIMER1_CTR_REG_A |= (1 << COM1A1);              // non-inverting output on OC1A
}