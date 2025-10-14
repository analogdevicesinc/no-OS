/******************************************************************************
* Copyright (c) 2002 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
// based on "ARM Profiling Implementation" from Sourcery G++ Lite for ARM EABI

.globl __gnu_mcount_nc
.type __gnu_mcount_nc, %function

__gnu_mcount_nc:
	push	{r0, r1, r2, r3, lr}
	subs	r1, lr, #0			/* callee - current lr */
	ldr	r0, [sp, #20]			/* caller - at the top of the stack */
	bl	mcount				/* when __gnu_mcount_nc is called */
	pop	{r0, r1, r2, r3, ip, lr}
	bx	ip

	.end __gnu_mcount_nc
