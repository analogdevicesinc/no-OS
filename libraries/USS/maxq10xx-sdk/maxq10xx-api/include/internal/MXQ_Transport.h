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
 * @internal
 * @file MXQ_Transport.h
 * @date Apr 2014
 * @authors Stéphane Di Vito, Maxim Integrated Products Inc.
 * @authors Benjamin Vinot, Maxim Integrated Products Inc.
 *
 * @ingroup Transport
 */


#ifndef __MXQ_TRANSPORT_H_INCLUDED__
#define __MXQ_TRANSPORT_H_INCLUDED__


/**
 * @internal
 * Configure the host system for using the MAXQ10xx
 * (e.g configuration of I/Os, SPI or I2C interface, allocation of buffers, etc).
 *
 * @return Status of execution
 * @ingroup Transport
 */
mxq_err_t init_interface(void);

/**
 * @internal
 * Close the communication interface
 *
 * @return Status of execution
 * @ingroup Transport
 */
mxq_err_t deinit_interface(void);


/**
 * @internal
 * Sends bytes to the MAXQ10xx security module over the selected host interface.
 *
 * @param buf buffer that contains the bytes to transmit toe the MAXQ10xx
 * @param len number of bytes to transmit.
 *
 * @return Status of execution
 * @return Number of received bytes of error status if <0
 * @retval if >0 number of received bytes
 * @retval if <=0 error (timeout or other host side hardware/driver error)

 * @ingroup Transport
 */
mxq_err_t sendbytes(const mxq_u1* buf, mxq_length len);

/**
 * @internal
 * Receives bytes from the MAXQ10xx security module over the currently selected host interface.
 *
 * @param buf        destination buffer for the bytes received from the MAXQ10xx
 * @param len        maximum number of bytes to receive
 * @param timeout_ms wait for bytes for timeout_ms milliseconds
 * @return Number of received bytes of error status if <0
 * @retval if >0 number of received bytes
 * @retval if <=0 error (timeout, bad CRC or wrong block format or other host side hardware/driver error)
 *
 * @ingroup Transport
 */
mxq_err_t receivebytes_first(mxq_u1* buf, mxq_length len);

/**
 * @internal
 * Receives bytes from the MAXQ10xx security module over the currently selected host interface.
 *
 * @param buf        destination buffer for the bytes received from the MAXQ10xx
 * @param len        maximum number of bytes to receive
 * @param timeout_ms wait for bytes for timeout_ms milliseconds
 * @return Number of received bytes of error status if <0
 * @retval if >0 number of received bytes
 * @retval if <=0 error (timeout, bad CRC or wrong block format or other host side hardware/driver error)
 *
 * @ingroup Transport
 */
mxq_err_t exchangebytes(mxq_u1* bufsrc, mxq_length len, mxq_u1* bufdst);

/**
 * @internal
 * Receives bytes from the MAXQ10xx security module over the currently selected host interface.
 *
 * @param buf        destination buffer for the bytes received from the MAXQ10xxs
 * @param len        maximum number of bytes to receive
 *
 * @return Number of received bytes of error status if <0
 * @retval if >0 number of received bytes
 * @retval if <=0 error (timeout, bad CRC or wrong block format or other host side hardware/driver error)
 *
 * @ingroup Transport
 */
mxq_err_t receivebytes(mxq_u1* buf, mxq_length len);

#endif /*  __MXQ_TRANSPORT_H_INCLUDED__ */
