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

#include <string.h>
#include <stdio.h>
#include "pn532.h"

const uint8_t PN532_ACK[] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};
const uint8_t PN532_FRAME_START[] = {0x00, 0x00, 0xFF};

int PN532_WriteFrame(PN532* pn532, uint8_t* data, uint16_t length) {
    if (length > 255) {
        return PN532_STATUS_ERROR; // Data must be array of 1 to 255 bytes.
    }
    // Build frame to send as:
    // - Preamble (0x00)
    // - Start code  (0x00, 0xFF)
    // - Command length (1 byte)
    // - Command length checksum
    // - Command bytes
    // - Checksum
    // - Postamble (0x00)
    uint8_t frame[length + 7];
    uint8_t checksum = 0;
    frame[0] = PN532_PREAMBLE;
    frame[1] = PN532_STARTCODE1;
    frame[2] = PN532_STARTCODE2;
    for (uint8_t i = 0; i < 3; i++) {
        checksum += frame[i];
    }
    frame[3] = length & 0xFF;
    frame[4] = (~length + 1) & 0xFF;
    for (uint8_t i = 0; i < length; i++) {
        frame[5 + i] = data[i];
        checksum += data[i];
    }
    frame[length + 5] = ~checksum & 0xFF;
    frame[length + 6] = PN532_POSTAMBLE;
    pn532->write_data(frame, length + 7);
    return PN532_STATUS_OK;
}

/* @brief: Read a response frame from the PN532 of at most length bytes in size.
 *    Returns the data inside the frame if found, otherwise raises an exception
 *    if there is an error parsing the frame.  Note that less than length bytes
 *    might be returned!
*/
int PN532_ReadFrame(PN532* pn532, uint8_t* response, uint16_t length) {
    uint8_t buff[255];
    uint8_t checksum = 0;
    // Read frame with expected length of data.
    pn532->read_data(buff, length + 8);
    // Swallow all the 0x00 values that preceed 0xFF.
    uint8_t offset = 0;
    while (buff[offset] == 0x00) {
        offset += 1;
        if (offset >= length + 8){
            printf("Response frame preamble does not contain 0x00FF!\r\n");
            return PN532_STATUS_ERROR;
        }
    }
    if (buff[offset] != 0xFF) {
        printf("Response frame preamble does not contain 0x00FF!\r\n");
        return PN532_STATUS_ERROR;
    }
    offset += 1;
    if (offset >= length + 8) {
        printf("Response contains no data!");
        return PN532_STATUS_ERROR;
    }
    // Check length & length checksum match.
    uint8_t frame_len = buff[offset];
    if (((frame_len + buff[offset+1]) & 0xFF) != 0) {
        printf("Response length checksum did not match length!\r\n");
        return PN532_STATUS_ERROR;
    }
    // Check frame checksum value matches bytes.
    // uint8_t checksum = sum(buff[offset+2:offset+2+frame_len+1]) & 0xFF;
    for (uint8_t i = 0; i < frame_len + 1; i++) {
        checksum += buff[offset + 2 + i];
    }
    checksum &= 0xFF;
    if (checksum != 0) {
        printf("Response checksum did not match expected checksum\r\n");
        return PN532_STATUS_ERROR;
    }
    // Return frame data.
    // return response[offset+2:offset+2+frame_len]
    for (uint8_t i = 0; i < frame_len; i++) {
        response[i] = buff[offset + 2 + i];
    }
    return PN532_STATUS_OK;
}

/* @brief: Send specified command to the PN532 and expect up to response_length
 *     bytes back in a response.  Note that less than the expected bytes might
 *     be returned!  Params can optionally specify an array of bytes to send as
 *     parameters to the function call.  Will wait up to timeout seconds
 *     for a response and return a bytearray of response bytes, or None if no
 *     response is available within the timeout.
 */
