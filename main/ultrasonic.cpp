#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

#include "src/pins.h"
#include "src/util.h"
#include "src/ultrasonic.h"

void us_sensor_init(){
    cli();
    DDRB |= ((OUTPUT<<US1_TRIG)|(OUTPUT<<US2_TRIG)); //Set TRIG pin to Output
    DIGITAL_WRITE_LOW(PORTB, US1_TRIG);
    DIGITAL_WRITE_LOW(PORTB, US2_TRIG);
    DDRB &= ~((~INPUT)<<US1_ECHO); 
    DDRD &= ~((~INPUT)<<US2_ECHO);
    sei();
}

uint16_t us_read(uint8_t target_us){
    uint16_t distance_cm = 0;
    if(target_us == 1 || target_us == 2){
        //Setup Registers
        uint8_t us_echo = (target_us == 1) ? US1_ECHO : US2_ECHO;
        uint8_t us_trig = (target_us == 1) ? US1_TRIG : US1_TRIG;
        uint8_t us_echo_port = (target_us == 1) ? PINB : PIND;
        uint8_t us_trig_port = PORTB;
        
        //Precompute Timer Counter
        TIMER1_CNTR = TIMER1_MAX_TICK - TICKS_60MS_256_PRESCALE;
        uint16_t pre_stamp = TIMER1_CNTR;

        //Start Ranging
        DIGITAL_WRITE_HIGH(us_trig_port, us_trig);
        delay_micro(10);
        DIGITAL_WRITE_LOW(us_trig_port, us_trig);

        //Wait for Echo to go High
        do {} while (!(DIGITAL_READ(PINB, us_echo)))

        //Start counting and wait for Echo to go Low or until timeout is reached
        TIMER1_CTR_REG_B = 0x04; //Set Prescaler to 256
        do {} while (DIGITAL_READ(PINB, us_echo) || (TIMER1_INT_FLAG_REG & (1 << TOV1)) == 0)

        //Stop counting and check if distance was recorded
        TIMER1_CTR_REG_B = 0;
        if(DIGITAL_READ(TIMER1_INT_FLAG_REG, TOV1)){
            //Distance was not recorded
            DIGITAL_WRITE_HIGH(TIMER1_INT_FLAG_REG, TOV1); //Clear Timer Overflow flag
        }

        else{
            //Distance was found
            uint16_t time_micro = TIMER1_CNTR - TICKS_60MS_256_PRESCALE * US_PER_TICK_256_PRESCALE;
            distance_cm = (time_micro <=116) ? 2 : time_micro / (uint16_t)58; //Minimum distance is 2cm
        }
    }

    else{
        //Throw Error through UART
    }

    return distance_cm;
}