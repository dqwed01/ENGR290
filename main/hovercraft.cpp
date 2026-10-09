#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

#include "src/hovercraft.h"
#include "src/pins.h"
#include "src/util.h"
#include "src/ultrasonic.h"
#include "src/timer.h"
#include "src/infrared.h"
#include "src/pwm.h"

void hovercraft_init(){
    UART_init();
    pwm_init();
    timer_init();
    us_sensor_init();
    //ifr_sensor_init();
    delay_ms(10);
}