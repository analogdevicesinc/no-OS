;/******************************************************************************
;* Copyright (c) 2020 Xilinx, Inc.  All rights reserved.
;* SPDX-License-Identifier: MIT
;******************************************************************************/
;/*****************************************************************************/
;/**
;* @file xil-crt0.S
;*
;* <pre>
;* MODIFICATION HISTORY:
;*
;* Ver   Who  Date     Changes
;* ----- ---- -------- ---------------------------------------------------
;* 7.3  dp  06/25/20   Initial version for armclang
;* </pre>
;*
;* @note
;*
;* None.
;*
;******************************************************************************/
#include "xparameters.h"
#include "bspconfig.h"

	IMPORT |Image$$BSS_SECTION$$Base|
	IMPORT Xil_InitializeExistingMPURegConfig
	EXPORT _startup
	IMPORT __cpu_init
	IMPORT __main
#ifdef SLEEP_TIMER_BASEADDR
	IMPORT XTime_StartTTCTimer
#endif
	AREA |.text|, CODE
	REQUIRE8 {TRUE}
	PRESERVE8 {TRUE}
	ENTRY

_startup
	bl	__cpu_init ;Initialize the CPU first (BSP provides this)
	;configure the timer if TTC is present
#ifdef SLEEP_TIMER_BASEADDR
	bl XTime_StartTTCTimer
#endif
	bl 	Xil_InitializeExistingMPURegConfig ;/* Initialize MPU config */
	;make sure argc and argv are valid
	mov	r0, #0
	mov	r1, #0

#ifdef XCLOCKING
	bl	Xil_ClockInit
#endif
	bl	__main ;Jump to main C code

	bl	exit

exit	;should never get here
	b exit

	END
