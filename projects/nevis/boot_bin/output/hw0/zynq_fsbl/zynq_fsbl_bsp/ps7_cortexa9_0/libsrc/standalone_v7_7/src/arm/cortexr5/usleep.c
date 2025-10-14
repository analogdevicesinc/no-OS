/******************************************************************************
* Copyright (c) 2014 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file usleep.c
*
* This function supports user configurable sleep implementation.
* This provides a microsecond delay using the timer specified by the user in
* the ARM Cortex R5 MP core.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 5.00 	pkp  	 02/20/14 First release
* 5.04  pkp		 02/19/16 usleep routine is modified to use TTC3 if present
*						  else it will use set of assembly instructions to
*						  provide the required delay
* 5.04	pkp		 03/09/16 Assembly routine for usleep is modified to avoid
*						  disabling the interrupt
* 5.04	pkp		 03/11/16 Compare the counter value to previously read value
*						  to detect the overflow for TTC3
* 6.0   asa      08/15/16 Updated the usleep signature. Fix for CR#956899.
* 6.6	srm      10/18/17 Updated sleep routines to support user configurable
*			  implementation. Now sleep routines will use TTC
*                         instance specified by user.
* 7.2   mus      01/29/20 Updated sleep routines to use CortexR5 PMU for delay
*                         generation, in case TTC3/TTC2 instances are not
*                         present in HW design. If user dont want to use PMU
*                         counter for sleep routines, BSP needs to be compiled
*                         with "DONT_USE_PMU_FOR_SLEEP_ROUTINES" flag. It fixes
*                         CR#1051591.
* 7.7	sk	 01/10/22 Modify usleep_R5 functions return type from int to
* 			  void to fix misra_c_2012_rule_17_7 violation.
* 7.7	sk	 03/02/22 Update usleep_R5 argument type to fix misra_c_2012_
* 			  directive_4_6 violation.
*
* </pre>
*
******************************************************************************/
/***************************** Include Files *********************************/

#include "sleep.h"
#include "xtime_l.h"
#include "xparameters.h"
#include "xil_types.h"
#include "xpseudo_asm.h"
#include "xreg_cortexr5.h"
#include "xpm_counter.h"

#if defined (SLEEP_TIMER_BASEADDR)
#include "xil_sleeptimer.h"
#endif

/*****************************************************************************/
/**
*
* This API gives a delay in microseconds
*
* @param	useconds requested
*
* @note		By default, usleep is implemented using TTC3. Although user is
*               given an option to select other instances of TTC. When the user
*               selects other instances of TTC, usleep is implemented by that
*		specific TTC instance. If the user didn't select any other instance
*		of TTC specifically and when TTC3 is absent, usleep is implemented
*      	        using assembly instructions which is tested with instruction and
*		data caches enabled and it gives proper delay. It may give more
*		delay than exepcted when caches are disabled. If interrupt comes
*		when usleep using assembly instruction is being executed, the delay
*		may be greater than what is expected since once the interrupt is
*		served count resumes from where it was interrupted unlike the case
*		of TTC3 where counter keeps running while interrupt is being served.
*
****************************************************************************/

void usleep_R5(ULONG useconds)
{
#if defined (SLEEP_TIMER_BASEADDR)
	Xil_SleepTTCCommon(useconds, COUNTS_PER_USECOND);
#elif !defined (DONT_USE_PMU_FOR_SLEEP_ROUTINES)
	Xpm_SleepPerfCounter(useconds, COUNTS_PER_USECOND);
#else
#if defined (__GNUC__)
	__asm__ __volatile__ (
#elif defined (__ICCARM__)
	__asm volatile (
#endif
		"push {r0,r1,r3} \n"
		"mov r0, %[usec] \n"
		"mov r1, %[iter] \n"
		"1: \n"
		"mov r3, r1 \n"
		"2: \n"
		"subs r3, r3, #0x1\n"
		"bne 2b \n"
		"subs r0, r0, #0x1 \n"
		"bne 1b \n"
		"pop {r0,r1,r3} \n"
		::[iter] "r" (ITERS_PER_USEC), [usec] "r" (useconds)
		);
#endif

}
