#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "src/util.h"
#include "src/pins.h"
#include "src/timer.h"

uint16_t read_pulse(volatile uint8_t* p_port, uint8_t pin, uint8_t value, uint16_t timeout_in_ticks){
    uint16_t time_elapsed = 0;
    uint16_t time_bits = (TIMER1_MAX_TICK) - timeout_in_ticks;
    //Precompute Timer Counter
    // TIMER1_CNTR = TIMER1_MAX_TICK - timeout_in_ticks;
    TIMER1_CNTR = time_bits;

    //Wait for pin to go High
    uint16_t guard = 60000;
    do {
        if(--guard == 0) return 0xFFFF; //Highest Time elapsed to prevent early collisions
    } while ((DIGITAL_READ(*p_port, pin)) != value);

    //Start counting and wait for pin to go Low or until timeout is reached
    TIMER1_CTR_REG_B = 0x04; //Set Prescaler to 256
    do {} while (DIGITAL_READ(*p_port, pin) == value && (TIMER1_INT_FLAG_REG & (1 << TOV1)) == 0);

    //Stop counting and check if timeout occured
    TIMER1_CTR_REG_B = 0;
    if(DIGITAL_READ(TIMER1_INT_FLAG_REG, TOV1)){
        //Loop ended due to timeout
        DIGITAL_WRITE_HIGH(TIMER1_INT_FLAG_REG, TOV1); //Clear Timer Overflow flag
    }

    else{
        //Pulse was registered
        time_elapsed = TIMER1_CNTR - time_bits;
    }
    return time_elapsed;
}

void UART_init(){
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

unsigned char UART_receive(void) {
    // Wait for data to be received
    while (!(UART_CTR_STATUS_REG0_A & (1 << RXC0)));
    // Get and return received data from buffer
    return UDR0;
}
