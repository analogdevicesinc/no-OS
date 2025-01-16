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
 * @file MXQ_APDU.c
 * @date Apr 2014
 * @authors Stéphane Di Vito, Maxim Integrated Products Inc.
 * @authors Benjamin Vinot, Maxim Integrated Products Inc.
 *
 * @brief Handles APDU communications with the MAXQ106x.
 * @details Refer to the architecture diagram for more information.
 * @ingroup APDU
 */


#include "MXQ_Internal.h"
#include "crypto/ecc/uECC.h"
#include "crypto/hmac/sha2.h"
#include "crypto/hmac/hmac_sha2.h"
#include "crypto/hmac/hkdf.h"
#include "crypto/aes_gcm/includes.h"
#include "crypto/aes_gcm/common.h"
#include "crypto/aes_gcm/aes.h"
#include "crypto/aes_gcm/aes_wrap.h"

#define S_SC_OTP_IKM_LEN        (32)
#define S_SC_OTP_WR_CNT_LEN     (4)
#define S_SC_OTP_SALT_LEN       (S_SC_OTP_WR_CNT_LEN + S_SC_HKDF_RANDOM_LEN)
#define S_SC_OTP_MAC_SIZE       (8)
#define S_SC_OTP_MAC_KEY_SIZE   (32)
#define AES_GCM_KEYLEN_SC       (32)
#define S_SC_TAG_LEN            16
#define S_SC_DATA_LEN		    80
#define S_SC_IV_LEN             12
#define S_SECURE_CHANNEL        "SECURECHANNEL"
#define MXQ_MAXQ1061_USN_LENGTH          (24)

#ifdef USS_ST
extern int Receive_Process_Cmd(_Bool);
#endif

#ifdef NO_SECURE_CHANNEL
#warning API: Secure channel disabled
#else
typedef union
{
	struct _PACKED
	{
		mxq_u1 IV_dec[AES_IV_LEN]; //16
		mxq_u1 IV_enc[AES_IV_LEN]; //16
		mxq_u1 IV_mac[AES_IV_LEN]; //16
		mxq_u1 Key_enc[AES_KEYLEN_SC]; //16
		mxq_u1 Key_mac[AES_KEYLEN_SC]; //16
	} cbc;
    struct _PACKED
	{
		mxq_u1 IV_dec[AES_IV_LEN]; //16
		mxq_u1 IV_enc[AES_IV_LEN]; //16
		mxq_u1 Key_enc[AES_GCM_KEYLEN_SC]; //32
		mxq_u1 Key_dec[AES_GCM_KEYLEN_SC]; //32
	} gcm;
    struct _PACKED
    {
        mxq_u1 rand[S_SC_HKDF_RANDOM_LEN]; //32
        mxq_u1 ikm[S_SC_OTP_IKM_LEN]; //32
        mxq_u4 counter; //4
        mxq_u1 prevHmac[S_SC_OTP_MAC_SIZE]; //8

    } otp;

	mxq_u1 raw[SC_secretsize];

} ctx_securechannel_u;


struct
{
	ctx_securechannel_u ctx;
	unsigned char state;
	unsigned char gSecChannelMode;

} ctx_securechannel;

#endif

static mxq_u1		g_cmd_buffer[MAX_BUFFER_SIZE];
static mxq_u1*		g_cmd_data = (g_cmd_buffer + 2 * CMD_HEADER_SIZE);
static mxq_length	g_offset = 0;
#ifndef NO_SECURE_CHANNEL
static mxq_u1		g_cmd_sc_buffer[MAX_BUFFER_SIZE];
mxq_u1 gSecChannelStatus;
mxq_u1 gScEcdheKeyPair[KEYCOMPLEN256*3];
mxq_u1 gScMaxqPubKey[KEYCOMPLEN256*3];
mxq_u1 gScChallenge[16*5+16+16];
#endif

mxq_u2 cmd_2_proc_endian(mxq_u2 i)
{
	/* Since the proc is LSB */
	return (mxq_u2)((i >> 8) | (i << 8));
}

mxq_u2 proc_endian_2_cmd(mxq_u2 i)
{
	/* Since the proc is LSB */
	return (mxq_u2)((i >> 8) | (i << 8));
}


void put_u2_lsb(mxq_u2 u2val, mxq_u1* pdest, mxq_length* offset)
{
	pdest[*offset]   = (mxq_u1)u2val;
	pdest[*offset + 1] = (mxq_u1)(u2val >> 8);
	*offset+=2;
}


void cmd_put_u4(mxq_u4 u4val)
{
	if((g_offset + 4) > MAX_BUFFER_SIZE) return;

	g_cmd_data[g_offset] = (mxq_u4)(u4val >> 24);
	g_cmd_data[g_offset + 1] = (mxq_u4)(u4val >> 16);
	g_cmd_data[g_offset + 2] = (mxq_u4)(u4val >> 8);
	g_cmd_data[g_offset + 3] = (mxq_u4)(u4val);
	g_offset += 4;
}


