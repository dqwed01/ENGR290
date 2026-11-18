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

    // A0..A5 are Arduino pin numbers (14..19); subtract A0 to get the ADC channel (0..5),
    DIDR0 |= (1 << (IR_SENSOR1_PIN - A0)) | (1 << (IR_SENSOR2_PIN - A0)); // Disable digital input buffer on IR pins
}

float ifr_read(uint8_t pin){
    if (pin < A0 || pin > A5) return -1.0; // Invalid pin

    pin -= A0; // Convert Arduino pin number to ADC channel
    uint16_t value = adc_read(pin);
    if (value == 0xFFFF) return -1.0;
    if (value < 80) value = 80;
    float voltage = value * (2.55 / 1023.0);
 
    return (float)29.988 * pow(voltage, -1.173);
}