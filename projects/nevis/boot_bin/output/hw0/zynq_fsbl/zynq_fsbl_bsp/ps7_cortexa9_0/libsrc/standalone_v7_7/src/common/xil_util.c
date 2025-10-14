/******************************************************************************/
/**
* Copyright (c) 2019 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/****************************************************************************/
/**
* @file xil_util.c
*
* xil_util.c file contains xil utility functions
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
* 7.2   nava     08/01/20 Updated Xil_WaitForEvent() and Xil_WaitForEvents(()
*                         API to use microsecond timeout instead of a free
*                         counter.
* 7.3   kal      06/30/20 Converted Xil_Ceil macro to API.
*       rpo      08/19/20 Added function for read,modify,write
*       kal      09/22/20 Changed the param type from const char to const char*
*                         to avoid copying key onto stack
*       td       10/16/20 Added Xil_Strcpy, Xil_Strcat, Xil_SecureMemCpy and
*                         Xil_MemCmp functions
* 7.4   am       11/26/20 Added Xil_StrCpyRange function
* 7.6   kpt      07/15/21 Added Xil_SecureZeroize function
* 7.7   kpt      11/09/21 Added Xil_SMemCmp, Xil_SMemCmp_CT, Xil_SMemCpy,
*                         Xil_SMemSet, Xil_SStrCat, Xil_SStrCmp, Xil_SStrCmp_CT
*                         Xil_SStrCpy functions
*       kpt      11/25/21 Added strnlen function to fix ARMCC compilation
*                         failure
* 7.7	sk	 01/10/22 Update functions return type and update RetVal variable
* 			  data type to fix misra_c_2012_directive_4_6 misrac
* 			  violations.
* 7.7	sk	 01/10/22 Update values from signed to unsigned to fix
* 			  misra_c_2012_rule_10_4 violation.
* 7.7	sk	 01/10/22 Add explicit parentheses to fix misra_c_2012_rule_12_1
* 			  violation.
* 7.7	sk	 01/10/22 Typecast character strings to u8 to fix misra_c_2012_rule_
* 			  10_3 violation.
* 7.7	sk	 01/10/22 Modify the code to reduce multiple break statements
* 			  and fix misra_c_2012_rule_15_4 violation.
* 7.7	sk	 01/10/22 Modify Xil_SMemCmp_CT and Xil_SMemCmp function argument
* 			  type to fix misra_c_2012_rule_8_3 violation.
* 7.7	sk	 01/10/22 Update conditional expression to fix misra_c_2012_rule_14_4
* 			  violation.
*       bm       01/20/22 Fix compilation warnings in Xil_SMemCpy
*       mmd      02/28/22 Added Xil_SMemMove function
* 7.7	sk	 03/02/22 Add explicit parentheses to fix misra_c_2012_rule_12_1
* 			  violation.
* 7.7	sk	 03/02/22 Add const qualifier to varaibles to fix misra_c_2012_rule_
* 			  11_8 violation.
* 7.7	sk	 03/02/22 Typecast expression with unsigned int to fix
* 			  misra_c_2012_rule_10_7 violation.
* 7.7	sk	 03/02/22 Typecast variables with unsigned or signed to fix misra_c
* 			  _2012_rule_10_3 violation.
* 7.7	sk	 03/02/22 Add const to unmodified variable to fix misra_c_2012
* 			   _rule_8_13 violation.
* 7.7	sk	 03/02/22 Typecast the function with void as return type is
* 			  not used and fix misra_c_2012_rule_17_7 violation.
* 7.7	sk	 03/02/22 Remove increment operations during comparision to
* 			  fix misra_c_2012_rule_13_3 violation.
* 7.7	sk	 03/02/22 Update values from signed to unsigned to fix
* 			  misra_c_2012_rule_10_4 violation.
*
* </pre>
*
*****************************************************************************/

/****************************** Include Files *********************************/
#include "xil_util.h"
#include "sleep.h"

/************************** Constant Definitions ****************************/
#define MAX_NIBBLES			8U

/************************** Function Prototypes *****************************/

#ifdef __ARMCC_VERSION
/******************************************************************************/
/**
 *
 * This API returns the length of the input string
 *
 * @param  StartPtr is the pointer to the input string
 * @param  StrSize is the maximum length of the input string
 *
 * @return Returns the length of the input string
 *
 ******************************************************************************/
static size_t strnlen (const char *StartPtr, size_t StrSize)
{
	const char *EndPtr = StartPtr;
	size_t StrLen = 0U;

	EndPtr = memchr(StartPtr, '\0', StrSize);
	if (EndPtr == NULL) {
		StrLen = StrSize;
	}
	else {
		StrLen = (size_t) (EndPtr - StartPtr);
	}

	return StrLen;
}
#endif

/******************************************************************************/
/**
* This API ceils the provided float value.
*
* @param	Value is a float variable which has to ceiled to nearest
*		integer.
*
* @return	Returns ceiled value.
*
*******************************************************************************/
s32 Xil_Ceil(float Value)
{
    s32 Result = Value;

	if (Value > Result) {
        Result = Result + 1;
    }

    return Result;
}