void cmd_put_u2(mxq_u2 u2val)
{
	if((g_offset + 2) > MAX_BUFFER_SIZE) return;

	g_cmd_data[g_offset] = u2val >> 8;
	g_cmd_data[g_offset + 1] = u2val;
	g_offset += 2;
}


void cmd_put_u1(mxq_u1 u1val)
{
	if((g_offset + 1) > MAX_BUFFER_SIZE) return;

	g_cmd_data[g_offset++] = u1val;
}



void cmd_put_buffer(const mxq_u1* psrc, mxq_u2 len)
{
	if((g_offset + len) > MAX_BUFFER_SIZE) return;

	while(len)
	{
		g_cmd_data[g_offset++] = *psrc;
		psrc++;
		len--;
	}
}

void cmd_reset_offset(void)
{
	g_offset = 0;
}

void incrementIV(mxq_pu1 pIv)
{
    mxq_s2 i;

    for (i = (S_SC_IV_LEN-1) ; i >= 0 ; i--)
    {
        pIv[i]++;

        if (pIv[i] != 0)
            return;
    }

    return;
}

#ifndef NO_SECURE_CHANNEL
mxq_err_t CMD_PrepareEcdheKeyPair(mxq_pu1 key)
{
    uECC_make_key(gScEcdheKeyPair, (gScEcdheKeyPair+KEYCOMPLEN256*2), uECC_secp256r1());
	MXQ_PRINTF("gScEcdheKeyPair\n");
	for (int i = 0; i < 3*KEYCOMPLEN256; i++)
		MXQ_PRINTF("%02X ", gScEcdheKeyPair[i]);
	MXQ_PRINTF("\n");
	memcpy(key,gScEcdheKeyPair,(KEYCOMPLEN256*2));
	return MXQ_OK;
}


mxq_err_t CMD_PrepareEcdheMode1065Params(mxq_pu1 pPrivateKey, mxq_pu1 pDevicePubKey)
{
    memcpy(&gScEcdheKeyPair[KEYCOMPLEN256 * 2], pPrivateKey, KEYCOMPLEN256);

    memcpy(&gScMaxqPubKey[0], pDevicePubKey, (KEYCOMPLEN256 * 2));

    return MXQ_OK;
}

#if defined(MAXQ106x)
mxq_err_t CMD_GetScChallengeWithMode(mxq_u1 mode, mxq_u1* p_buff_out, mxq_length len)
{
    ctx_securechannel.gSecChannelMode = mode;

    return MXQ_Get_Challenge(p_buff_out, len);
}
#endif

mxq_err_t CMD_GetScChallenge(mxq_u1 mode)
{
	ctx_securechannel.gSecChannelMode = mode;
    if((mode == eSCInit) || (mode == eSCAesCbcInit)) {
		return MXQ_Get_Challenge(gScChallenge,sizeof(gScChallenge));
    } else if((mode == eSCAesGcmInit) || (mode == eSCAuthEcdhe) || (mode == eSCPreSharedKeyMode) || (mode == eSCEcdheMode1065)) {
		return MXQ_Get_Challenge(gScChallenge,S_SC_RANDOM_LEN);
    } else
        return MXQ_ERR_WRONG_PARAMETER;
}

mxq_err_t CMD_SignSCParams(mxq_pu1 pSignature, mxq_pu1 pKey)
{
	mxq_u1 lMessage[S_SC_RANDOM_LEN+(KEYCOMPLEN256*2)];
    mxq_u1 lDigest[SHA256_DIGEST_SIZE];

	memcpy(lMessage,gScChallenge,S_SC_RANDOM_LEN);
	memcpy(lMessage+S_SC_RANDOM_LEN,gScEcdheKeyPair,KEYCOMPLEN256*2);
    sha256(lMessage, sizeof(lMessage), lDigest);
    uECC_sign(pKey, lDigest, SHA256_DIGEST_SIZE, pSignature, uECC_secp256r1());
	return MXQ_OK;
}

mxq_err_t CMD_SignSCParams1065(mxq_pu1 pSignature, mxq_pu1 pKey, mxq_pu1 pUsn)
{
    mxq_u1 lMessage[S_SC_RANDOM_LEN + MXQ_MAXQ1061_USN_LENGTH];
    mxq_u1 lDigest[SHA256_DIGEST_SIZE];

    memcpy(lMessage, gScChallenge, S_SC_RANDOM_LEN);
    memcpy(lMessage + S_SC_RANDOM_LEN, pUsn, MXQ_MAXQ1061_USN_LENGTH);
    sha256(lMessage, sizeof(lMessage), lDigest);
    uECC_sign(pKey, lDigest, SHA256_DIGEST_SIZE, pSignature, uECC_secp256r1());
    return MXQ_OK;
}

