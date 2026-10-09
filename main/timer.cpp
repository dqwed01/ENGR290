#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "src/util.h"
#include "src/timer.h"
#include "src/pins.h"

uint16_t led_ms = 0;

ISR(TIMER1_COMPA_vect){                          // fires every 1 ms
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

    TCCR1B = 0;
    TCCR1A = 0;
    TCCR1B = (1 << WGM12);                 // CTC, TOP = OCR1A
    OCR1A  = 1999;                         // 16 MHz / 8 / 2000 = 1 kHz
    TCNT1  = 0;
    TIMSK1 = (1 << OCIE1A);
    TCCR1B |= (1 << CS11);                 // prescaler 8
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
