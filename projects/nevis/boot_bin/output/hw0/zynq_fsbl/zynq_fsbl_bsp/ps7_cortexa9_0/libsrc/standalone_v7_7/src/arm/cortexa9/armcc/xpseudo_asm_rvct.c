/******************************************************************************
* Copyright (c) 2009 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xpseudo_asm_rvct.c
*
* This header file contains functions for using assembler code. It is
* written specifically for RVCT.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a sdm  11/18/09 First Release
* 7.2   asa  04/04/20 Renamed the str macro to strw.
* </pre>
*
******************************************************************************/

/***************************** Include Files ********************************/

#include "xpseudo_asm_rvct.h"

/************************** Constant Definitions ****************************/

/**************************** Type Definitions ******************************/

/***************** Macros (Inline Functions) Definitions ********************/

/************************** Variable Definitions ****************************/

/************************** Function Prototypes *****************************/

/* embedded assembler instructions */
__asm void cpsiei(void)
{
	cpsie	i
	bx	lr
}
__asm void cpsidi(void)
{
	cpsid	i
	bx	lr
}

__asm void cpsief(void)
{
	cpsie	f
	bx	lr
}

__asm void cpsidf(void)
{
	cpsid	f
	bx	lr
}

/* memory synchronization operations */

/* Instruction Synchronization Barrier */
__asm void isb(void)
{
	isb
	bx lr
}

/* Data Synchronization Barrier */
__asm void dsb(void)
{
	dsb
	bx lr
}

/* Data Memory Barrier */
__asm void dmb(void)
{
	dmb
	bx lr
}

/* Memory Operations */
__asm u32 ldr(u32 adr)
{
	ldr	r0, [r0]
	bx	lr
}

__asm u32 ldrb(u32 adr)
{
	ldrb	r0, [r0]
	bx	lr
}

__asm void strw(u32 adr, u32 val)
{
	str	r1, [r0]
	bx	lr
}

__asm void strb(u32 adr, u32 val)
{
	strb	r1, [r0]
	bx	lr
}

/* Count leading zeroes (clz) */
__asm u32 clz(u32 arg)
{
	clz	r0, r0
	bx	lr
}

__asm u32 mfcpsr(void)
{
	mrs	r0, cpsr
	bx lr
}
