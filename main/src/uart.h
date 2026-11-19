#ifndef UART_H
#define UART_H

#define UART_BAUD_RATE 9600
#define UBRR_VAL ((F_CPU / (16UL * UART_BAUD_RATE)) - 1)

void UART_init();
void UART_transmit_char(unsigned char data);
void UART_transmit(const char* message);
unsigned char UART_receive();

// Call from the main loop to send at most one byte without waiting for the UART.
void uart_tx_pump();

// Queue one report. Returns false while a previous report is still queued.
// Call from the main loop, alongside uart_tx_pump().
bool uart_queue_report(float roll, float pitch, float yaw,
                       float ax, float ay, float az, float positionx);

#endif
