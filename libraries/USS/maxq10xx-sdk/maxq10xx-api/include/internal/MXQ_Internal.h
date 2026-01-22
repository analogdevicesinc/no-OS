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
 * @file MXQ_Internal.h
 * @date Apr, May 2015
 * @authors Stéphane Di Vito, Maxim Integrated Products Inc.
 * @authors Benjamin Vinot, Maxim Integrated Products Inc.
 *
 * @internal
 */

#ifndef __MXQ_INTERNAL_H_INCLUDED__
#define __MXQ_INTERNAL_H_INCLUDED__

#include "MXQ_Config.h"
#include "MXQ_Types.h"
#include "MXQ_Error.h"
#include "MXQ_API.h"
#include "MXQ_CMD.h"
#include "MXQ_Transport.h"
#include "MXQ_Host.h"
#include "MXQ_Print.h"
#include "MXQ_GCU.h"


/**
 * Waits for the given number of microseconds.
 *
 * @param usec Specifies the waiting time in microseconds
 *
 * @ingroup Platform
 * @note This function must be implemented by the API user.
 */
void HOST_wait_usec(mxq_u4 usec);

/**
 * Computes a AES CBC-MAC.
 * @internal
 * @ingroup HostCrypto
 */
mxq_err_t HOST_aes_cbc_mac(
	    		    mxq_u1* pkey, mxq_length keylen,
					mxq_u1* pIV,
					mxq_u1* pdata,
					mxq_u1* pmac_out,
					mxq_length nblocks
					);

/**
 * Encrypts/decrypts using AES.
 * @internal
 * @ingroup HostCrypto
 */
#define MXQ_AES_ENCRYPT 1
#define MXQ_AES_DECRYPT 0

mxq_err_t HOST_aes_cbc(
	    		    mxq_u1* pkey, mxq_length keylen,
					mxq_u1* pIV,
					mxq_u1  op,
					mxq_u1* pdata, /* in */
					mxq_u1* poutput, /* out */
					mxq_length nblocks
					);



/**
 * Returns the CRC value of the provided data.
 * @internal
 * @param data
 * @param length
 * @return
 * @ingroup HostCrypto
 */
mxq_u2 HOST_CRCGet(mxq_pu1 data, mxq_length length);


#define CONFIG_MAX_PING_SIZE			2048
#define CONFIG_MAX_GCU_SEND_CMD_TRY		128

#define DIRECTION_IN 							POLARSSL_ENCRYPT /* input direction for maxq. */
#define DIRECTION_OUT							POLARSSL_DECRYPT

#if !defined(USE_PROTOCOL_SPI) && !defined(USE_PROTOCOL_I2C) && !defined(USE_PROTOCOL_PIPE) && !defined(USE_PROTOCOL_MAILBOX)
 #error Please select at least one communication protocol (define one of USE_PROTOCOL_SPI or USE_PROTOCOL_I2C).
#endif

#if defined(USE_PROTOCOL_SPI) &&  (defined(USE_PROTOCOL_I2C) || defined(USE_PROTOCOL_PIPE) || defined(USE_PROTOCOL_MAILBOX))
 #error Multiple communication interfaces selected (define one of USE_PROTOCOL_SPI or USE_PROTOCOL_I2C).
#endif

#if defined(USE_PROTOCOL_I2C) && (defined(USE_PROTOCOL_SPI) || defined(USE_PROTOCOL_PIPE) || defined(USE_PROTOCOL_MAILBOX))
 #error Multiple communication interfaces selected (define one of USE_PROTOCOL_SPI or USE_PROTOCOL_I2C).
#endif

#if defined(USE_PROTOCOL_PIPE) && (defined(USE_PROTOCOL_SPI) || defined(USE_PROTOCOL_I2C) || defined(USE_PROTOCOL_MAILBOX))
 #error Multiple communication interfaces selected (define one of USE_PROTOCOL_SPI or USE_PROTOCOL_I2C).
#endif

#if defined(USE_PROTOCOL_MAILBOX) && (defined(USE_PROTOCOL_SPI) || defined(USE_PROTOCOL_I2C) || defined(USE_PROTOCOL_PIPE))
 #error Multiple communication interfaces selected (define one of USE_PROTOCOL_SPI or USE_PROTOCOL_I2C).
#endif

#endif /* __MXQ_INTERNAL_H_INCLUDED__ */
