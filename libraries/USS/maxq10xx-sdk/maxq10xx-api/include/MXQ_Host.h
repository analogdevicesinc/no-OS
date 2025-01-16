/*******************************************************************************
* Copyright (C) 2018 Maxim Integrated Products, Inc. (now owned by Analog Devices, Inc.),
* 2024 Analog Devices, Inc. This software is proprietary and
* confidential to Analog Devices, Inc. and its licensors. , All rights Reserved.
*
* This software is protected by copyright laws of the United States and
* of foreign countries. This material may also be protected by patent laws
* and technology transfer regulations of the United States and of foreign
* countries. This software is furnished under a license agreement and/or a
* nondisclosure agreement and may only be used or reproduced in accordance
* with the terms of those agreements. Dissemination of this information to
* any party or parties not specified in the license agreement and/or
* nondisclosure agreement is expressly prohibited.
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
******************************************************************************/


/**
 *
 * @file
 * @date Apr, May 2015
 * @authors Stéphane Di Vito, Maxim Integrated Products Inc.
 * @authors Benjamin Vinot, Maxim Integrated Products Inc.
 *
 * @ingroup Platform
 * @internal
 */

#ifndef __MXQ_HOST_H_INCLUDED__
#define __MXQ_HOST_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes the host platform SPI interface for communication with the MAXQ106x.
 * In case GPIO must be driven by the HOST, this function shall also intialize them:
 *  - RESET pin as output, set to high (if connected)
 *  - WAKEUP pin as output, set to high (if connected)
 *  - WDI pin as output, set to high (if connected)
 *  - CMS pin as output, set to high (I2C) or low (SPI). This pin does not need to be dynamically driven. 
 *    It is usually hard wired to a state corresponding to the protocol in use.
 *  - MODE pin as output, set to high (if connected)
 *
 * @return Status of execution
 *
 * @retval 0 OK
 * @retval if <=0, error
 *
 * @note This function must be implemented by the API user.
 * @ingroup Platform
 */
mxq_err_t HOST_init_spi(void);

/**
 * Initializes the host platform I2C interface for communication with the MAXQ106x.
 * In case GPIO must be driven by the HOST, this function shall also intialize them.
 *
 * @return Status of execution
 * @retval 0 OK
 * @retval if <=0, error
 *
 * @note This function must be implemented by the API user.
 * @ingroup Platform
 */
mxq_err_t HOST_init_i2c(void);

/**
 * Reserved for special use
 * @internal
 * @return
 */
mxq_err_t HOST_init_pipe(void);

/**
 * Reserved for special use
 * @internal
 *
 * @return
 */
mxq_err_t HOST_init_serial(void);

/** 
 * Mailbox implementation
 * @internal 
 * 
 * @return 
 */
mxq_err_t HOST_init_mailbox(void);

/**
 * Sends bytes to the MAXQ106x over the host SPI interface (when available).
 *
 * Refer to the user guide for a description of the communication protocol.
 *
 * @param src source buffer containing bytes to transmit
 * @param len number of bytes to transmit
 *
 * @return Status of execution
 * @retval 0 OK
 * @retval if <=0, error
 *
 * @note This function must be implemented by the API user.
 * @ingroup Platform
 */
mxq_err_t HOST_send_bytes_spi(const mxq_u1* src, mxq_length len);

/**
 * Exchange bytes with the MAXQ106x over the host SPI interface (when available).
 *
 * This is used for the AES-SPI mode only.
 *
 * Refer to the user guide for a description of the communication protocol.
 *
 * @param src    source buffer containing bytes to transmit
 * @param len    number of bytes to transmit
 * @param dest   destination buffer for the received bytes
 *
 * @return Status of execution
 * @retval 0 OK
 * @retval if <=0, error
 *
 * @note This function must be implemented by the API user.
 * @ingroup Platform
 */
mxq_err_t HOST_exchange_bytes_spi(const mxq_u1* src, mxq_length len, mxq_u1* dest);

/**
 * Receives bytes from the MAXQ106x over the host SPI interface.
 *
 * The host shall send arbitrary bytes over MOSI in order to get back bytes over MISO.
 *
 * A timeout can be implemented (see MAXQ106x User guide for command timings)
 *
 * Refer to the MAXQ106x User Guide for a description of the communication protocol.
 *
 * @param dest       destination buffer for the received bytes
 * @param len        maximum number of bytes to receive
 *
 * @return number of bytes received or error
 * @retval if >0, number of bytes received
 * @retval if <=0, error
 *
 * @note This function must be implemented by the API user.
 * @ingroup Platform
 */
