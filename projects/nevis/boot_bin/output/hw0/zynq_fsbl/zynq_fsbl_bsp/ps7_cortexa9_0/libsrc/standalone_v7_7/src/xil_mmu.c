/******************************************************************************
* Copyright (c) 2012 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
* @file xil_mmu.c
*
* This file provides APIs for enabling/disabling MMU and setting the memory
* attributes for sections, in the MMU translation table.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------------
* 1.00a sdm  01/12/12 Initial version
* 3.05a asa  03/10/12 Modified the Xil_EnableMMU to invalidate the caches
*		      before enabling back.
* 3.05a asa  04/15/12 Modified the Xil_SetTlbAttributes routine so that
*		      translation table and branch predictor arrays are
*		      invalidated, D-cache flushed before the attribute
*		      change is applied. This is done so that the user
*		      need not call Xil_DisableMMU before calling
*		      Xil_SetTlbAttributes.
* 3.10a  srt 04/18/13 Implemented ARM Erratas. Please refer to file
*		      'xil_errata.h' for errata description
* 3.11a  asa 09/23/13 Modified Xil_SetTlbAttributes to flush the complete
*			 D cache after the translation table update. Removed the
*			 redundant TLB invalidation in the same API at the beginning.
* 6.8   aru  09/06/18 Removed compilation warnings for ARMCC toolchain.
*                     It fixes CR#1008309.
* </pre>
*
* @note
*
* None.
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xil_cache.h"
#include "xpseudo_asm.h"
#include "xil_types.h"
#include "xil_mmu.h"
#include "xil_errata.h"

/***************** Macros (Inline Functions) Definitions *********************/

/**************************** Type Definitions *******************************/

/************************** Constant Definitions *****************************/
#define     ARM_AR_MEM_TTB_SECT_SIZE               1024*1024
#define     ARM_AR_MEM_TTB_SECT_SIZE_MASK          (~(ARM_AR_MEM_TTB_SECT_SIZE-1UL))
/************************** Variable Definitions *****************************/

extern u32 MMUTable;

/************************** Function Prototypes ******************************/

/*****************************************************************************/
/**
* @brief	This function sets the memory attributes for a section covering 1MB
*			of memory in the translation table.
*
* @param	Addr: 32-bit address for which memory attributes need to be set.
* @param	attrib: Attribute for the given memory region. xil_mmu.h contains
*			definitions of commonly used memory attributes which can be
*			utilized for this function.
*
*
* @return	None.
*
* @note		The MMU or D-cache does not need to be disabled before changing a
*			translation table entry.
*
******************************************************************************/
void Xil_SetTlbAttributes(INTPTR Addr, u32 attrib)
{
	u32 *ptr;
	u32 section;

	section = Addr / 0x100000U;
	ptr = &MMUTable;
	ptr += section;
	if(ptr != NULL) {
		*ptr = (Addr & 0xFFF00000U) | attrib;
	}

	Xil_DCacheFlush();

	mtcp(XREG_CP15_INVAL_UTLB_UNLOCKED, 0U);
	/* Invalidate all branch predictors */
	mtcp(XREG_CP15_INVAL_BRANCH_ARRAY, 0U);

	dsb(); /* ensure completion of the BP and TLB invalidation */
    isb(); /* synchronize context on this processor */
}

/*****************************************************************************/
/**
* @brief	Enable MMU for cortex A9 processor. This function invalidates the
*			instruction and data caches, and then enables MMU.
*
* @return	None.
*
******************************************************************************/
void Xil_EnableMMU(void)
{
	u32 Reg;
	Xil_DCacheInvalidate();
	Xil_ICacheInvalidate();

#ifdef __GNUC__
	Reg = mfcp(XREG_CP15_SYS_CONTROL);
#elif defined (__ICCARM__)
	mfcp(XREG_CP15_SYS_CONTROL, Reg);
#else
	{ volatile register u32 Cp15Reg __asm(XREG_CP15_SYS_CONTROL);
	  Reg = Cp15Reg; }
#endif
	Reg |= (u32)0x05U;
	mtcp(XREG_CP15_SYS_CONTROL, Reg);

	dsb();
	isb();
}

/*****************************************************************************/
/**
* @brief	Disable MMU for Cortex A9 processors. This function invalidates
*			the TLBs, Branch Predictor Array and flushed the D Caches before
*			disabling the MMU.
*
* @return	None.
*
* @note		When the MMU is disabled, all the memory accesses are treated as
*			strongly ordered.
******************************************************************************/
void Xil_DisableMMU(void)
{
	u32 Reg;

	mtcp(XREG_CP15_INVAL_UTLB_UNLOCKED, 0U);
	mtcp(XREG_CP15_INVAL_BRANCH_ARRAY, 0U);
	Xil_DCacheFlush();

#ifdef __GNUC__
	Reg = mfcp(XREG_CP15_SYS_CONTROL);
#elif defined (__ICCARM__)
	mfcp(XREG_CP15_SYS_CONTROL, Reg);
#else
	{ volatile register u32 Cp15Reg __asm(XREG_CP15_SYS_CONTROL);
	  Reg = Cp15Reg; }
#endif
	Reg &= (u32)(~0x05U);
#ifdef CONFIG_ARM_ERRATA_794073
	/* Disable Branch Prediction */
	Reg &= (u32)(~0x800U);
#endif
	mtcp(XREG_CP15_SYS_CONTROL, Reg);
}

/*****************************************************************************/
/**
* @brief   Memory mapping for Cortex A9 processor.
*
* @param   PhysAddr is physical address.
* @param   size is size of region.
* @param   flags is flags used to set translation table.
*
* @return  Pointer to virtual address.
*
* @note: Previously this was implemented in libmetal. Move to embeddedsw as this
*       functionality is specific to A9 processor.
*
******************************************************************************/
void* Xil_MemMap(UINTPTR PhysAddr, size_t size, u32 flags)
{
   u32 Sectionoffset;
   u32 Ttbaddr;

   if (!flags)
       return (void*)PhysAddr;

   /* Ensure alignment on a section boundary */
   PhysAddr &= ARM_AR_MEM_TTB_SECT_SIZE_MASK;

   /* Loop through entire region of memory (one MMU section at a time).
      Each section requires a TTB entry. */
   for (Sectionoffset = 0; Sectionoffset < size;
		Sectionoffset += ARM_AR_MEM_TTB_SECT_SIZE) {
       /* Calculate translation table entry for this memory section */
       Ttbaddr = (PhysAddr + Sectionoffset);

       /* Write translation table entry value to entry address */
       Xil_SetTlbAttributes(Ttbaddr, flags);
   }
   return (void*)PhysAddr;
}