#if defined(MAXQ106x)
void CMD_CloseSecureChannel(mxq_u1 cmd_open_status)
{
	(void) cmd_open_status;
    MXQ_MEMSET(&ctx_securechannel, 0, sizeof(ctx_securechannel));
	ctx_securechannel.state = CMD_SEC_NONE;

	return;
}
#else
void CMD_CloseSecureChannel(mxq_u1 cmd_open_status)
{
	if(gSecChannelStatus == cmd_open_status)
	{
		memset(&ctx_securechannel,0,sizeof(ctx_securechannel));
		ctx_securechannel.state = CMD_SEC_NONE;
	}

	return ;
}
#endif
#if defined(MAXQ106x)
mxq_err_t CMD_InitSecureChannel(mxq_u1* rnd, mxq_u1* local_key, mxq_u1 mode)
{
	mxq_err_t err;
    do{
        if(mode==eSCInit) {
            err = HOST_aes_cbc(local_key, AES_KEYLEN_SC, rnd, MXQ_AES_ENCRYPT, rnd, ctx_securechannel.ctx.raw, SC_secretsize / 16);
            if (err != MXQ_OK)  break;;
        }
        else if(mode==eSCAesCbcInit) {
            err = HOST_aes_cbc(local_key, AES_KEYLEN_SC, rnd, MXQ_AES_ENCRYPT, rnd+16, ctx_securechannel.ctx.raw, SC_secretsize / 16);
            if (err != MXQ_OK)  break;
        }
        else
        {
            return -1;
        }
        ctx_securechannel.state = CMD_SEC_ACTIVE;
        gSecChannelStatus = mode;

        cmd_print_seccmd("___Initial Host SC: ", ctx_securechannel.ctx.raw, SC_secretsize);

        return MXQ_OK;
    } while (0);

	CMD_CloseSecureChannel(gSecChannelStatus);
	return err;
}
#else
mxq_err_t CMD_InitSecureChannel(mxq_u1* local_key, mxq_u1* pEcdheKey, mxq_u1 pCMDOpenStatus)
{
	mxq_err_t err;
	mxq_u4 i;
	mxq_u1 lZLen = 0;
	mxq_u1 lZVal[S_SC_DATA_LEN + S_SC_TAG_LEN];
    sha256_ctx sha256Ctx;

    gSecChannelStatus = pCMDOpenStatus;
    do {
        if((ctx_securechannel.gSecChannelMode == eSCInit)||(ctx_securechannel.gSecChannelMode == eSCAesCbcInit))
        {
            if(ctx_securechannel.gSecChannelMode == eSCInit)
                err=HOST_aes_cbc(local_key,AES_KEYLEN_SC,gScChallenge,MXQ_AES_ENCRYPT,gScChallenge,ctx_securechannel.ctx.raw,SC_secretsize/16);
            else
                err=HOST_aes_cbc(local_key,AES_KEYLEN_SC,gScChallenge,MXQ_AES_ENCRYPT,gScChallenge+16,ctx_securechannel.ctx.raw,SC_secretsize/16);
            if (err!=MXQ_OK)  break;

            ctx_securechannel.state = CMD_SEC_ACTIVE;

            MXQ_PRINTF("___Initial Host SC: ");
            for (i = 0; i < SC_secretsize; i++)
                MXQ_PRINTF("%02X %s", ctx_securechannel.ctx.raw[i], (i+1)%16?"":" - ");
            MXQ_PRINTF("\n");
        }
        else if (ctx_securechannel.gSecChannelMode == eSCPreSharedKeyMode)
        {
            memcpy(ctx_securechannel.ctx.otp.rand, gScChallenge, S_SC_HKDF_RANDOM_LEN);
            memcpy(ctx_securechannel.ctx.otp.ikm, local_key, S_SC_OTP_IKM_LEN);
            ctx_securechannel.ctx.otp.counter = 0;
            memset(ctx_securechannel.ctx.otp.prevHmac, 0, S_SC_OTP_MAC_SIZE);

            MXQ_PRINTF("HOST SC KEY (eSCPreSharedKeyMode): ");
            for (i = 0; i < S_SC_OTP_IKM_LEN; i++)
                MXQ_PRINTF("%02X ", ctx_securechannel.ctx.otp.ikm[i]);
            MXQ_PRINTF("\n");

            ctx_securechannel.state = CMD_SEC_ACTIVE;
        }
        else if (ctx_securechannel.gSecChannelMode == eSCAesGcmInit)
        {
            err = aes_gcm_ae(local_key, AES_GCM_KEYLEN_SC, gScChallenge, S_SC_IV_LEN,
               &gScChallenge[16], S_SC_DATA_LEN,
               NULL, 0, lZVal, &lZVal[S_SC_DATA_LEN]);
            if (err!=MXQ_OK) {
                break;
            }
            MXQ_PRINTF("Z Value \n");
            lZLen = S_SC_TAG_LEN + S_SC_DATA_LEN;
            for (i = 0; i < lZLen; i++)
                MXQ_PRINTF("%02X ", lZVal[i]);
            MXQ_PRINTF("\n");

        }
        else if ((ctx_securechannel.gSecChannelMode == eSCEcdhe) || (ctx_securechannel.gSecChannelMode == eSCAuthEcdhe))
        {
            MXQ_PRINTF("gScEcdheKeyPair\n");
            for (i = 0; i < KEYCOMPLEN256; i++)
                MXQ_PRINTF("%02X ", gScEcdheKeyPair[2*KEYCOMPLEN256+i]);
            MXQ_PRINTF("\n");
            err = uECC_shared_secret(pEcdheKey,
                           &gScEcdheKeyPair[2*KEYCOMPLEN256],
                           lZVal,
                           uECC_secp256r1());

            if (!err) break;
            lZLen = KEYCOMPLEN256;
        }
        else if (ctx_securechannel.gSecChannelMode == eSCEcdheMode1065)
        {
            MXQ_PRINTF("HOST PRIVATE KEY (eSCEcdheMode1065):\n");
            for (i = 0; i < KEYCOMPLEN256; i++)
                MXQ_PRINTF("%02X ", gScEcdheKeyPair[i + (2 * KEYCOMPLEN256)]);
            MXQ_PRINTF("\n");
            err = uECC_shared_secret(gScMaxqPubKey,
                           &gScEcdheKeyPair[2*KEYCOMPLEN256],
                           lZVal,
                           uECC_secp256r1());
            if (!err) {
                MXQ_PRINTF("CMD_InitSecureChannel (FAIL): sp800 56\n");
                break;
            }

            memcpy(ctx_securechannel.ctx.otp.rand, gScChallenge, S_SC_HKDF_RANDOM_LEN);
            memcpy(ctx_securechannel.ctx.otp.ikm, lZVal, KEYCOMPLEN256);
            ctx_securechannel.ctx.otp.counter = 0;
            memset(ctx_securechannel.ctx.otp.prevHmac, 0, S_SC_OTP_MAC_SIZE);

            MXQ_PRINTF("HOST Z VALUE (eSCEcdheMode1065):\n");
            for (i = 0; i < KEYCOMPLEN256; i++)
                MXQ_PRINTF("%02X ", lZVal[i]);
            MXQ_PRINTF("\n");

            ctx_securechannel.state = CMD_SEC_ACTIVE;
        }

        if( (ctx_securechannel.gSecChannelMode == eSCAesGcmInit) || (ctx_securechannel.gSecChannelMode == eSCEcdhe) || (ctx_securechannel.gSecChannelMode == eSCAuthEcdhe) )
        {
            mxq_u1 lDummyBuf[8] = {0};
            sha256_init(&sha256Ctx);
            sha256_update(&sha256Ctx, lDummyBuf,sizeof(lDummyBuf));
            sha256_update(&sha256Ctx, lZVal, lZLen);
            sha256_update(&sha256Ctx, (mxq_u1 *)S_SECURE_CHANNEL,sizeof(S_SECURE_CHANNEL)-1);
            sha256_final(&sha256Ctx, ctx_securechannel.ctx.gcm.Key_dec);

            lDummyBuf[7] = 0x01;
            sha256_init(&sha256Ctx);
            sha256_update(&sha256Ctx, lDummyBuf,sizeof(lDummyBuf));
            sha256_update(&sha256Ctx, lZVal, lZLen);
            sha256_update(&sha256Ctx, (mxq_u1 *)S_SECURE_CHANNEL,sizeof(S_SECURE_CHANNEL)-1);
            sha256_final(&sha256Ctx, ctx_securechannel.ctx.gcm.Key_enc);

            MXQ_PRINTF("Enc Key Value \n");
            for (i = 0; i < AES_GCM_KEYLEN_SC; i++)
                MXQ_PRINTF("%02X ", ctx_securechannel.ctx.gcm.Key_enc[i]);
            MXQ_PRINTF("\nDec Key Value \n");
            for (i = 0; i < AES_GCM_KEYLEN_SC; i++)
                MXQ_PRINTF("%02X ", ctx_securechannel.ctx.gcm.Key_dec[i]);
            MXQ_PRINTF("\n");
            ctx_securechannel.state = CMD_SEC_ACTIVE;
        }

        return MXQ_OK;
    } while (0);

	CMD_CloseSecureChannel(gSecChannelStatus);
	return err;
}
#endif

