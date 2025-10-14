/******************************************************************************
* Copyright (c) 2012 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file fsbl_hooks.h
*
* Contains the function prototypes, defines and macros required by fsbl_hooks.c
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver	Who	Date		Changes
* ----- ---- -------- -------------------------------------------------------
* 3.00a	np/mb	10/08/12	Initial release
*				Corrected the prototype
*
* </pre>
*
* @note
*
******************************************************************************/
#ifndef FSBL_HOOKS_H_
#define FSBL_HOOKS_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#include "fsbl.h"


/************************** Function Prototypes ******************************/

/* FSBL hook function which is called before bitstream download */
u32 FsblHookBeforeBitstreamDload(void);

/* FSBL hook function which is called after bitstream download */
u32 FsblHookAfterBitstreamDload(void);

/* FSBL hook function which is called before handoff to the application */
u32 FsblHookBeforeHandoff(void);

/* FSBL hook function which is called in FSBL fallback */
void FsblHookFallback(void);

#ifdef __cplusplus
}
#endif

#endif	/* end of protection macro */
