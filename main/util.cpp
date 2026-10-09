#define F_CPU 16000000UL

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "src/util.h"
#include "src/pins.h"
#include "src/timer.h"

uint32_t read_pulse(volatile uint8_t* p_port, uint8_t pin, uint8_t value, uint16_t timeout_in_ticks){
    uint16_t time_elapsed = 0;
    uint16_t time_bits = (TIMER1_MAX_TICK) - timeout_in_ticks;
    //Precompute Timer Counter
    TIMER1_CNTR = time_bits;

    //Wait for pin to reach value
    uint16_t guard = 60000;
    do {
        if(--guard == 0){
            UART_transmit("Failed at guard");
            return 0xFFFF; //Highest Time elapsed to prevent early collisions
        } 
    } while ((DIGITAL_READ(*p_port, pin)) != value);

    //Start counting and wait for pin to go Low or until timeout is reached
    uint16_t max_overflows = timeout_in_ticks >> 8;   // 3750 -> 14 (about 57 ms)
    uint16_t overflows = 0;

    TIMER0_CTR_REG_B = 0;
    TIMER0_CNTR = 0;
    TIMER0_INT_FLAG_REG = (1 << TOV0);
    TIMER0_CTR_REG_B = (1 << CS02);                   // prescaler 256 -> 16 us/tick

    while (DIGITAL_READ(*p_port, pin) == value) {
        if (TIMER0_INT_FLAG_REG & (1 << TOV0)) {
            TIMER0_INT_FLAG_REG = (1 << TOV0);
            if (++overflows >= max_overflows) {
                TIMER0_CTR_REG_B = 0;
                UART_transmit("Failed at overflow");
                return 0xFFFF;                             // timed out
            }
        }
    }

    //Stop counting and check if timeout occured
    TIMER0_CTR_REG_B = 0;                             // stop first, then read
    uint8_t low = TIMER0_CNTR;
    if (TIMER0_INT_FLAG_REG & (1 << TOV0)) {          // overflow landed just before the stop
        TIMER0_INT_FLAG_REG = (1 << TOV0);
        overflows++;
    }
    return (overflows << 8) | low;
}

uint32_t adc_read(uint8_t pin){
    if(pin >= 8){
        return 0xFFFF; //Invalid ADC pin
    } 

    char message[16];
    ADMUX = pin; //Ensure that AREF is used
    DIGITAL_WRITE_HIGH(ADCSRA, ADSC); //Start ADC conversion
    int guard = 20000;
    do {
        if(--guard == 0){
            sprintf(message, "ADC Failure\n");
            UART_transmit(message);
            return 0xFFFF;
        }
    } while (DIGITAL_READ(ADCSRA, ADSC));

    sprintf(message, "ADC %d\n", ADC);
    UART_transmit(message);

    uint32_t value = ADCL;
    value |= (ADCH << 8);
    return value;
}

void UART_init(){
    //Baud Rate current hard coded to 9600 but should look to make it modifiable
    // Set baud rate in UBRR0H and UBRR0L
    UART_BAUD_RATE_REG0_H = (unsigned char)(UBRR_VAL >> 8);
    UART_BAUD_RATE_REG0_L = (unsigned char)UBRR_VAL;
    
    // Enable receiver and transmitter
    UART_CTR_STATUS_REG0_B = (1 << RXEN0) | (1 << TXEN0);
    
    // Set frame format: 8 data bits, 1 stop bit, no parity (8N1)
    UART_CTR_STATUS_REG0_C = (1 << UCSZ01) | (1 << UCSZ00);
}

void UART_transmit_char(unsigned char data) {
    // Wait for empty transmit buffer
    while (!(UART_CTR_STATUS_REG0_A & (1 << UDRE0)));
    // Put data into buffer, sends the data
    UDR0 = data;
}

void UART_transmit(char* message){
    for(uint8_t i = 0; message[i] != '\0'; i++){
        UART_transmit_char((message[i]));
    }
}

unsigned char UART_receive(void) {
    // Wait for data to be received
    while (!(UART_CTR_STATUS_REG0_A & (1 << RXC0)));
    // Get and return received data from buffer
    return UDR0;
}