mxq_err_t HOST_receive_bytes_spi(mxq_u1* dest, mxq_length len);

/**
 * Waits for the MAXQ106x response over SPI by polling, then receives more bytes.
 *
 * The host shall send arbitrary bytes over MOSI in order to get back bytes over MISO.
 *
 * When the MAXQ106x replies with 0x55, then read more bytes.
 *
 * Receives bytes from the MAXQ106x over the host I2C interface (when available).
 *
 * Process:
 *     1. Exchange 1 byte over SPI: transmit a polling byte (any value) to the MAXQ106x SPI slave
 *        and simultanesouly receive 1 byte
 *     2. If the received byte is not 0x55, wait for a while, then go to 1.
 *     3. If 0x55 is received: continue receiving more bytes (up to `len`) by sending dummy bytes
 * 
 * A timeout can be implemented (see MAXQ106x User guide for command timings).
 *
 * Refer to the MAXQ106x User Guide for a description of the communication protocol.
 *
 * @param dest       destination buffer for the received bytes
 * @param len        maximum number of bytes to receive
 *
 * @return number of bytes received or error
 * @retval if >0, number of bytes received
 * @retval if <=0, error
 *
 * @note This function must be implemented by the API user.
 * @ingroup Platform
 */
mxq_err_t HOST_receive_bytes_spi_first(mxq_u1* dest, mxq_length len);


/**
 * Sends bytes to the MAXQ106x over the host I2C interface using I2C write.
 *
 * Refer to the MAXQ106x User Guide for a description of the communication protocol.
 *
 * @param src source buffer containing bytes to transmit
 * @param len number of bytes to transmit
 *
 * @return Status of execution
 * @retval 0 OK
 * @retval if <=0, error
 *
 * @note This function must be implemented by the API user.
 * @ingroup Platform
 */
mxq_err_t HOST_send_bytes_i2c(const mxq_u1* src, mxq_u2 len);

/**
 * Receives bytes from the MAXQ106x over I2C, using I2C read.
 *
 * A timeout can be implemented (see MAXQ106x User guide for command timings)
 *
 * Refer to the MAXQ106x User Guide for a description of the communication protocol.
 *
 * @param dest destination buffer for the received bytes
 * @param len  maximum number of bytes to receive
 *
 * @return number of bytes received or error
 * @retval if >0, number of bytes received
 * @retval if <=0, error
 *
 * @note This function must be implemented by the API user.
 * @ingroup Platform
 */
mxq_err_t HOST_receive_bytes_i2c(mxq_u1* dest, mxq_u2 len);

/**
 * Waits for the MAXQ106x response over I2C by polling, then receives more bytes.
 *
 * Process:
 *     1. Send a polling byte (any value) using I2C write 
 *     2. Read 1 byte using I2C read
 *     3. If byte is not 0x55, wait for a while, then go to 1.
 *     4. if 0x55 is received: continue receiving more bytes (up to `len`) using I2C read
 * 
 * A timeout can be implemented (see MAXQ106x User guide for command timings)
 *
 * Refer to the MAXQ106x User Guide for a description of the communication protocol.
 *
 * @param dest       destination buffer for the received bytes
 * @param len        maximum number of bytes to receive
 *
 * @return number of bytes received or error
 * @retval if >0, number of bytes received
 * @retval if <=0, error
 *
 * @note This function must be implemented by the API user.
 * @ingroup Platform
 */
mxq_err_t HOST_receive_bytes_i2c_first(mxq_u1* dest, mxq_u2 len);


/**
 * Reserved for special use.
 * @internal
 *
 * @param src
 * @param len
 * @return
 */
mxq_err_t HOST_send_bytes_serial(const mxq_u1* src, mxq_u2 len);

/**
 * Reserved for special use.
 * @internal
 * @param dest
 * @param len
 * @return
 */
mxq_err_t HOST_receive_bytes_serial_first(mxq_u1* dest, mxq_u2 len);
/**
 * Reserved for special use.
 * @internal
 * @param dest
 * @param len
 * @return
 */
mxq_err_t HOST_receive_bytes_serial(mxq_u1* dest, mxq_u2 len);

/**
 * Reserved for special use.
 * @internal
 * @param src
 * @param len
 * @return
 */
mxq_err_t HOST_send_bytes_pipe(const mxq_u1* src, mxq_length len);

/**
 * Reserved for special use.
 * @internal
 * @param dest
 * @param len
 * @return
 */
