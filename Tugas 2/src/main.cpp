#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>
#include <SPI.h>
#include <w25q64.hpp>
#include <cstring>
#include <cstdlib>

#define COBA_SATU_ENABLE

void monitorLux(float lux);

#ifdef COBA_SATU_ENABLE
#define LED1_PIN 2
#define LED2_PIN 4
#define LED3_PIN 16
#define LED4_PIN 17
#define BUTTON_PIN 15

// Setup untuk sensor cahaya
BH1750 lightMeter(0x23);

// Setup untuk GPIO Intterrupt
bool changeAutoBrightness = false;
bool stateAutoBrightness = false;
portMUX_TYPE gpioIntMux = portMUX_INITIALIZER_UNLOCKED;

// setup untuk w25q64
unsigned char writeBuffer[256];
unsigned char readBuffer[256];
byte chipId[4] = "";

w25q64 spiChip;

// Deklarasi fungsi
const char* charPtrToString(unsigned char* data);

const char* boolToString(bool value);

bool stringToBool(const char* str);

void IRAM_ATTR gpioISR();

void setup()
{
    // Setup
    Serial.begin(9600);
    Wire.begin();
    spiChip.begin();


    // Inisialisasi pin LED sebagai OUTPUT
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    pinMode(LED3_PIN, OUTPUT);
    pinMode(LED4_PIN, OUTPUT);

    // untuk i2c
    lightMeter.begin();

    // interrupts
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(BUTTON_PIN, &gpioISR, FALLING);

    // untuk w25q64
    spiChip.getId(chipId);

    // Baca hasil penulisan data di W25Q64
    ESP_LOGI("SETUP W25Q64", "Start Reading...");
    spiChip.readPages(readBuffer, 0xFFFF, 1);
    ESP_LOGI("SETUP W25Q64", "Data read from chip: %s", readBuffer);
    stateAutoBrightness = stringToBool(charPtrToString(readBuffer));

}

void loop()
{
    if (changeAutoBrightness)
    {
        portENTER_CRITICAL(&gpioIntMux);
        changeAutoBrightness = false;
        portEXIT_CRITICAL(&gpioIntMux);

        // Ganti statenya
        stateAutoBrightness = !stateAutoBrightness;
        ESP_LOGI("Auto Brightness", "status: %s", boolToString(stateAutoBrightness));

        // Simpan perubahan ke SPI
        memcpy(writeBuffer, boolToString(stateAutoBrightness), 6);
        spiChip.erasePageSector(0xFFFF);
        spiChip.pageWrite(writeBuffer, 0xFFFF);
        ESP_LOGI("W25Q64", "Done writing");
        delay(1000);
    }


    float lux = lightMeter.readLightLevel();
    monitorLux(lux);


    if (stateAutoBrightness)
    {
        if (lux <= 250)
        {
            digitalWrite(LED1_PIN, HIGH);
            digitalWrite(LED2_PIN, HIGH);
            digitalWrite(LED3_PIN, HIGH);
            digitalWrite(LED4_PIN, HIGH);
        }
        else if (lux > 250 && lux <= 500)
        {
            digitalWrite(LED1_PIN, LOW);
            digitalWrite(LED2_PIN, HIGH);
            digitalWrite(LED3_PIN, HIGH);
            digitalWrite(LED4_PIN, HIGH);
        }
        else if (lux > 500 && lux <= 750)
        {
            digitalWrite(LED1_PIN, LOW);
            digitalWrite(LED2_PIN, LOW);
            digitalWrite(LED3_PIN, HIGH);
            digitalWrite(LED4_PIN, HIGH);
        }
        else if (lux > 750 && lux <= 1000)
        {
            digitalWrite(LED1_PIN, LOW);
            digitalWrite(LED2_PIN, LOW);
            digitalWrite(LED3_PIN, LOW);
            digitalWrite(LED4_PIN, HIGH);

        }
        else
        {
            digitalWrite(LED1_PIN, LOW);
            digitalWrite(LED2_PIN, LOW);
            digitalWrite(LED3_PIN, LOW);
            digitalWrite(LED4_PIN, LOW);
        }
    }
    else
    {
        digitalWrite(LED1_PIN, HIGH);
        digitalWrite(LED2_PIN, HIGH);
        digitalWrite(LED3_PIN, HIGH);
        digitalWrite(LED4_PIN, HIGH);
    }


    delay(1000); // Tunda pembacaan sensor untuk 1 detik
}

// void monitorLux(float lux) {
//     ESP_LOGI("NOTE", "lux: %f", lux);
// }

void IRAM_ATTR gpioISR() {
    portENTER_CRITICAL(&gpioIntMux);
    changeAutoBrightness = true;
    portEXIT_CRITICAL(&gpioIntMux);
}

const char* boolToString(bool value) {
    return value ? "true" : "false";
}

bool stringToBool(const char* str) {
    // Membandingkan string dengan "true"
    if (std::strcmp(str, "true") == 0) {
        return true;
    }
    // Membandingkan string dengan "false"
    else if (std::strcmp(str, "false") == 0) {
        return false;
    }
    // Jika string tidak cocok dengan "true" atau "false", Anda dapat menangani kesalahan atau mengembalikan nilai default
    else {
        // Di sini, saya mengembalikan nilai default, yaitu false
        return false;
    }
}

const char* charPtrToString(unsigned char* data) {
    return reinterpret_cast<const char*>(data);
}
#else
#include <Arduino.h>
#include <BH1750.h>
#include <Wire.h>

#define PWM_CHANNEL 0
#define PWM_FREQ 5000
#define PWM_RESOLUTION 8

BH1750 lightMeter;

#define led 4

int intensitas = 0;

void setup() {
    Serial.begin(9600);
    // Coba
    ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    // attach ke led
    ledcAttachPin(led, PWM_CHANNEL);
    Wire.begin();
    lightMeter.begin();
    Serial.println(F("BH1750 Test begin"));


}

void loop() {
    float lux = lightMeter.readLightLevel();
    intensitas = 255-(lux/10);

    monitorLux(lux);

     if (lux >2500) {
        ledcWrite(PWM_CHANNEL, 0);
        delay(5);
    } else {
        ledcWrite(PWM_CHANNEL, intensitas);
        delay(5);
    }
    delay(1000);


}
#endif

void monitorLux(float lux) {
    ESP_LOGI("NOTE", "lux: %f", lux);
}
