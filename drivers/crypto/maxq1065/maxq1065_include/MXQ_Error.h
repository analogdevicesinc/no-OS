/*******************************************************************************
* Copyright (C) 2022 Maxim Integrated Products, Inc., All rights Reserved.
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
*******************************************************************************/


/**
 * @internal
 * @file MXQ_Error.h
 *
 * @ingroup API
 */

#ifndef MXQ_ERROR_H_
#define MXQ_ERROR_H_

#include "MXQ_Types.h"


/**
 * Status of execution.
 * @ingroup Types
 */
typedef enum _PACKED
{
	MXQ_OK									=  (int)0x000000,		/*!< Success, No Error */

	/* MAXQ10xx Error Codes - DO NOT USE AS RETURN CODES WITHIN API or APPLICATION*/
	MXQ_RSP_BAD_OBJ_ACCESS                  = (mxq_u2)-0x0800,

    MXQ_RSP_ID_ALREADY_EXISTS               = (mxq_u2)-0x0001,          /*!< ID already used */
    MXQ_RSP_BAD_PARAM                       = (mxq_u2)-0x0001,          /*!< Incorrect parameter */
    MXQ_RSP_FS_WRONG_PARAM_ACCESS_COND      = (mxq_u2)-0x0002,
 
	MXQ_RSP_FS_WRONG_OBJECT_TYPE			= (mxq_u2)-0x0003,			
	MXQ_RSP_NOT_FOUND						= (mxq_u2)-0x0004,			/*!< Object not found */
	MXQ_RSP_FS_OUT_OF_BOUNDS				= (mxq_u2)-0x0005,			/*!< Wrong offset or length */

	MXQ_RSP_ERROR_UNKNOWN					= (mxq_u2)-0x1000,			/*!< Unknown Error, or wrong length in parameters*/
	MXQ_RSP_WRONG_LENGTH					= (mxq_u2)-0x1000,			/*!< Unknown Error, or wrong length in parameters*/
	MXQ_RSP_MEMORY_FULL						= (mxq_u2)-0x1001,			/*!< Memory is Full */

	MXQ_RSP_OTHER_UNKNOWN_ERROR         	= (mxq_u2)-0x2000,			/*!< Security condition not satisfied, access denied */
	MXQ_RSP_NO_HASH_AVAIL					= (mxq_u2)-0x2001,			/*!< No previously calculated Hash Available */
	MXQ_RSP_ACCESS_DENIED					= (mxq_u2)-0x2001,			/*!< Access Denied */
	MXQ_RSP_WRONG_MAC						= (mxq_u2)-0x2002,			/*!< Wrong MAC */
	MXQ_RSP_NO_RANDOM_AVAIL					= (mxq_u2)-0x2002,			/*!< No previously generated random Available */

	MXQ_RSP_SIGNATURE_IS_WRONG				= (mxq_u2)-0x3000,			/*!< Signature is wrong / memory error */

	MXQ_RSP_SC_ERROR                        = (mxq_u2)-0x4000,          /* Secure Channel error */
 
	MXQ_RSP_ERR_NOT_IMPLEMENTED             = (mxq_u2)-0x5000,          /*!< Function not implemented */

	MXQ_RSP_NOT_ALLOWED                     = (mxq_u2)-0x6000,          /*!< Command not allowed in this context */
    MXQ_RSP_CIPHER_BAD_INPUT_DATA			= (mxq_u2)-0x6100,			/*!< Encoding Error in Server Parameters */

	MXQ_RSP_ERR_WRONG_MODE                  = (mxq_u2)-0x7000,          /*!< Wrong mode of operation (AES-SPI vs Crypto toolbox mode) */

	MXQ_RSP_CR_NOK                          = (mxq_u2)-0x8000,          /*!< Crypto operation failed */
	MXQ_RSP_BAD_KEY_TYPE					= (mxq_u2)-0x8001,			/*!< Unsupported key Type */
	MXQ_RSP_CRYPTO_FAILURE					= (mxq_u2)-0x8002,			/*!< Error during cryptographic operation (bad key length, bad sequence, or other error) */
	MXQ_RSP_ALGO_NOT_SUPPORTED				= (mxq_u2)-0x8004,			/*!< Algorithm not supported / mem alignment error or memory alignment error or memory access error*/
	MXQ_RSP_WRONG_PARAMS					= (mxq_u2)-0x8006,			/*!< Invalid Object ID */
    MXQ_RSP_CR_PADDING_ERROR                = (mxq_u2)-0x800A,          /*!< Padding error */
	MXQ_RSP_WRONG_SIZE         		    	= (mxq_u2)-0x800D,			/*!< Inconsistent message length (crypto) */
	MXQ_RSP_BUFFER_TOO_SMALL				= (mxq_u2)-0x8011,			/*!< Signature Length Invalid */
    MXQ_RSP_HASH_OUTPUT_LENGTH_TOO_SHORT    = (mxq_u2)-0x8012,          /*!< Invalid hash algo vs EC curve length */
	MXQ_RSP_CURVE_NOT_SUPPORTED     		= (mxq_u2)-0x8013,			/*!< ECC Curve not supported */
	MXQ_RSP_FS_NAME_ALREADY_EXISTS          = (mxq_u2)-0x8016,          /*!< Name already used */
 	MXQ_RSP_FS_INVALID_OBJECT               = (mxq_u2)-0x8017,
 	MXQ_RSP_COUNTER_OVERFLOW 				= (mxq_u2)-0x8030,			/*!< Counter reach its limit */

	MXQ_RSP_CONSISTENCY_ERROR				= (mxq_u2)-0x9000,			/*!< Bad format of certificate data */
	MXQ_RSP_OBJECT_EMPTY					= (mxq_u2)-0x9001,			/*!< The Object is empty */

 	MXQ_RSP_INTERNAL_BUFFER_OVERFLOW        = (mxq_u2)-0xA000,          /*!< Memory overflow issue */
	MXQ_RSP_BUFFER_OVERFLOW                 = (mxq_u2)-0xC000,          /*!< Memory overflow issue */

	MXQ_RSP_INVALID_SIZE					= (mxq_u2)-0xC030,			/*!< Invalid size ( too big ), wrong param */

 	MXQ_RSP_ACC_DENIED                      = (mxq_u2)-0xfffa,
	MXQ_RSP_WRONG_PARAM 					= (mxq_u2)-0xfffb,			/*!< Wrong command parameter */

	/* API Error Codes */
	MXQ_ERR_INTERNAL_ERROR					= (int)-0x010001,		/*!< Non specific internal error */
	MXQ_ERR_WRONG_PARAMETER					= (int)-0x010002,		/*!< Wrong parameter provided to the function (bad pointer...) */
	MXQ_ERR_BAD_LENGTH						= (int)-0x010010,		/*!< Bad Length */
	
	MXQ_ERR_TRANS_ERROR						= (int)-0x020C01,		/*!< Generic Transmission error */
	MXQ_ERR_TRANS_UNSUPPORTED_INTERFACE		= (int)-0x020C01,		/*!< Unsupported Interface (SPI,I2C) Interface not implemented */
	MXQ_ERR_TRANS_BUFFER_OVERFLOW			= (int)-0x020C02,		/*!< Buffer Overflow during reception*/
	MXQ_ERR_TRANS_NOT_ENOUGH_DATA			= (int)-0x020C03,        /*!< Not enough data received */
	MXQ_ERR_TRANS_BAD_CRC					= (int)-0x020C04,        /*!< Bad CRC */
	MXQ_ERR_TRANS_RX_BAD_FORMAT				= (int)-0x020C05,        /*!< Bad format of the received frame */
	MXQ_ERR_TRANS_TIMEOUT					= (int)-0x020C06,        /*!< Transmission Timeout */

	MXQ_ERR_SECURE_CHANNEL_INTEGRITY		= (int)-0x030001,		/*!< Error when verifying SECURE CHANNEL integrity */

	MXQ_ERR_GCU_TIMEOUT						= (int)-0x051001,		/*!< Timeout during GCU communication */
	MXQ_ERR_GCU_ERROR_STATUS				= (int)-0x051002,		/*!< Bad GCU status */
	MXQ_ERR_GCU_UNKNOWN_STATUS				= (int)-0x051003,		/*!< Unknown GCU status */
	MXQ_ERR_MODE				         	= (int)-0x057000,	    /*!< Wrong mode  (aes spi vs crypto toolbox)*/
	
} mxq_err_t;

#endif /* MXQ_ERROR_H_ */