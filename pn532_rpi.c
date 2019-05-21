/**************************************************************************
 *  @file     pn532_rpi.c
 *  @author   Yehui from Waveshare
 *  @license  BSD
 *  
 *  This implements the peripheral interfaces.
 *  
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 **************************************************************************/

#include <stdio.h>
#include <time.h>
#include "wiringPi.h"
#include "wiringPiSPI.h"
#include "pn532_rpi.h"

#define _SPI_STATREAD                   0x02
#define _SPI_DATAWRITE                  0x01
#define _SPI_DATAREAD                   0x03
#define _SPI_READY                      0x01

#define _SPI_CHANNEL                    0



uint8_t reverse_bit(uint8_t num) {
    uint8_t result = 0;
    for (uint8_t i = 0; i < 8; i++) {
        result <<= 1;
        result += (num & 1);
        num >>= 1;
    }
    return result;
}

void rpi_spi_rw(uint8_t* data, uint8_t count) {
#ifndef _SPI_HARDWARE_LSB
    for (uint8_t i = 0; i < count; i++) {
        data[i] = reverse_bit(data[i]);
    }
    wiringPiSPIDataRW (_SPI_CHANNEL, data, count) ;
    for (uint8_t i = 0; i < count; i++) {
        data[i] = reverse_bit(data[i]);
    }
#else
    wiringPiSPIDataRW (_SPI_CHANNEL, data, count) ;
#endif
}

int PN532_RPi_Reset(void) {
    // TODO: in most cases, reset pin is no need for SPI
   return PN532_STATUS_OK;
}

int PN532_RPi_ReadData(uint8_t* data, uint16_t count) {
    uint8_t frame[count + 1];
    frame[0] = _SPI_DATAREAD;
    delay(20);
    rpi_spi_rw(frame, count + 1);
    for (uint8_t i = 0; i < count; i++) {
        data[i] = frame[i + 1];
    }
    return PN532_STATUS_OK;
}

int PN532_RPi_WriteData(uint8_t *data, uint16_t count) {
    uint8_t frame[count + 1];
    frame[0] = _SPI_DATAWRITE;
    for (uint8_t i = 0; i < count; i++) {
        frame[i + 1] = data[i];
    }
    rpi_spi_rw(frame, count + 1);
    return PN532_STATUS_OK;
}

bool PN532_RPi_WaitReady(uint32_t timeout) {
    uint8_t status[] = {_SPI_STATREAD, 0x00};
    struct timespec timenow;
    long tickstart = 0;
    long tickend  = 0;
    clock_gettime(CLOCK_MONOTONIC, &timenow);
    tickend = tickstart = timenow.tv_nsec;
    while (tickend - tickstart < timeout) {
        clock_gettime(CLOCK_MONOTONIC, &timenow);
        tickend = timenow.tv_nsec;
        delay(20);
        rpi_spi_rw(status, sizeof(status));
        if (status[1] == _SPI_READY) {
            return true;
        } else {
            delay(10);
        }
    }
    return false;
}

int PN532_RPi_Wakeup(void) {
    // Send any special commands/data to wake up PN532
    uint8_t data[] = {0x00};
    delay(1000);
    rpi_spi_rw(data, 1);
    delay(1000);
    return PN532_STATUS_OK;
}

void PN532_RPi_Log(const char* log) {
    printf(log);
    printf("\r\n");
}

void PN532_RPi_Init(PN532* pn532) {
    // init the pn532 functions
    pn532->reset =  PN532_RPi_Reset;
    pn532->read_data = PN532_RPi_ReadData;
    pn532->write_data = PN532_RPi_WriteData;
    pn532->wait_ready = PN532_RPi_WaitReady;
    pn532->wakeup = PN532_RPi_Wakeup;
    pn532->log = PN532_RPi_Log;
    // SPI setup
    if (wiringPiSetupGpio() < 0) {  // using Broadcom GPIO pin mapping
        return;
    }
    wiringPiSPISetup (_SPI_CHANNEL, 11520) ;
    // hardware wakeup
    pn532->wakeup();
}
