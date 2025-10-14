/******************************************************************************/
/**
* Copyright (c) 2019 - 2022  Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/****************************************************************************/
/**
* @file xil_util.h
* @addtogroup common_utilities Common Utility APIs
* @{
* @details
*
* xil_util.h file contains xil utility functions declarations
* Except few functions, most of these functions are wrappers to standard functions.
* The standard string functions do not validate the input and that results into
* buffer overflows. To avoid it, the wrapper function validates the input and
* then passed to standard function. There are few constant time functions
* ( xxx_CT() ) which are used to compare the data in constant time.
* The constant time functions should be used while comparing secure data
* like password, keys which prevent disclosing of the data using
* timing analysis.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 6.4   mmd      04/21/19 First release.
* 6.5   kal      02/29/20 Added Xil_ConvertStringToHexBE API
* 7.3   kal      06/30/20 Converted Xil_Ceil macro to API
*       rpo      08/19/20 Added function for read, modify and write
*       bsv      08/21/20 Added XSECURE_TEMPORAL_CHECK macro to add
*                         redundancy in security critical functions, to avoid
*                         glitches from altering the return values of security
*                         critical functions. The macro requires a label to be
*                         passed to "go to" in case of error.
*      kpt       09/03/20 Added XSECURE_TEMPORAL_IMPL macro for redundancy
*      kal       09/22/20 Changed the param type from const char to const char*
*                         to avoid copying key onto stack
*      td        10/16/20 Added Xil_Strcpy, Xil_Strcat, Xil_SecureMemCpy and
*                         Xil_MemCmp functions
*      am        10/13/20 Resolved Coverity warning
*      td        11/19/20 Updated XSECURE_TEMPORAL_CHECK and
*                         XSECURE_TEMPORAL_IMPL to fix MISRA C Rule 15.3
* 7.4  am        11/26/20 Added Xil_StrCpyRange function
* 7.6  kpt       07/15/21 Added Xil_SecureZeroize function
* 7.7  kpt       11/09/21 Added Xil_SMemCmp, Xil_SMemCmp_CT, Xil_SMemCpy,
*                         Xil_SMemSet, Xil_SStrCat, Xil_SStrCmp, Xil_SStrCmp_CT
*                         Xil_SStrCpy functions
* 7.7	sk	 01/10/22 Update functions return type to fix misra_c_2012_
* 			  directive_4_6 violations.
*      mmd       02/28/22 Added Xil_SMemMove function prototype
*
* </pre>
*
*****************************************************************************/

#ifndef XIL_UTIL_H_
#define XIL_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "xil_types.h"
#include "xil_io.h"
#include "xstatus.h"

/*************************** Constant Definitions *****************************/
#define XIL_SIZE_OF_NIBBLE_IN_BITS	4U
#define XIL_SIZE_OF_BYTE_IN_BITS	8U

/* Maximum string length handled by Xil_ValidateHexStr function */
#define XIL_MAX_HEX_STR_LEN	512U


/****************** Macros (Inline Functions) Definitions *********************/
#ifdef __GNUC__
/******************************************************************************/
/**
 *
 * Updates the return value of the called function into Var and VarTmp variables
 * for redundancy. This is to avoid glitches from altering the return values of
 * security critical functions.
 *
 * @param   Var is the variable which holds the return value of function
 *          executed
 * @param   VarTmp is the variable which holds the value stored in Var
 * @param	Function is the function to be executed
 * @param	Other params are arguments to the called function
 *
 * @return	None
 *
 ******************************************************************************/
#define XSECURE_TEMPORAL_IMPL(Var, VarTmp, Function, ...) \
		{ \
			Var = XST_FAILURE; \
			VarTmp = XST_FAILURE; \
			Var = Function(__VA_ARGS__); \
			VarTmp = Var; \
		}

/******************************************************************************/
/**
 *
 * Adds redundancy while checking the status of the called function.
 * This is to avoid glitches from altering the return values of security
 * critical functions. The macro requires a label to be passed to "go to"
 * in case of error.
 *
 * @param   Label is the label defined in function and the control
 *          will jump to the label in case of XST_FAILURE
 * @param   Status is the variable which holds the return value of
 *          function executed
 * @param	Function is the function to be executed
 * @param	Other params are arguments to the called function
 *
 * @return	None
 *
 ******************************************************************************/
