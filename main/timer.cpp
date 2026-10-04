#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "src/util.h"
#include "src/timer.h"
#include "src/pins.h"

void timer_init(){
    TIMER0_CTR_REG_B = 0; //Stop Timer
    TIMER0_INT_MASK = 0; //No Interrupt Timers
    TIMER0_CTR_REG_A = 0; //Disconnect OC0x pins and use Normal Waveform Generator
    TIMER0_INT_FLAG_REG = ~0; // Clear all interrupt flags

    TIMER1_CTR_REG_B = 0;
    TIMER1_INT_MASK = 0;
    TIMER1_CTR_REG_A = 0;
    TIMER1_INT_FLAG_REG = ~0;
}

void delay_one_ms(){
    TIMER0_CNTR = (TIMER0_MAX_TICK) - (TICKS_1MS_64_PRESCALE);
    TIMER0_CTR_REG_B = 0x03; //Set Prescaler to 64

    while((TIMER0_INT_FLAG_REG & (1 << TOV0)) == 0);

    TIMER0_CTR_REG_B = 0; //Stop Timer upon overflow
    DIGITAL_WRITE_HIGH(TIMER0_INT_FLAG_REG, TOV0); //Clear Timer Overflow flag
}

void delay_micro(uint16_t time_micro){
    uint16_t time_bits = (TIMER1_MAX_TICK) - time_micro * (TICKS_1US_8_PRESCALE);

    TIMER1_CNTR_H = (time_bits & 0xFF00) >> 8;
    TIMER1_CNTR_L = (time_bits & 0X00FF);
    TIMER1_CTR_REG_B = 0x02; //Set Prescaler to 8

    while((TIMER1_INT_FLAG_REG & (1 << TOV1)) == 0);

    TIMER1_CTR_REG_B = 0; //Stop Timer upon overflow
    DIGITAL_WRITE_HIGH(TIMER1_INT_FLAG_REG, TOV1); //Clear Timer Overflow flag
}

void delay_ms(uint16_t time_ms){
    for(uint16_t i = 0; i < time_ms; i++){
        delay_one_ms();
    }
}
