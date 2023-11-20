#include <Arduino.h>
#include "praktikal1.h"

#define PIN_4 4
#define PIN_16 16
#define PIN_17 17

void setupLEDControl() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PIN_4, OUTPUT);
    pinMode(PIN_16, OUTPUT);
    pinMode(PIN_17, OUTPUT);
}

void LEDControl(char serialInput) {
    switch (serialInput)
    {
        case '1':
            digitalWrite(LED_BUILTIN, HIGH);
            delay(2000);
            digitalWrite(LED_BUILTIN, LOW);
            break;

        case '2':
            digitalWrite(PIN_4, HIGH);
            delay(2000);
            digitalWrite(PIN_4, LOW);
            break;

        case '3':
            digitalWrite(PIN_16, HIGH);
            delay(2000);
            digitalWrite(PIN_16, LOW);
            break;

        case '4':
            digitalWrite(PIN_17, HIGH);
            delay(2000);
            digitalWrite(PIN_17, LOW);
            break;

        default:
            Serial.println("Perintah tidak dikenal!");
    }
}