int PN532_CallFunction(PN532* pn532, uint8_t command, uint8_t* response, uint16_t response_length, uint8_t* params, uint16_t params_length, uint32_t timeout) {
    // Build frame data with command and parameters.
    uint8_t buff[255];
    uint8_t data[params_length + 2];
    data[0] = PN532_HOSTTOPN532;
    data[1] = command & 0xFF;
    // for i, val in enumerate(params):
    //     data[2+i] = val
    for (uint8_t i = 0; i < params_length; i++) {
        data[2 + i] = params[i];
    }
    // Send frame and wait for response.
    if (PN532_WriteFrame(pn532, data, params_length + 2) != PN532_STATUS_OK) {
        pn532->wakeup();
        printf("Trying to wakeup\r\n");
        return PN532_STATUS_ERROR;
    }
    if (!pn532->wait_ready(timeout)) {
        return PN532_STATUS_ERROR;
    }
    // Verify ACK response and wait to be ready for function response.
    pn532->read_data(buff, sizeof(PN532_ACK));
    if (memcmp(PN532_ACK, buff, sizeof(PN532_ACK)) != 0) {
        printf("Did not receive expected ACK from PN532!\r\n");
        return PN532_STATUS_ERROR;
    }
    if (!pn532->wait_ready(timeout)) {
        return PN532_STATUS_ERROR;
    }
    // Read response bytes.
    // response = self._read_frame(response_length+2)
    PN532_ReadFrame(pn532, buff, response_length + 2);

    // Check that response is for the called function.
    if (! ((buff[0] == PN532_PN532TOHOST) && (buff[1] == (command+1)))) {
        printf("Received unexpected command response!\r\n");
        return PN532_STATUS_ERROR;
    }
    // Return response data.
    for (uint8_t i = 0; i < response_length; i++) {
        response[i] = buff[i + 2];
    }
    return PN532_STATUS_OK;
}

/* @brief: Call PN532 GetFirmwareVersion function and return a buff with the IC,
 *  Ver, Rev, and Support values.
 */
int PN532_GetFirmwareVersion(PN532* pn532, uint8_t* version) {
    if (PN532_CallFunction(pn532, PN532_COMMAND_GETFIRMWAREVERSION, version, 4, NULL, 0, 500) != PN532_STATUS_OK) {
        printf("Failed to detect the PN532");
        return PN532_STATUS_ERROR;
    }
    return PN532_STATUS_OK;
}

/* @brief: Configure the PN532 to read MiFare cards.
*/
int PN532_SamConfiguration(PN532* pn532) {
    // Send SAM configuration command with configuration for:
    // - 0x01, normal mode
    // - 0x14, timeout 50ms * 20 = 1 second
    // - 0x01, use IRQ pin
    // Note that no other verification is necessary as call_function will
    // check the command was executed as expected.
    uint8_t params[] = {0x01, 0x14, 0x01};
    PN532_CallFunction(pn532, PN532_COMMAND_SAMCONFIGURATION, NULL, 0, params, sizeof(params), 1000);
    return PN532_STATUS_OK;
}

/* @brief: Wait for a MiFare card to be available and return its UID when found.
 *     Will wait up to timeout seconds and return None if no card is found,
 *     otherwise a bytearray with the UID of the found card is returned.
 * @retval: Length of UID, or -1 if ERROR
 */
int PN532_ReadPassiveTarget(PN532* pn532, uint8_t* response, uint8_t card_baud, uint32_t timeout) {
    // Send passive read command for 1 card.  Expect at most a 7 byte UUID.
    uint8_t params[] = {0x01, card_baud};
    uint8_t buff[19];
    int status = PN532_STATUS_OK;
    status = PN532_CallFunction(pn532, PN532_COMMAND_INLISTPASSIVETARGET, buff, sizeof(buff), params, sizeof(params), timeout);
    if (status != PN532_STATUS_OK) {
        return PN532_STATUS_ERROR; // No card found
    }
    // Check only 1 card with up to a 7 byte UID is present.
    if (buff[0] != 0x01) {
        printf("More than one card detected!\r\n");
        return PN532_STATUS_ERROR;
    }
    if (buff[5] > 7) {
        printf("Found card with unexpectedly long UID!\r\n");
        return PN532_STATUS_ERROR;
    }
    for (uint8_t i = 0; i < buff[5]; i++) {
        response[i] = buff[6 + i];
    }
    return buff[5];
}
