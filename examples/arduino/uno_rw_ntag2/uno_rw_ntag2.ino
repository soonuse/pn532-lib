#include "pn532.h"
#include "pn532_uno.h"

uint8_t buff[255];
uint8_t uid[MIFARE_UID_MAX_LENGTH];
int32_t uid_len = 0;
uint8_t key_a[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint32_t pn532_error = PN532_ERROR_NONE;

PN532 pn532;

void setup() {
  // put your setup code here, to run once:
  PN532_I2C_Init(&pn532);
  Serial.println("Hello!");
  if (PN532_GetFirmwareVersion(&pn532, buff) == PN532_STATUS_OK) {
    Serial.print("Found PN532 with firmware version: ");
    Serial.print(buff[1], DEC);
    Serial.print(".");
    Serial.println(buff[2], DEC);
    Serial.println("Waiting for RFID/NFC card...");
  } else {
    return;
  }
  PN532_SamConfiguration(&pn532);
  while (1)
  {
    // Check if a card is available to read
    uid_len = PN532_ReadPassiveTarget(&pn532, uid, PN532_MIFARE_ISO14443A, 1000);
    if (uid_len == PN532_STATUS_ERROR) {
      Serial.print(".");
    } else {
      Serial.print("Found card with UID: ");
      for (uint8_t i = 0; i < uid_len; i++) {
        if (uid[i] <= 0xF) {
          Serial.print("0");
        }
        Serial.print(uid[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
      break;
    }
  }
  // Write block #6
  uint8_t block_number = 6;
  uint8_t DATA[] = {0x00, 0x01, 0x02, 0x03};
  pn532_error = PN532_Ntag2xxWriteBlock(&pn532, DATA, block_number);
  if (pn532_error) {
    Serial.print("Error: 0x");
    Serial.print(pn532_error, HEX);
    return;
  }
  pn532_error = PN532_Ntag2xxReadBlock(&pn532, buff, block_number);
  if (pn532_error) {
    Serial.print("Error: 0x");
    Serial.print(pn532_error, HEX);
    return;
  }
  for (uint8_t i = 0; i < sizeof(DATA); i++) {
    if (DATA[i] != buff[i]) {
      Serial.print("Write block ");
      Serial.print(block_number, DEC);
      Serial.print(" failed\r\n");
      return;
    }
  }
  Serial.print("Write block ");
  Serial.print(block_number, DEC);
  Serial.print(" successfully\r\n");
}

void loop() {
  // put your main code here, to run repeatedly:
}
