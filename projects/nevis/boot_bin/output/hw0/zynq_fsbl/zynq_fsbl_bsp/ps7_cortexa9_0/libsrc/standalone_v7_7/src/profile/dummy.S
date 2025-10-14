/******************************************************************************
* Copyright (c) 2002 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

	.globl dummy_f

#ifdef PROC_MICROBLAZE
	.text
	.align 2
	.ent dummy_f

dummy_f:
	nop

	.end dummy_f
#endif

#ifdef PROC_PPC
	.section .text
	.align 2
	.type dummy_f@function

dummy_f:
	b dummy_f

#endif

#ifdef PROC_CORTEXA9
	.section .text
	.align 2
	.type dummy_f, %function

dummy_f:
	b dummy_f

#endif
