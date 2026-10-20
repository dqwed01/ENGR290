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
#include "src/global.h"

void hovercraft_init(){
    cli();
    UART_init();
    pwm_init();
    timer_init();
    //us_sensor_init();
    ifr_sensor_init();
    sei();
    delay_ms(10);
}