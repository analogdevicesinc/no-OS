/******************************************************************************
* Copyright (c) 2019 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xpseudo_asm_armclang.h
*
* This header file contains macros for using __inline assembler code.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 7.0   cjp  02/26/19 First Release
* 7.2   asa  04/03/20 Renamed the str macro to strw.
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
			    asm volatile("mrs %x0, DAIF" : "=r" (rval)); \
			    rval; \
			})

#define mtcpsr(v) 	__asm__ __volatile__ ("msr DAIF, %x0" : : "r" (v))

#define cpsiei()
#define cpsidi()
#define cpsief()
#define cpsidf()
#define mtgpr(rn, v)
#define mfgpr(rn)

/* Instruction Synchronization Barrier */
#define isb()		__asm__ __volatile__ ("isb sy")

/* Data Synchronization Barrier */
#define dsb()		__asm__ __volatile__("dsb sy")

/* Data Memory Barrier */
#define dmb()		__asm__ __volatile__("dmb sy")

/* Memory Operations */
#define ldr(adr)	({u64 rval; \
			    __asm__ __volatile__( \
			    "ldr %x0, [%x1]" \
			    : "=r" (rval) : "r" (adr) \
			    ); \
			    rval; \
			})

#define ldrb(adr)	({u8 rval; \
			    __asm__ __volatile__( \
			    "ldrb %x0, [%x1]" \
			    : "=r" (rval) : "r" (adr) \
			    ); \
			    rval; \
			})

#define strw(adr, val)	__asm__ __volatile__( \
			"str %x0, [%x1]\n" \
			: : "r" (val), "r" (adr) \
			)

#define strb(adr, val)	__asm__ __volatile__(\
			  "strb	%x0,[%x1]\n"\
			  : : "r" (val), "r" (adr)\
			)

/* Count leading zeroes (clz) */
#define clz(arg)	({u8 rval; \
			    __asm__ __volatile__( \
			    "clz %x0, %x1" \
			    : "=r" (rval) : "r" (arg) \
			    ); \
			    rval; \
			})

#define mtcpdc(reg,val)	__asm__ __volatile__("dc " #reg ",%x0" : : "r" (val))
#define mtcpic(reg,val)	__asm__ __volatile__("ic " #reg ",%x0" : : "r" (val))

#define mtcpicall(reg)	__asm__ __volatile__("ic " #reg)
#define mtcptlbi(reg)	__asm__ __volatile__("tlbi " #reg)
#define mtcpat(reg,val)	__asm__ __volatile__("at " #reg ",%x0" : : "r" (val))

/* CP15 operations */
#define mfcp(reg)	({u64 rval = 0U; \
			    __asm__ __volatile__( \
			    "mrs %x0, " #reg : "=r" (rval)); \
			    rval;\
			})

#define mtcp(reg,val)	__asm__ __volatile__("msr " #reg ",%x0" : : "r" (val))

/************************** Variable Definitions ****************************/

/************************** Function Prototypes *****************************/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XPSEUDO_ASM_ARMCLANG_H */

/**
 *@endcond
 */
