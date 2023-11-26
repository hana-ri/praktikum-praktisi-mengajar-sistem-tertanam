#include <Arduino.h>
#include <Preferences.h>
#include "esp_log.h"

#define STORAGE_NAME "storage"
#define LED_STATUS_KEY "led"
#define SSID_KEY "ssid"
#define SSID_PASSWORD_KEY "password"
#define BUTTON_PIN 18

bool ledStatus = LOW;
bool changLedStatus = false;

portMUX_TYPE gpioIntMux = portMUX_INITIALIZER_UNLOCKED;

Preferences storage;
String input, ssid, password;

void IRAM_ATTR gpioISR();
bool endsWithChar(String str, char suffix);

void setup() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(BUILTIN_LED, OUTPUT);
    attachInterrupt(BUTTON_PIN, &gpioISR, FALLING);


    Serial.begin(9600);
    // Initialize Preferences and read LED status
    storage.begin(STORAGE_NAME);
    delay(100);
    ledStatus = storage.getBool(LED_STATUS_KEY);
    digitalWrite(BUILTIN_LED, ledStatus);
    storage.end();

    // Print current SSID
    storage.begin(STORAGE_NAME);
    ssid = storage.getString(SSID_KEY);
    password = storage.getString(SSID_PASSWORD_KEY);
    ESP_LOGI("SETUP", "SSID name: %s", ssid);
    ESP_LOGI("SETUP", "SSID password: %s", password);
    password.clear();
    ssid.clear();
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

    if (Serial.available()) {
        input = Serial.readString();

        ESP_LOGI("UART RECEIVE", "input: %s", input);

        int firstSemicolon = input.indexOf(';');

        if (firstSemicolon != -1) {
            // Extract the substring before the first semicolon and store it in ssid
            ssid = input.substring(0, firstSemicolon);
            storage.begin(STORAGE_NAME);
            storage.putString(SSID_KEY, ssid);
            storage.end();
            ESP_LOGI("Storage", "saved SSID: %s", ssid);

            // Find the position of the second semicolon
            int secondSemicolon = input.indexOf(';', firstSemicolon + 1);

            // Check if the second semicolon is found
            if (secondSemicolon != -1) {
                // Extract the substring between the first and second semicolons and store it in y
                password = input.substring(firstSemicolon + 1, secondSemicolon);
                storage.begin(STORAGE_NAME);
                storage.putString(SSID_PASSWORD_KEY, password);
                storage.end();
                ESP_LOGI("Storage", "saved password: %s", password);
            }
        }
        input.clear();
    }

}

void IRAM_ATTR gpioISR() {
    portENTER_CRITICAL(&gpioIntMux);
    changLedStatus = true;
    portEXIT_CRITICAL(&gpioIntMux);
}
