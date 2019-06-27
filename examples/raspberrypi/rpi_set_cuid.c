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
    uint8_t cuid[4];
    if (argc == 1) {
        cuid[0] = 0x01;
        cuid[1] = 0x23;
        cuid[2] = 0x45;
        cuid[3] = 0x67;
    } else {
        for (uint8_t i = 0; i < 4; i++) {
            uint8_t s1 = argv[1][i * 2];
            uint8_t s2 = argv[1][i * 2 + 1];
            if (s1 >= 'A') {
                s1 = (s1 & 0xCF) - 'A' + 0x0A;
            } else {
                s1 -= '0';
            }
            if (s2 >= 'A') {
                s2 = (s2 & 0xCF) - 'A' + 0x0A;
            } else {
                s2 -= '0';
            }
            cuid[i] = s1 << 4 | s2;
        }
    }
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
    // Write block #0
    uint8_t block_number = 0;
    uint8_t xor = 0x00;
    uint8_t DATA[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x04, 0x00,
                      0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69};
    for (uint8_t i = 0; i < 4; i++) {
        xor ^= cuid[i];
        DATA[i] = cuid[i];
    }
    DATA[4] = xor;
    pn532_error = PN532_MifareClassicAuthenticateBlock(&pn532, uid, uid_len,
            block_number, MIFARE_CMD_AUTH_A, key_a);
    if (pn532_error) {
        printf("Error: 0x%02x\r\n", pn532_error);
        return -1;
    }
    pn532_error = PN532_MifareClassicWriteBlock(&pn532, DATA, block_number);
    if (pn532_error) {
        printf("Error: 0x%02x\r\n", pn532_error);
        return -1;
    }
    pn532_error = PN532_MifareClassicReadBlock(&pn532, buff, block_number);
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
    printf("Set CUID as %02X %02X %02X %02X\r\n",
            cuid[0], cuid[1], cuid[2], cuid[3]);
}
