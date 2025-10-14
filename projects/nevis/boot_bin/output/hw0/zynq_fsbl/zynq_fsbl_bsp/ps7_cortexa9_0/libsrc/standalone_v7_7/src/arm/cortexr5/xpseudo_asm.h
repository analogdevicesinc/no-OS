/******************************************************************************
* Copyright (c) 2014 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xpseudo_asm.h
*
* @addtogroup r5_specific Cortex R5 Processor Specific Include Files
*
* The xpseudo_asm.h includes xreg_cortexr5.h and xpseudo_asm_gcc.h.
*
* The xreg_cortexr5.h file contains definitions for inline assembler code.
* It provides inline definitions for Cortex R5 GPRs, SPRs,co-processor
* registers and Debug register
*
* The xpseudo_asm_gcc.h contains the definitions for the most often used
* inline assembler instructions, available as macros. These can be very
* useful for tasks such as setting or getting special purpose registers,
* synchronization,or cache manipulation. These inline assembler instructions
* can be used from drivers and user applications written in C.
*
* @{
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 5.00  pkp  02/10/14 Initial version
* 6.2   mus  01/27/17 Updated to support IAR compiler
* 7.3   dp   06/25/20 Initial version for armclang
* </pre>
*
******************************************************************************/
#ifndef XPSEUDO_ASM_H /* prevent circular inclusions */
#define XPSEUDO_ASM_H /* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

#include "xreg_cortexr5.h"
#if defined (__clang__)
#include "xpseudo_asm_armclang.h"
#elif defined (__GNUC__)
#include "xpseudo_asm_gcc.h"
#elif defined (__ICCARM__)
#include "xpseudo_asm_iccarm.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* XPSEUDO_ASM_H */
/**
* @} End of "addtogroup r5_specific".
*/
