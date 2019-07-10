#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "pn532.h"
#include "pn532_rpi.h"

int main(int argc, char** argv) {
    uint8_t buff[3];
    bool pin_state = false;
    PN532 pn532;
    //PN532_SPI_Init(&pn532);
    PN532_I2C_Init(&pn532);
    //PN532_UART_Init(&pn532);
    if (PN532_GetFirmwareVersion(&pn532, buff) != PN532_STATUS_OK) {
        printf("PN532 not found\r\n");
        return -1;
    }
    printf("Found PN532 with firmware version: %d.%d\r\n", buff[1], buff[2]);
    PN532_ReadGpio(&pn532, buff);
    printf("Port P3: 0x%02x\r\n", buff[0]);
    printf("Port P7: 0x%02x\r\n", buff[1]);
    printf("Port I: 0x%02x\r\n", buff[3]);
    for (uint8_t i = 30; i < 36; i++) {
        pin_state = PN532_ReadGpioP(&pn532, i);
        printf("Pin P%d: %d\r\n", i, pin_state);
    }
    for (uint8_t i = 0; i < 2; i++) {
        pin_state = PN532_ReadGpioI(&pn532, i);
        printf("Pin I%d: %d\r\n", i, pin_state);
    }
}
