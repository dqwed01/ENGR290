#ifndef UART_H
#define UART_H

#include <stdint.h>

#define UART_BAUD_RATE 9600
#define UBRR_VAL ((F_CPU / (16UL * UART_BAUD_RATE)) - 1)

void UART_init();
void UART_transmit_char(unsigned char data);
void UART_transmit(const char* message);
unsigned char UART_receive();

// Start the report schedule after hovercraft initialization completes.
void uart_report_init(uint32_t start_ms);

// Call once per main-loop iteration. Sends at most one queued byte without
// waiting and queues a fresh report every 1.5 s when the buffer is available.
void uart_report_update(uint32_t now, float roll, float pitch, float yaw,
                        float ax, float ay, float az, float positionx);

#endif