/****************************************************************************/
/**
 * Converts the char into the equivalent nibble.
 *	Ex: 'a' -> 0xa, 'A' -> 0xa, '9'->0x9
 *
 * @param   InChar - Input character to be converted to nibble.
 *                   Valid characters are between 0-9, a-f, A-F
 * @param   Num    - Memory location where nibble is to be stored
 *
 * @return
 *          XST_SUCCESS - Character converted to nibble
 *          XST_FAILURE - Invalid input character
 *
 * @note    None.
 *
 *****************************************************************************/

u32 Xil_ConvertCharToNibble(u8 InChar, u8 *Num)
{
	u32 Status;

	/* Convert the char to nibble */
	if ((InChar >= (u8)'0') && (InChar <= (u8)'9')) {
		*Num = InChar - (u8)'0';
		Status = XST_SUCCESS;
	}
	else if ((InChar >= (u8)'a') && (InChar <= (u8)'f')) {
		*Num = InChar - (u8)'a' + 10U;
		Status = XST_SUCCESS;
	}
	else if ((InChar >= (u8)'A') && (InChar <= (u8)'F')) {
		*Num = InChar - (u8)'A' + 10U;
		Status = XST_SUCCESS;
	}
	else {
		Status = XST_FAILURE;
	}

	return Status;
}

/****************************************************************************/
/*
 * Converts the string into the equivalent Hex buffer.
 *	Ex: "abc123" -> {0xab, 0xc1, 0x23}
 *
 * @param   Str - Pointer to string to be converted to Hex.
 *                Accepted characters in string are between 0-9, a-f and A-F
 * @param   Buf - Pointer to memory location where converted hex values are to
 *                be stored.
 * @param   Len - Length of input string
 *
 * @return
 *          XST_SUCCESS - Input string is converted to hex
 *          XST_FAILURE - Invalid character in inpit string
 *
 * @note    None.
 *
 *****************************************************************************/
u32 Xil_ConvertStringToHex(const char *Str, u32 *buf, u8 Len)
{
	u32 Status = XST_FAILURE;
	u8 ConvertedLen = 0U, index = 0U;
	u8 Nibble[MAX_NIBBLES] = {0U};
	u8 i;

	while (ConvertedLen < Len) {
		for (i = 0U; i < MAX_NIBBLES; i++) {
			Status = Xil_ConvertCharToNibble((u8)Str[ConvertedLen],
			                                &Nibble[i]);
			ConvertedLen = ConvertedLen +1U;
			if (Status != XST_SUCCESS) {
				/* Error converting char to nibble */
				goto END;
			}
		}

		buf[index] = (((u32)Nibble[0] << (u8)28U) | ((u32)Nibble[1] << (u8)24U) |
		              ((u32)Nibble[2] << (u8)20U) | ((u32)Nibble[3] << (u8)16U) |
		              ((u32)Nibble[4] << (u8)12U) | ((u32)Nibble[5] << (u8)8U)  |
		              ((u32)Nibble[6] << (u8)4U)  | (u32)Nibble[7]);
		index++;
	}
END:
	return Status;
}

/****************************************************************************/
/*
 * Waits for the event
 *
 * @param   RegAddr   - Address of register to be checked for event(s) occurrence
 * @param   EventMask - Mask indicating event(s) to be checked
 * @param   Event     - Specific event(s) value to be checked
 * @param   Timeout   - Max number of microseconds to wait for an event(s).
 *
 * @return
 *          XST_SUCCESS - On occurrence of the event(s).
 *          XST_FAILURE - Event did not occur before counter reaches 0
 *
 * @note    None.
 *
 *****************************************************************************/
u32 Xil_WaitForEvent(u32 RegAddr, u32 EventMask, u32 Event, u32 Timeout)
{
	u32 EventStatus;
	u32 PollCount = Timeout;
	u32 Status = XST_FAILURE;

	while(PollCount > 0U) {
		EventStatus = Xil_In32(RegAddr) & EventMask;
		if (EventStatus == Event) {
			Status = XST_SUCCESS;
			break;
		}
		PollCount--;
		usleep(1U);
	}

	return Status;
}


/******************************************************************************/
/**
 * Waits for the events. Returns on occurrence of first event / timeout.
 *
 * @param   EventsRegAddr - Address of register to be checked for event(s)
 *                          occurrence
 * @param   EventsMask - Mask indicating event(s) to be checked
 * @param   WaitEvents - Specific event(s) to be checked
 * @param   Timeout    - Max number of microseconds to wait for an event(s).
 * @param   Events     - Mask of Events occurred returned in memory pointed by
 *                       this variable
 *
 * @return
 *          XST_SUCCESS - On occurrence of the event(s).
 *          XST_FAILURE - Event did not occur before counter reaches 0
 *
 ******************************************************************************/
