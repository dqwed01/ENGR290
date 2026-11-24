#ifndef ULTRASONIC_H
#define ULTRASONIC_H

// Farthest distance we care about (bar distance + margin). Keeps us_read()
// from blocking for the sensor's full ~4 m range. Must be <= 1129.
#define US_MAX_CM       100
#define US_TIMEOUT_US   (US_MAX_CM * 58U)   // echo pulse is ~58 µs per cm (U: int is 16-bit on AVR)

// us_read() return values that aren't distances
#define US_NO_READING   (-1.0f)             // sensor busy or not responding
#define US_OUT_OF_RANGE ((float)US_MAX_CM + 1.0f) // nothing within US_MAX_CM

void us_sensor_init();
float us_read();

#endif