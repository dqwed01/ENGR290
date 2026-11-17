#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <math.h>

#include "src/pins.h"
#include "src/util.h"
#include "src/infrared.h"
#include "src/timer.h"
#include "src/global.h"

void ifr_sensor_init(){
    ADMUX = 0; //Use AREF
    //Prescaler of 128 (125kHz ADC clock for 16MHz system clock)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    DIDR0 |= (1 << IR_SENSOR1_PIN) | (1 << IR_SENSOR2_PIN); // Disable digital input buffer on pin1 and pin2
}

// pin is the ADC pin number (0-7)
float ifr_read(uint8_t pin){
    uint16_t value = adc_read(pin);
    if (value < 80) value = 80;
    float voltage = value * (2.55 / 1023.0);
 
    return (float)29.988 * pow(voltage, -1.173);
}