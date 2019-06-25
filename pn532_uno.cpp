/**************************************************************************
 *  @file     pn532_uno.c
 *  @author   Yehui from Waveshare
 *  @license  BSD
 *  
 *  This implements the peripheral interfaces.
 *  
 *  Check out the links above for our tutorials and wiring diagrams 
 *  These chips use SPI communicate.
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

#include <stdbool.h>
#include <stdint.h>

#include <SPI.h>
#include <Wire.h>

#include "pn532_uno.h"

#define PN532_SPI_STATREAD              (0x02)
#define PN532_SPI_DATAWRITE             (0x01)
#define PN532_SPI_DATAREAD              (0x03)
#define PN532_SPI_READY                 (0x01)

#define PN532_SPI_TIMEOUT               (10)
// This indicates if the bits read/write should be reversed
#define PN532_SPI_HARDWARE_LSB

#define PN532_NSS                       (4)
#define PN532_REQ                       (2)
#define PN532_RST                       (3)

#define PN532_I2C_READY                 (0x01)
#define PN532_I2C_ADDRESS               (0x48 >> 1)


/**************************************************************************
 * Reset and Log implements
 **************************************************************************/
int PN532_Reset(void) {
    digitalWrite(PN532_RST, HIGH);
    delay(100);
    digitalWrite(PN532_RST, LOW);
    delay(500);
    digitalWrite(PN532_RST, HIGH);
    delay(100);
    return PN532_STATUS_OK;
}

void PN532_Log(const char* log) {
    Serial.println(log);
}
/**************************************************************************
 * End: Reset and Log implements
 **************************************************************************/
/**************************************************************************
 * SPI
 **************************************************************************/
uint8_t reverse_bit(uint8_t num) {
    uint8_t result = 0;
    for (uint8_t i = 0; i < 8; i++) {
        result <<= 1;
        result += (num & 1);
        num >>= 1;
    }
    return result;
}

void spi_rw(uint8_t* data, uint8_t count) {
    digitalWrite(PN532_NSS, LOW);
    SPI.transfer(data, count);
    digitalWrite(PN532_NSS, HIGH);
}

int PN532_SPI_ReadData(uint8_t* data, uint16_t count) {
    uint8_t frame[count + 1];
    frame[0] = PN532_SPI_DATAREAD;
    delay(5);
    spi_rw(frame, count + 1);
    for (uint8_t i = 0; i < count; i++) {
        data[i] = frame[i + 1];
    }
    return PN532_STATUS_OK;
}

int PN532_SPI_WriteData(uint8_t* data, uint16_t count) {
    uint8_t frame[count + 1];
    frame[0] = PN532_SPI_DATAWRITE;
    for (uint8_t i = 0; i < count; i++) {
        frame[i + 1] = data[i];
    }
    spi_rw(frame, count + 1);
    return PN532_STATUS_OK;
}

bool PN532_SPI_WaitReady(uint32_t timeout) {
    uint8_t status[] = {PN532_SPI_STATREAD, 0x00};
    uint32_t tickstart = millis();
    while (millis() - tickstart < timeout) {
        delay(10);
        spi_rw(status, sizeof(status));
        if (status[1] == PN532_SPI_READY) {
            return true;
        } else {
            delay(5);
        }
    }
    return false;
}

int PN532_SPI_Wakeup(void) {
    // Send any special commands/data to wake up PN532
    uint8_t data[] = {0x00};
    delay(1000);
    digitalWrite(PN532_NSS, LOW);
    delay(2); // T_osc_start
    spi_rw(data, 1);
    delay(2); // T_osc_start
    digitalWrite(PN532_NSS, HIGH);
    delay(1000);
    return PN532_STATUS_OK;
}


void PN532_SPI_Init(PN532* pn532) {
    // init the pn532 functions
    pn532->reset =  PN532_Reset;
    pn532->read_data = PN532_SPI_ReadData;
    pn532->write_data = PN532_SPI_WriteData;
    pn532->wait_ready = PN532_SPI_WaitReady;
    pn532->wakeup = PN532_SPI_Wakeup;
    pn532->log = PN532_Log;
    Serial.begin(115200);
    pinMode(PN532_REQ, OUTPUT);
    pinMode(PN532_RST, OUTPUT);
    pinMode(PN532_NSS, OUTPUT);
    digitalWrite(PN532_REQ, HIGH);
    digitalWrite(PN532_RST, HIGH);
    digitalWrite(PN532_NSS, HIGH);
    SPI.begin();
    SPI.beginTransaction(SPISettings(10000, LSBFIRST, SPI_MODE0));
    // hardware wakeup
    pn532->wakeup();
}
/**************************************************************************
 * End: SPI
 **************************************************************************/
/**************************************************************************
 * I2C
 **************************************************************************/
void i2c_read(uint8_t* data, uint16_t count) {
    Wire.requestFrom(PN532_I2C_ADDRESS, count);
    for (uint16_t i = 0; i < count; i++) {
        data[i] = Wire.read();
    }
}

void i2c_write(uint8_t* data, uint16_t count) {
    Wire.beginTransmission(PN532_I2C_ADDRESS);
    Wire.write(data, count);
    Wire.endTransmission();
}

int PN532_I2C_ReadData(uint8_t* data, uint16_t count) {
    uint8_t status[] = {0x00};
    uint8_t frame[count + 1];
    i2c_read(status, sizeof(status));
    if (status[0] != PN532_I2C_READY) {
        return PN532_STATUS_ERROR;
    }
    i2c_read(frame, count + 1);
    for (uint8_t i = 0; i < count; i++) {
        data[i] = frame[i + 1];
    }
    return PN532_STATUS_OK;
}

int PN532_I2C_WriteData(uint8_t* data, uint16_t count) {
    i2c_write(data, count);
    return PN532_STATUS_OK;
}

bool PN532_I2C_WaitReady(uint32_t timeout) {
    uint8_t status[] = {0x00};
    uint32_t tickstart = millis();
    while (millis() - tickstart < timeout) {
        i2c_read(status, sizeof(status));
        if (status[0] == PN532_I2C_READY) {
            return true;
        } else {
            delay(5);
        }
    }
    return false;
}

int PN532_I2C_Wakeup(void) {
    digitalWrite(PN532_REQ, HIGH);
    delay(100);
    digitalWrite(PN532_REQ, LOW);
    delay(100);
    digitalWrite(PN532_REQ, HIGH);
    delay(500);
    return PN532_STATUS_OK;
}

void PN532_I2C_Init(PN532* pn532) {
    // init the pn532 functions
    pn532->reset = PN532_Reset;
    pn532->read_data = PN532_I2C_ReadData;
    pn532->write_data = PN532_I2C_WriteData;
    pn532->wait_ready = PN532_I2C_WaitReady;
    pn532->wakeup = PN532_I2C_Wakeup;
    pn532->log = PN532_Log;
    Serial.begin(115200);
    pinMode(PN532_REQ, OUTPUT);
    pinMode(PN532_RST, OUTPUT);
    Wire.begin();
    Wire.beginTransmission(PN532_I2C_ADDRESS);
    // hardware reset
    pn532->reset();
    // hardware wakeup
    pn532->wakeup();
}
/**************************************************************************
 * End: I2C
 **************************************************************************/