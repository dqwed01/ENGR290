/**
 * @file main.c
 * @author minht
 * @date 2026-09-28
 * @brief Main function
 */
#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

#include "src/hovercraft.h"
#include "src/util.h"
#include "src/timer.h"
#include "src/ultrasonic.h"

int main(){
    Serial.begin(9600);
    hovercraft_init();
    uint16_t distance_cm = 0;
    while(1) {
        distance_cm = us_read(1);
        Serial.print(distance_cm);
        Serial.println(" cm");
        delay_ms(500);
    }

    return 0;
}
