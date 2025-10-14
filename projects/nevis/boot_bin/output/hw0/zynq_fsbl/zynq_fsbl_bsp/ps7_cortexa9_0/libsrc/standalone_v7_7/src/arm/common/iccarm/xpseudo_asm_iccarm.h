/******************************************************************************
* Copyright (c) 2009 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xpseudo_asm_iccarm.h
*
* This header file contains macros for using inline assembler code. It is
* written specifically for the IAR C/C++ compiler.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 1.00a ecm/sdm  10/28/09 First release
* 3.12a asa		 11/02/13  Removed the macro mfcpsr to make it a function.
* 5.3	pkp		 10/09/15 Modified dsb, dmb and isb definitions
* 5.4	pkp		 03/02/16 Included header file intrinsic.h for assembly
*						  instructions definitions
* 6.2   kvn      03/03/17 Added support thumb mode
* 7.2   asa              04/03/20 Renamed the str macro to strw.
* </pre>
*
******************************************************************************/

/**
 *@cond nocomments
 */

#ifndef XPSEUDO_ASM_ICCARM_H  /* prevent circular inclusions */
#define XPSEUDO_ASM_ICCARM_H  /* by using protection macros */

/***************************** Include Files ********************************/

#include "xil_types.h"
#include <intrinsics.h>
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************** Constant Definitions ****************************/

/**************************** Type Definitions ******************************/

/***************** Macros (Inline Functions) Definitions ********************/

/* necessary for pre-processor */
#define stringify(s)	tostring(s)
#define tostring(s)	#s

/* pseudo assembler instructions */

#define mtcpsr(v)	__asm volatile(\
			  "msr	cpsr_cf,%0\n"\
			  : : "r" (v)\
			)

#define cpsiei()	__asm volatile("cpsie	i\n")
#define cpsidi()	__asm volatile("cpsid	i\n")

#define cpsief()	__asm volatile("cpsie	f\n")
#define cpsidf()	__asm volatile("cpsid	f\n")



#define mtgpr(rn, v)	__asm volatile(\
			  "mov r" stringify(rn) ", %0 \n"\
			  : : "r" (v)\
			)

#define mfgpr(rn)	({u32 rval; \
			  __asm volatile(\
			    "mov %0,r" stringify(rn) "\n"\
			    : "=r" (rval)\
			  );\
			  rval;\
			 })

/* memory synchronization operations */

/* Instruction Synchronization Barrier */
#define isb() __ISB();

/* Data Synchronization Barrier */
#define dsb() __DSB();

/* Data Memory Barrier */
#define dmb() __DMB();


/* Memory Operations */
#define ldr(adr)	({u32 rval; \
			  __asm volatile(\
			    "ldr	%0,[%1]"\
			    : "=r" (rval) : "r" (adr)\
			  );\
			  rval;\
			 })

#define ldrb(adr)	({u8 rval; \
			  __asm volatile(\
			    "ldrb	%0,[%1]"\
			    : "=r" (rval) : "r" (adr)\
			  );\
			  rval;\
			 })

#define strw(adr, val)	__asm volatile(\
			  "str	%0,[%1]\n"\
			  : : "r" (val), "r" (adr)\
			)

#define strb(adr, val)	__asm volatile(\
			  "strb	%0,[%1]\n"\
			  : : "r" (val), "r" (adr)\
			)

/* Count leading zeroes (clz) */
#define clz(arg)	({u8 rval; \
			  __asm volatile(\
			    "clz	%0,%1"\
			    : "=r" (rval) : "r" (arg)\
			  );\
			  rval;\
			 })

/* CP15 operations */
#define mtcp(rn, v)	__asm volatile(\
			 "mcr " rn "\n"\
			 : : "r" (v)\
			);

/*#define mfcp(rn)	({u32 rval; \
			 __asm volatile(\
			   "mrc " rn "\n"\
			   : "=r" (rval)\
			 );\
			 rval;\
			 }) */

#define mfcp(rn, v)	__asm volatile ("mrc " rn : "=r" (v));

/************************** Variable Definitions ****************************/

/************************** Function Prototypes *****************************/
int mfcpsr (void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

/**
 *@endcond
 */

#endif /* XPSEUDO_ASM_ICCARM_H */
