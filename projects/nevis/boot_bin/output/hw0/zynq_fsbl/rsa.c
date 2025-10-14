/******************************************************************************
* Copyright (c) 2012 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file rsa.c
*
* Contains code for the RSA authentication
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 4.00a sgd	02/28/13 Initial release
* 6.00a kc	07/30/13 Added FSBL_DEBUG_RSA to print more RSA buffers
* 					 Fix for CR#724165 - Partition Header used by FSBL is
*                                        not authenticated
*                    Fix for CR#724166 - FSBL doesn’t use PPK authenticated
*                                        by Boot ROM for authenticating
*                                        the Partition images
*                    Fix for CR#722979 - Provide customer-friendly
*                                        changelogs in FSBL
* 9.00a kc  04/16/14 Fix for CR#724166 - SetPpk() will fail on secure
*					 					 fallback unless FSBL* and FSBL are
*					 					 identical in length
*					 Fix for CR#791245 - Use of xilrsa in FSBL
* 10.0  vns 03/18/22 Fixed CR#1125470 to authenticate the parition header buffer
*                    which is being used instead of one from DDR. Modified
*                    prototype of AuthenticatePartition() API
* </pre>
*
* @note
*
******************************************************************************/

/***************************** Include Files *********************************/
#ifdef RSA_SUPPORT
#include "fsbl.h"
#include "rsa.h"
#include "xilrsa.h"

#ifdef	XPAR_XWDTPS_0_BASEADDR
#include "xwdtps.h"
#endif

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/
#ifdef XPAR_XWDTPS_0_BASEADDR
extern XWdtPs Watchdog;	/* Instance of WatchDog Timer	*/
#endif


/************************** Variable Definitions *****************************/

static u8 *PpkModular;
static u8 *PpkModularEx;
static u32	PpkExp;
static u32 PpkAlreadySet=0;

extern u32 FsblLength;

void FsblPrintArray (u8 *Buf, u32 Len, char *Str)
{
#ifdef FSBL_DEBUG_RSA
	int Index;
	fsbl_printf(DEBUG_INFO, "%s START\r\n", Str);
	for (Index=0;Index<Len;Index++)
	{
		fsbl_printf(DEBUG_INFO, "%02x",Buf[Index]);
		if ((Index+1)%16 == 0){
			fsbl_printf(DEBUG_INFO, "\r\n");
		}
	}
	fsbl_printf(DEBUG_INFO, "\r\n %s END\r\n",Str);
#endif
	return;
}


/*****************************************************************************/
/**
*
* This function is used to set ppk pointer to ppk in OCM
*
* @param	None
*
* @return
*
* @note		None
*
******************************************************************************/

void SetPpk(void )
{
	u32 PadSize;
	u8 *PpkPtr;
	
	/*
	 * Set PPK only if is not already set
	 */
	if(PpkAlreadySet == 0)
	{
	
		/*
		 * Set PpkPtr to PPK in OCM
		 */
	 
		/*
		 * Skip FSBL Length
		 */
		PpkPtr = (u8 *)(FsblLength);
		/*
		 * Skip to 64 byte Boundary
		 */
		PadSize = ((u32)PpkPtr % 64);
		if(PadSize != 0)
		{
			PpkPtr += (64 - PadSize);
		}

		/*
		 * Increment the pointer by authentication Header size
		 */
		PpkPtr += RSA_HEADER_SIZE;

		/*
		 * Increment the pointer by Magic word size
		 */
		PpkPtr += RSA_MAGIC_WORD_SIZE;

		/*
		 * Set pointer to PPK
		 */
		PpkModular = (u8 *)PpkPtr;
		PpkPtr += RSA_PPK_MODULAR_SIZE;
		PpkModularEx = (u8 *)PpkPtr;
		PpkPtr += RSA_PPK_MODULAR_EXT_SIZE;
		PpkExp = *((u32 *)PpkPtr);
	
		/*
		 * Setting variable to avoid resetting PPK pointers
		 */
		PpkAlreadySet=1;
	}
	
	return;
}


