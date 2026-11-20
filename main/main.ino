/*
 * Lift fan test for the ENGR290 controller v3.11 (16 MHz ATmega328P).
 * P4 pin 1 = fan positive; P4 pin 2 = fan negative.
 * Serial Monitor: 115200 baud. '+' adds 10%, '-' subtracts 10%, 's' stops.
 */
#include <avr/io.h>

constexpr uint32_t UART_BAUD_RATE = 115200UL;
constexpr uint16_t UART_DIVISOR =
    (F_CPU + 4UL * UART_BAUD_RATE) / (8UL * UART_BAUD_RATE) - 1;
constexpr uint8_t DUTY_STEP_PERCENT = 10;

void uart_init() {
    UCSR0A = (1 << U2X0);  // Double speed gives a closer match to 115200 baud.
    UBRR0H = static_cast<uint8_t>(UART_DIVISOR >> 8);
    UBRR0L = static_cast<uint8_t>(UART_DIVISOR);
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // 8 data bits, no parity, 1 stop bit.
}

void lift_init() {
    TCCR0B = 0;                 // Stop Timer0 while configuring it.
    TCCR0A = 0;                 // Disconnect both PWM outputs.
    PORTD &= ~(1 << PD6);       // P4 control low = fan off.
    DDRD |= (1 << PD6);         // PD6/OC0A controls P4's power MOSFET.
    TIMSK0 = 0;                 // Hardware PWM needs no timer interrupts.
    TCNT0 = 0;
    OCR0A = 0;
    TCCR0A = (1 << WGM01) | (1 << WGM00);  // Fast PWM, TOP = 255.
    TCCR0B = (1 << CS01) | (1 << CS00);    // 16 MHz / 64 / 256 = 976.6 Hz.
}

void lift_set_duty(uint8_t percent) {
    if (percent == 0 || percent >= 100) {
        // Exact off/on levels avoid the timer's narrow pulse at zero duty.
        if (percent == 0) {
            PORTD &= ~(1 << PD6);
        } else {
            PORTD |= (1 << PD6);
        }
        TCCR0A &= ~((1 << COM0A1) | (1 << COM0A0));
    } else {
        // 1..99% uses non-inverting hardware PWM on OC0A.
        OCR0A = (static_cast<uint16_t>(percent) * 256UL + 50UL) / 100UL - 1;
        TCCR0A |= (1 << COM0A1);
    }
}

int main() {
    lift_init();
    uart_init();
    uint8_t duty_percent = 0;
    char report[] = "Lift P4: 000%\r\n";
    const char* tx_message = "Lift P4: 000%\r\nCommands: +10%, -10%, s=stop\r\n";
    uint8_t tx_position = 0;
    bool report_pending = false;

    while (1) {
        // Handle commands before serial output so a stop is applied promptly.
        if (UCSR0A & (1 << RXC0)) {
            const char command = UDR0;
            switch (command) {
                case '+':
                    if (duty_percent < 100) duty_percent += DUTY_STEP_PERCENT;
                    break;
                case '-':
                    if (duty_percent > 0) duty_percent -= DUTY_STEP_PERCENT;
                    break;
                case 's':
                case 'S':
                    duty_percent = 0;
                    break;
                default:
                    continue;  // Ignore line endings and unrecognized characters.
            }
            lift_set_duty(duty_percent);
            report_pending = true;
        }

        if (report_pending && tx_message[tx_position] == '\0') {
            // Reuse the buffer after its previous line is sent. Report the latest setting.
            report[9] = '0' + duty_percent / 100;
            report[10] = '0' + (duty_percent / 10) % 10;
            report[11] = '0' + duty_percent % 10;
            tx_message = report;
            tx_position = 0;
            report_pending = false;
        }

        // Send at most one byte per pass, keeping reception responsive.
        if (tx_message[tx_position] != '\0' && (UCSR0A & (1 << UDRE0))) {
            UDR0 = tx_message[tx_position++];
        }
    }
}
