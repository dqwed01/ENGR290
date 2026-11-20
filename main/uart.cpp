#include <avr/io.h>

#include "src/uart.h"

namespace {
const char startup_message[] =
    "Lift P4: 000% | Servo P9: +000deg\r\n"
    "+/- lift 10%, s stop; a/d servo 10deg, c center\r\n"
    "Auto-start 100% after 3s; any valid command cancels.\r\n";
char report[] = "Lift P4: 000% | Servo P9: +000deg\r\n";
constexpr uint8_t LIFT_VALUE_OFFSET = sizeof("Lift P4: ") - 1;
constexpr uint8_t SERVO_VALUE_OFFSET = sizeof("Lift P4: 000% | Servo P9: ") - 1;
const char* tx_message = startup_message;
uint8_t tx_position = 0;
bool report_pending = false;
uint8_t pending_lift = 0;
int8_t pending_servo = 0;
}

void UART_init() {
    tx_message = startup_message;
    tx_position = 0;
    report_pending = false;
    pending_lift = 0;
    pending_servo = 0;
    UCSR0A = (1 << U2X0);
    UBRR0H = static_cast<uint8_t>(UBRR_VAL >> 8);
    UBRR0L = static_cast<uint8_t>(UBRR_VAL);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void UART_transmit_char(unsigned char data) {
    while (!(UCSR0A & (1 << UDRE0))) {}
    UDR0 = data;
}

void UART_transmit(const char* message) {
    while (*message != '\0') UART_transmit_char(*message++);
}

unsigned char UART_receive() {
    while (!(UCSR0A & (1 << RXC0))) {}
    return UDR0;
}

bool uart_try_receive(char* command) {
    if (!(UCSR0A & (1 << RXC0))) return false;
    *command = UDR0;
    return true;
}

void uart_report_controls(uint8_t lift_percent, int8_t servo_angle) {
    pending_lift = lift_percent;
    pending_servo = servo_angle;
    report_pending = true;
}

void uart_poll() {
    if (report_pending && tx_message[tx_position] == '\0') {
        report[LIFT_VALUE_OFFSET] = '0' + pending_lift / 100;
        report[LIFT_VALUE_OFFSET + 1] = '0' + (pending_lift / 10) % 10;
        report[LIFT_VALUE_OFFSET + 2] = '0' + pending_lift % 10;
        report[SERVO_VALUE_OFFSET] = pending_servo < 0 ? '-' : '+';
        const uint8_t magnitude = pending_servo < 0 ? -pending_servo : pending_servo;
        report[SERVO_VALUE_OFFSET + 1] = '0' + magnitude / 100;
        report[SERVO_VALUE_OFFSET + 2] = '0' + (magnitude / 10) % 10;
        report[SERVO_VALUE_OFFSET + 3] = '0' + magnitude % 10;
        tx_message = report;
        tx_position = 0;
        report_pending = false;
    }
    if (tx_message[tx_position] != '\0' && (UCSR0A & (1 << UDRE0))) {
        UDR0 = tx_message[tx_position++];
    }
}
