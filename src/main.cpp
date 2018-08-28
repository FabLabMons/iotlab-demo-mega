#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

static const uint8_t ledLiving1 = 2;
static const uint8_t ledLiving2 = 3;
static const uint8_t ledBedroom = 4;
static const uint8_t ledBathroom = 5;
static const uint8_t ledKitchen = 6;

static const int liquidSensor = A0;

static const uint8_t rfidRST = 22;
static const uint8_t rfidSS = 23;

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

MFRC522 rfid(SS_PIN, RST_PIN);  // Create MFRC522 instance
byte nuidPICC[4];

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
    pinMode(ledLiving1, OUTPUT);
    pinMode(ledLiving2, OUTPUT);
    pinMode(ledBedroom, OUTPUT);
    pinMode(ledBathroom, OUTPUT);
    pinMode(ledKitchen, OUTPUT);
}

void setupSensors() {
    pinMode(liquidSensor, INPUT);
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
    int liquidLevel = analogRead(liquidSensor);

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

    if (rfid.uid.uidByte[0] != nuidPICC[0] ||
        rfid.uid.uidByte[1] != nuidPICC[1] ||
        rfid.uid.uidByte[2] != nuidPICC[2] ||
        rfid.uid.uidByte[3] != nuidPICC[3] ) {
        Serial.println(F("A new card has been detected."));

        for (byte i = 0; i < 4; i++) {
            nuidPICC[i] = rfid.uid.uidByte[i];
        }

        Serial.println(F("The NUID tag is:"));
        Serial.print(F("In hex: "));
        printHex(rfid.uid.uidByte, rfid.uid.size);
        Serial.println();
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}