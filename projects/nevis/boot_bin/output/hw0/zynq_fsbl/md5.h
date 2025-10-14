/******************************************************************************
* Copyright (c) 2012 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file md5.h
*
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 5.00a sgd	05/17/13 Initial release
*
* </pre>
*
* @note
*
******************************************************************************/
#ifndef ___MD5_H___
#define ___MD5_H___


#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "xil_types.h"

/************************** Constant Definitions *****************************/

#define MD5_SIGNATURE_BYTE_SIZE	64

/**************************** Type Definitions *******************************/

typedef u8 boolean;
typedef u8 signature[ MD5_SIGNATURE_BYTE_SIZE ];

struct MD5Context
	{
	u32			buffer[ 4 ];
	u32			bits[ 2 ];
	signature	intermediate;
	};
typedef struct MD5Context MD5Context;

/***************** Macros (Inline Functions) Definitions *********************/

/*
 * The four core functions - F1 is optimized somewhat
 */
#define F1( x, y, z ) ( z ^ ( x & ( y ^ z ) ) )
#define F2( x, y, z ) F1( z, x, y )
#define F3( x, y, z ) ( x ^ y ^ z )
#define F4( x, y, z ) ( y ^ ( x | ~z ) )


/*
 * This is the central step in the MD5 algorithm
 */
#define MD5_STEP( f, w, x, y, z, data, s ) \
	( w += f( x, y, z ) + data,  w = w << s | w >> ( 32 - s ),  w += x )


/************************** Function Prototypes ******************************/

void * MD5Memset( void *dest, int ch, u32 count );

void * MD5Memcpy( void *dest, const void *src, u32 count, boolean doByteSwap );

void MD5Transform( u32 *buffer, u32 *intermediate );

void MD5Init( MD5Context *context );

void MD5Update( MD5Context *context, u8 *buffer, u32 len, boolean doByteSwap );

void MD5Final( MD5Context *context, u8 *digest, boolean doByteSwap );

void md5( u8 *input, u32	len, u8 *digest, boolean doByteSwap );

/************************** Variable Definitions *****************************/

#ifdef __cplusplus
}
#endif


#endif /* ___MD5_H___ */
