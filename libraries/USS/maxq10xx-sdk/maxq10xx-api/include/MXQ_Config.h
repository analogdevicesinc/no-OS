/*******************************************************************************
* Copyright (C) 2018-2019 Maxim Integrated Products, Inc. (now owned by Analog Devices, Inc.),
* 2024 Analog Devices, Inc. This software is proprietary and
* confidential to Analog Devices, Inc. and its licensors. , All rights Reserved.
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
******************************************************************************/

/**
 * @internal
 * @file MXQ_Config.h
 * @date Feb 2018
 * @authors Benjamin Vinot, Maxim Integrated Products Inc.
 *
 * @ingroup API
 */

#ifndef _MXQ_CONFIG_H_
#define _MXQ_CONFIG_H_

/* C library dependencies */
#include <string.h>
#include <stdio.h>

/* Notes: the SDK build system already provides definitions for:
 - USE_PROTOCOL_SPI or USE_PROTOCOL_I2C
 - MXQ_DUMP_COMM
 - MXQ_DUMP_DEBUG
   When using an alternative build environment, those symbols can be defined here.
 */

#if (!defined (USE_PROTOCOL_SPI)) && (!defined(USE_PROTOCOL_I2C)) && (!defined(USE_PROTOCOL_PIPE)) && (!defined(USE_PROTOCOL_MAILBOX))
#define USE_PROTOCOL_SPI
//#define USE_PROTOCOL_I2C
#endif


/* MXQ wrappers to standard libc functions */
#define MXQ_MEMCPY(dest, src, len) 				memcpy(dest, src, len)
#define MXQ_MEMSET(dest, value, len)			memset(dest, value, len)
#define MXQ_MEMCMP(ptr1, ptr2, len)				memcmp(ptr1, ptr2, len)

/* Define the output of command exchange via MXQ_PRINTF: 

 - MXQ_DUMP_COMM: 
     -  (-1) permanently disabled (not compiled)
     -  0, 1 or 2: No, B&W or Color output, dynamically selectable
 - MXQ_DEMU DEBUG:
     -  (-1): permanently disabled (not compiled)
     -  0, 1: None or Verbose, dynamically selectable
 */
//#define MXQ_NO_PRINTF // Override: Force no printf output. Automatically selected if all DUMP options are -1

#if defined(MXQ_NO_PRINTF)

// Force no printf output
#undef MXQ_DUMP_COMM
#undef MXQ_DUMP_DEBUG
#undef MXQ_DUMP_ERROR

#define MXQ_DUMP_COMM  (-1)
#define MXQ_DUMP_DEBUG (-1)
#define MXQ_DUMP_ERROR (-1)

#else

// Select default outputs
#ifndef MXQ_DUMP_COMM
#define MXQ_DUMP_COMM  0
#endif

#ifndef MXQ_DUMP_DEBUG
#define MXQ_DUMP_DEBUG 0
#endif

#ifndef MXQ_DUMP_ERROR
#define MXQ_DUMP_ERROR 0
#endif

#endif

#if (MXQ_DUMP_COMM==-1)&&(MXQ_DUMP_ERROR==-1)&&(MXQ_DUMP_DEBUG==-1)
#ifndef MXQ_NO_PRINTF
#define MXQ_NO_PRINTF
#endif
#endif

/*--------------------*/
#ifdef MXQ_ON_ARDUINO
/*--------------------*/

// Arduino(r) platform needs a custom printf
#ifndef MXQ_NO_PRINTF

#include <stdarg.h>
#ifdef __cplusplus
extern "C" {
#endif

void ard_printf(char * format,...);
void ard_vprintf(char * format, va_list arg);

#ifdef __cplusplus
}
#endif

#define MXQ_PRINT_ERR(...)						ard_printf(__VA_ARGS__)
#define MXQ_PRINTF(...) 						ard_printf(__VA_ARGS__)
#define MXQ_VPRINTF(...)						ard_vprintf(__VA_ARGS__)

#endif

// Arduino pin and peripheral mapping
#define MYSERIAL Serial1
#define MYSPI    SPI1
#define MYSS_PIN 10

// Note: not supporting I2C yet on Arduino

/*-----------------------*/
#elif defined MXQ_ON_MBED
/*-----------------------*/

#define MYRST_PIN    P0_4
#define MYMODE_PIN   P0_2
#define MYWAKEUP_PIN P0_3

#define MYSS_PIN     P1_3
#define MYMISO_PIN   P1_2
#define MYMOSI_PIN   P1_1
#define MYSCLK_PIN   P1_0

// Note: not supporting I2C yet on MBED


#ifndef MXQ_NO_PRINTF
#define MXQ_PRINT_ERR(...)						printf(__VA_ARGS__)
#define MXQ_PRINTF(...) 						printf(__VA_ARGS__)
#define MXQ_VPRINTF(...)						vprintf(__VA_ARGS__)
#endif

/*--------------------*/
#else 
/*--------------------*/

#ifndef MXQ_NO_PRINTF
#define MXQ_PRINT_ERR(...)						fprintf(stderr, __VA_ARGS__)
#define MXQ_PRINTF(...)							printf(__VA_ARGS__)
#define MXQ_VPRINTF(...)						vprintf(__VA_ARGS__)
#endif

#endif //MXQ_ON_...

#ifndef MXQ_PRINTF
#define MXQ_PRINTF(...)
#endif

#ifndef MXQ_PRINT_ERR
#define MXQ_PRINT_ERR(...)
#endif

#ifndef MXQ_VPRINTF
#define MXQ_VPRINTF(...)
#endif


#endif /* _MXQ_CONFIG_H_ */
