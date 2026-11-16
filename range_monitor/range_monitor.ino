/*
 * Open this sketch for a classic Arduino Nano and set Serial Monitor to 115200.
 * Sharp GP2Y0A21 analog output: A0. This sketch expects an external AREF.
 * HC-SR04 trigger/echo: US1 = D12/D8, US2 = D11/D2. Use a common ground.
 */
#include <Arduino.h>
#include <math.h>

constexpr uint8_t US1_TRIGGER = 12;
constexpr uint8_t US1_ECHO = 8;
constexpr uint8_t US2_TRIGGER = 11;
constexpr uint8_t US2_ECHO = 2;
constexpr float IR_AREF_VOLTS = 2.55f;  // Match the measured external AREF.
constexpr unsigned long ECHO_TIMEOUT_US = 30000;
constexpr unsigned long PING_GAP_MS = 65;  // Allow echoes to fade between sensors.

unsigned long readEchoMicros(uint8_t triggerPin, uint8_t echoPin) {
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);
    return pulseInLong(echoPin, HIGH, ECHO_TIMEOUT_US);
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
    analogRead(A0);  // Apply the reference and discard the first conversion.

    pinMode(US1_TRIGGER, OUTPUT);
    pinMode(US2_TRIGGER, OUTPUT);
    digitalWrite(US1_TRIGGER, LOW);
    digitalWrite(US2_TRIGGER, LOW);
    pinMode(US1_ECHO, INPUT);
    pinMode(US2_ECHO, INPUT);
}

void loop() {
    const unsigned long us1 = readEchoMicros(US1_TRIGGER, US1_ECHO);
    delay(PING_GAP_MS);
    const unsigned long us2 = readEchoMicros(US2_TRIGGER, US2_ECHO);
    const int ir = analogRead(A0);

    Serial.print(F("IR_raw="));
    Serial.print(ir);
    Serial.print(F(" IR_cm="));
    printInfraredCm(ir);
    Serial.print(F(" US1_cm="));
    printUltrasonicCm(us1);
    Serial.print(F(" US2_cm="));
    printUltrasonicCm(us2);
    Serial.println();

    delay(PING_GAP_MS);
}
