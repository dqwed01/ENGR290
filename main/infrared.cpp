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

void adc_init(){
    //Prescaler of 128 (125kHz ADC clock for 16MHz system clock)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    // Disable digital input buffers on ADC0 and ADC1 to reduce power consumption
    // Values are hardcoded, make sure to update these if the IR pins change
    DIDR0 |= (1 << ADC0D) | (1 << ADC1D); 
}

float ifr_read(uint8_t pin){
    if (pin >= 8) return -1.0; // Invalid pin number

    uint16_t value = adc_read(pin);
    if (value == 0xFFFF) return -1.0;
    
    if (value < 80) value = 80;
    float voltage = value * (2.55 / 1023.0);
    
    float distance_cm = 29.988 * pow(voltage, -1.173);
#ifdef DEBUGGING
    char num[7];
    dtostrf(distance_cm, 1, 2, num);
    sprintf(debug_message, "ifr_read: %s cm\n", num);
    UART_transmit(debug_message);
#endif
    return distance_cm;
}