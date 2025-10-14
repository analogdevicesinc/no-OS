/******************************************************************************
* Copyright (c) 2009 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xpseudo_asm_rvct.h
*
* This header file contains macros for using __inline assembler code. It is
* written specifically for RVCT.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a sdm  11/18/09 First Release
* 7.2   asa  04/03/20 Renamed the str macro to strw.
* </pre>
*
******************************************************************************/
/**
 *@cond nocomments
 */

#ifndef XPSEUDO_ASM_RVCT_H  /* prevent circular inclusions */
#define XPSEUDO_ASM_RVCT_H  /* by using protection macros */

/***************************** Include Files ********************************/
#include "xil_types.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************** Constant Definitions ****************************/

/**************************** Type Definitions ******************************/

/***************** Macros (Inline Functions) Definitions ********************/

/* necessary for pre-processor */
#define stringify(s)	tostring(s)
#define tostring(s)	#s


#define mtcpsr(v)	{ volatile register u32 Reg __asm("cpsr");\
			  Reg = v; }

/* general purpose register read/write */
/*#define mfgpr(rn) 	({ unsigned int val; \
			  register unsigned int Reg __asm("r" stringify(rn));\
			  val = Reg; \
			  val;})*/

#define mtgpr(rn, v)	{ volatile register u32 Reg __asm("r" stringify(rn));\
			  Reg = v; }

/* CP15 operations */
/*#define mfcp(rn)	({ unsigned int val; \
			  val = register unsigned int Reg __asm(rn); \
			  val;})*/

#define mtcp(rn, v)	{ volatile register u32 Reg __asm(rn); \
			  Reg = v; }

/************************** Variable Definitions ****************************/

/************************** Function Prototypes *****************************/

__asm void cpsiei(void);

__asm void cpsidi(void);

__asm void cpsief(void);

__asm void cpsidf(void);

/* memory synchronization operations */

/* Instruction Synchronization Barrier */
__asm void isb(void);

/* Data Synchronization Barrier */
__asm void dsb(void);

/* Data Memory Barrier */
__asm void dmb(void);

/* Memory Operations */
__asm u32 ldr(u32 adr);

__asm u32 ldrb(u32 adr);

__asm void strw(u32 adr, u32 val);

__asm void strb(u32 adr, u32 val);

/* Count leading zeroes (clz) */
__asm u32 clz(u32 arg);
__asm u32 mfcpsr(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XPSEUDO_ASM_RVCT_H */

/**
 *@endcond
 */
