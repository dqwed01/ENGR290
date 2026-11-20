#include <stdint.h>

#include "src/hovercraft.h"
#include "src/pwm.h"
#include "src/servo.h"
#include "src/uart.h"

namespace {
constexpr uint8_t LIFT_STEP_PERCENT = 10;
constexpr int8_t SERVO_STEP_DEG = 10;
uint8_t lift_percent = 0;
int8_t servo_angle = 0;
}

void hovercraft_init() {
    lift_percent = 0;
    servo_angle = 0;
    pwm_init();
    servo_init();
    UART_init();
}

void hovercraft_update() {
    char command;
    if (uart_try_receive(&command)) {
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
                uart_poll();
                return;  // Ignore line endings and unsupported commands.
        }
        uart_report_controls(lift_percent, servo_angle);
    }
    uart_poll();
}