#define XSECURE_TEMPORAL_CHECK(Label, Status, Function, ...)   \
	{ \
		volatile int StatusTmp = XST_FAILURE; \
		XSECURE_TEMPORAL_IMPL(Status, StatusTmp, Function, __VA_ARGS__); \
		if ((Status != XST_SUCCESS) || \
			(StatusTmp != XST_SUCCESS)) { \
			Status |= StatusTmp;\
			goto Label; \
		} \
	 }
#endif

/*************************** Function Prototypes ******************************/
/* Ceils the provided float value */
s32 Xil_Ceil(float Value);

/* Converts input character to nibble */
u32 Xil_ConvertCharToNibble(u8 InChar, u8 *Num);

/* Convert input hex string to array of 32-bits integers */
u32 Xil_ConvertStringToHex(const char *Str, u32 *buf, u8 Len);

/* Waits for specified event */
u32 Xil_WaitForEvent(u32 RegAddr, u32 EventMask, u32 Event, u32 Timeout);

/* Waits for specified events */
u32 Xil_WaitForEvents(u32 EventsRegAddr, u32 EventsMask, u32 WaitEvents,
			 u32 Timeout, u32* Events);

/* Validate input hex character */
u32 Xil_IsValidHexChar(const char *Ch);

/* Validate the input string contains only hexadecimal characters */
u32 Xil_ValidateHexStr(const char *HexStr);

/* Convert string to hex numbers in little enidian format */
u32 Xil_ConvertStringToHexLE(const char *Str, u8 *Buf, u32 Len);

/* Returns length of the input string */
u32 Xil_Strnlen(const char *Str, u32 MaxLen);

/* Convert string to hex numbers in big endian format */
u32 Xil_ConvertStringToHexBE(const char * Str, u8 * Buf, u32 Len);

/*Read, Modify and Write to an address*/
void Xil_UtilRMW32(u32 Addr, u32 Mask, u32 Value);

/* Copies source string to destination string */
int Xil_Strcpy(char *DestPtr, const char *SrcPtr, const u32 Size);

/* Copies specified range from source string to destination string */
int Xil_StrCpyRange(const u8 *Src, u8 *Dest, u32 From, u32 To, u32 MaxSrcLen,
	u32 MaxDstLen);

/* Appends string2 to string1 */
int Xil_Strcat(char* Str1Ptr, const char* Str2Ptr, const u32 Size);

/* Copies Len bytes from source memory to destination memory */
int Xil_SecureMemCpy(void * DestPtr, u32 DestPtrLen, const void * SrcPtr, u32 Len);

/* Compares Len bytes from memory1 and memory2 */
int Xil_MemCmp(const void * Buf1Ptr, const void * Buf2Ptr, u32 Len);

/* Zeroizes the memory of given length */
int Xil_SecureZeroize(u8 *DataPtr, const u32 Length);

/* Copies Len bytes from source memory to destination memory */
int Xil_SMemCpy (void *Dest, const u32 DestSize,
	const void *Src, const u32 SrcSize, const u32 CopyLen);

/* Copies Len bytes from source memory to destination memory, allows
   overlapped memory between source and destination */
int Xil_SMemMove(void *Dest, const u32 DestSize,
	const void *Src, const u32 SrcSize, const u32 CopyLen);

/* Compares Len bytes between source and destination memory */
int Xil_SMemCmp (const void *Src1, const u32 Src1Size,
	const void *Src2, const u32 Src2Size, const u32 CmpLen);

/* Compares Len bytes between source and destination memory with constant time */
int Xil_SMemCmp_CT (const void *Src1, const u32 Src1Size,
	const void *Src2, const u32 Src2Size, const u32 CmpLen);

/* Sets the destination memory of given length with given data */
int Xil_SMemSet (void *Dest, const u32 DestSize,
	const u8 Data, const u32 Len);

/* Copies source string to destination string */
int Xil_SStrCpy (u8 *DestStr, const u32 DestSize,
	const u8 *SrcStr, const u32 SrcSize);

/* Compares source string with destination string */
int Xil_SStrCmp (const u8 *Str1, const u32 Str1Size,
	const u8 *Str2, const u32 Str2Size);

/* Compares source string with destination string with constant time */
int Xil_SStrCmp_CT (const u8 *Str1, const u32 Str1Size,
	const u8 *Str2, const u32 Str2Size);

/* Concatenates source string to destination string */
int Xil_SStrCat (u8 *DestStr, const u32 DestSize,
	const u8 *SrcStr, const u32 SrcSize);

#ifdef __cplusplus
}
#endif

#endif	/* XIL_UTIL_H_ */
/**
* @} End of "addtogroup common_utilities".
*/
