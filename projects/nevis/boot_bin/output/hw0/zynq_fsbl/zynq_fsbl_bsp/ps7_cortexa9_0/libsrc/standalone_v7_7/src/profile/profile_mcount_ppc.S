/******************************************************************************
* Copyright (c) 2002 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

	.globl _mcount

	#define _MCOUNT_STACK_FRAME 48
	.section .text
	.align 2
	.type _mcount@function


_mcount:
	stwu 1,	-_MCOUNT_STACK_FRAME(1)
	stw 3, 8(1)
	stw 4, 12(1)
	stw 5, 16(1)
	stw 6, 20(1)
	stw 7, 24(1)
	stw 8, 28(1)
	stw 9, 32(1)
	stw 10, 36(1)
	stw 11, 40(1)
	stw 12, 44(1)
	mflr 4
	stw 4, (_MCOUNT_STACK_FRAME+4)(1)
	lwz 3, (_MCOUNT_STACK_FRAME)(1)
	lwz 3, 4(3)
	bl mcount
	lwz 4, (_MCOUNT_STACK_FRAME+4)(1)
	mtlr 4
	lwz 12, 44(1)
	lwz 11, 40(1)
	lwz 10, 36(1)
	lwz 9, 32(1)
	lwz 8, 28(1)
	lwz 7, 24(1)
	lwz 6, 20(1)
	lwz 5, 16(1)
	lwz 4, 12(1)
	lwz 3, 8(1)
	addi 1,1, _MCOUNT_STACK_FRAME
	blr
