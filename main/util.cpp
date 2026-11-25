#define F_CPU 16000000UL

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "src/util.h"
#include "src/global.h"

uint16_t adc_read(uint8_t pin){
    if(pin >= 8){
        return 0xFFFF; //Invalid ADC pin
    } 

    char message[16];
    ADMUX = pin; // Select ADC pin
    _delay_us(10);

    //Flush out previous channel's charge
    DIGITAL_WRITE_HIGH(ADCSRA, ADSC); //Start ADC conversion
    while(DIGITAL_READ(ADCSRA, ADSC));

    //Start read reading
    DIGITAL_WRITE_HIGH(ADCSRA, ADSC); //Start ADC conversion
    int guard = 20000;
    do {
        if(--guard == 0){
            #ifdef DEBUGGING
            UART_transmit("adc_read: Guard Failed\n");
            #endif
            return 0xFFFF;
        }
    } while (DIGITAL_READ(ADCSRA, ADSC));

    #ifdef DEBUGGING
    sprintf(message, "ADC %d\n", ADC);
    UART_transmit(message);
    #endif

    uint16_t value = ADCL;
    value |= (ADCH << 8);
    return value;
}