mxq_u1  sc_otp_key_buffer[MAX_BUFFER_SIZE];
mxq_err_t SC_OTP_Encrypt_Decrypt(const mxq_u1 *pInBuff, mxq_u1 *pOutBuff, mxq_u4 pLen, mxq_u1 *pMacBuff, mxq_u4 pMacLen)
{
    int lRes;
    mxq_u1 lSalt[S_SC_OTP_SALT_LEN];
    mxq_u4 lIndex = 0, lKeySize;
    mxq_u1 *lOutputKey = sc_otp_key_buffer;
    hmac_sha256_ctx ctx;

    // SALT PREPARATION (Starting with the SC COUNTER)
    memcpy(&lSalt[0], &ctx_securechannel.ctx.otp.counter, S_SC_OTP_WR_CNT_LEN);
    // SC RANDOM
    memcpy(&lSalt[S_SC_OTP_WR_CNT_LEN], ctx_securechannel.ctx.otp.rand, S_SC_HKDF_RANDOM_LEN);

    // HKDF
    lRes = hkdf_sha256(lOutputKey,
                           (pLen + S_SC_OTP_MAC_KEY_SIZE),
                           ctx_securechannel.ctx.otp.ikm,
                           S_SC_OTP_IKM_LEN,
                           NULL,
                           0,
                           lSalt,
                           S_SC_OTP_SALT_LEN);
    if (lRes)
    {
        MXQ_PRINTF("SC_OTP_Encrypt_Decrypt (FAIL): hkdf_sha256\n");
        return MXQ_RSP_CRYPTO_FAILURE;
    }
    lKeySize = pLen;
    while (lKeySize)
    {
        if (pInBuff)
        {
            pOutBuff[lIndex] = pInBuff[lIndex] ^ lOutputKey[lIndex];
        }
        lIndex++;
        lKeySize--;
    }

    if (pMacLen)
    {
        hmac_sha256_init(&ctx, &lOutputKey[pLen],S_SC_OTP_MAC_KEY_SIZE);
        hmac_sha256_update(&ctx, pOutBuff, pLen);
        hmac_sha256_update(&ctx, ctx_securechannel.ctx.otp.prevHmac,S_SC_OTP_MAC_SIZE);
        hmac_sha256_final(&ctx, pMacBuff,pMacLen);
    }

    return MXQ_OK;
}