u32 Xil_WaitForEvents(u32 EventsRegAddr, u32 EventsMask, u32 WaitEvents,
			 u32 Timeout, u32* Events)
{
	u32 EventStatus;
	u32 PollCount = Timeout;
	u32 Status = XST_TIMEOUT;

	*Events = 0x00;
	do {
		EventStatus = Xil_In32(EventsRegAddr);
		EventStatus &= EventsMask;
		if((EventStatus & WaitEvents) != 0U) {
			Status = XST_SUCCESS;
			*Events = EventStatus;
			break;
		}
		PollCount--;
		usleep(1U);
	}
	while(PollCount > 0U);

	return Status;
}

/******************************************************************************/
/**
 * Checks whether the passed character is a valid hex digit
 *
 * @param   Ch - Pointer to the input character
 *
 * @return
 *          XST_SUCCESS	- on valid hex digit
 *          XST_FAILURE - on invalid hex digit
 *
 * @note    None.
 *
 ******************************************************************************/
u32 Xil_IsValidHexChar(const char *Ch)
{
	u32 Status = XST_FAILURE;

	if(NULL == Ch) {
		goto END;
	}
	if (((*Ch >= '0') && (*Ch <='9'))||
		((*Ch >= 'a') && (*Ch <='f'))||
		((*Ch >= 'A') && (*Ch <='F'))) {

		Status = XST_SUCCESS;
	}
END:
	return Status;
}

/******************************************************************************/
/**
 * Validate the input string contains only hexadecimal characters
 *
 * @param   HexStr - Pointer to string to be validated
 *
 * @return
 *          XST_SUCCESS	- On valid input hex string
 *          XST_INVALID_PARAM - On invalid length of the input string
 *          XST_FAILURE	- On non hexadecimal character in string
 *
 * @note    None
 *
 ******************************************************************************/
u32 Xil_ValidateHexStr(const char *HexStr)
{
	u32 Idx;
	u32 Len;
	u32 Status = XST_INVALID_PARAM;

	if(NULL == HexStr) {
		goto END;
	}

	Len = Xil_Strnlen(HexStr, XIL_MAX_HEX_STR_LEN + 1U);
	if (Len > XIL_MAX_HEX_STR_LEN) {
		goto END;
	}

	for (Idx = 0U; Idx < Len; Idx++) {
		Status = Xil_IsValidHexChar(&HexStr[Idx]);
		if (Status != XST_SUCCESS) {
			break;
		}
	}

END:
	return Status;
}

/****************************************************************************/
/**
 * Converts the string into the equivalent Hex buffer.
 *	Ex: "abc123" -> {0xab, 0xc1, 0x23}
 *
 * @param	Str is a Input String. Will support the lower and upper case values.
 * 		Value should be between 0-9, a-f and A-F
 * @param	Buf is Output buffer.
 * @param	Len of the input string. Should have even values
 *
 * @return
 * 		- XST_SUCCESS no errors occurred.
 *		- XST_FAILURE an error when input parameters are not valid
 *		- an error when input buffer has invalid values
 *
 *	TDD Test Cases:
 *	---Initialization---
 *	Len is odd
 *	Len is zero
 *	Str is NULL
 *	Buf is NULL
 *	---Functionality---
 *	Str input with only numbers
 *	Str input with All values in A-F
 *	Str input with All values in a-f
 *	Str input with values in a-f, 0-9, A-F
 *	Str input with values in a-z, 0-9, A-Z
 *	Boundary Cases
 *	Memory Bounds of buffer checking
 * ****************************************************************************/
u32 Xil_ConvertStringToHexBE(const char *Str, u8 *Buf, u32 Len)
{
	u32 ConvertedLen;
	u8 LowerNibble = 0U;
	u8 UpperNibble = 0U;
	u32 Status = (u32)XST_FAILURE;

	if ((Str == NULL) || (Buf == NULL)) {
		Status = (u32)XST_INVALID_PARAM;
		goto END;
	}

	if ((Len == 0U) || ((Len % XIL_SIZE_OF_BYTE_IN_BITS) != 0U)) {
		Status = (u32)XST_INVALID_PARAM;
		goto END;
	}

	if(Len != (strlen(Str) * XIL_SIZE_OF_NIBBLE_IN_BITS)) {
		Status = (u32)XST_INVALID_PARAM;
		goto END;
	}

	ConvertedLen = 0U;
	while (ConvertedLen < (Len / XIL_SIZE_OF_NIBBLE_IN_BITS)) {
		if ((Xil_ConvertCharToNibble(((u8)Str[ConvertedLen]),&UpperNibble)
			== (u32)XST_SUCCESS) && (Xil_ConvertCharToNibble(((u8)Str[ConvertedLen+1U]),
				&LowerNibble) == (u32)XST_SUCCESS)) {
				Buf[ConvertedLen/2U] =
				(UpperNibble << XIL_SIZE_OF_NIBBLE_IN_BITS) |
								LowerNibble;
		}
		else {
			Status = (u32)XST_INVALID_PARAM;
			goto END;
		}
		ConvertedLen += 2U;
	}
	Status = (u32)XST_SUCCESS;
END:
	return Status;
}