mxq_err_t HOST_receive_bytes_pipe_first(mxq_u1* dest, mxq_length len);

/**
 * Mailbox implementation 
 * @internal
 * @param src
 * @param len
 * @return
 */
mxq_err_t HOST_send_bytes_mailbox(const mxq_u1* src, mxq_u2 len);

/**
 * Mailbox implementation
 * @internal
 * @param dest
 * @param len
 * @return
 */
mxq_err_t HOST_receive_bytes_mailbox_first(mxq_u1* dest, mxq_u2 len);
/**
 * Mailbox implementation
 * @internal
 * @param dest
 * @param len
 * @return
 */
mxq_err_t HOST_receive_bytes_mailbox(mxq_u1* dest, mxq_u2 len);

/**
 * Reserved for special use.
 * @internal
 * @param dest
 * @param len
 * @return
 */
mxq_err_t HOST_receive_bytes_pipe(mxq_u1* dest, mxq_length len);

/**
 * Deasserts the PW_DOWN pin of the MAXQ1065 to enter sleep mode.
 *
 * Sequence is:
 *   1. Deassert the WAKEUP pin (0)
 *
 * @return Status of execution
 * @retval MXQ_OK Success
 *
 * @ingroup Platform
 *
 * @note This function may be implemented by the API user.
 */
mxq_err_t HOST_sleep_maxq10xx(void);

/**
 * Asserts the WAKEUP pin of the MAXQ106x to resume from sleep mode.
 *
 * Sequence is:
 *   1. Assert the WAKEUP pin (0)
 *   2. Hold WAKEUP pin for 100uS
 *   3. De-assert the WAKEUP pin (1)
 *
 * @return Status of execution
 * @retval MXQ_OK Success
 *
 * @ingroup Platform
 *
 * @note This function may be implemented by the API user.
 */
mxq_err_t HOST_wakeup_maxq106x(void);

/**
 * Triggers the RESET pin of the MAXQ106x.
 *
 * Sequence is:
 *   1. Assert the RESET pin (0)
 *   2. Hold RESET pin for 100uS
 *   3. De-assert the RESET pin (1)
 *
 * @note Host side context (authentication state, secure channel, ...) must also be reset.
 * 
 * @return Status of execution
 * @retval MXQ_OK Success
 *
 * @ingroup Platform
 * @note This function may be implemented by the API user.
 */
mxq_err_t HOST_reset_maxq106x(void);

/**
 * Toggles the WDI pin to reset the MAXQ106x watchdog counter.
 *
 * The MAXQ106x watchdog is disabled by default. It can be enabled using MWX_SetConfig().
 *
 * Sequence is:
 *   1. Assert the WDI pin (0)
 *   2. De-assert the WDI pin (1)
 *
 * @return Status of execution
 * @retval MXQ_OK Success
 *
 * @ingroup Platform
 * @note This function may be implemented by the API user.
 */
mxq_err_t HOST_toggle_wdi(void);

/**
 * Activate Crypto toolbox Mode by setting the MODE pin to 0.
 * This also performs a reset of the MAXQ106x through the RESET pin.
 *
 * Sequence is:
 *   1. Set MODE pin to 0
 *   2. Assert the RESET pin (0)
 *   3. Hold RESET pin for 100uS
 *   4. De-assert the RESET pin (1)
 *
 * @note Host side context (authentication state, secure channel, ...) must also be reset.
 *
 * @return Status of execution
 * @retval MXQ_OK Success
 *
 * @ingroup Platform
 * @note This function must be implemented by the API user.
 */
mxq_err_t HOST_activate_tls(void);
mxq_err_t HOST_follow_tls(void);

/**
 * Activate AES SPI Mode by setting the MODE pin to 1.
 * This also performs a reset of the MAXQ106x through the RESET pin.
 *
 * Sequence is:
 *   1. Set MODE pin to 1
 *   2. Assert the RESET pin (0)
 *   3. Hold RESET pin for 100uS
 *   4. De-assert the RESET pin (1)
 *
 * @note Host side context (authentication state, secure channel, ...) must also be reset.
 *
 * @return Status of execution
 * @retval MXQ_OK Success
 *
 * @ingroup Platform
 * @note This function must be implemented by the API user.
 */
mxq_err_t HOST_activate_gcu(void);
mxq_err_t HOST_follow_gcu(void);
void* HOST_get_interface(void);
void HOST_set_interface(void* i);
mxq_err_t HOST_deinit_interface(void);

#ifdef __cplusplus
}
#endif

#endif /* __MXQ_HOST_H_INCLUDED__ */
