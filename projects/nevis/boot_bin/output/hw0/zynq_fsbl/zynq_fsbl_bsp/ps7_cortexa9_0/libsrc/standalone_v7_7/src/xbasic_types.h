/******************************************************************************
* Copyright (c) 2010 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xbasic_types.h
*
*
* @note  Dummy File for backwards compatibility
*

*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date   Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a adk   1/31/14  Added in bsp common folder for backward compatibility
* 7.0   aru   01/21/19 Modified the typedef of u32,u16,u8
* 7.0 	aru   02/06/19 Included stdint.h and stddef.h
* </pre>
*
******************************************************************************/

/**
 *@cond nocomments
 */

#ifndef XBASIC_TYPES_H	/* prevent circular inclusions */
#define XBASIC_TYPES_H	/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/** @name Legacy types
 * Deprecated legacy types.
 * @{
 */
typedef uint8_t	Xuint8;		/**< unsigned 8-bit */
typedef char		Xint8;		/**< signed 8-bit */
typedef uint16_t	Xuint16;	/**< unsigned 16-bit */
typedef short		Xint16;		/**< signed 16-bit */
typedef uint32_t	Xuint32;	/**< unsigned 32-bit */
typedef long		Xint32;		/**< signed 32-bit */
typedef float		Xfloat32;	/**< 32-bit floating point */
typedef double		Xfloat64;	/**< 64-bit double precision FP */
typedef unsigned long	Xboolean;	/**< boolean (XTRUE or XFALSE) */

#if !defined __XUINT64__
typedef struct
{
	Xuint32 Upper;
	Xuint32 Lower;
} Xuint64;
#endif

/** @name New types
 * New simple types.
 * @{
 */
#ifndef __KERNEL__
#ifndef XIL_TYPES_H
typedef Xuint32         u32;
typedef Xuint16         u16;
typedef Xuint8          u8;
#endif
#else
#include <linux/types.h>
#endif

#ifndef TRUE
#  define TRUE		1U
#endif

#ifndef FALSE
#  define FALSE		0U
#endif

#ifndef NULL
#define NULL		0U
#endif

/*
 * Xilinx NULL, TRUE and FALSE legacy support. Deprecated.
 * Please use NULL, TRUE and FALSE
 */
#define XNULL		NULL
#define XTRUE		TRUE
#define XFALSE		FALSE

/*
 * This file is deprecated and users
 * should use xil_types.h and xil_assert.h\n\r
 */
#warning  The xbasics_type.h file is deprecated and users should use xil_types.h and xil_assert.
#warning  Please refer the Standalone BSP UG647 for further details

#ifdef __cplusplus
}
#endif

#endif	/* end of protection macro */

/**
 *@endcond
 */