mxq_err_t SC_PacketSecure(mxq_u1** pcmd, mxq_length* datalen)
{
	mxq_err_t err;
	mxq_u2  nblocks;
	mxq_u2 crc;
    mxq_u2  lDataLen;

	lDataLen = *datalen;
    do {
        if((ctx_securechannel.gSecChannelMode == eSCInit)||(ctx_securechannel.gSecChannelMode == eSCAesCbcInit))
        {

            nblocks = *datalen;

            if(nblocks & 0x0f)
            {
                /*  Pad with 00.00 */
                MXQ_MEMSET( *pcmd + *datalen, 0, 16 - (nblocks & 0x0f));
                nblocks = (nblocks >> 4) + 1;
            }
            else
            {
                nblocks = nblocks >> 4;
            }

            //cmd_print_seccmd("___Before TX Host SC: ", ctx_securechannel.ctx.raw, SC_secretsize);

            /*  *** Encrypt data */
            err = HOST_aes_cbc(
                        ctx_securechannel.ctx.cbc.Key_enc,
                        AES_KEYLEN_SC,
                        ctx_securechannel.ctx.cbc.IV_enc,
                        MXQ_AES_ENCRYPT,
                        *pcmd, /* in */
                        *pcmd, /* out */
                        nblocks /* len */
                        );

            if (err != MXQ_OK) break;

            /*  Update IV enc */
            /*  We will use the last ciphertext as IV for the next packet. */
            MXQ_MEMCPY(ctx_securechannel.ctx.cbc.IV_enc, (*pcmd) + ((nblocks - 1) << 4), AES_IV_LEN );

            /*  *** Compute and append MAC */
            err = HOST_aes_cbc_mac(
                            ctx_securechannel.ctx.cbc.Key_mac,
                            AES_KEYLEN_SC,
                            ctx_securechannel.ctx.cbc.IV_mac,
                            *pcmd, /* in */
                            ctx_securechannel.ctx.cbc.IV_mac, /* out */
                            nblocks /* len */
                            );
            if (err != MXQ_OK) break;


            MXQ_MEMCPY(*pcmd + (nblocks << 4), ctx_securechannel.ctx.cbc.IV_mac, AES_MACLEN_SC);

            //cmd_print_seccmd("___After TX Host SC: ", ctx_securechannel.ctx.raw, SC_secretsize);

            nblocks++;
            lDataLen = (nblocks<<4);
            *datalen = (nblocks << 4) + CMD_HEADER_SIZE;
        }
        else if ((ctx_securechannel.gSecChannelMode == eSCPreSharedKeyMode) || (ctx_securechannel.gSecChannelMode == eSCEcdheMode1065))
        {
            ctx_securechannel.ctx.otp.counter++;

            err = SC_OTP_Encrypt_Decrypt( (*pcmd), (*pcmd), lDataLen, ((*pcmd) + lDataLen), S_SC_OTP_MAC_SIZE );
            if ( err ) {
                break;
            }

            memcpy(ctx_securechannel.ctx.otp.prevHmac, ((*pcmd) + lDataLen), S_SC_OTP_MAC_SIZE);
            lDataLen += S_SC_OTP_MAC_SIZE;
            *datalen = lDataLen + CMD_HEADER_SIZE;
        }
        else
        {
            // does not work with overlapping input and output buffers
            err = aes_gcm_ae(ctx_securechannel.ctx.gcm.Key_enc, AES_GCM_KEYLEN_SC, ctx_securechannel.ctx.gcm.IV_enc, S_SC_IV_LEN,
                                            (*pcmd), lDataLen, NULL, 0, g_cmd_sc_buffer, (g_cmd_sc_buffer + lDataLen));
            if (err!=MXQ_OK) break;
            lDataLen += S_SC_TAG_LEN;
            *datalen =  lDataLen + CMD_HEADER_SIZE;

            // copy the encrypted data to the cmd buffer
            memcpy((*pcmd), g_cmd_sc_buffer, lDataLen);

            incrementIV(ctx_securechannel.ctx.gcm.IV_enc);
        }

        *pcmd -= CMD_HEADER_SIZE;

        (*pcmd)[0] = 0xAA;
        (*pcmd)[1] = 0xEC;
        (*pcmd)[2] = 0xEC;
        (*pcmd)[3] = (lDataLen>>8);
        (*pcmd)[4] = lDataLen;

        crc = HOST_CRCGet(*pcmd, *datalen);

        (*pcmd)[*datalen] = crc;
        (*pcmd)[*datalen + 1] = crc >> 8;

        *datalen += 2;

        return MXQ_OK;

    } while (0);

    ctx_securechannel.ctx.otp.counter--;
	CMD_CloseSecureChannel(gSecChannelStatus);
	return err;
}


