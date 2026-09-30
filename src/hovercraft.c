#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

#include "include/hovercraft.h"
#include "include/pins.h"
#include "include/util.h"
#include "include/ultrasonic.h"


void hovercraft_init(){
    us_sensor_init();
}