/*****************************************************************************/
/**
*
* This function Authenticate Partition Signature
*
* @param	AC is the pointer to authentication certificate
* @param	Hash is the pointer which holds the SHA2 digest of data
*		to be authenticated.
*
* @return
*		- XST_SUCCESS if Authentication passed
*		- XST_FAILURE if Authentication failed
*
* @note		None
*
******************************************************************************/
u32 AuthenticatePartition(u8 *Ac, u8* Hash)
{
	u8 DecryptSignature[256];
	u8 HashSignature[32];
	u8 *SpkModular;
	u8 *SpkModularEx;
	u32 SpkExp;
	u8 *SignaturePtr;
	u32 Status;

#ifdef	XPAR_XWDTPS_0_BASEADDR
	/*
	 * Prevent WDT reset
	 */
	XWdtPs_RestartWdt(&Watchdog);
#endif

	/*
	 * Point to Authentication Certificate
	 */
	SignaturePtr = (u8 *)Ac;

	/*
	 * Increment the pointer by authentication Header size
	 */
	SignaturePtr += RSA_HEADER_SIZE;

	/*
	 * Increment the pointer by Magic word size
	 */
	SignaturePtr += RSA_MAGIC_WORD_SIZE;

	/*
	 * Increment the pointer beyond the PPK
	 */
	SignaturePtr += RSA_PPK_MODULAR_SIZE;
	SignaturePtr += RSA_PPK_MODULAR_EXT_SIZE;
	SignaturePtr += RSA_PPK_EXPO_SIZE;

	/*
	 * Calculate Hash Signature
	 */
	sha_256((u8 *)SignaturePtr, (RSA_SPK_MODULAR_EXT_SIZE +
				RSA_SPK_EXPO_SIZE + RSA_SPK_MODULAR_SIZE),
				HashSignature);
	FsblPrintArray(HashSignature, 32, "SPK Hash Calculated");

   	/*
   	 * Extract SPK signature
   	 */
	SpkModular = (u8 *)SignaturePtr;
	SignaturePtr += RSA_SPK_MODULAR_SIZE;
	SpkModularEx = (u8 *)SignaturePtr;
	SignaturePtr += RSA_SPK_MODULAR_EXT_SIZE;
	SpkExp = *((u32 *)SignaturePtr);
	SignaturePtr += RSA_SPK_EXPO_SIZE;

	/*
	 * Decrypt SPK Signature
	 */
	rsa2048_pubexp((RSA_NUMBER)DecryptSignature,
			(RSA_NUMBER)SignaturePtr,
			(u32)PpkExp,
			(RSA_NUMBER)PpkModular,
			(RSA_NUMBER)PpkModularEx);
	FsblPrintArray(DecryptSignature, RSA_SPK_SIGNATURE_SIZE,
					"SPK Decrypted Hash");


	Status = RecreatePaddingAndCheck(DecryptSignature, HashSignature);
	if (Status != XST_SUCCESS) {
		fsbl_printf(DEBUG_INFO, "Partition SPK Signature "
				"Authentication failed\r\n");
		return XST_FAILURE;
	}
	SignaturePtr += RSA_SPK_SIGNATURE_SIZE;

	/*
	 * Decrypt Partition Signature
	 */
	rsa2048_pubexp((RSA_NUMBER)DecryptSignature,
			(RSA_NUMBER)SignaturePtr,
			(u32)SpkExp,
			(RSA_NUMBER)SpkModular,
			(RSA_NUMBER)SpkModularEx);
	FsblPrintArray(DecryptSignature, RSA_PARTITION_SIGNATURE_SIZE,
					"Partition Decrypted Hash");

	Status = RecreatePaddingAndCheck(DecryptSignature, Hash);
	if (Status != XST_SUCCESS) {
		fsbl_printf(DEBUG_INFO, "Partition Signature "
				"Authentication failed\r\n");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}


/*****************************************************************************/
/**
*
* This function recreates the and check signature
*
* @param	Partition signature
* @param	Partition hash value which includes boot header, partition data
* @return
*		- XST_SUCCESS if check passed
*		- XST_FAILURE if check failed
*
* @note		None
*
******************************************************************************/
u32 RecreatePaddingAndCheck(u8 *signature, u8 *hash)
{
	u8 T_padding[] = {0x30, 0x31, 0x30, 0x0D, 0x06, 0x09, 0x60, 0x86, 0x48,
			0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20 };
    u8 * pad_ptr = signature + 256;
    u32 pad = 256 - 3 - 19 - 32;
    u32 ii;

    /*
    * Re-Create PKCS#1v1.5 Padding
    * MSB  ----------------------------------------------------LSB
    * 0x0 || 0x1 || 0xFF(for 202 bytes) || 0x0 || T_padding || SHA256 Hash
    */
    if (*--pad_ptr != 0x00 || *--pad_ptr != 0x01) {
    	return XST_FAILURE;
    }

    for (ii = 0; ii < pad; ii++) {
    	if (*--pad_ptr != 0xFF) {
        	return XST_FAILURE;
        }
    }

    if (*--pad_ptr != 0x00) {
       	return XST_FAILURE;
    }

    for (ii = 0; ii < sizeof(T_padding); ii++) {
    	if (*--pad_ptr != T_padding[ii]) {
        	return XST_FAILURE;
        }
    }

    for (ii = 0; ii < 32; ii++) {
       	if (*--pad_ptr != hash[ii])
       		return XST_FAILURE;
    }

	return XST_SUCCESS;
}
#endif
