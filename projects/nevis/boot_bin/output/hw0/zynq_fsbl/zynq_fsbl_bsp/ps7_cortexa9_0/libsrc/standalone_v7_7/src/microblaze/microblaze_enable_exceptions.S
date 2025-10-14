/******************************************************************************
* Copyright (c) 2004 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/******************************************************************************
*
* Enable exceptions on microblaze.
*
*
******************************************************************************/

#include "xparameters.h"

	.text
	.globl	microblaze_enable_exceptions
	.ent	microblaze_enable_exceptions
	.align 2
microblaze_enable_exceptions:
#if XPAR_MICROBLAZE_USE_MSR_INSTR == 1
	rtsd    r15, 8;
        msrset  r0, 0x100
#else /*XPAR_MICROBLAZE_USE_MSR_INSTR == 1*/
        mfs     r4, rmsr;
        ori     r4, r4, 0x100;                  /* Turn ON the EE bit */
        mts     rmsr, r4;
        rtsd    r15, 8;
        nop;
#endif /*XPAR_MICROBLAZE_USE_MSR_INSTR == 1*/
.end microblaze_enable_exceptions
