#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define LED_SALON 2
#define LED_EATING_ROOM 3
#define LED_BEDROOM 4
#define LED_BATHROOM 5
#define LED_KITCHEN 6

#define LIQUID_SENSOR A0
#define TEMP_HUMID_TYPE DHT22
#define TEMP_HUMID_SENSOR 22
#define VIBRATION_SENSOR 23

#define RFID_RST_PIN 49
#define RFID_SS_PIN 53

MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN);  // Create MFRC522 instance
byte rfidUid[4];

DHT_Unified dht(TEMP_HUMID_SENSOR, TEMP_HUMID_TYPE);
uint32_t dhtDelayMs;
uint32_t dhtLastMeasureMs;

void setupLeds();
void setupSensors();
void setupRfidReader();
void setupTemperatureAndHumiditySensor();
void setupLiquidSensor();

void readSensors();
void readLiquidLevel();
void readTemperatureAndHumidity();
void printHex(byte *buffer, byte bufferSize);

void readRfid();

void setupVibrationSensor();

void readVibration();

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        delay(10);
    }
    Serial.println(F("Setup START"));

    //setupLeds();
    setupSensors();
    //setupRfidReader();

    Serial.println(F("Setup STOP"));
}

void setupLeds() {
    pinMode(LED_SALON, OUTPUT);
    pinMode(LED_EATING_ROOM, OUTPUT);
    pinMode(LED_BEDROOM, OUTPUT);
    pinMode(LED_BATHROOM, OUTPUT);
    pinMode(LED_KITCHEN, OUTPUT);
}

void setupSensors() {
    //setupLiquidSensor();
    //setupTemperatureAndHumiditySensor();
    setupVibrationSensor();
}

void setupLiquidSensor() { pinMode(LIQUID_SENSOR, INPUT); }

void setupTemperatureAndHumiditySensor() {
    dht.begin();
    sensor_t sensor;
    dht.humidity().getSensor(&sensor);
    dhtDelayMs = sensor.min_delay / 1000;
}

void setupVibrationSensor() {
    pinMode(VIBRATION_SENSOR, INPUT);
}

void setupRfidReader() {
    SPI.begin();
    rfid.PCD_Init();
}

void loop() {
    readSensors();

    //readRfid();

    delay(1000);
}

void readSensors() {
    //readLiquidLevel();
    //readTemperatureAndHumidity();
    //readVibration();
}

void readLiquidLevel() {
    int liquidLevel = analogRead(LIQUID_SENSOR);

    Serial.print("liquid level=");
    Serial.println(liquidLevel);
}

void readTemperatureAndHumidity() {
    if ((millis() - dhtLastMeasureMs) < dhtDelayMs) {
        return;
    }

    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
        Serial.println("Error reading temperature!");
    }
    else {
        Serial.print("Temperature: ");
        Serial.print(event.temperature);
        Serial.println(" *C");
    }
    // Get humidity event and print its value.
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
        Serial.println("Error reading humidity!");
    }
    else {
        Serial.print("Humidity: ");
        Serial.print(event.relative_humidity);
        Serial.println("%");
    }

    dhtLastMeasureMs = millis();
}

void readVibration() {
    int vibrationDetected = digitalRead(VIBRATION_SENSOR);
    Serial.print("Vibration detected = ");
    Serial.println(vibrationDetected == 1);
}

void readRfid() {
    if ( ! rfid.PICC_IsNewCardPresent())
        return;

    if ( ! rfid.PICC_ReadCardSerial())
        return;

    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = MFRC522::PICC_GetType(rfid.uid.sak);
    Serial.println(MFRC522::PICC_GetTypeName(piccType));

    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
        piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
        piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        return;
    }

    if (rfid.uid.uidByte[0] != rfidUid[0] ||
        rfid.uid.uidByte[1] != rfidUid[1] ||
        rfid.uid.uidByte[2] != rfidUid[2] ||
        rfid.uid.uidByte[3] != rfidUid[3] ) {
        Serial.println(F("A new card has been detected."));

        for (byte i = 0; i < 4; i++) {
            rfidUid[i] = rfid.uid.uidByte[i];
        }

        Serial.println(F("The NUID tag is:"));
        Serial.print(F("In hex: "));
        printHex(rfid.uid.uidByte, rfid.uid.size);
        Serial.println();
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}

void printHex(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