mxq_err_t SC_PacketVerify(mxq_u1** psrc)
{
	mxq_err_t err;
	mxq_u2 crcchk;
	mxq_u2 status = (mxq_u2)(((*psrc)[1] << 8) + (*psrc)[2]);
	mxq_u2 blocklen = (mxq_u2)(((*psrc)[3] << 8) + (*psrc)[4]);
	mxq_u2 crc = (mxq_u2)(((*psrc)[CMD_HEADER_SIZE + blocklen]) + ((*psrc)[CMD_HEADER_SIZE + blocklen + 1] << 8));
	mxq_u2 nblocks;
	mxq_u1* mac_from_cmd;
	mxq_u1 bck[AES_IV_LEN];

	//cmd_print_seccmd("___Before RX Host SC: ", ctx_securechannel.ctx.raw, SC_secretsize);
	
	if(status != 0xECEC)
		return MXQ_ERR_TRANS_ERROR; /* Timeout or error */

	/*  Check inner CRC */
    crcchk = HOST_CRCGet(*psrc, blocklen + CMD_HEADER_SIZE);
	if( crc != crcchk) return MXQ_ERR_TRANS_BAD_CRC;
    do{
        if((ctx_securechannel.gSecChannelMode == eSCInit)||(ctx_securechannel.gSecChannelMode == eSCAesCbcInit))
        {

            nblocks = blocklen;
            if(nblocks & 0x0f)
                return MXQ_ERR_TRANS_RX_BAD_FORMAT;

            mac_from_cmd = *psrc + (CMD_HEADER_SIZE + nblocks - AES_MACLEN_SC);

            nblocks>>=4;

            /*  *** CHECK MAC *** */
            err = HOST_aes_cbc_mac(
                            ctx_securechannel.ctx.cbc.Key_mac,
                            AES_KEYLEN_SC,
                            ctx_securechannel.ctx.cbc.IV_mac,
                            *psrc+CMD_HEADER_SIZE, /* in */
                            ctx_securechannel.ctx.cbc.IV_mac, /* out */
                            nblocks - 1 /* len */
                            );
            if (err != MXQ_OK) break;

            if(MXQ_MEMCMP(ctx_securechannel.ctx.cbc.IV_mac, mac_from_cmd, AES_MACLEN_SC) != 0)
            {
                err = MXQ_ERR_SECURE_CHANNEL_INTEGRITY;
                break;
            }

            /*  *** DECRYPT *** */

            MXQ_MEMCPY(bck, mac_from_cmd - AES_IV_LEN, AES_IV_LEN );


            err = HOST_aes_cbc(
                            ctx_securechannel.ctx.cbc.Key_enc,
                            AES_KEYLEN_SC,
                            ctx_securechannel.ctx.cbc.IV_dec,
                            MXQ_AES_DECRYPT,
                            *psrc+CMD_HEADER_SIZE, /* in */
                            *psrc+CMD_HEADER_SIZE, /* out */
                            nblocks-1 /* len */
                            );

            if (err != MXQ_OK) break;

            /*  We will use the last ciphertext as IV for the next packet. */
            MXQ_MEMCPY(ctx_securechannel.ctx.cbc.IV_dec, bck, AES_IV_LEN);

            /*  skip it */
            *psrc += CMD_HEADER_SIZE;
        }
        else if ((ctx_securechannel.gSecChannelMode == eSCPreSharedKeyMode) || (ctx_securechannel.gSecChannelMode == eSCEcdheMode1065))
        {
            mxq_u1 lMacBuff[S_SC_OTP_MAC_SIZE];

            ctx_securechannel.ctx.otp.counter++;

            // Check MAC first
            err = SC_OTP_Encrypt_Decrypt( NULL,
                                          ((*psrc) + CMD_HEADER_SIZE),
                                          (blocklen - S_SC_OTP_MAC_SIZE),
                                          lMacBuff,
                                          S_SC_OTP_MAC_SIZE );
            if (err)
            {
                break;
            }

            if ( memcmp(lMacBuff, ((*psrc) + CMD_HEADER_SIZE + blocklen - S_SC_OTP_MAC_SIZE), S_SC_OTP_MAC_SIZE) ) {
                MXQ_PRINTF("SC_PacketVerify (FAIL): MAC CHECK!\n");
                err = MXQ_RSP_WRONG_MAC;
                break;
            }

            err = SC_OTP_Encrypt_Decrypt( ((*psrc) + CMD_HEADER_SIZE),
                                          ((*psrc) + CMD_HEADER_SIZE),
                                          (blocklen - S_SC_OTP_MAC_SIZE),
                                          NULL,
                                          0 );
            if ( err ) {
                break;
            }

            memcpy(ctx_securechannel.ctx.otp.prevHmac, ((*psrc) + CMD_HEADER_SIZE + blocklen - S_SC_OTP_MAC_SIZE), S_SC_OTP_MAC_SIZE);

            // skip it
            *psrc+=CMD_HEADER_SIZE;
        }
        else
        {
            // does not work with overlapping input and output buffers
            err = aes_gcm_ad(ctx_securechannel.ctx.gcm.Key_dec, AES_GCM_KEYLEN_SC, ctx_securechannel.ctx.gcm.IV_dec, S_SC_IV_LEN,
                                    ((*psrc) + CMD_HEADER_SIZE), (blocklen - S_SC_TAG_LEN), NULL, 0,
                                    ((*psrc) + CMD_HEADER_SIZE + blocklen - S_SC_TAG_LEN), g_cmd_sc_buffer);
            if ( err ) {
                break;
            }

            // copy the decrypted data to the cmd buffer
            memcpy( ((*psrc) + CMD_HEADER_SIZE), g_cmd_sc_buffer, (blocklen - S_SC_TAG_LEN) );

            incrementIV(ctx_securechannel.ctx.gcm.IV_dec);
            // skip it
            *psrc+=CMD_HEADER_SIZE;

        }

        //cmd_print_seccmd("___After RX Host SC: ", ctx_securechannel.ctx.raw, SC_secretsize);

        return MXQ_OK;
    } while(0);

    ctx_securechannel.ctx.otp.counter--;
	CMD_CloseSecureChannel(gSecChannelStatus);
	return err;
}
#endif /* NO_SECURE_CHANNEL */

