/******************************************************************************
* Copyright (c) 2014 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
* @file xil_mmu.c
*
* This file provides APIs for enabling/disabling MMU and setting the memory
* attributes for sections, in the MMU translation table.
* MMU APIs are yet to be implemented. They are left blank to avoid any
* compilation error
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------------
* 5.00 	pkp  05/29/14 First release
* 6.02  pkp	 01/22/17 Added support for EL1 non-secure
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
#include "bspconfig.h"
/***************** Macros (Inline Functions) Definitions *********************/

/**************************** Type Definitions *******************************/

/************************** Constant Definitions *****************************/

#define BLOCK_SIZE_2MB 0x200000U
#define BLOCK_SIZE_1GB 0x40000000U
#define ADDRESS_LIMIT_4GB 0x100000000UL

/************************** Variable Definitions *****************************/

extern INTPTR MMUTableL1;
extern INTPTR MMUTableL2;

/************************** Function Prototypes ******************************/
/*****************************************************************************/
/**
* @brief	It sets the memory attributes for a section, in the translation
* 			table. If the address (defined by Addr) is less than 4GB, the
*			memory attribute(attrib) is set for a section of 2MB memory. If the
*			address (defined by Addr) is greater than 4GB, the memory attribute
*			(attrib) is set for a section of 1GB memory.
*
* @param	Addr: 64-bit address for which attributes are to be set.
* @param	attrib: Attribute for the specified memory region. xil_mmu.h
*			contains commonly used memory attributes definitions which can be
*			utilized for this function.
*
* @return	None.
*
* @note		The MMU and D-cache need not be disabled before changing an
*			translation table attribute.
*
******************************************************************************/
void Xil_SetTlbAttributes(UINTPTR Addr, u64 attrib)
{
	INTPTR *ptr;
	INTPTR section;
	u64 block_size;
	/* if region is less than 4GB MMUTable level 2 need to be modified */
	if(Addr < ADDRESS_LIMIT_4GB){
		/* block size is 2MB for addressed < 4GB*/
		block_size = BLOCK_SIZE_2MB;
		section = Addr / block_size;
		ptr = &MMUTableL2 + section;
	}
	/* if region is greater than 4GB MMUTable level 1 need to be modified */
	else{
		/* block size is 1GB for addressed > 4GB */
		block_size = BLOCK_SIZE_1GB;
		section = Addr / block_size;
		ptr = &MMUTableL1 + section;
	}
	*ptr = (Addr & (~(block_size-1))) | attrib;

	Xil_DCacheFlush();

	if (EL3 == 1)
		mtcptlbi(ALLE3);
	else if (EL1_NONSECURE == 1)
		mtcptlbi(VMALLE1);

	dsb(); /* ensure completion of the BP and TLB invalidation */
    isb(); /* synchronize context on this processor */

}
