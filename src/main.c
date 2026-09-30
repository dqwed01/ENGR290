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

#include "include/hovercraft.h"

int main(){
    hovercraft_init();
    // Add your code here and press Ctrl + Shift + B to build
    while(1) {
        //TODO
    }

    return 0;
}
