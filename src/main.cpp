#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define LED_SALON 2
#define LED_EATING_ROOM 3
#define LED_BEDROOM 4
#define LED_BATHROOM 5
#define LED_KITCHEN 6

#define LIQUID_SENSOR A0

#define RFID_RST_PIN 49
#define RFID_SS_PIN 53

MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN);  // Create MFRC522 instance
byte rfidUid[4];

void setupLeds();
void setupSensors();
void setupRfidReader();

void readSensors();
void readRfid();

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        delay(10);
    }
    Serial.println(F("Setup START"));

    setupLeds();
    setupSensors();
    setupRfidReader();

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
    pinMode(LIQUID_SENSOR, INPUT);
}

void printHex(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], DEC);
    }
}


void setupRfidReader() {
    SPI.begin();
    rfid.PCD_Init();
}

void loop() {
    //readSensors();

    readRfid();

    delay(1000);
}

void readSensors() {
    int liquidLevel = analogRead(LIQUID_SENSOR);

    Serial.print("liquid level=");
    Serial.println(liquidLevel);
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