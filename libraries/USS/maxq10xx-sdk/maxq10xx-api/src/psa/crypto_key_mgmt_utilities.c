/*******************************************************************************
 * Copyright (C) 2018-2019 Maxim Integrated Products, Inc. (now owned by Analog
 * Devices, Inc.), 2024 Analog Devices, Inc., All Rights Reserved. This software
 * is proprietary and confidential to Analog Devices, Inc. and its licensors.
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
 *******************************************************************************
 */



#define _XOPEN_SOURCE 700

#include "MXQ_API.h"
#include "psa/examples.h"
#include "crypto/ecc/uECC.h"
#include "crypto/hmac/sha2.h"
#include "psa/crypto_common.h"
#include "trng.h"


#include <stdlib.h>
#include <time.h>

mxq_u1 KeyPairAdmin[]=
		{// {0xd0,0x97,0x31,0xc7,0x63,0xc0,0x9e,0xe3,0x9a,0xb4,0xd0,0xce,0xa7,0x89,
		// 0xab,0x52,0xc8,0x80,0x3a,0x91,0x77,0x29,0xc3,0xa0,0x79,0x2e,0xe6,0x61,0x8b,
		// 0x2d,0x53,0x70,0xcc,0xa4,0x62,0xd5,0x4a,0x47,0x74,0xea,0x22,0xfa,0xa9,0xd4,
		// 0x95,0x4e,0xca,0x32,0x70,0x88,0xd6,0xeb,0x58,0x24,0xa3,0xc5,0xbf,0x29,0xdc,
		// 0xfd,0xe5,0xde,0x8f,0x48,
		0x19,0xe8,0xc6,0x4f,0xf2,0x46,0x10,0xe2,0x58,0xb9,0xb6,0x72,0x5e,0x88,0xaf,
		0xc2,0xee,0x8b,0x6f,0xe5,0x36,0xe3,0x60,0x7c,0xf8,0x2c,0xea,0x3a,0x4f,0xe3,
		0x6d,0x73};

mxq_u1 pub_key[]= 
	{0xd0,0x97,0x31,0xc7,0x63,0xc0,0x9e,0xe3,0x9a,0xb4,0xd0,0xce,0xa7,0x89,
		0xab,0x52,0xc8,0x80,0x3a,0x91,0x77,0x29,0xc3,0xa0,0x79,0x2e,0xe6,0x61,0x8b,
		0x2d,0x53,0x70,0xcc,0xa4,0x62,0xd5,0x4a,0x47,0x74,0xea,0x22,0xfa,0xa9,0xd4,
		0x95,0x4e,0xca,0x32,0x70,0x88,0xd6,0xeb,0x58,0x24,0xa3,0xc5,0xbf,0x29,0xdc,
		0xfd,0xe5,0xde,0x8f,0x48};

mxq_u1 KeyPairImport[]=
		{0x19,0xe8,0xc6,0x4f,0xf2,0x46,0x10,0xe2,0x58,0xb9,0xb6,0x72,0x5e,0x88,0xaf,
		0xc2,0xee,0x8b,0x6f,0xe5,0x36,0xe3,0x60,0x7c,0xf8,0x2c,0xea,0x3a,0x4f,0xe3,
		0x6d,0x73};

mxq_u1 pub_key_import[]=
{
	0xd0,0x97,0x31,0xc7,0x63,0xc0,0x9e,0xe3,0x9a,0xb4,0xd0,0xce,0xa7,0x89,
		0xab,0x52,0xc8,0x80,0x3a,0x91,0x77,0x29,0xc3,0xa0,0x79,0x2e,0xe6,0x61,0x8b,
		0x2d,0x53,0x70,0xcc,0xa4,0x62,0xd5,0x4a,0x47,0x74,0xea,0x22,0xfa,0xa9,0xd4,
		0x95,0x4e,0xca,0x32,0x70,0x88,0xd6,0xeb,0x58,0x24,0xa3,0xc5,0xbf,0x29,0xdc,
		0xfd,0xe5,0xde,0x8f,0x48
};

int Administrator_Authentication(void)
{
	mxq_u1 prng[16];
	mxq_u1 signature[64] = {0};

	/**
	 *  Get a Random Number from the MAXQ106x to perform the challenge response authentication
	 */

	ASSERTSUCCESS(MXQ_Get_Challenge(prng, sizeof(prng)));

	/**
	 *  Locally sign the random number with a valid administrator private key.
	 *  Here we use the default Maxim Import key 'KeyPairAdmin' with the public part already loaded in the MAXQ106x
	 * and fully available in the source file "host_keys.c"
	 */
	ASSERTSUCCESS(ECDSA_sign_secp256r1_sha256(signature, KeyPairAdmin, prng, sizeof(prng)));

	/**
	 * Finally to perform the authentication by the sending back the random number along with the signature
	 */
	ASSERTSUCCESS(MXQ_AdminAuth(PUBKEYADMINID, ALGO_ECDSA_SHA_256, signature, KEYCOMPLEN * 2));

	/**
	 * We are now authenticated as an Administrator
	 */
	return 0;
}

mxq_err_t get_random_u_ecc(mxq_u1* p_buff_out, mxq_length len)
{
	mxq_err_t err = MXC_TRNG_Random(p_buff_out,len);
	if(MXQ_OK == err)
	{
		return 1; // uECC requires a random number generation function that returns 1 on success
	}
	else
	{
		return 0;
	}
}

int ECDSA_sign_secp256r1_sha256(mxq_u1* dest, mxq_u1* key, mxq_u1* data, mxq_length data_length)
{
	int err = 0;	
    unsigned char hash[32];
#ifndef USS_PSA_EMULATION
	MXC_TRNG_Init();
#endif
	uECC_set_rng(get_random_u_ecc);
	sha256(data, data_length, hash);
	const struct uECC_Curve_t * curve = uECC_secp256r1();
	if(0 == err)
	{
		err = uECC_sign(key, hash, sizeof(hash), dest, curve);
	}
	return !err;
}