/******************************************************************************/
/**
 * Converts the string into the equivalent Hex buffer.
 *	Ex: "abc123" -> {0x23, 0xc1, 0xab}
 *
 * @param   Str - Input String to be converted to hex number in little
 *                endian format. Valid characters of input strin are between
 *                0-9, a-f and A-F
 * @param   Buf - Pointer to memory location where converted hex numbers are to
 *                be stored.
 * @param   Len - Expected number of output bits
 *
 * @return
 *          XST_SUCCESS - Input string is converted to hex number(s)
 *          XST_FAILURE - Invalid input character detected in input string
 *
 * @note
 *
 ******************************************************************************/
u32 Xil_ConvertStringToHexLE(const char *Str, u8 *Buf, u32 Len)
{
	u32 ConvertedLen;
	u8 LowerNibble = 0U;
	u8 UpperNibble = 0U;
	u32 StrIndex;
	u32 Status = XST_FAILURE;

	if ((NULL == Str) || (NULL == Buf)) {
		Status = XST_INVALID_PARAM;
		goto END;
	}

	if ((Len == 0U) || ((Len % XIL_SIZE_OF_BYTE_IN_BITS) != 0U)) {
		Status = XST_INVALID_PARAM;
		goto END;
	}

	if(Len != (strlen(Str) * XIL_SIZE_OF_NIBBLE_IN_BITS)) {
		Status = XST_INVALID_PARAM;
		goto END;
	}

	StrIndex = (Len / XIL_SIZE_OF_BYTE_IN_BITS) - 1U;
	ConvertedLen = 0U;
	while (ConvertedLen < (Len / XIL_SIZE_OF_NIBBLE_IN_BITS)) {
		if ((Xil_ConvertCharToNibble(((u8)Str[ConvertedLen]),
						&UpperNibble) == XST_SUCCESS) &&
			(Xil_ConvertCharToNibble(((u8)Str[ConvertedLen + 1U]),
						&LowerNibble) == XST_SUCCESS)) {
				Buf[StrIndex] =
				   (UpperNibble << XIL_SIZE_OF_NIBBLE_IN_BITS) |
				   LowerNibble;
				StrIndex = StrIndex - 1U;
		}
		else {
			Status = XST_INVALID_PARAM;
			goto END;
		}
		ConvertedLen += 2U;
	}

	Status = XST_SUCCESS;
END:
	return Status;
}

/******************************************************************************/
/**
 * Returns the length of input string.
 *
 * @param   Str - Input string
 * @param   MaxLen - Maximum expected length of the input string
 *
 * @return
 *          Returns length of the input string if length is less than MaxLen.
 *          Returns MaxLen if the length of the input string is >= MaxLen.
 *
 * @note
 *
 ******************************************************************************/
u32 Xil_Strnlen(const char *Str, u32 MaxLen)
{
	const char *InStr = Str;
	u32 StrLen = 0U;

	if (NULL == Str) {
		goto END;
	}

	while(StrLen < MaxLen) {
		if ('\0' == *InStr) {
			break;
		}
		StrLen++;
		InStr++;
	}

END:
	return StrLen;
}

/*****************************************************************************/
/**
 * @brief	This function will Read, Modify and Write to an address.
 *
 * @param	Addr denotes Address
 * @param	Mask denotes the bits to be modified
 * @param	Value is the value to be written to the address
 *
 * @return	None
 *
 *****************************************************************************/
void Xil_UtilRMW32(u32 Addr, u32 Mask, u32 Value)
{
	u32 Val;

	Val = Xil_In32(Addr);
	Val = (Val & (~Mask)) | (Mask & Value);
	Xil_Out32(Addr, Val);
}

/*****************************************************************************/
/**
 * @brief	This functions copies source string to destination string. This
 * 			function is a safe version of strcpy
 *
 * @param	DestPtr is pointer to destination string
 * @param	SrcPtr is pointer to source string
 * @param	Size is the maximum number of bytes of the source string
 *			to be copied
 *
 * @return	XST_SUCCESS on success and error code on failure
 *
 ******************************************************************************/
int Xil_Strcpy(char *DestPtr, const char *SrcPtr, const u32 Size)
{
	int Status = XST_FAILURE;
	u32 Count;

	if ((SrcPtr == NULL) || (DestPtr == NULL) || (Size == 0U)) {
		goto END;
	}

	for (Count = 0U; (SrcPtr[Count] != '\0') && (Count < Size); ++Count) {
		DestPtr[Count] = SrcPtr[Count];
	}
	if (Count == Size) {
		DestPtr[0U] = '\0';
		goto END;
	}
	DestPtr[Count] = '\0';
	Status = XST_SUCCESS;

END:
	return Status;
}

