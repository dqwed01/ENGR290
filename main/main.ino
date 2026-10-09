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
#include "src/pwm.h"

//#define US_TESTING

int main(){
    char message[64];
    hovercraft_init();
    float distance_cm = 0;
    //Serial.begin(9600);
    while(1) {
#ifdef US_TESTING
        distance_cm = us_read(2);
        sprintf(message, "%f cm ultrasonic\n", distance_cm);
        UART_transmit(message);
#else
        distance_cm = ifr_read();
        sprintf(message, "%d cm infrared\n", (uint16_t) distance_cm * 100);
        UART_transmit(message);
        //Serial.println(distance_cm);
#endif

        if(distance_cm < 16 || distance_cm > 49){
            TIMSK1 |= (1 << OCIE1A);                 // LED blinks
        }
        else{
            TIMSK1 &= ~(1 << OCIE1A);                // LED stops
            PORTB &= ~(1 << PB5);                    // and turns off
        }

        int pwm_duty = 0;
        if(distance_cm < 16){
            pwm_duty = 100;
        }
        else if (distance_cm < 49){
            pwm_duty = ((49-distance_cm)*100)/33;
        }
        pwm_set_duty(pwm_duty);
        delay_ms(500);
    }

    return 0;
}
