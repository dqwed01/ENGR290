/*
 * Open this sketch for a classic Arduino Nano and set Serial Monitor to 115200.
 * Sharp GP2Y0A21 on connector P5. This sketch expects an external AREF.
 * HC-SR04 on connector P6: pin 3 trigger, pin 2 echo. Use a common ground.
 */
#include <Arduino.h>
#include <math.h>

// The board is labeled ADC0/P5. The v3.10 pin table maps P5/1 to ADC6.
// Select A0 here if a wiring check identifies the input as PC0/ADC0.
constexpr uint8_t IR_INPUT = A6;
constexpr uint8_t US_TRIGGER = 11;  // P6/3, PB3
constexpr uint8_t US_ECHO = 2;      // P6/2, PD2
constexpr float IR_AREF_VOLTS = 2.55f;  // Match the measured external AREF.
constexpr unsigned long ECHO_TIMEOUT_US = 30000;
constexpr unsigned long PING_GAP_MS = 65;  // Allow echoes to fade before the next ping.

unsigned long readEchoMicros() {
    digitalWrite(US_TRIGGER, HIGH);
    delayMicroseconds(10);
    digitalWrite(US_TRIGGER, LOW);
    return pulseInLong(US_ECHO, HIGH, ECHO_TIMEOUT_US);
}

void printUltrasonicCm(unsigned long echoMicros) {
    if (echoMicros == 0) {
        Serial.print(F("no_echo"));
    } else if (echoMicros < 2UL * 58 || echoMicros > 400UL * 58) {
        Serial.print(F("out_of_range"));
    } else {
        Serial.print(echoMicros / 58.0f, 1);
    }
}

void printInfraredCm(int adc) {
    if (adc == 0 || adc == 1023) {
        Serial.print(F("out_of_range"));
        return;
    }
    // Same voltage-to-distance fit as the assignment's infrared driver.
    const float volts = adc * (IR_AREF_VOLTS / 1023.0f);
    const float cm = 29.988f * powf(volts, -1.173f);
    if (cm < 10.0f || cm > 80.0f) {
        Serial.print(F("out_of_range"));
    } else {
        Serial.print(cm, 1);
    }
}

void setup() {
    Serial.begin(115200);
    analogReference(EXTERNAL);
    analogRead(IR_INPUT);  // Apply the reference and discard the first conversion.

    pinMode(US_TRIGGER, OUTPUT);
    digitalWrite(US_TRIGGER, LOW);
    pinMode(US_ECHO, INPUT);
}

void loop() {
    const unsigned long echoMicros = readEchoMicros();
    const int ir = analogRead(IR_INPUT);

    Serial.print(F("IR_raw="));
    Serial.print(ir);
    Serial.print(F(" IR_cm="));
    printInfraredCm(ir);
    Serial.print(F(" US_cm="));
    printUltrasonicCm(echoMicros);
    Serial.println();

    delay(PING_GAP_MS);
}
