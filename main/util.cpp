#define F_CPU 16000000UL

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "src/util.h"
#include "src/global.h"

#ifdef DEBUGGING
static char debug_message[64] = {0};
#endif

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

void UART_init(){
    //Baud Rate current hard coded to 9600 but should look to make it modifiable
    // Set baud rate in UBRR0H and UBRR0L
    UBRR0H = (unsigned char)(UBRR_VAL >> 8);
    UBRR0L = (unsigned char)UBRR_VAL;
    
    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    
    // Set frame format: 8 data bits, 1 stop bit, no parity (8N1)
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void UART_transmit_char(unsigned char data) {
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)));
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
    while (!(UCSR0A & (1 << RXC0)));
    // Get and return received data from buffer
    return UDR0;
}
