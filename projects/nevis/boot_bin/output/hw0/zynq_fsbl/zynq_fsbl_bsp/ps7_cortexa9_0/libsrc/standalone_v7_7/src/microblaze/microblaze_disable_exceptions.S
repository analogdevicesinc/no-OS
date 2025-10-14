/******************************************************************************
* Copyright (c) 2004 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/******************************************************************************
*
* Disable exceptions on microblaze.
*
*
******************************************************************************/

#include "xparameters.h"

	.text
	.globl	microblaze_disable_exceptions
	.ent	microblaze_disable_exceptions
	.align 2
microblaze_disable_exceptions:
#if XPAR_MICROBLAZE_USE_MSR_INSTR == 1
	rtsd    r15, 8
        msrclr  r0, 0x100
#else
        mfs     r4, rmsr;
        andi    r4, r4, ~(0x100);                       /* Turn OFF the EE bit */
        mts     rmsr, r4;
        rtsd    r15, 8;
        nop;
#endif
.end microblaze_disable_exceptions