mxq_err_t CMD_SendBypassCMD(mxq_u1** buf_p, mxq_u2* buf_len)
{
	mxq_err_t err;
	mxq_u1* pdest = *buf_p;
	mxq_length blocklen = 0;
	mxq_err_t inclen = 0;
	mxq_u2 crcchk;

	/*  Send the  command */
	err = sendbytes(pdest, *buf_len );
	if(err < MXQ_OK) return err;

    /* Only used with USS */
    #if (defined USS_ST) && (defined USE_PROTOCOL_MAILBOX)  // Single threaded USS mailbox variant
    if( Receive_Process_Cmd(MXQ_TRUE) != 0 )
    {
        return MXQ_ERR_TRANS_ERROR;
    }
    #endif

	/*  **** Wait for a response **** */

	inclen = receivebytes_first(pdest, CMD_HEADER_SIZE);

	/* If incoming length is negative that means it is an error code */
	if(inclen < 0){
		return inclen;
	}

	if(inclen != CMD_HEADER_SIZE)
		return MXQ_ERR_TRANS_NOT_ENOUGH_DATA;

	blocklen = (pdest[3] << 8) + pdest[4];

	if ( blocklen > MAX_BUFFER_SIZE )
		return MXQ_ERR_TRANS_BUFFER_OVERFLOW; /*  Avoid RX buffer overflow */

	/*  Receive data+crc */
	inclen = receivebytes(pdest+CMD_HEADER_SIZE, blocklen+sizeof(crcchk));
	if(inclen < 0){
		return inclen;
	}

	if (inclen != (int)(blocklen+sizeof(crcchk)))
		return MXQ_ERR_TRANS_NOT_ENOUGH_DATA;
	cmd_print("MAXQ->Host: ", pdest, blocklen + CMD_HEADER_SIZE + sizeof(crcchk));

	/*  Check inner CRC */
	crcchk = HOST_CRCGet(pdest, blocklen+CMD_HEADER_SIZE);
	if( (pdest[blocklen + CMD_HEADER_SIZE] != (mxq_u1)(crcchk))
		||
		(pdest[blocklen + CMD_HEADER_SIZE + 1] != (mxq_u1)(crcchk >> 8)) ) return MXQ_ERR_TRANS_BAD_CRC;

    *buf_len = CMD_HEADER_SIZE + blocklen + sizeof(crcchk);
	return MXQ_OK;

}

