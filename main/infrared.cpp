#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "src/pins.h"
#include "src/util.h"
#include "src/infrared.h"
#include "src/timer.h"

void ifr_sensor_init(){
    ADMUX = 0; //Use AREF
    //Prescaler of 128
    ADCSRA =(1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    //Disable digital input buffer on ADC0
    DIDR0 |= (1 << ADC0D); 
}

uint16_t ifr_read(){
    uint16_t value = adc_read(0);
    if (value < 80) value = 80;
    return (uint16_t)(4800.0 / (value - 20.0));
}