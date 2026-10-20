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
#include "src/global.h"

//#define US_TESTING

int main(){
    char message[64];
    hovercraft_init();
    float distance_cm = 0;
    while(1) {
#ifdef US_TESTING
        distance_cm = us_read(2);
        sprintf(message, "%d cm ultrasonic\n", (uint16_t)(distance_cm * 100));
        UART_transmit(message);
#else
        distance_cm = ifr_read();
        sprintf(message, "%d cm infrared\n", (uint16_t) (distance_cm * 100));
        UART_transmit(message);
#endif

        if(distance_cm < 16 || distance_cm > 84){
            blink_enabled = 1;
        }
        else{
            blink_enabled = 0;
            DIGITAL_WRITE_LOW(PORTB, PB5);
        }

        int pwm_duty = 0;
        if(distance_cm < 16){
            pwm_duty = 100;
        }
        else if (distance_cm < 49){
            pwm_duty = ((49-((int)distance_cm))*100)/33;
        }
        sprintf(message, "PWM Duty: %d\n", pwm_duty);
        UART_transmit(message);
        pwm_set_duty(pwm_duty);
        delay_ms(100);
    }

    return 0;
}