mxq_err_t CMD_SendCommand(cmd_num_t CMD, cmd_response_t *response)
{
	mxq_err_t err;
	mxq_u2 crc;
	mxq_u1* pdest;
	mxq_length mylen=0;
	mxq_length blocklen = 0;
	mxq_err_t inclen = 0;
	mxq_u2 crcchk;
	mxq_length maxlen;
	mxq_length cmddata_length = g_offset;


	/*  If expected length is NULL, we expect 0 bytes */
	/*  No way to discard incoming bytes for now */
	/*  If destination buffer is NULL, expected length must be 0 ( or NULL) */

	if(response->pdata_length==NULL)
	{
		response->pdata_length=&mylen;
	}

	if(( response->p_data==NULL) && (*response->pdata_length!=0) )
	{
		return MXQ_ERR_WRONG_PARAMETER;
	}

	pdest =  g_cmd_data - CMD_HEADER_SIZE;


	/*  Set Header */
	pdest[0] = 0xAA;
	pdest[1] = 0x00; //Commands are defined as 1 byte
	pdest[2] = CMD;
	pdest[3] = cmddata_length >> 8;
	pdest[4] = cmddata_length;

	cmddata_length += CMD_HEADER_SIZE;

	crc = HOST_CRCGet(pdest, cmddata_length);

	pdest[cmddata_length]   = crc;
	pdest[cmddata_length + 1] = crc >> 8;

	cmddata_length += sizeof(crc);

	cmd_print("Host->MAXQ: ", pdest, cmddata_length);

#ifndef NO_SECURE_CHANNEL
	if(ctx_securechannel.state == CMD_SEC_ACTIVE)
	{
		err = SC_PacketSecure(&pdest, &cmddata_length);
		if(err != MXQ_OK) return err;

		cmd_print("Host->MAXQ (SEC): ", pdest, cmddata_length);
	}
#endif 
	/*  Send the  command */
	err = sendbytes(pdest, cmddata_length);
	if(err < MXQ_OK) return err;

    /* Only used with USS */
    #if (defined USS_ST) && (defined USE_PROTOCOL_MAILBOX)  // Single threaded USS mailbox variant
    if( Receive_Process_Cmd(MXQ_TRUE) != 0 )
    {
        return MXQ_ERR_TRANS_ERROR;
    }
    #endif

	/*  **** Wait for a response **** */

	maxlen = *response->pdata_length;

	*response->pdata_length = 0; /*  Set to 0 in case of premature error */

	inclen = receivebytes_first(pdest, CMD_HEADER_SIZE);

	/* If incoming length is negative that means it is an error code */
	if(inclen < 0){
		return inclen;
	}

	if(inclen != CMD_HEADER_SIZE)
		return MXQ_ERR_TRANS_NOT_ENOUGH_DATA;

	blocklen = (pdest[3] << 8) + pdest[4];

	if ( blocklen > MAX_BUFFER_SIZE )
		return MXQ_ERR_TRANS_BUFFER_OVERFLOW; /*  Avoid RX buffer overflow */

	/*  Receive data+crc */
	inclen = receivebytes(pdest+CMD_HEADER_SIZE, blocklen+sizeof(crcchk));
	if(inclen < 0){
		return inclen;
	}

	if (inclen != (int)(blocklen+sizeof(crcchk)))
		return MXQ_ERR_TRANS_NOT_ENOUGH_DATA;

#ifndef NO_SECURE_CHANNEL
	/*  Check and remove security */
	if(ctx_securechannel.state==CMD_SEC_ACTIVE)
	{
		cmd_print("MAXQ->Host (SEC): ", pdest, blocklen + CMD_HEADER_SIZE + sizeof(crcchk));

		err = SC_PacketVerify(&pdest);
		if(err != MXQ_OK) return err;

		blocklen = (pdest[3] << 8) + pdest[4];
	}
#endif
	cmd_print("MAXQ->Host: ", pdest, blocklen + CMD_HEADER_SIZE + sizeof(crcchk));

	/*  Check inner CRC */
	crcchk = HOST_CRCGet(pdest, blocklen+CMD_HEADER_SIZE);
	if( (pdest[blocklen + CMD_HEADER_SIZE] != (mxq_u1)(crcchk))
		||
		(pdest[blocklen + CMD_HEADER_SIZE + 1] != (mxq_u1)(crcchk >> 8)) ) return MXQ_ERR_TRANS_BAD_CRC;

	/*  Get inner data and status */
	if ( blocklen > maxlen ) blocklen = maxlen;

	MXQ_MEMCPY(response->p_data, pdest + CMD_HEADER_SIZE , blocklen);

	response->status = -((pdest[1] << 8) + pdest[2]);
	*response->pdata_length = blocklen;

	return MXQ_OK;

}

mxq_err_t CMD_SendRawCommand(cmd_num_t CMD)
{
	mxq_err_t err;
	mxq_u2 crc;
	mxq_u1* pdest;
	mxq_length cmddata_length = g_offset;

	pdest =  g_cmd_data - CMD_HEADER_SIZE;

	/* Set Header */
	pdest[0] = 0xAA;
	pdest[1] = 0x00;
	pdest[2] = CMD;
	pdest[3] = cmddata_length >> 8;
	pdest[4] = cmddata_length;

	cmddata_length += CMD_HEADER_SIZE;

	crc = HOST_CRCGet(pdest, cmddata_length);

	pdest[cmddata_length] = crc;
	pdest[cmddata_length + 1] = crc >> 8;

	cmddata_length += sizeof(crc);

	err = sendbytes(pdest, cmddata_length);
	return err;
}
