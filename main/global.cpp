#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <math.h>

#include "src/global.h"

volatile bool servo_move = false;