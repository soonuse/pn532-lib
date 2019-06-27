#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "pn532.h"
#include "pn532_rpi.h"

int main(int argc, char** argv) {
    uint8_t buff[255];
    uint8_t uid[MIFARE_UID_MAX_LENGTH];
    uint8_t key_a[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint32_t pn532_error = PN532_ERROR_NONE;
    int32_t uid_len = 0;
    printf("Hello!\r\n");
    PN532 pn532;
    PN532_UART_Init(&pn532);
    if (PN532_GetFirmwareVersion(&pn532, buff) == PN532_STATUS_OK) {
        printf("Found PN532 with firmware version: %d.%d\r\n", buff[1], buff[2]);
    } else {
        return -1;
    }
    PN532_SamConfiguration(&pn532);
    printf("Waiting for RFID/NFC card...\r\n");
    while (1)
    {
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
    /**
      * Warning: DO NOT write the blocks of 4N+3 (3, 7, 11, ..., 63)
      * or else you will change the password for blocks 4N ~ 4N+2.
      * Note: 
      * 1.  The first 6 bytes (KEY A) of the 4N+3 blocks are always shown as 0x00,
      * since 'KEY A' is unreadable. In contrast, the last 6 bytes (KEY B) of the 
      * 4N+3 blocks are readable.
      * 2.  Block 0 is unwritable. 
      */
    uint8_t block_number = 63;
    uint8_t FIRST_BLOCK[] = {0x37, 0xf9, 0x20, 0x69, 0x87, 0x08, 0x04, 0x00,
                            0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69};
    uint8_t EMPTY_BLOCK[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    for (block_number = 63; block_number > 0; block_number--) {
        if (block_number % 4 == 3) {
            continue;
        }
        pn532_error = PN532_MifareClassicAuthenticateBlock(&pn532, uid, uid_len,
                block_number, MIFARE_CMD_AUTH_A, key_a);
        if (pn532_error) {
            printf("Error: 0x%02x\r\n", pn532_error);
            printf("Write block %d failed\r\n", block_number);
            continue;
        }
        pn532_error = PN532_MifareClassicWriteBlock(&pn532, EMPTY_BLOCK, block_number);
        if (pn532_error) {
            printf("Error: 0x%02x\r\n", pn532_error);
            printf("Write block %d failed\r\n", block_number);
            continue;
        }
        pn532_error = PN532_MifareClassicReadBlock(&pn532, buff, block_number);
        if (pn532_error) {
            printf("Error: 0x%02x\r\n", pn532_error);
            printf("Write block %d failed\r\n", block_number);
            continue;
        }
        for (uint8_t i = 0; i < sizeof(EMPTY_BLOCK); i++) {
            if (EMPTY_BLOCK[i] != buff[i]) {
                printf("Write block %d failed\r\n", block_number);
                break;
            }
        }
        printf(".");
        fflush(stdout);
    }
    do {
        pn532_error = PN532_MifareClassicAuthenticateBlock(&pn532, uid, uid_len,
                block_number, MIFARE_CMD_AUTH_A, key_a);
        if (pn532_error) {
            printf("Error: 0x%02x\r\n", pn532_error);
            printf("Write block %d failed\r\n", block_number);
            break;
        }
        pn532_error = PN532_MifareClassicWriteBlock(&pn532, FIRST_BLOCK, block_number);
        if (pn532_error) {
            printf("Error: 0x%02x\r\n", pn532_error);
            printf("Write block %d failed\r\n", block_number);
            break;
        }
        pn532_error = PN532_MifareClassicReadBlock(&pn532, buff, block_number);
        if (pn532_error) {
            printf("Error: 0x%02x\r\n", pn532_error);
            printf("Write block %d failed\r\n", block_number);
            break;
        }
        for (uint8_t i = 0; i < sizeof(FIRST_BLOCK); i++) {
            if (FIRST_BLOCK[i] != buff[i]) {
                printf("Write block %d failed\r\n", block_number);
                break;
            }
        }
        printf(".");
        fflush(stdout);
    } while(0);
    printf("Done\r\n");
}
