/******************************************************************************
* Copyright (c) 2009 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xpseudo_asm.h
*
* @addtogroup a9_specific Cortex A9 Processor Specific Include Files
*
* The xpseudo_asm.h includes xreg_cortexa9.h and xpseudo_asm_gcc.h.
*
* The xreg_cortexa9.h file contains definitions for inline assembler code.
* It provides inline definitions for Cortex A9 GPRs, SPRs, MPE registers,
* co-processor registers and Debug registers.
*
* The xpseudo_asm_gcc.h contains the definitions for the most often used inline
* assembler instructions, available as macros. These can be very useful for
* tasks such as setting or getting special purpose registers, synchronization,
* or cache manipulation etc. These inline assembler instructions can be used
* from drivers and user applications written in C.
*
* @{
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a ecm  10/18/09 First release
* 3.04a sdm  01/02/12 Remove redundant dsb in mcr instruction.
* 6.8   aru  09/06/18 Removed compilation warnings for ARMCC toolchain.
* </pre>
*
******************************************************************************/
#ifndef XPSEUDO_ASM_H
#define XPSEUDO_ASM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "xreg_cortexa9.h"
#ifdef __GNUC__
 #include "xpseudo_asm_gcc.h"
#elif defined (__ICCARM__)
 #include "xpseudo_asm_iccarm.h"
#else
 #include "xpseudo_asm_rvct.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* XPSEUDO_ASM_H */
/**
* @} End of "addtogroup a9_specific".
*/
