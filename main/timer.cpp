#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "src/util.h"
#include "src/timer.h"
#include "src/pins.h"

uint16_t led_ms = 0;

ISR(TIMER2_COMPA_vect){                          // fires every 1 ms
    if(++led_ms >= 750){
        led_ms = 0;
        PINB = (1 << PB5);           // writing 1 to PINx toggles the pin
    }
}

void timer_init(){
    TIMER0_CTR_REG_B = 0; //Stop Timer
    TIMER0_INT_MASK = 0; //No Interrupt Timers
    TIMER0_CTR_REG_A = 0; //Disconnect OC0x pins and use Normal Waveform Generator
    TIMER0_INT_FLAG_REG = ~0; // Clear all interrupt flags

    TCCR2B = 0;                                  // stop Timer2
    TCCR2A = (1 << WGM21);                       // CTC mode
    OCR2A  = 124;                                // 16 MHz / 128 / 125 = 1 kHz
    TCNT2  = 0;
    TIFR2  = (1 << OCF2A);                       // clear stale flag
    TIMSK2 = (1 << OCIE2A);                      // enable compare-match interrupt

}

void delay_one_ms(){
    TIMER0_CNTR = (TIMER0_MAX_TICK) - (TICKS_1MS_64_PRESCALE);
    TIMER0_CTR_REG_B = 0x03; //Set Prescaler to 64

    while((TIMER0_INT_FLAG_REG & (1 << TOV0)) == 0);

    TIMER0_CTR_REG_B = 0; //Stop Timer upon overflow
    DIGITAL_WRITE_HIGH(TIMER0_INT_FLAG_REG, TOV0); //Clear Timer Overflow flag
}

void delay_ms(uint16_t time_ms){
    for(uint16_t i = 0; i < time_ms; i++){
        delay_one_ms();
    }
}
