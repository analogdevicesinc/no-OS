/******************************************************************************
* Copyright (c) 2012 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file microblaze_exceptions_i.h
*
* This header file contains defines for structures used by the microblaze
* hardware exception handler.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Date     Changes
* ----- -------- -----------------------------------------------
* 1.00a 06/24/04 First release
* </pre>
*
******************************************************************************/

#ifndef MICROBLAZE_EXCEPTIONS_I_H /* prevent circular inclusions */
#define MICROBLAZE_EXCEPTIONS_I_H /* by using protection macros */

/***************************** Include Files *********************************/

#include "xil_types.h"
#include "xil_assert.h"
#include "xil_exception.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *@cond nocomments
 */

typedef struct
{
   Xil_ExceptionHandler Handler;
   void *CallBackRef;
} MB_ExceptionVectorTableEntry;

/* Exception IDs */
#define XEXC_ID_FSL                     0U
#define XEXC_ID_UNALIGNED_ACCESS        1U
#define XEXC_ID_ILLEGAL_OPCODE          2U
#define XEXC_ID_M_AXI_I_EXCEPTION       3U
#define XEXC_ID_IPLB_EXCEPTION          3U
#define XEXC_ID_M_AXI_D_EXCEPTION       4U
#define XEXC_ID_DPLB_EXCEPTION          4U
#define XEXC_ID_DIV_BY_ZERO             5U
#define XEXC_ID_FPU                     6U
#define XEXC_ID_STACK_VIOLATION         7U
#define XEXC_ID_MMU                     7U

/**
 *@endcond
 */

void microblaze_register_exception_handler(u32 ExceptionId, Xil_ExceptionHandler Handler, void *DataPtr);

#ifdef __cplusplus
}
#endif
#endif /* end of protection macro */
