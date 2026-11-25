#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#include "src/uart.h"

namespace {
constexpr uint32_t REPORT_PERIOD_MS = 250;
char tx_buffer[144];
uint8_t tx_length = 0;
uint8_t tx_position = 0;
uint32_t last_report_ms = 0;

// A signed 32-bit float needs up to 39 integer digits, a decimal point,
// two fractional digits, and a terminator when formatted with dtostrf().
constexpr size_t FLOAT_TEXT_CAPACITY = 44;

static void uart_tx_pump() {
    if (tx_position < tx_length && (UCSR0A & (1 << UDRE0))) {
        UDR0 = tx_buffer[tx_position++];
    }
}

static bool uart_queue_report(float roll, float pitch, float yaw,
                       float ax, float ay, float az, float ifr1, float ifr2, float us) {
    if (tx_position < tx_length) {
        return false;
    }

    char roll_text[FLOAT_TEXT_CAPACITY], pitch_text[FLOAT_TEXT_CAPACITY];
    char yaw_text[FLOAT_TEXT_CAPACITY], ax_text[FLOAT_TEXT_CAPACITY];
    char ay_text[FLOAT_TEXT_CAPACITY], az_text[FLOAT_TEXT_CAPACITY];
    char ifr1_text[FLOAT_TEXT_CAPACITY], ifr2_text[FLOAT_TEXT_CAPACITY];
    char us_text[FLOAT_TEXT_CAPACITY];
    dtostrf(roll, 1, 2, roll_text);
    dtostrf(pitch, 1, 2, pitch_text);
    dtostrf(yaw, 1, 2, yaw_text);
    dtostrf(ax, 1, 2, ax_text);
    dtostrf(ay, 1, 2, ay_text);
    dtostrf(az, 1, 2, az_text);
    dtostrf(ifr1, 1, 2, ifr1_text);
    dtostrf(ifr2, 1, 2, ifr2_text);
    dtostrf(us, 1, 2, us_text);

    int length = snprintf(tx_buffer, sizeof(tx_buffer),
                          "roll=%s pitch=%s yaw=%s deg | ax=%s ay=%s az=%s | \r\n ifr1=%s ifr2=%s us%s \r\n\n",
                          roll_text, pitch_text, yaw_text, ax_text, ay_text,
                          az_text, ifr1_text, ifr2_text, us_text);
    if (length < 0) {
        return false;
    }
    if (length >= static_cast<int>(sizeof(tx_buffer))) {
        length = sizeof(tx_buffer) - 1;
    }
    tx_length = static_cast<uint8_t>(length);
    tx_position = 0;
    return true;
}
}

void UART_init() {
    tx_length = 0;
    tx_position = 0;
    UCSR0A = 0;  // Normal speed, matching the divisor in UBRR_VAL.
    UBRR0H = static_cast<uint8_t>(UBRR_VAL >> 8);
    UBRR0L = static_cast<uint8_t>(UBRR_VAL);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // 8 data bits, no parity, 1 stop bit.
}

void UART_transmit_char(unsigned char data) {
    while (!(UCSR0A & (1 << UDRE0))) {}
    UDR0 = data;
}

void UART_transmit(const char* message) {
    for (const char* current = message; *current != '\0'; ++current) {
        UART_transmit_char(*current);
    }
}

unsigned char UART_receive() {
    while (!(UCSR0A & (1 << RXC0))) {}
    return UDR0;
}

void uart_report_init(uint32_t start_ms) {
    last_report_ms = start_ms;
}

void uart_report_update(uint32_t now, float roll, float pitch, float yaw,
                        float ax, float ay, float az, float ifr1, float ifr2, float us) {
    uart_tx_pump();
    if (static_cast<uint32_t>(now - last_report_ms) >= REPORT_PERIOD_MS &&
        uart_queue_report(roll, pitch, yaw, ax, ay, az, ifr1, ifr2, us)) {
        last_report_ms += REPORT_PERIOD_MS;
    }
}
