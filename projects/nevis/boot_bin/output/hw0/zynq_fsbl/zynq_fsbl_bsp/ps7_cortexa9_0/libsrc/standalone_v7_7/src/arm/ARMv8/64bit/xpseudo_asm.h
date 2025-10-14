/******************************************************************************
* Copyright (c) 2014 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xpseudo_asm.h
*
* @addtogroup a53_64_specific Cortex A53 64bit Processor Specific Include Files
*
* The xpseudo_asm.h includes xreg_cortexa53.h and xpseudo_asm_gcc.h.
* The xreg_cortexa53.h file contains definitions for inline assembler code.
* It provides inline definitions for Cortex A53 GPRs, SPRs and floating point
* registers.
*
* The xpseudo_asm_gcc.h contains the definitions for the most often used inline
* assembler instructions, available as macros. These can be very useful for
* tasks such as setting or getting special purpose registers, synchronization,
* or cache manipulation etc. These inline assembler instructions can be used
* from drivers and user applications written in C.
*
* @{
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 5.00 	pkp  05/29/14 First release
* </pre>
*
******************************************************************************/
#ifndef XPSEUDO_ASM_H
#define XPSEUDO_ASM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "xreg_cortexa53.h"
#ifdef __clang__
#include "xpseudo_asm_armclang.h"
#else
#include "xpseudo_asm_gcc.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* XPSEUDO_ASM_H */
/**
* @} End of "addtogroup a53_64_specific".
*/
