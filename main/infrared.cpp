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

// ---- Smoothing -------------------------------------------------------------
// Two stages, both on the raw ADC value (before the nonlinear cm conversion):
//  1. Median of IFR_SAMPLES back-to-back reads rejects single-sample spikes
//  2. An exponential moving average per pin smooths what's left.
//     Higher IFR_EMA_ALPHA = faster response, lower = smoother.
#define IFR_SAMPLES    5      // odd number, kept small since each read is ~0.2 ms
#define IFR_EMA_ALPHA  0.3f

static float ifr_ema[8];
static uint8_t ifr_ema_ready = 0;  // bit n set once pin n has a first value

static float adc_to_cm(float value){
    if (value < 80) value = 80;
    float voltage = value * (2.55 / 1023.0);
    return 29.988 * pow(voltage, -1.173);
}

// Returns 0xFFFF if any read fails
static uint16_t adc_read_median(uint8_t pin){
    uint16_t s[IFR_SAMPLES];
    for (uint8_t i = 0; i < IFR_SAMPLES; i++) {
        s[i] = adc_read(pin);
        if (s[i] == 0xFFFF) return 0xFFFF;
    }
    // Insertion sort, fine for 5 elements
    for (uint8_t i = 1; i < IFR_SAMPLES; i++) {
        uint16_t v = s[i];
        int8_t j = i - 1;
        while (j >= 0 && s[j] > v) { s[j + 1] = s[j]; j--; }
        s[j + 1] = v;
    }
    return s[IFR_SAMPLES / 2];
}

#ifdef LOGGING
// "label:value" lines in the format the Arduino Serial Plotter expects.
// Prints the median-only and fully filtered values so you can compare the
// two traces. Failed reads show up as -1.
static void ifr_plot(uint8_t pin, float raw_cm, float filtered_cm){
    char raw[10];
    char filt[10];
    char line[48];
    dtostrf(raw_cm, 1, 2, raw);
    dtostrf(filtered_cm, 1, 2, filt);
    sprintf(line, "ir%u_raw:%s ir%u_cm:%s\n", pin, raw, pin, filt);
    UART_transmit(line);
}
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

    uint16_t value = adc_read_median(pin);
    if (value == 0xFFFF) {
        #ifdef LOGGING
        ifr_plot(pin, -1.0, -1.0);
        #endif
        return -1.0;
    }

    uint8_t mask = 1 << pin;
    if (ifr_ema_ready & mask) {
        ifr_ema[pin] += IFR_EMA_ALPHA * (value - ifr_ema[pin]);
    } else {
        ifr_ema[pin] = value;  // seed with the first reading
        ifr_ema_ready |= mask;
    }

    float distance_cm = adc_to_cm(ifr_ema[pin]);
    #ifdef DEBUGGING
    char num[7];
    dtostrf(distance_cm, 1, 2, num);
    sprintf(debug_message, "ifr_read: %s cm\n", num);
    UART_transmit(debug_message);
    #endif
    #ifdef LOGGING
    ifr_plot(pin, adc_to_cm(value), distance_cm);
    #endif
    return distance_cm;
}