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
    uint8_t EMPTY_BLOCK[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    printf("Formatting, please wait\r\n");
    for (block_number = 63; block_number > 0; block_number--) {
        if (block_number % 4 == 3) {
            continue;
        }
        do {
            pn532_error = PN532_MifareClassicAuthenticateBlock(&pn532, uid,
                    uid_len, block_number, MIFARE_CMD_AUTH_A, key_a);
            if (pn532_error) {
                break;
            }
            pn532_error = PN532_MifareClassicWriteBlock(&pn532, EMPTY_BLOCK,
                    block_number);
            if (pn532_error) {
                break;
            }
            pn532_error = PN532_MifareClassicReadBlock(&pn532, buff, block_number);
            if (pn532_error) {
                break;
            }
            for (uint8_t i = 0; i < sizeof(EMPTY_BLOCK); i++) {
                if (EMPTY_BLOCK[i] != buff[i]) {
                    printf("Write block %d failed\r\n", block_number);
                    printf("Unknown Error\r\n");
                    break;
                }
            }
            printf(">");
            fflush(stdout);
        } while (0);
        if (pn532_error) {
            printf("Error: 0x%02x\r\n", pn532_error);
            printf("Write block %d failed\r\n", block_number);
        }
    }
    printf("Done\r\n");
}
