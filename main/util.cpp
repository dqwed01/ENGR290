#define F_CPU 16000000UL

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "src/util.h"
#include "src/pins.h"
#include "src/timer.h"
#include "src/global.h"

#ifdef DEBUGGING
static char debug_message[64] = {0};
#endif

uint16_t read_pulse(volatile uint8_t* p_port, uint8_t pin, uint8_t value, uint16_t timeout_in_ticks){
    uint16_t time_elapsed = 0;

    //Wait for pin to reach value
    uint16_t guard = 60000;
    do {
        if(--guard == 0){
#ifdef DEBUGGING 
            UART_transmit("read_pulse: Failed at guard\n");
#endif
            return 0xFFFF; //Highest Time elapsed to prevent early collisions
        } 
    } while ((DIGITAL_READ(*p_port, pin)) != value);

    //Start counting and wait for pin to go Low or until timeout is reached
    uint16_t max_overflows = timeout_in_ticks >> 8;   // 3750 -> 14 (about 57 ms)
    uint16_t overflows = 0;

    // This sucks, and needs fixing
    // I suggest adding a timer_micros() to timer.cpp which reads TCNT0 (µs since last 1ms interrupt).
    // Then just subtract end - start instead of reprogramming Timer0
    TCCR0B = 0;
    TCNT0 = 0;
    TIFR0 = (1 << TOV0);
    TCCR0B = (1 << CS02);                   // prescaler 256 -> 16 us/tick

    while (DIGITAL_READ(*p_port, pin) == value) {
        if (TIFR0 & (1 << TOV0)) {
            TIFR0 = (1 << TOV0);
            if (++overflows >= max_overflows) {
                TCCR0B = 0;
#ifdef DEBUGGING
                UART_transmit("read_pulse: Failed at overflow\n");
#endif
                return 0xFFFF;                             // timed out
            }
        }
    }

    //Stop counting and check if timeout occured
    TCCR0B = 0;                             // stop first, then read
    uint8_t low = TCNT0;
    if (TIFR0 & (1 << TOV0)) {          // overflow landed just before the stop
        TIFR0 = (1 << TOV0);
        overflows++;
    }
    uint16_t pulse = (overflows << 8) | low;
#ifdef DEBUGGING
    sprintf(debug_message, "read_pulse: pulse %d\n", pulse);
    UART_transmit(debug_message);
#endif
    return (overflows << 8) | low;
}

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
