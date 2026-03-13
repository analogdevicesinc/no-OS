 /**
 * \file adrv903x_platform_byte_order.h
 * 
 * \brief   Contains platform byte order (endianness) conversion macros
 *
 * \details Contains platform byte order (endianness) conversion macros
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_PLATFORM_BYTE_ORDER_H__
#define __ADRV903X_PLATFORM_BYTE_ORDER_H__

#ifndef ADI_ADRV903X_FW
#include "adi_library_types.h"
#include "adi_adrv903x_user.h"
#ifdef ADI_ADRV903X_LITTLE_ENDIAN
#define ADRV903X_LITTLE_ENDIAN ADI_ADRV903X_LITTLE_ENDIAN
#endif
#endif

#ifndef ADRV903X_LITTLE_ENDIAN
#ifdef __GNUC__
#define ADRV903X_LITTLE_ENDIAN (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#elif defined  __ICCARM__
#define ADRV903X_LITTLE_ENDIAN (__LITTLE_ENDIAN__ == 1)
#else
#error( "Define the ADRV903X_LITTLE_ENDIAN macro for your platform.")
#endif
#endif

/*
 * Endianess conversion macros naming:
 *
 *   C is the adrv903x's CPU byte-order i.e. little-endian 
 *   
 *   H is the byte-order of the host on which the API is running which can be either big or little endian. Setting
 *   ADRVGEN_LITTLE_ENDIAN to match the endianess of the host will cuase the xTOy conversion macros swap bytes or not as
 *   appropriate for the host endianess.  
 *
 *   N is 'network byte-order' i.e. big-endian.
 */

#define ADRV903X_BYTESWAP_S(x) \
    ((((x) & 0xFF00u) >> 8) | \
     (((x) & 0x00FFu) << 8))

#define ADRV903X_BYTESWAP_L(x) \
    ((((x) & 0xFF000000ul) >> 24) | \
     (((x) & 0x00FF0000ul) >> 8)  | \
     (((x) & 0x0000FF00ul) << 8)  | \
     (((x) & 0x000000FFul) << 24))

#define ADRV903X_BYTESWAP_LL( x ) \
    ((((x) & 0xFF00000000000000ull) >> 56u) | \
     (((x) & 0x00FF000000000000ull) >> 40u) | \
     (((x) & 0x0000FF0000000000ull) >> 24u) | \
     (((x) & 0x000000FF00000000ull) >>  8u) | \
     (((x) & 0x00000000FF000000ull) <<  8u) | \
     (((x) & 0x0000000000FF0000ull) << 24u) | \
     (((x) & 0x000000000000FF00ull) << 40u) | \
     (((x) & 0x00000000000000FFull) << 56u))

#define ADRV903X_NTOCL( x ) ADRV903X_BYTESWAP_L((x))
#define ADRV903X_CTONL( x ) ADRV903X_BYTESWAP_L((x))

#if ADRV903X_LITTLE_ENDIAN == 1
/* Host is LE */
#define ADRV903X_HTOCLL( x ) ( x )
#define ADRV903X_CTOHLL( x ) ( x )
#define ADRV903X_HTOCL( x )  ( x )
#define ADRV903X_CTOHL( x )  ( x )
#define ADRV903X_HTOCS( x )  ( x )
#define ADRV903X_CTOHS( x )  ( x )
#define ADRV903X_NTOHL( x ) ADRV903X_BYTESWAP_L((x))
#define ADRV903X_HTONL( x ) ADRV903X_BYTESWAP_L((x))
#elif ADRV903X_LITTLE_ENDIAN == 0
/* Host is BE */
#define ADRV903X_HTOCLL( x )  ADRV903X_BYTESWAP_LL((x)) 
#define ADRV903X_CTOHLL( x )  ADRV903X_BYTESWAP_LL((x)) 
#define ADRV903X_HTOCL( x )  ADRV903X_BYTESWAP_L((x)) 
#define ADRV903X_CTOHL( x )  ADRV903X_BYTESWAP_L((x)) 
#define ADRV903X_HTOCS( x )  ADRV903X_BYTESWAP_S((x)) 
#define ADRV903X_CTOHS( x )  ADRV903X_BYTESWAP_S((x)) 
#define ADRV903X_NTOHL( x ) ( x )
#define ADRV903X_HTONL( x ) ( x )
#else
#error "ADRV903X_LITTLE_ENDIAN value not recognized. Must be either 0 or 1."
#endif /* ADRV903X_LITTLE_ENDIAN */

#endif /* __ADRV903X_PLATFORM_BYTE_ORDER_H__ */


