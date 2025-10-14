/******************************************************************************
* Copyright (c) 2020 - 2021 Xilinx, Inc.  All rights reserved.
*
* SPDX-License-Identifier: MIT
*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file xpseudo_asm_armclang.h
*
* This header file contains macros for using inline assembler code. It is
* written specifically for the GNU compiler.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 7.3  dp  06/25/20   Initial version for armclang
* </pre>
*
******************************************************************************/

/**
 *@cond nocomments
 */

#ifndef XPSEUDO_ASM_ARMCLANG_H  /* prevent circular inclusions */
#define XPSEUDO_ASM_ARMCLANG_H  /* by using protection macros */

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


/* pseudo assembler instructions */
#define mfcpsr()	({u32 rval = 0U; \
			  __asm volatile ("mrs %0, cpsr" : "=r" (rval));\
			  rval;\
			 })

#define mtcpsr(v) asm volatile("msr cpsr,%0\n" : : "r" (v) : "cc")

#define cpsiei()	__asm__ __volatile__("cpsie	i\n")
#define cpsidi()	__asm__ __volatile__("cpsid	i\n")

#define cpsief()	__asm__ __volatile__("cpsie	f\n")
#define cpsidf()	__asm__ __volatile__("cpsid	f\n")



#define mtgpr(rn, v)	__asm__ __volatile__(\
			  "mov r" stringify(rn) ", %x0 \n"\
			  : : "r" (v)\
			)

#define mfgpr(rn)	({u32 rval; \
			  __asm__ __volatile__(\
			    "mov %x0,r" stringify(rn) "\n"\
			    : "=r" (rval)\
			  );\
			  rval;\
			 })

/* memory synchronization operations */

/* Instruction Synchronization Barrier */
#define isb() __asm__ __volatile__ ("isb" : : : "memory")

/* Data Synchronization Barrier */
#define dsb() __asm__ __volatile__ ("dsb" : : : "memory")

/* Data Memory Barrier */
#define dmb() __asm__ __volatile__ ("dmb" : : : "memory")


/* Memory Operations */
#define ldr(adr)	({u32 rval; \
			  __asm__ __volatile__(\
			    "ldr %x0,[%x1]"\
			    : "=r" (rval) : "r" (adr)\
			  );\
			  rval;\
			 })


#define ldrb(adr)	({u8 rval; \
			  __asm__ __volatile__(\
			    "ldrb %x0,[%x1]"\
			    : "=r" (rval) : "r" (adr)\
			  );\
			  rval;\
			 })

#define str(adr, val)	__asm__ __volatile__(\
			  "str %x0,[%x1]\n"\
			  : : "r" (val), "r" (adr)\
			)

#define strb(adr, val)	__asm__ __volatile__(\
			  "strb %0,[%x1]\n"\
			  : : "r" (val), "r" (adr)\
			)

/* Count leading zeroes (clz) */
#define clz(arg)	({u8 rval; \
			  __asm__ __volatile__(\
			    "clz %x0,%x1"\
			    : "=r" (rval) : "r" (arg)\
			  );\
			  rval;\
			 })

/* CP15 operations */
#define mtcp(rn, v)	__asm__ __volatile__(\
			 "mcr " rn "\n"\
			 : : "r" (v)\
			);

#define mfcp(rn)	({u32 rval = 0U; \
			 __asm__ __volatile__(\
			   "mrc " rn "\n"\
			   : "=r" (rval)\
			 );\
			 rval;\
			 })

/************************** Variable Definitions ****************************/

/************************** Function Prototypes *****************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XPSEUDO_ASM_ARMCLANG_H */
/**
 *@endcond
 */