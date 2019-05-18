/**************************************************************************
 *  @file     pn532_impl.c
 *  @author   Yehui from Waveshare
 *  @license  BSD
 *  
 *  This implements the device interface, e.g. SPI, I2C, UART ...
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

#include "stm32f1xx_hal.h"
#include "main.h"
#include "pn532_impl.h"

#define _SPI_STATREAD                   0x02
#define _SPI_DATAWRITE                  0x01
#define _SPI_DATAREAD                   0x03
#define _SPI_READY                      0x01

extern SPI_HandleTypeDef hspi1;


void pn532_reset(void) {
    // TODO: in most cases, reset pin is no need for SPI
    /*
    HAL_GPIO_WritePin(RSTPD_N_GPIO_Port, RSTPD_N_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(RSTPD_N_GPIO_Port, RSTPD_N_Pin, GPIO_PIN_RESET);
    HAL_Delay(500);
    HAL_GPIO_WritePin(RSTPD_N_GPIO_Port, RSTPD_N_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
    */
}

void pn532_read_data(uint8_t* data, uint16_t count, uint32_t timeout) {
    HAL_GPIO_WritePin(SS_GPIO_Port, SS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Receive(&hspi1, data, count, timeout);
    HAL_GPIO_WritePin(SS_GPIO_Port, SS_Pin, GPIO_PIN_SET);
}

void pn532_write_data(uint8_t *data, uint16_t count, uint32_t timeout) {
    HAL_GPIO_WritePin(SS_GPIO_Port, SS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, data, count, timeout);
    HAL_GPIO_WritePin(SS_GPIO_Port, SS_Pin, GPIO_PIN_SET);
}

bool pn532_wait_ready(uint32_t timeout) {
    uint8_t status[] = {_SPI_STATREAD, 0x00};
    uint32_t tickstart = HAL_GetTick();
    while (HAL_GetTick() - tickstart < timeout) {
        HAL_Delay(20);
        HAL_SPI_Receive(&hspi1, status, sizeof(status), timeout);
        if (status[1] == 0x01) {
            return true;
        } else {
            HAL_Delay(10);
        }
    }
    return false;
}

void pn532_wakeup(void) {
    // Send any special commands/data to wake up PN532
    HAL_Delay(1000);
    HAL_SPI_Transmit(&hspi1, (uint8_t*){0x00}, 1, 0);
    HAL_Delay(1000);
}
