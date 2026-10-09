#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <math.h>

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

float ifr_read(){
    uint32_t value = adc_read(0);
    // if (value < 80) value = 80;
    // float voltage = value * (5.0 / 1023.0);
    if(value > 690) return 7.0f;
    if(value > 654) return 8.0f;
    if(value > 604) return 9.0f;
    if(value > 564) return 10.0f;
    if(value > 484) return 12.0f;
    if(value > 426) return 14.0f;
    if(value > 380) return 16.0f;
    if(value > 349) return 18.0f;
    if(value > 321) return 20.0f;
    if(value > 298) return 22.0f;
    if(value > 281) return 24.0f;
    if(value > 266) return 26.0f;
    if(value > 243) return 28.0f;
    if(value > 233) return 30.0f;
    if(value > 211) return 35.0f;
    if(value > 191) return 40.0f;
    if(value > 176) return 45.0f;
    if(value > 168) return 50.0f;
    if(value > 157) return 55.0f;
    if(value > 146) return 60.0f;
    return 65.0f;
 
    // return (float)29.988 * pow(voltage, -1.173);
}