/******************************************************************************
* Copyright (c) 2014 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/*****************************************************************************/
/**
* @file sleep.h
*
*  This header file contains ARM Cortex A53,A9,R5,Microblaze specific sleep
*  related APIs.
*
* <pre>
* MODIFICATION HISTORY :
*
* Ver   Who  Date	 Changes
* ----- ---- -------- -------------------------------------------------------
* 6.6   srm  11/02/17 Added processor specific sleep routines
*								 function prototypes.
* 7.7	sk   01/10/22 Typecast sleep declaration argument from unsigned int to
* 		      u32 to fix misra_c_2012_directive_4_6 violation.
* 7.7	sk   01/10/22 Modify the return type of sleep_R5 and usleep_R5 from
* 		      unsigned to void to fix misra_c_2012_rule_17_7 violation.
* 7.7	sk   03/02/22 Update usleep_R5 and usleep parameter types to fix misra_
*		      c_2012_directive_4_6 violation.
*
* </pre>
*
******************************************************************************/

#ifndef SLEEP_H
#define SLEEP_H

#include "xil_types.h"
#include "xil_io.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/**
*
* This macro polls an address periodically until a condition is met or till the
* timeout occurs.
* The minimum timeout for calling this macro is 100us. If the timeout is less
* than 100us, it still waits for 100us. Also the unit for the timeout is 100us.
* If the timeout is not a multiple of 100us, it waits for a timeout of
* the next usec value which is a multiple of 100us.
*
* @param            IO_func - accessor function to read the register contents.
*                   Depends on the register width.
* @param            ADDR - Address to be polled
* @param            VALUE - variable to read the value
* @param            COND - Condition to checked (usually involves VALUE)
* @param            TIMEOUT_US - timeout in micro seconds
*
* @return           0 - when the condition is met
*                   -1 - when the condition is not met till the timeout period
*
* @note             none
*
*****************************************************************************/
#define Xil_poll_timeout(IO_func, ADDR, VALUE, COND, TIMEOUT_US) \
 ( {	  \
	u64 timeout = TIMEOUT_US/100;    \
	if(TIMEOUT_US%100!=0)	\
		timeout++;   \
	for(;;) { \
		VALUE = IO_func(ADDR); \
		if(COND) \
			break; \
		else {    \
			usleep(100);  \
			timeout--; \
			if(timeout==0) \
			break;  \
		}  \
	}    \
	(timeout>0) ? 0 : -1;  \
 }  )

void usleep(ULONG useconds);
void sleep(u32 seconds);
void usleep_R5(ULONG useconds);
void sleep_R5(u32 seconds);
int usleep_MB(unsigned long useconds);
unsigned sleep_MB(unsigned int seconds);
int usleep_A53(unsigned long useconds);
unsigned sleep_A53(unsigned int seconds);
int usleep_A9(unsigned long useconds);
unsigned sleep_A9(unsigned int seconds);

#ifdef __cplusplus
}
#endif

#endif
