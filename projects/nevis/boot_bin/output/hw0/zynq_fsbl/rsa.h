/******************************************************************************
* Copyright (c) 2012 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file rsa.h
*
* This file contains the RSA algorithm functions
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 4.00a sg	02/28/13 Initial release
* 5.0   vns     03/18/22 Modified prototype of AuthenticatePartition() API
*
* </pre>
*
* @note
*
******************************************************************************/
#ifndef ___RSA_H___
#define ___RSA_H___

#ifdef __cplusplus
extern "C" {
#endif
/***************************** Include Files *********************************/


#define RSA_PPK_MODULAR_SIZE			256
#define RSA_PPK_MODULAR_EXT_SIZE		256
#define RSA_PPK_EXPO_SIZE				64
#define RSA_SPK_MODULAR_SIZE			256
#define RSA_SPK_MODULAR_EXT_SIZE		256
#define RSA_SPK_EXPO_SIZE				64
#define RSA_SPK_SIGNATURE_SIZE			256
#define RSA_PARTITION_SIGNATURE_SIZE	256
#define RSA_SIGNATURE_SIZE				0x6C0 	/* Signature size in bytes */
#define RSA_HEADER_SIZE					4 		/* Signature header size in bytes */
#define RSA_MAGIC_WORD_SIZE				60		/* Magic word size in bytes */

void SetPpk(void );
u32 AuthenticatePartition(u8 *Ac, u8 *Hash);
u32 RecreatePaddingAndCheck(u8 *signature, u8 *hash);

#ifdef __cplusplus
}
#endif

#endif /* ___RSA_H___ */