/****************************************************************************/
/**
 * @brief	Copies specified range from source string to destination string
 *
 * @param	Src  is a pointer to source string
 * @param	Dest  is a pointer to destination string
 * @param	From is 0 based index from where string copy starts
 * @param	To   is 0 based index till which string is copied
 * @param	MaxSrcLen is the maximum length of source string
 * @param	MaxDstLen is the maximum length of destination string
 *
 * @return	XST_SUCCESS on success
 * 		XST_FAILURE on failure
 *
 * @note	None
 *
 ****************************************************************************/
int Xil_StrCpyRange(const u8 *Src, u8 *Dest, u32 From, u32 To, u32 MaxSrcLen,
	u32 MaxDstLen)
{
	int Status = XST_FAILURE;
	u32 SrcLength;
	u32 Index;

	if ((Src == NULL) || (Dest == NULL)) {
		Status = XST_INVALID_PARAM;
		goto END;
	}

	if ((To >= MaxSrcLen) || (To < From)) {
		Status = XST_INVALID_PARAM;
		goto END;
	}

	if ((To - From + 1U) >= MaxDstLen) {
		Status = XST_INVALID_PARAM;
		goto END;
	}

	SrcLength = Xil_Strnlen((const char *)Src, MaxSrcLen);
	if (To >= SrcLength) {
		Status = XST_INVALID_PARAM;
		goto END;
	}

	for (Index = From; (Index <= To) && (Src[Index]!= (u8)'\0'); Index++) {
		Dest[Index - From] = Src[Index];
	}

	Dest[Index - From] = (u8)'\0';
	Status = XST_SUCCESS;

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function appends string2 to string1. This function is a safe
 * 			version of strcat
 *
 * @param	Str1Ptr is pointer to string1
 * @param	Str2Ptr is pointer to string2
 * @param	Size is the maximum number of bytes Str1 can hold
 *
 * @return	XST_SUCCESS on success and error code on failure
 *
 ******************************************************************************/
int Xil_Strcat(char* Str1Ptr, const char* Str2Ptr, const u32 Size)
{
	int Status = XST_FAILURE;
	u32 Count = 0U;
	u32 CountTmp = 0U;

	if ((Str1Ptr == NULL) || (Str2Ptr == NULL) || (Size == 0U)) {
		goto END;
	}

	while ((Count < Size) && (Str1Ptr[Count] != '\0')) {
		Count++;
	}

	while ((Str2Ptr[CountTmp] != '\0') && (Count < Size)) {
		Str1Ptr[Count] = Str2Ptr[CountTmp];
		Count++;
		CountTmp++;
	}
	if (Count == Size) {
		Str1Ptr[0U] = '\0';
		goto END;
	}
	Str1Ptr[Count] = '\0';
	Status = XST_SUCCESS;

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function copies Len bytes from source memory to destination
 *			memory. If Len is greater than DestPtrLen, then DestPtr is also
 *			filled with 0s till DestPtrLen bytes and is considered as a failure.
 *			This function is a secure implementation of memcpy
 *
 * @param	DestPtr is pointer to destination address
 * @param	DestPtrLen is the memory alloted to the destination buffer
 * @param	SrcPtr is pointer to source address
 * @param	Len is number of bytes to be copied
 *
 * @return	XST_SUCCESS on success and error code on failure
 *
 ******************************************************************************/
int Xil_SecureMemCpy(void * DestPtr, u32 DestPtrLen, const void * SrcPtr, u32 Len)
{
	int Status = XST_FAILURE;
	u8 *Dest = (u8 *)DestPtr;
	const u8 *Src = (const u8 *)SrcPtr;

	if ((DestPtr == NULL) || (SrcPtr == NULL)) {
		goto END;
	}

	if (Len > DestPtrLen) {
		while (DestPtrLen != 0U) {
			*Dest = 0U;
			Dest++;
			DestPtrLen--;
		}
		goto END;
	}

	/* Loop and copy.  */
	while (Len != 0U) {
		*Dest = *Src;
		Dest++;
		Src++;
		Len--;
	}
	Status = XST_SUCCESS;

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function compares Len bytes from memory1 and memory2. This
 * 			function is a secure implementation of memcmp
 *
 * @param	Buf1Ptr is pointer to memory1
 * @param	Buf2Ptr is pointer to memory2
 * @param	Len is number of byets to be compared
 *
 * @return	0 if contents of both the memory regions are same,
 * 			-1 if first non-matching character has lower value in Buf1Ptr
 * 			1 if first non-matching character is greater value in Buf1Ptr
 *
 ******************************************************************************/
int Xil_MemCmp(const void * Buf1Ptr, const void * Buf2Ptr, u32 Len)
{
	volatile int RetVal = 1;
	const u8 *Buf1 = Buf1Ptr;
	const u8 *Buf2 = Buf2Ptr;
	u32 Size = Len;

	/* Assert validates the input arguments */
	if ((Buf1 == NULL) || (Buf2 == NULL) || (Len == 0x0U)) {
		goto END;
	}

	/* Loop and compare */
	while (Size != 0U) {
		if (*Buf1 > *Buf2) {
			RetVal = 1;
			goto END;
		} else if (*Buf1 < *Buf2) {
			RetVal = -1;
			goto END;
		} else {
			Buf1++;
			Buf2++;
			Size--;
		}
	}

	/* Make sure size is zero to know the whole of data is compared */
	if (Size == 0U) {
		RetVal = 0;
	}

END:
	return RetVal;
}

/*****************************************************************************/
/**
 * @brief	This function is used to zeroize the memory
 *
 * @param	DataPtr Pointer to the memory which need to be zeroized.
 * @param	Length	Length of the data in bytes.
 *
 * @return
 *		- XST_SUCCESS: If Zeroization is successful.
 *		- XST_FAILURE: If Zeroization is not successful.
 ********************************************************************************/
int Xil_SecureZeroize(u8 *DataPtr, const u32 Length)
{
	u32 Index;
	int Status = XST_FAILURE;

	/* Clear the data */
	(void)memset(DataPtr, 0, Length);

	/* Read it back to verify */
	 for (Index = 0U; Index < Length; Index++) {
		if (DataPtr[Index] != 0x00U) {
			goto END;
		}
	}
	if (Index == Length) {
		Status = XST_SUCCESS;
	}

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function compares two memory regions for specified number
 *		of bytes. This function takes size of two memory regions to
 *		make sure not to read from or write to out of bound memory
 *		region.
 *
 * @param	Src1     - Pointer to first memory range
 * @param	Src1Size - Maximum size of first memory range
 * @param	Src2     - Pointer to second memory range
 * @param	Src2Size - Maximum size of second memory range
 * @param	CmpLen   - Number of bytes to be compared
 *
 * @return
 *		XST_SUCCESS - If specified number of bytes matches
 * 		XST_FAILURE - If there is a mistmatch found during comparison
 * 		XST_INVALID_PARAM - Invalid inputs
 *
 *****************************************************************************/
int Xil_SMemCmp(const void *Src1, const u32 Src1Size,
	const void *Src2, const u32 Src2Size, const u32 CmpLen)
{
	int Status = XST_FAILURE;

	if ((Src1 == NULL) || (Src2 == NULL)) {
		Status =  XST_INVALID_PARAM;
	}
	else if ((CmpLen == 0U) || (Src1Size < CmpLen) || (Src2Size < CmpLen)) {
		Status =  XST_INVALID_PARAM;
	}
	else {
		Status = memcmp (Src1, Src2, CmpLen);
		if (Status != 0) {
			Status = XST_FAILURE;
		}
	}

	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function compares two memory regions for specified number
 *		of bytes. This function takes size of two memory regions to
 *		make sure not to read from or write to out of bound memory
 *		region.
 *		Note that this function compares till end to make it execute
 *		in constant time irrespective of the content of input memory.
 *
 * @param	Src1     - Pointer to first memory range
 * @param	Src1Size - Maximum size of first memory range
 * @param	Src2     - Pointer to second memory range
 * @param	Src2Size - Maximum size of second memory range
 * @param	CmpLen   - Number of bytes to be compared
 *
 * @return
 *		XST_SUCCESS - If specified number of bytes matches
 * 		XST_FAILURE - If mistmatch
 * 		XST_INVALID_PARAM - Invalid inputs
 *
 *****************************************************************************/
int Xil_SMemCmp_CT(const void *Src1, const u32 Src1Size,
	const void *Src2, const u32 Src2Size, const u32 CmpLen)
{
	volatile int Status = XST_FAILURE;
	volatile int StatusRedundant = XST_FAILURE;
	volatile u32 Data = 0U;
	volatile u32 DataRedundant = 0xFFFFFFFFU;
	u32 Cnt = CmpLen;
	const u8 *Src_1 = (const u8 *)Src1;
	const u8 *Src_2 = (const u8 *)Src2;


	if ((Src1 == NULL) || (Src2 == NULL)) {
		Status =  XST_INVALID_PARAM;
	}
	else if ((CmpLen == 0U) || (Src1Size < CmpLen) || (Src2Size < CmpLen)) {
		Status =  XST_INVALID_PARAM;
	}
	else {
		while (Cnt >= sizeof(u32)) {
			Data |= (*(const u32 *)Src_1 ^ *(const u32 *)Src_2);
			DataRedundant &= ~Data;
			Src_1 += sizeof(u32);
			Src_2 += sizeof(u32);
			Cnt -= sizeof(u32);
		}

		while (Cnt > 0U) {
			Data |= (u32)(*Src_1 ^ *Src_2);
			DataRedundant &= ~Data;
			Src_1++;
			Src_2++;
			Cnt--;
		}

		if ((Data == 0U) && (DataRedundant == 0xFFFFFFFFU)) {
			Status = XST_SUCCESS;
			StatusRedundant = XST_SUCCESS;
		}
	}

	return (Status | StatusRedundant);
}

/*****************************************************************************/
/**
 * @brief	This is wrapper function to memcpy function. This function
 *		takes size of two memory regions to make sure not read from
 *		or write to out of bound memory region.
 *
 * @param	Dest      - Pointer to destination memory
 * @param	DestSize  - Memory available at destination
 * @param	Src       - Pointer to source memory
 * @param	SrcSize   - Maximum data that can be copied from source
 * @param	CopyLen   - Number of bytes to be copied
 *
 * @return
 *		XST_SUCCESS - Copy is successful
 * 		XST_INVALID_PARAM - Invalid inputs
 *
 *****************************************************************************/
int Xil_SMemCpy(void *Dest, const u32 DestSize,
	const void *Src, const u32 SrcSize, const u32 CopyLen)
{
	int Status = XST_FAILURE;
	const u8 *Src8 = (const u8 *) Src;
	const u8 *Dst8 = (u8 *) Dest;
	void * volatile DestTemp = Dest;
	const void * volatile SrcTemp = Src;

	if ((Dest == NULL) || (Src == NULL)) {
		Status =  XST_INVALID_PARAM;
	}
	else if ((CopyLen == 0U) || (DestSize < CopyLen) || (SrcSize < CopyLen)) {
		Status =  XST_INVALID_PARAM;
	}
	/* Return error for overlap string */
	else if ((Src8 < Dst8) && (&Src8[CopyLen - 1U] >= Dst8)) {
		Status =  XST_INVALID_PARAM;
	}
	else if ((Dst8 < Src8) && (&Dst8[CopyLen - 1U] >= Src8)) {
		Status =  XST_INVALID_PARAM;
	}
	else {
		(void)memcpy(DestTemp, SrcTemp, CopyLen);
		Status = XST_SUCCESS;
	}

	return Status;
}

/*****************************************************************************/
/**
 * @brief	This is wrapper function to memset function. This function
 *		writes specified byte to destination specified number of times.
 *		This function also takes maximum string size that destination
 *		holds to make sure not to write out of bound area.
 *
 * @param	Dest     - Pointer to destination memory
 * @param	DestSize - Memory available at destination
 * @param	Data     - Any value from 0 to 255
 * @param	Len      - Number of bytes to be copied
 *
 * @return
 *		XST_SUCCESS - Copy is successful
 * 		XST_INVALID_PARAM - Invalid inputs
 *
 *****************************************************************************/
int Xil_SMemSet(void *Dest, const u32 DestSize,
	const u8 Data, const u32 Len)
{
	int Status = XST_FAILURE;

	if ((Dest == NULL) || (DestSize < Len) || (Len == 0U)) {
		Status =  XST_INVALID_PARAM;
	}
	else {
		(void)memset(Dest, (s32)Data, Len);
		Status = XST_SUCCESS;
	}

	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function concatenates two strings. This function
 *		takes size of both strings to make sure not to read from /
 *		write to out of bound area.
 *
 * @param	DestStr  - Pointer to destination string
 * @param	DestSize - Maximum string size that detination can hold
 * @param	SrcStr   - Pointer to source string
 * @param	SrcSize  - Maximum string size that source can hold
 *
 * @return
 *		XST_SUCCESS - Copy is successful
 * 		XST_INVALID_PARAM - Invalid inputs
 *
 *****************************************************************************/
int Xil_SStrCat (u8 *DestStr, const u32 DestSize,
	const u8 *SrcStr, const u32 SrcSize)
{
	int Status = XST_FAILURE;
	u32 SrcLen;
	u32 DstLen;

	if ((DestStr == NULL) || (SrcStr == NULL)) {
		Status =  XST_INVALID_PARAM;
		goto END;
	}

	SrcLen = strnlen((const char*)SrcStr, SrcSize);
	DstLen = strnlen((const char*)DestStr, DestSize);

	if ((DestSize <= DstLen) || (SrcSize <= SrcLen)) {
		Status =  XST_INVALID_PARAM;
	}
	else if (DestSize <= (SrcLen + DstLen)) {
		Status =  XST_INVALID_PARAM;
	}
	else {
		(void)strcat((char*)DestStr, (const char*)SrcStr);
		Status = XST_SUCCESS;
	}

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function compares two strings. It also takes maximum string
 *		size that Src1 and Src2 can hold to make sure not to read out of
 *		bound data for comparison.
 *
 * @param	Str1     - Pointer to first string
 * @param	Str1Size - Maximum string size that Str1 can hold
 * @param	Str2     - Pointer to second string
 * @param	Str2Size - Maximum string size that Str2 can hold
 *
 * @return
 *		XST_SUCCESS - If both strings are same
 *		XST_FAILURE - If there is difference between two strings
 * 		XST_INVALID_PARAM - Invalid inputs
 *
 *****************************************************************************/
int Xil_SStrCmp(const u8 *Str1, const u32 Str1Size,
	const u8 *Str2, const u32 Str2Size)
{
	int Status = XST_FAILURE;
	u32 Str1Len = 0U;
	u32 Str2Len = 0U;

	if ((Str1 == NULL) || (Str2 == NULL)) {
		Status =  XST_INVALID_PARAM;
		goto END;
	}

	Str1Len = strnlen((const char*)Str1, Str1Size);
	Str2Len = strnlen((const char*)Str2, Str2Size);

	if ((Str1Size <= Str1Len) || (Str2Size <= Str2Len)) {
		Status =  XST_INVALID_PARAM;
	}
	else if ((Str1Len < Str2Len) || (Str1Len > Str2Len)) {
		Status = XST_FAILURE;
	}
	else {
		Status = memcmp(Str1, Str2, Str1Len);
		if (Status != 0) {
			Status = XST_FAILURE;
		}
	}

END:
	return Status;
}


/*****************************************************************************/
/**
 * @brief	This function compares two strings. It also takes maximum string
 *		size that Src1 and Src2 can hold to make sure not to read out of
 *		bound data for comparison. This function compares each character
 *		of the strings so that execution time of the function is not
 *		dependent on the content of two input strings. The execution
 *		time is constant when string size are same.
 *
 * @param	Str1     - Pointer to first string
 * @param	Str1Size - Maximum string size that Str1 can hold
 * @param	Str2     - Pointer to second string
 * @param	Str2Size - Maximum string size that Str2 can hold
 *
 * @return
 *		XST_SUCCESS - If both strings are same
 *		XST_FAILURE - If there is difference between two strings
 * 		XST_INVALID_PARAM - Invalid inputs
 *
 *****************************************************************************/
int Xil_SStrCmp_CT (const u8 *Str1, const u32 Str1Size,
	const u8 *Str2, const u32 Str2Size)
{
	int Status = XST_FAILURE;
	u32 Str1Len = 0U;
	u32 Str2Len = 0U;

	if ((Str1 == NULL) || (Str2 == NULL)) {
		Status =  XST_INVALID_PARAM;
		goto END;
	}

	Str1Len = strnlen((const char*)Str1, Str1Size);
	Str2Len = strnlen((const char*)Str2, Str2Size);

	if ((Str1Size <= Str1Len) || (Str2Size <= Str2Len)) {
		Status =  XST_INVALID_PARAM;
	}
	else if (Str1Len != Str2Len) {
		Status = XST_FAILURE;
	}
	else {
		Status = Xil_SMemCmp_CT (Str1, Str1Size, Str2, Str2Size, Str1Len);
	}

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This function copies one string to other. This function
 *		takes size of both strings to make sure not to read from /
 *		write to out of bound area.
 *
 * @param	DestStr  - Pointer to destination string
 * @param	DestSize - Maximum string size that detination can hold
 * @param	SrcStr   - Pointer to source string
 * @param	SrcSize  - Maximum string size that source can hold
 *
 * @return
 *		XST_SUCCESS - Copy is successful
 * 		XST_INVALID_PARAM - Invalid inputs
 *
 *****************************************************************************/
int Xil_SStrCpy(u8 *DestStr, const u32 DestSize,
	const u8 *SrcStr, const u32 SrcSize)
{
	int Status = XST_FAILURE;
	u32 SrcLen = 0U;

	if ((DestStr == NULL) || (SrcStr == NULL)) {
		Status =  XST_INVALID_PARAM;
		goto END;
	}

	SrcLen = strnlen((const char*)SrcStr, SrcSize);

	if ((DestSize <= SrcLen) || (SrcSize <= SrcLen)) {
		Status =  XST_INVALID_PARAM;
	}
	else {
		(void)memcpy(DestStr, SrcStr, SrcLen + 1U);
		Status = XST_SUCCESS;
	}

END:
	return Status;
}

/*****************************************************************************/
/**
 * @brief	This is wrapper function to memmove function. This function
 *		takes size of two memory regions to avoid out of bound memory region.
 *
 * @param	Dest      - Pointer to destination memory
 * @param	DestSize  - Memory available at destination
 * @param	Src       - Pointer to source memory
 * @param	SrcSize   - Maximum data that can be copied from source
 * @param	CopyLen   - Number of bytes to be copied
 *
 * @return
 *		XST_SUCCESS - Copy is successful
 * 		XST_INVALID_PARAM - Invalid inputs
 *
 *****************************************************************************/
int Xil_SMemMove(void *Dest, const u32 DestSize,
	const void *Src, const u32 SrcSize, const u32 CopyLen)
{
	volatile int Status = XST_FAILURE;
	void *Output = NULL;

	if ((Dest == NULL) || (Src == NULL)) {
		Status =  XST_INVALID_PARAM;
	}
	else if ((CopyLen == 0U) || (DestSize < CopyLen) || (SrcSize < CopyLen)) {
		Status =  XST_INVALID_PARAM;
	}
	else {
		Output = memmove(Dest, Src, CopyLen);
		if (Output != NULL) {
			Status = XST_SUCCESS;
		}
	}

	return Status;
}