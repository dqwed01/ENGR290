#include <stdint.h>
#include <avr/interrupt.h>

#include "src/hovercraft.h"
#include "src/pwm.h"
#include "src/servo.h"
#include "src/timer.h"
#include "src/uart.h"

namespace {
constexpr uint8_t LIFT_STEP_PERCENT = 10;
constexpr int8_t SERVO_STEP_DEG = 10;
constexpr uint32_t AUTOSTART_DELAY_MS = 3000;
uint8_t lift_percent = 0;
int8_t servo_angle = 0;
uint32_t startup_ms = 0;
bool autostart_pending = false;
}

void hovercraft_init() {
    cli();
    lift_percent = 0;
    servo_angle = 0;
    pwm_init();
    servo_init();
    UART_init();
    timer_clock_init();
    startup_ms = timer_millis();
    autostart_pending = true;
    sei();
}

void hovercraft_update() {
    char command;
    // Process waiting commands before checking the startup deadline.
    while (uart_try_receive(&command)) {
        bool command_handled = true;
        switch (command) {
            case '+':
                if (lift_percent < 100) lift_percent += LIFT_STEP_PERCENT;
                pwm_set_duty(lift_percent);
                break;
            case '-':
                if (lift_percent > 0) lift_percent -= LIFT_STEP_PERCENT;
                pwm_set_duty(lift_percent);
                break;
            case 's':
            case 'S':
                lift_percent = 0;
                pwm_set_duty(lift_percent);
                break;
            case 'a':
                if (servo_angle > SERVO_MIN_ANGLE_DEG) servo_angle -= SERVO_STEP_DEG;
                servo_set_angle(servo_angle);
                break;
            case 'd':
                if (servo_angle < SERVO_MAX_ANGLE_DEG) servo_angle += SERVO_STEP_DEG;
                servo_set_angle(servo_angle);
                break;
            case 'c':
                servo_angle = 0;
                servo_set_angle(servo_angle);
                break;
            default:
                command_handled = false;  // Ignore line endings and unsupported commands.
                break;
        }
        if (command_handled) {
            autostart_pending = false;
            uart_report_controls(lift_percent, servo_angle);
        }
    }
    if (autostart_pending &&
        static_cast<uint32_t>(timer_millis() - startup_ms) >= AUTOSTART_DELAY_MS) {
        autostart_pending = false;
        lift_percent = 100;
        pwm_set_duty(lift_percent);
        uart_report_controls(lift_percent, servo_angle);
    }
    uart_poll();
}
