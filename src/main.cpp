#include <Arduino.h>

static const uint8_t ledLiving1 = 2;
static const uint8_t ledLiving2 = 3;
static const uint8_t ledBedroom = 4;
static const uint8_t ledBathroom = 5;
static const uint8_t ledKitchen = 6;

static const int liquidSensor = A0;

void setupLeds();
void setupSensors();
void readSensors();

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        delay(10);
    }
    Serial.println(F("Setup START"));

    setupLeds();
    setupSensors();

    Serial.println(F("Setup STOP"));
}

void setupLeds() {
    pinMode(ledLiving1, OUTPUT);
    pinMode(ledLiving2, OUTPUT);
    pinMode(ledBedroom, OUTPUT);
    pinMode(ledBathroom, OUTPUT);
    pinMode(ledKitchen, OUTPUT);
}

void setupSensors() {
    pinMode(liquidSensor, INPUT);
}

void loop() {
    readSensors();

    delay(1000);
}

void readSensors() {
    int liquidLevel = analogRead(liquidSensor);

    Serial.print("liquid level=");
    Serial.println(liquidLevel);
}