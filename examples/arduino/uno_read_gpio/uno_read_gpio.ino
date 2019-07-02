#include <stdbool.h>
#include "pn532.h"
#include "pn532_uno.h"

uint8_t buff[255];
uint32_t pn532_error = PN532_ERROR_NONE;
bool pin_state = false;
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
  } else {
    return;
  }
  PN532_ReadGpio(&pn532, buff);
  Serial.print("Port P3: 0x");
  if (buff[0] <= 0xF) {
    Serial.print("0");
  }
  Serial.println(buff[0], HEX);
  Serial.print("Port P7: 0x");
  if (buff[1] <= 0xF) {
    Serial.print("0");
  }
  Serial.println(buff[1], HEX);
  Serial.print("Port I: 0x");
  if (buff[3] <= 0xF) {
    Serial.print("0");
  }
  Serial.println(buff[3], HEX);
  for (uint8_t i = 30; i < 36; i++) {
    pin_state = PN532_ReadGpioP(&pn532, i);
    Serial.print("Pin P");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(pin_state);
  }
  for (uint8_t i = 0; i < 2; i++) {
    pin_state = PN532_ReadGpioI(&pn532, i);
    Serial.print("Pin I");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(pin_state);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}
