#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "src/pins.h"
#include "src/util.h"
#include "src/infrared.h"
#include "src/timer.h"
#include "src/global.h"

#ifdef DEBUGGING
static char debug_message[64] = {0};
#endif

void ifr_sensor_init(){
    ADMUX = 0; //Use AREF
    //Prescaler of 128
    ADCSRA =(1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    //Disable digital input buffer on ADC0
    DIDR0 |= (1 << ADC0D); 
}

float ifr_read(){
    uint16_t value = adc_read(0);
    if (value < 80) value = 80;
    float voltage = value * (2.55 / 1023.0);
    
    float distance_cm = 29.988 * pow(voltage, -1.173);
#ifdef DEBUGGING
    char num[7];
    dtostrf(distance_cm, 1, 2, num);
    sprintf(debug_message, "ifr_read: %s cm\n", num);
#endif
    return distance_cm;
}