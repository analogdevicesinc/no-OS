 /**
 * \file adrv904x_platform_byte_order.h
 * 
 * \brief   Contains platform byte order (endianness) conversion macros
 *
 * \details Contains platform byte order (endianness) conversion macros
 *
 * ADRV904X API Version: 2.9.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_PLATFORM_BYTE_ORDER_H__
#define __ADRV904X_PLATFORM_BYTE_ORDER_H__

#ifndef ADI_ADRV904X_FW
#include "adi_adrv904x_user.h"
#ifdef ADI_ADRV904X_LITTLE_ENDIAN
#define ADRV904X_LITTLE_ENDIAN ADI_ADRV904X_LITTLE_ENDIAN
#endif
#endif

#ifndef ADRV904X_LITTLE_ENDIAN
#ifdef __GNUC__
#define ADRV904X_LITTLE_ENDIAN (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#elif defined  __ICCARM__
#define ADRV904X_LITTLE_ENDIAN (__LITTLE_ENDIAN__ == 1)
#else
#error( "Define the ADRV904X_LITTLE_ENDIAN macro for your platform.")
#endif
#endif

/*
 * Endianess conversion macros naming:
 *
 *   C is the adrv904x's CPU byte-order i.e. little-endian 
 *   
 *   H is the byte-order of the host on which the API is running which can be either big or little endian. Setting
 *   ADRVGEN_LITTLE_ENDIAN to match the endianess of the host will cuase the xTOy conversion macros swap bytes or not as
 *   appropriate for the host endianess.  
 *
 *   N is 'network byte-order' i.e. big-endian.
 */

#define ADRV904X_BYTESWAP_S(x) \
    ((((x) & 0xFF00u) >> 8) | \
     (((x) & 0x00FFu) << 8))

#define ADRV904X_BYTESWAP_L(x) \
    ((((x) & 0xFF000000ul) >> 24) | \
     (((x) & 0x00FF0000ul) >> 8)  | \
     (((x) & 0x0000FF00ul) << 8)  | \
     (((x) & 0x000000FFul) << 24))

#define ADRV904X_BYTESWAP_LL( x ) \
    ((((x) & 0xFF00000000000000ull) >> 56u) | \
     (((x) & 0x00FF000000000000ull) >> 40u) | \
     (((x) & 0x0000FF0000000000ull) >> 24u) | \
     (((x) & 0x000000FF00000000ull) >>  8u) | \
     (((x) & 0x00000000FF000000ull) <<  8u) | \
     (((x) & 0x0000000000FF0000ull) << 24u) | \
     (((x) & 0x000000000000FF00ull) << 40u) | \
     (((x) & 0x00000000000000FFull) << 56u))

#define ADRV904X_NTOCL( x ) ADRV904X_BYTESWAP_L((x))
#define ADRV904X_CTONL( x ) ADRV904X_BYTESWAP_L((x))

#if ADRV904X_LITTLE_ENDIAN == 1
/* Host is LE */
#define ADRV904X_HTOCLL( x ) ( x )
#define ADRV904X_CTOHLL( x ) ( x )
#define ADRV904X_HTOCL( x )  ( x )
#define ADRV904X_CTOHL( x )  ( x )
#define ADRV904X_HTOCS( x )  ( x )
#define ADRV904X_CTOHS( x )  ( x )
#define ADRV904X_NTOHL( x ) ADRV904X_BYTESWAP_L((x))
#define ADRV904X_HTONL( x ) ADRV904X_BYTESWAP_L((x))
#elif ADRV904X_LITTLE_ENDIAN == 0
/* Host is BE */
#define ADRV904X_HTOCLL( x )  ADRV904X_BYTESWAP_LL((x)) 
#define ADRV904X_CTOHLL( x )  ADRV904X_BYTESWAP_LL((x)) 
#define ADRV904X_HTOCL( x )  ADRV904X_BYTESWAP_L((x)) 
#define ADRV904X_CTOHL( x )  ADRV904X_BYTESWAP_L((x)) 
#define ADRV904X_HTOCS( x )  ADRV904X_BYTESWAP_S((x)) 
#define ADRV904X_CTOHS( x )  ADRV904X_BYTESWAP_S((x)) 
#define ADRV904X_NTOHL( x ) ( x )
#define ADRV904X_HTONL( x ) ( x )
#else
#error "ADRV904X_LITTLE_ENDIAN value not recognized. Must be either 0 or 1."
#endif /* ADRV904X_LITTLE_ENDIAN */

#endif /* __ADRV904X_PLATFORM_BYTE_ORDER_H__ */


