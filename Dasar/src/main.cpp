#include <Arduino.h>
#include "praktikal1.h"


void setup() {
    setupLEDControl();
}

void loop() {
    if (Serial.available()) {
        LEDControl(Serial.read());
    }
}
