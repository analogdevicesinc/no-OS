/******************************************************************************
* Copyright (c) 2014 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xpseudo_asm_gcc.h
*
* This header file contains macros for using inline assembler code. It is
* written specifically for the GNU compiler.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 5.00 	pkp		 05/21/14 First release
* 6.0   mus      07/27/16 Consolidated file for a53,a9 and r5 processors
* 7.2   asa      04/03/20 Renamed the str macro to strw.
* 7.2   dp       04/30/20 Added clobber "cc" to mtcpsr for aarch32 processors
* </pre>
*
******************************************************************************/

/**
 *@cond nocomments
 */

#ifndef XPSEUDO_ASM_GCC_H  /* prevent circular inclusions */
#define XPSEUDO_ASM_GCC_H  /* by using protection macros */

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

#if defined (__aarch64__)
/* pseudo assembler instructions */
#define mfcpsr()	({u32 rval = 0U; \
			   asm volatile("mrs %0,  DAIF" : "=r" (rval));\
			  rval;\
			 })

#define mtcpsr(v) __asm__ __volatile__ ("msr DAIF, %0" : : "r" (v))

#define cpsiei()	//__asm__ __volatile__("cpsie	i\n")
#define cpsidi()	//__asm__ __volatile__("cpsid	i\n")

#define cpsief()	//__asm__ __volatile__("cpsie	f\n")
#define cpsidf()	//__asm__ __volatile__("cpsid	f\n")



#define mtgpr(rn, v)	/*__asm__ __volatile__(\
			  "mov r" stringify(rn) ", %0 \n"\
			  : : "r" (v)\
			)*/

#define mfgpr(rn)	/*({u32 rval; \
			  __asm__ __volatile__(\
			    "mov %0,r" stringify(rn) "\n"\
			    : "=r" (rval)\
			  );\
			  rval;\
			 })*/

/* memory synchronization operations */

/* Instruction Synchronization Barrier */
#define isb() __asm__ __volatile__ ("isb sy")

/* Data Synchronization Barrier */
#define dsb() __asm__ __volatile__("dsb sy")

/* Data Memory Barrier */
#define dmb() __asm__ __volatile__("dmb sy")


/* Memory Operations */
#define ldr(adr)	({u64 rval; \
			  __asm__ __volatile__(\
			    "ldr	%0,[%1]"\
			    : "=r" (rval) : "r" (adr)\
			  );\
			  rval;\
			 })

#define mfelrel3() ({u64 rval = 0U; \
                   asm volatile("mrs %0,  ELR_EL3" : "=r" (rval));\
                  rval;\
                 })

#define mtelrel3(v) __asm__ __volatile__ ("msr ELR_EL3, %0" : : "r" (v))

#else

/* pseudo assembler instructions */
#define mfcpsr()	({u32 rval = 0U; \
			  __asm__ __volatile__(\
			    "mrs	%0, cpsr\n"\
			    : "=r" (rval)\
			  );\
			  rval;\
			 })

#define mtcpsr(v)	__asm__ __volatile__(\
			  "msr	cpsr,%0\n"\
			  : : "r" (v) : "cc" \
			)

#define cpsiei()	__asm__ __volatile__("cpsie	i\n")
#define cpsidi()	__asm__ __volatile__("cpsid	i\n")

#define cpsief()	__asm__ __volatile__("cpsie	f\n")
#define cpsidf()	__asm__ __volatile__("cpsid	f\n")



#define mtgpr(rn, v)	__asm__ __volatile__(\
			  "mov r" stringify(rn) ", %0 \n"\
			  : : "r" (v)\
			)

#define mfgpr(rn)	({u32 rval; \
			  __asm__ __volatile__(\
			    "mov %0,r" stringify(rn) "\n"\
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
			    "ldr	%0,[%1]"\
			    : "=r" (rval) : "r" (adr)\
			  );\
			  rval;\
			 })

#endif

#define ldrb(adr)	({u8 rval; \
			  __asm__ __volatile__(\
			    "ldrb	%0,[%1]"\
			    : "=r" (rval) : "r" (adr)\
			  );\
			  rval;\
			 })

#define strw(adr, val)	__asm__ __volatile__(\
			  "str	%0,[%1]\n"\
			  : : "r" (val), "r" (adr)\
			)

#define strb(adr, val)	__asm__ __volatile__(\
			  "strb	%0,[%1]\n"\
			  : : "r" (val), "r" (adr)\
			)

/* Count leading zeroes (clz) */
#define clz(arg)	({u8 rval; \
			  __asm__ __volatile__(\
			    "clz	%0,%1"\
			    : "=r" (rval) : "r" (arg)\
			  );\
			  rval;\
			 })

#if defined (__aarch64__)
#define mtcpdc(reg,val)	__asm__ __volatile__("dc " #reg ",%0"  : : "r" (val))
#define mtcpic(reg,val)	__asm__ __volatile__("ic " #reg ",%0"  : : "r" (val))

#define mtcpicall(reg)	__asm__ __volatile__("ic " #reg)
#define mtcptlbi(reg)	__asm__ __volatile__("tlbi " #reg)
#define mtcpat(reg,val)	__asm__ __volatile__("at " #reg ",%0"  : : "r" (val))
/* CP15 operations */
#define mfcp(reg)	({u64 rval = 0U;\
			__asm__ __volatile__("mrs	%0, " #reg : "=r" (rval));\
			rval;\
			})

#define mtcp(reg,val)	__asm__ __volatile__("msr " #reg ",%0"  : : "r" (val))

#else
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
#endif

/************************** Variable Definitions ****************************/

/************************** Function Prototypes *****************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

/**
 *@endcond
 */

#endif /* XPSEUDO_ASM_GCC_H */
