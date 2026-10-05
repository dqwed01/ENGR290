/**
 * @file main.c
 * @author minht
 * @date 2026-09-28
 * @brief Main function
 */
#define F_CPU 16000000UL

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

#include "src/hovercraft.h"
#include "src/util.h"
#include "src/timer.h"
#include "src/ultrasonic.h"
#include "src/infrared.h"

int main(){
    char message[64];
    hovercraft_init();
    uint16_t distance_cm = 0;
    uint32_t tick = 0;
    while(1) {
        distance_cm = us_read(1);
        sprintf(message, "%d cm ultrasonic\n", distance_cm);
        UART_transmit(message);

        // distance_cm = ifr_read();
        // sprintf(message, "%d cm infrared\n", distance_cm);
        // UART_transmit(message);

        if(distance_cm < 16 || distance_cm > 84){
            TCCR2B = (1 << CS22) | (1 << CS20); // prescaler 128, starts the timer
        }
        else{
          TCCR2B = 0;  
        }
        delay_ms(500);
    }

    return 0;
}
