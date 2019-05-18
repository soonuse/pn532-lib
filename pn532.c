/**************************************************************************
 *  @file     pn532.c
 *  @author   Yehui from Waveshare
 *  @license  BSD
 *  
 *  This is a library for the Waveshare PN532 NFC modules
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

#include "pn532.h"
#include "pn532_impl.h"

/* @brief: Initialize GPIO or other interfaces. Implemented by the user.
*/
__weak void pn532_init(PN532* pn532) {
    pn532->reset = pn532_reset;
    pn532->read_data = pn532_read_data;
    pn532->write_data = pn532_write_data;
    pn532->wait_ready = pn532_wait_ready;
    pn532->wakeup = pn532_wakeup;
}

/* @brief: Reset the module. Implemented by the user.
*/
__weak void pn532_reset(void) {
}

/* @brief: Read data from the module. Implemented by the user.
 * @param data: used to save the data read
 * @param count: length of one reading
 * @param timeout: system tick
*/
__weak void pn532_read_data(uint8_t* data, uint16_t count, uint32_t timeout) {
}

/* @brief: Write data to the module. Implemented by the user.
 * @param dev: user defined handler
 * @param data: data to be sent
 * @param count: length of one writing
 * @param timeout: system tick
*/
__weak void pn532_write_data(uint8_t *data, uint16_t count, uint32_t timeout) {
}

/* @brief: Wait until the module is ready. Implemented by the user.
 * @param timeout: system tick
 * @retval: true if ready else false
*/
__weak bool pn532_wait_ready(uint32_t timeout) {
    return true;
}

/* @brief: Wakeup the module. Implemented by the user.
*/
__weak void pn532_wakeup(void) {
}

/*
void pn532_write_frame(PN532* dev, uint8_t* data, uint16_t length) {
    if (length > 255) {
        return; // Data must be array of 1 to 255 bytes.
    }
    // Build frame to send as:
    // - Preamble (0x00)
    // - Start code  (0x00, 0xFF)
    // - Command length (1 byte)
    // - Command length checksum
    // - Command bytes
    // - Checksum
    // - Postamble (0x00)
    uint8_t frame[length + 8];
    uint8_t checksum = 0;
    // frame = bytearray(length+8)
    frame[0] = _PREAMBLE
    frame[1] = _STARTCODE1
    frame[2] = _STARTCODE2
    for (uint8_t i = 0; i < 3; i++) {
        checksum += frame[i];
    }
    // checksum = sum(frame[0:3])
    frame[3] = length & 0xFF
    frame[4] = (~length + 1) & 0xFF

    for (uint8_t i = 0; i < length; i++) {
        frame[5 + i] = data[i];
        checksum += data[i];
    }
    // frame[5:-2] = data
    // checksum += sum(data)
    frame[length - 2] = ~checksum & 0xFF;
    frame[length - 1] = _POSTAMBLE;
    // frame[-2] = ~checksum & 0xFF
    // frame[-1] = _POSTAMBLE
    // Send frame.
    // self._write_data(bytes(frame))
    pn532.write_data(dev, )
}
*/
