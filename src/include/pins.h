#ifndef PINS_H
#define PINS_H

//Timer Pins
#define TIMER0_CNTR TCNT0
#define TIMER0_CTR_REG_A TCCR0A
#define TIMER0_CTR_REG_B TCCR0B 
#define TIMER0_INT_MASK TIMSK0
#define TIMER0_INT_FLAG_REG TIFR0
//PWM Pins
#define PWM0 PB1

//Ultrasonic Sensor Pins
#define US1_ECHO PB0
#define US1_TRIG PB5

#define US2_ECHO PD2
#define US2_TRIG PB3

//Servo Motor Pins
#define SERVO1_CTRL PB2

#endif