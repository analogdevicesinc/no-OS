/*******************************************************************************
* Copyright (C) 2018-2019 Maxim Integrated Products, Inc., All rights Reserved.
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
*******************************************************************************/

/**
 * @internal
 * @file MXQ_Types.h
 * @date Apr, May 2014
 * @authors Stéphane Di Vito, Maxim Integrated Products Inc.
 * @authors Benjamin Vinot, Maxim Integrated Products Inc.
 *
 * @ingroup Types
 */


#ifndef __MXQ_TYPES_H_INCLUDED__
#define __MXQ_TYPES_H_INCLUDED__

#define PRIVATE static

/**
 * 8-bit unsigned integer
 * @ingroup Types
 */
typedef unsigned  char          mxq_u1;
/**
 * 8-bit signed integer
 * @ingroup Types
 */
typedef signed    char          mxq_s1;
/**
 * Pointer to 8-bit unsigned integer
 * @ingroup Types
 */
typedef mxq_u1 *                mxq_pu1;

/**
 * 16-bit unsigned integer
 * @ingroup Types
 */
typedef unsigned  short         mxq_u2;
/**
 * 16-bit signed integer
 * @ingroup Types
 */
typedef signed    short         mxq_s2;
/**
 * Pointer to 16-bit unsigned integer
 * @ingroup Types
 */
typedef mxq_u2 *                mxq_pu2;



/**
 * 32-bit unsigned integer
 * @ingroup Types
 */
typedef unsigned  long int      mxq_u4;
/**
 * 32-bit signed integer
 * @ingroup Types
 */
typedef signed    long int      mxq_s4;
/**
 * Pointer to 32-bit unsigned integer
 * @ingroup Types
 */
typedef mxq_u4 *                mxq_pu4;


/**
 * Type used for buffer lengths.
 * @ingroup Types
 */
typedef unsigned int      mxq_length;
/**
 *  Type used for pointer to buffer lengths.
 * @ingroup Types
 */
typedef unsigned int *    mxq_plength;

/**
 * Boolean type
 * @ingroup Types
 */
typedef int      mxq_bool;

/**
 * True value for the mxq_bool type
 * @ingroup Types
 */
#define MXQ_FALSE   (0)

/**
 * False value for the mxq_bool type
 * @ingroup Types
 */
#define MXQ_TRUE    (1)

/**
 * @internal
 * Counter structure
 * @ingroup Types
 */
typedef struct {
		mxq_u4 countbound;
		mxq_u4 countval;
} counter_t;


#ifdef __GNUC__
#   define _PACKED __attribute__ ((__packed__))     /* For structs mapped to the filesystem image and all enums */
#else
#   define _PACKED
#   warning _PACKED defined to NOTHING: structure alignment may be wrong
#endif


#endif /* __MXQ_TYPES_H_INCLUDED__ */
