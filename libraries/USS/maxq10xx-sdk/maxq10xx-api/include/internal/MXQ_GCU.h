/*******************************************************************************
* Copyright (C) 2018 Maxim Integrated Products, Inc., All rights Reserved.
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
 * @file MXQ_GCU.h
 * @date June 2016
 * @authors Benjamin Vinot, Maxim Integrated Products Inc.
 *
 * @internal
 */

#ifndef __MXQ_GCU_H_INCLUDED__
#define __MXQ_GCU_H_INCLUDED__


/**
 *
 */
typedef enum  _PACKED{
	MXQ_GCU_LINK_OPCODE 			= 0x01,		/*!< MXQ_GCU_LINK_OPCODE */
	MXQ_GCU_RESET_OPCODE 			= 0x02,		/*!< MXQ_GCU_RESET_OPCODE */
	MXQ_GCU_SHUTDOWN_OPCODE 		= 0x03,		/*!< MXQ_GCU_SHUTDOWN_OPCODE */
	MXQ_GCU_ABORT_OPCODE 			= 0x04,		/*!< MXQ_GCU_ABORT_OPCODE */
	MXQ_GCU_LOAD_KEY_OPCODE			= 0x05,		/*!< MXQ_GCU_LOAD_KEY_OPCODE */
	MXQ_GCU_LOAD_IV_OPCODE 			= 0x06,		/*!< MXQ_GCU_LOAD_IV_OPCODE */
	MXQ_GCU_AUTH_OP_OPCODE 			= 0x07,		/*!< MXQ_GCU_AUTH_OP_OPCODE */
	MXQ_GCU_ENCR_OP_OPCODE 			= 0x08,		/*!< MXQ_GCU_ENCR_OP_OPCODE */
	MXQ_GCU_AUTH_ENCR_OP_OPCODE 	= 0x09,		/*!< MXQ_GCU_AUTH_ENCR_OP_OPCODE */
	MXQ_GCU_AUTH_DECR_OP_OPCODE 	= 0x0A,		/*!< MXQ_GCU_AUTH_DECR_OP_OPCODE */
	MXQ_GCU_ECB_ENCR_OP_OPCODE 		= 0x0B,		/*!< MXQ_GCU_ECB_ENCR_OP_OPCODE */
	MXQ_GCU_ECB_DECR_OP_OPCODE 		= 0x0C,		/*!< MXQ_GCU_ECB_DECR_OP_OPCODE */
	MXQ_GCU_SEND_BLOCK_OPCODE 		= 0x0D,		/*!< MXQ_GCU_SEND_BLOCK_OPCODE */
	MXQ_GCU_REQUEST_BLOCK_OPCODE 	= 0x0E,		/*!< MXQ_GCU_REQUEST_BLOCK_OPCODE */
	MXQ_GCU_REQUEST_TAG_OPCODE 		= 0x0F		/*!< MXQ_GCU_REQUEST_TAG_OPCODE */
} gcu_cmd_num_t;


#define MXQ_GCU_BLOCK_SIZE				16
#define MXQ_GCU_IV_LENGTH				12
#define MXQ_GCU_LENGTH_FIELD_SIZE		4




/**
 * @internal
 */
typedef struct
{
    mxq_length pdata_length;
    mxq_u1* p_data;
    mxq_u1 encdec;
    mxq_u1 algo;
    mxq_u1 initial;
	mxq_u1 auth;
} gcu_context_t;


/**
 *
 * @param command
 * @return
 */
mxq_err_t GCU_SendCommand(mxq_u1 command);

/**
 *
 * @param key
 * @return
 */
mxq_err_t GCU_LoadKey( mxq_u1 *key);

/**
 *
 * @param iv
 * @return
 */
mxq_err_t GCU_LoadIV(mxq_u1 *iv);

/**
 *
 * @return
 */
mxq_err_t GCU_Reset(void);

/**
 *
 * @return
 */
mxq_err_t GCU_Shutdown(void);

/**
 *
 * @return
 */
mxq_err_t GCU_Abort(void);

/**
 *
 * @return
 */
mxq_err_t GCU_LinkTest(void);

/**
 *
 * @param block
 * @return
 */
mxq_err_t GCU_SendBlock(const mxq_u1* block);

/**
 *
 * @param block
 * @return
 */
mxq_err_t GCU_RequestBlock(mxq_u1 *block);

/**
 *
 * @param tag
 * @return
 */
mxq_err_t GCU_RequestTag(mxq_u1 *tag);

/**
 *
 * @param mode
 * @param block
 * @return
 */
mxq_err_t GCU_ECBEncDec(mxq_u1 mode, const mxq_u1* block);

/**
 *
 * @param data_length
 * @param block
 * @return
 */
mxq_err_t GCU_GCMEncDec(mxq_length data_length, const mxq_u1* block);

/**
 *
 * @param aad_length
 * @param aad_block
 * @return
 */
mxq_err_t GCU_GMAC(mxq_length aad_length, mxq_u1* aad_block);

/**
 *
 * @param mode
 * @param aad_length
 * @param data_length
 * @param aad_block
 * @return
 */
mxq_err_t GCU_GCMAuthEncDec(mxq_u1 mode, mxq_length aad_length, const mxq_length data_length, const mxq_u1* aad_block);

#endif /* __MXQ_GCU_H_INCLUDED__ */


