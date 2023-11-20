#include <Arduino.h>
#include <Preferences.h>

#define STORAGE_NAME "storage"
#define LED_STATUS_KEY "led"
#define BUTTON_PIN 18

bool ledStatus = LOW;
bool changLedStatus = false;

portMUX_TYPE gpioIntMux = portMUX_INITIALIZER_UNLOCKED;

Preferences storage;

void IRAM_ATTR gpioISR();

void setup() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(BUILTIN_LED, OUTPUT);
    attachInterrupt(BUTTON_PIN, &gpioISR, FALLING);

    storage.begin(STORAGE_NAME);
    delay(100);
    ledStatus = storage.getBool(LED_STATUS_KEY);
    digitalWrite(BUILTIN_LED, ledStatus);
    storage.end();
}

void loop() {
    if(changLedStatus) {
        portENTER_CRITICAL(&gpioIntMux);
        changLedStatus = false;
        portEXIT_CRITICAL(&gpioIntMux);

        ledStatus = !ledStatus;
        digitalWrite(BUILTIN_LED, ledStatus);

        storage.begin(STORAGE_NAME);
        storage.putBool(LED_STATUS_KEY, ledStatus);
        storage.end();
    }
}

void IRAM_ATTR gpioISR() {
    portENTER_CRITICAL(&gpioIntMux);
    changLedStatus = true;
    portEXIT_CRITICAL(&gpioIntMux);
}
