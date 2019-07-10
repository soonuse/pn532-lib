/**
  * This example shows connecting to the PN532 and writing an NTAG215
  * type RFID tag
  */
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "pn532.h"
#include "pn532_rpi.h"

int main(int argc, char** argv) {
    uint8_t buff[255];
    uint8_t uid[MIFARE_UID_MAX_LENGTH];
    uint32_t pn532_error = PN532_ERROR_NONE;
    int32_t uid_len = 0;
    printf("Hello!\r\n");
    PN532 pn532;
    PN532_SPI_Init(&pn532);
    //PN532_I2C_Init(&pn532);
    //PN532_UART_Init(&pn532);
    if (PN532_GetFirmwareVersion(&pn532, buff) == PN532_STATUS_OK) {
        printf("Found PN532 with firmware version: %d.%d\r\n", buff[1], buff[2]);
    } else {
        return -1;
    }
    PN532_SamConfiguration(&pn532);
    printf("Waiting for RFID/NFC card...\r\n");
    while (1) {
        // Check if a card is available to read
        uid_len = PN532_ReadPassiveTarget(&pn532, uid, PN532_MIFARE_ISO14443A, 1000);
        if (uid_len == PN532_STATUS_ERROR) {
            printf(".");
            fflush(stdout);
        } else {
            printf("Found card with UID: ");
            for (uint8_t i = 0; i < uid_len; i++) {
                printf("%02x ", uid[i]);
            }
            printf("\r\n");
            break;
        }
    }
    // Write block #6
    uint8_t block_number = 6;
    uint8_t DATA[] = {0x00, 0x01, 0x02, 0x03};
    pn532_error = PN532_Ntag2xxWriteBlock(&pn532, DATA, block_number);
    if (pn532_error) {
        printf("Error: 0x%02x\r\n", pn532_error);
        return -1;
    }
    pn532_error = PN532_Ntag2xxReadBlock(&pn532, buff, block_number);
    if (pn532_error) {
        printf("Error: 0x%02x\r\n", pn532_error);
        return -1;
    }
    for (uint8_t i = 0; i < sizeof(DATA); i++) {
        if (DATA[i] != buff[i]) {
            printf("Write block %d failed\r\n", block_number);
            return -1;
        }
    }
    printf("Write block %d successfully\r\n", block_number);
}
