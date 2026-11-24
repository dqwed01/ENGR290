#define F_CPU 16000000UL

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "src/pins.h"
#include "src/util.h"
#include "src/ultrasonic.h"
#include "src/timer.h"
#include "src/global.h"

#ifdef DEBUGGING
static char debug_message[64] = {0};
#endif

void us_sensor_init(){
    DDRB |= (OUTPUT<<US1_TRIG); //Set TRIG pin to Output
    DIGITAL_WRITE_LOW(PORTB, US1_TRIG);
    DDRB &= ~((1)<<US1_ECHO); 
}

// Time after the trigger for the echo pin to go HIGH. The sensor sends its
// 40 kHz burst first, which takes a few hundred µs (I think) and
// does not depend on distance, so it gets its own fixed timeout.
#define US_START_TIMEOUT_US 2000

// Returns the pulse width in µs.
// 0      -> pulse never started (sensor not responding)
// 0xFFFF -> pulse lasted longer than timeout_us (nothing within range)
static uint16_t read_pulse(volatile uint8_t* p_port, uint8_t pin, uint8_t value, uint16_t timeout_us){
    //Wait for pin to reach value
    uint32_t start = timer_micros();
    while (DIGITAL_READ(*p_port, pin) != value) {
        if (timer_micros() - start >= US_START_TIMEOUT_US) {
            #ifdef DEBUGGING
            UART_transmit("read_pulse: Timed out waiting for start edge\n");
            #endif
            return 0;
        }
    }

    //Time how long the pin stays at value
    start = timer_micros();
    while (DIGITAL_READ(*p_port, pin) == value) {
        if (timer_micros() - start >= timeout_us) {
            #ifdef DEBUGGING
            UART_transmit("read_pulse: Timed out waiting for end edge\n");
            #endif
            return 0xFFFF;
        }
    }

    uint16_t pulse_us = (uint16_t)(timer_micros() - start);
    #ifdef DEBUGGING
    sprintf(debug_message, "read_pulse: pulse %u us\n", pulse_us);
    UART_transmit(debug_message);
    #endif
    return pulse_us;
}

float us_read(){
    float distance_cm = 0;
    //Setup Registers
    uint8_t us_echo = US1_ECHO;
    uint8_t us_trig = US1_TRIG;
    volatile uint8_t* us_echo_port = &PINB;
    volatile uint8_t* us_trig_port = &PORTB;

    // A previous pulse that we timed out on is still running (the sensor holds
    // ECHO HIGH for ~38 ms with no echo). Triggering now would be ignored, and
    // we'd time the tail of the old pulse as a fake short distance.
    if (DIGITAL_READ(*us_echo_port, us_echo)) {
        #ifdef DEBUGGING
        UART_transmit("us_read: Sensor busy, skipping\n");
        #endif
        return US_NO_READING;
    }

    //Start Ranging
    DIGITAL_WRITE_HIGH(*us_trig_port, us_trig);
    _delay_us(10);
    DIGITAL_WRITE_LOW(*us_trig_port, us_trig);

    //Get Echo pulse
    uint16_t pulse_us = read_pulse(us_echo_port, us_echo, HIGH, US_TIMEOUT_US);
    if (pulse_us == 0) {
        return US_NO_READING;           // Sensor never responded
    } else if (pulse_us == 0xFFFF) {
        distance_cm = US_OUT_OF_RANGE;  // Nothing within US_MAX_CM
    } else {
        distance_cm = (pulse_us <= 116) ? 2 : pulse_us / 58.0; //Minimum distance is 2cm
    }
    #ifdef DEBUGGING
    char num[7];
    dtostrf(distance_cm, 1, 2, num);
    sprintf(debug_message, "us_read: %s cm\n", num);
    UART_transmit(debug_message);
    #endif
    return distance_cm;
}