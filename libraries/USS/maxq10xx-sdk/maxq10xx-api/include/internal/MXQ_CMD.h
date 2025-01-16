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
 * @file MXQ_CMD.h
 * @date Apr 2014
 * @authors Stéphane Di Vito, Maxim Integrated Products Inc.
 * @authors Benjamin Vinot, Maxim Integrated Products Inc.
 *
 * @ingroup CMD
 * @internal
 */


#ifndef __MXQ_CMD_H_INCLUDED__
#define __MXQ_CMD_H_INCLUDED__


#define MAX_BUFFER_SIZE 2300U
#define CMD_HEADER_SIZE 5U /* 0xaa / cmd1 cmd2 / ll ll / .... / crc1 crc2 */
#define MAX_DATA_SIZE (MAX_BUFFER_SIZE-CMD_HEADER_SIZE)

#define AES_KEYLEN_SC 16 /*  AES 128 is enough */
#define AES_IV_LEN 16
#define AES_MACLEN_SC 16 /*  Mac is 16 bytes */

//#ifdef MXQ_FW_UPDATE_CAP
#define FW_UPDATE_IV_LEN            (12)
#define FW_UPDATE_AAD_LEN           (15)
#define FW_UPDATE_TAG_LEN           (16)
#define FW_UPDATE_BODY_FIELD_LEN    (4)
#if defined(MAXQ108x)
#define FW_UPDATE_SIGNATURE_SIZE    (132)
#define FW_UPDATE_IMG_HEADER_LEN    (FW_UPDATE_IV_LEN + FW_UPDATE_AAD_LEN + FW_UPDATE_TAG_LEN + FW_UPDATE_BODY_FIELD_LEN)
#elif defined(MAXQ1065)
#define FW_UPDATE_SIGNATURE_SIZE    (FW_UPDATE_TAG_LEN)
#define FW_UPDATE_IMG_HEADER_LEN    (FW_UPDATE_IV_LEN + FW_UPDATE_AAD_LEN + FW_UPDATE_TAG_LEN + FW_UPDATE_BODY_FIELD_LEN + FW_UPDATE_TAG_LEN)
#endif

#define FW_UPDATE_AWAIT_BOOTLOADER_USEC     20000000U


//#endif

#define SC_secretsize (AES_IV_LEN*3+AES_KEYLEN_SC*2)
#define S_SC_RANDOM_LEN              96
#define S_SC_HKDF_RANDOM_LEN         32

typedef struct
{
    mxq_length* pdata_length;
    mxq_pu1 p_data;
    mxq_u2 status;
} cmd_response_t;

/**
 * Command State security state.
 * @ingroup CMD
 */
typedef enum _PACKED
{
	CMD_SEC_NONE   = 0,  /**> */
	CMD_SEC_ACTIVE = 1 /**> */
} cmd_sec_state_t;

/**
 *
 * @param i
 * @return
 */
mxq_u2 cmd_2_proc_endian(mxq_u2 i);


/**
 *
 * @param i
 * @return
 */
mxq_u2 proc_endian_2_cmd(mxq_u2 i);

/**
 *
 * @param u2val
 * @param pdest
 * @param offset
 */
void put_u2_lsb(mxq_u2 u2val, mxq_u1* pdest, mxq_length* offset);

/**
 *
 * @param u4val
 */
void cmd_put_u4(mxq_u4 u4val);

/**
 *
 * @param u2val
 */
void cmd_put_u2(mxq_u2 u2val);

/**
 *
 * @param u1val
 */
void cmd_put_u1(mxq_u1 u1val);

/**
 *
 * @param psrc
 * @param len
 */
void cmd_put_buffer(const mxq_u1* psrc, mxq_u2 len);

/**
 *
 */
void cmd_reset_offset(void);
/**
 * Copies MAXQ1065 ECDHE parameters to buffer
 *
 * @param pPrivateKey The secure channel Host private Key
 * @param pDevicePubKey The secure channel MAXQ public Key
 * @return
 */

mxq_err_t CMD_PrepareEcdheMode1065Params(mxq_pu1 pPrivateKey, mxq_pu1 pDevicePubKey);
/**
 * Generates ECDHE key
 *
 * @param key Pointer to the generated key
 * @return
 */
mxq_err_t CMD_PrepareEcdheKeyPair(mxq_pu1 key);
/**
 * Generates the signature of the secure channel parameters
 *
 * @param pSignature Pointer to calculated signature
 * @param pKey Private key used for signature generation
 * @return
 */
 mxq_err_t CMD_SignSCParams(mxq_pu1 pSignature, mxq_pu1 pKey);

 /**
 * Reads necessary challenge from MAXQ according to secure channel mode
 *
 * @param mode The secure channel mode
 * @return
 */
mxq_err_t CMD_GetScChallengeWithMode(mxq_u1 mode, mxq_u1* p_buff_out, mxq_length len);

 /**
 * Reads necessary challenge from MAXQ according to secure channel mode
 *
 * @param mode The secure channel mode
 * @return
 */
mxq_err_t CMD_GetScChallenge(mxq_u1 mode);

/**
 * @internal
 * Sends a command and waits for a response from the MAXQ106x.
 * If the returned data is shorter than expected, the returned length reflects the number of
 * bytes actually received
 * If the returned data is longer than expected, only the expected number of bytes is returned
 * but the returned length reflects the total size of the data that the MAXQ106x intended to return.
 *
 * @param[in] CMD command ID
 * @param[in] cmddata_length: length of input command data (Lc)
 * @param[in] cmddata: pointer to APDU input command data
 * @param[in, out] response: pointer to structure for setting and storing APDU response, that is:
 *                 [in]  response->data_length: expected length (Le)
 *                 [out] response->data_length: actual reply data length
 *                 [out] response->p_data: pointer to reply data
 *                 [out] response->status: APDU command response status word aka trailer
 *
 * @return Execution status.
 * @retval MXQ_OK Everything is fine
 * @retval MXQ_ERR_APDU_TX_ERROR
 * @retval MXQ_ERR_APDU_RX_ERROR
 * @retval MXQ_ERR_APDU_BAD_LC_VS_CMDDATA          Input cmddata_length is 0 whereas cmddata is not NULL.
 * @retval Other                                   TRANS_ReceiveBuffer or SC_unwrap return value
 *
 * @pre No precondition
 * @post state=APDU_IDLE
 */
mxq_err_t CMD_SendCommand(cmd_num_t CMD, cmd_response_t *response);

//TODO: Doxygen will be added
mxq_err_t CMD_SendBypassCMD(mxq_u1** buf_p, mxq_u2* buf_len);
mxq_err_t CMD_SendRawCommand(cmd_num_t CMD);

/**
 * Computes secure channel keys and initiates a secure channel with the MAXQ106x
 * 
 * @param rnd
 * @param local_key
 * @param mode The secure channel mode (use mode 2 on C3 revision, mode 1 on C2)
 * @return
 */
mxq_err_t CMD_InitSecureChannel(mxq_u1* rnd, mxq_u1* local_key, mxq_u1 mode);

/**
 * Closes the currently running secure channel.
 */
void CMD_CloseSecureChannel(mxq_u1 cmd_open_status);

/**
 *
 * @param pcmd
 * @param datalen
 * @return
 */
mxq_err_t SC_PacketSecure(mxq_u1** pcmd, mxq_length* datalen);

/**
 *
 * @param psrc
 * @return
 */
mxq_err_t SC_PacketVerify(mxq_u1** psrc);

#endif /*   __MXQ_CMD_H_INCLUDED__ */
