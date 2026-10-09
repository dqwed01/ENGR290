#include <avr/io.h>

#include "src/pins.h"
#include "src/util.h"
#include "src/pwm.h"

void pwm_init(void){
    DIGITAL_WRITE_HIGH(DDRD, PWM0);       // PD3 output
    DIGITAL_WRITE_LOW(PORTD, PWM0);       // idle low

    TCCR2B = 0;                                   // stop timer
    TCCR2A = (1 << WGM21) | (1 << WGM20);         // mode 3: fast PWM, TOP = 0xFF
    OCR2B  = 0;
    TCNT2  = 0;
    TIFR2  = ~0;
    TCCR2B = (1 << CS21);                         // prescaler 8, starts timer
    // OC2B stays disconnected until a non-zero duty is set

}

void pwm_set_duty(uint8_t percent){
    if(percent == 0){
        TCCR2A &= ~(1 << COM2B1);                 // disconnect OC2B so the pin is truly low
        DIGITAL_WRITE_LOW(PORTD, PWM0);
        return;
    }
    else if(percent > 100) percent = 100;
    OCR2B = (uint8_t)(((uint16_t)255 * percent) / 100);   // 100% -> 255 = constant high
    TCCR2A |= (1 << COM2B1);                      // non-inverting output on OC2B

}