/******************************************************************************
* Copyright (c) 2002 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

	.globl _mcount
	.text
	.align 2
	.ent _mcount

	#ifndef PROFILE_NO_GRAPH

_mcount:
	addi r1, r1, -48
	swi r11, r1, 44
	swi r12, r1, 40
	swi r5, r1, 36
	swi r6, r1, 32
	swi r7, r1, 28
	swi r8, r1, 24
	swi r9, r1, 20
	swi r10, r1, 16
	swi r15, r1, 12
	add r5, r0, r15
	brlid r15, mcount
	add r6, r0, r16

	lwi r11, r1, 44
	lwi r12, r1, 40
	lwi r5, r1, 36
	lwi r6, r1, 32
	lwi r7, r1, 28
	lwi r8, r1, 24
	lwi r9, r1, 20
	lwi r10, r1, 16
	lwi r15, r1, 12
	rtsd r15, 4
	addi r1, r1, 48

	#endif	/* PROFILE_NO_GRAPH */

	.end _mcount
