/******************************************************************************
* Copyright (c) 2004 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file microblaze_interrupt_handler.c
*
* This file contains the standard interrupt handler for the MicroBlaze processor.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Date     Changes
* ----- -------- -----------------------------------------------
* 1.00b 10/03/03 First release
* </pre>
*
******************************************************************************/


/***************************** Include Files *********************************/

#include "xil_exception.h"
#include "microblaze_interrupts_i.h"

/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/
#ifdef __clang__
void _interrupt_handler (void) __attribute__ ((interrupt_handler));
#else
void __interrupt_handler (void) __attribute__ ((interrupt_handler));
#endif
void microblaze_register_handler(XInterruptHandler Handler, void *DataPtr);

/************************** Variable Definitions *****************************/

extern MB_InterruptVectorTableEntry MB_InterruptVectorTable[MB_INTERRUPT_VECTOR_TABLE_ENTRIES];
/*****************************************************************************/
/**
*
* @brief 	This function is the standard interrupt handler used by the MicroBlaze processor.
* 			It saves all volatile registers, calls the users top level interrupt handler.
* 			When this returns, it restores all registers, and returns using a rtid instruction.
*
* @return 	None.
*
******************************************************************************/
#ifdef __clang__
void _interrupt_handler(void)
#else
void __interrupt_handler(void)
#endif
{
	/* The compiler saves all volatiles and the MSR */
	(void)MB_InterruptVectorTable[0].Handler(MB_InterruptVectorTable[0].CallBackRef);
	/* The compiler restores all volatiles and MSR, and returns from interrupt */
}


/*****************************************************************************/
/**
*
* @brief 	Registers a top-level interrupt handler for the MicroBlaze. The
* 			argument provided in this call as the DataPtr is used as the argument
* 			for the handler when it is called.
*
* @param    Handler: Top level handler.
* @param    DataPtr: a reference to data that will be passed to the handler
*           when it gets called.

* @return   None.
*
*
****************************************************************************/
void microblaze_register_handler(XInterruptHandler Handler, void *DataPtr)
{
   MB_InterruptVectorTable[0].Handler = Handler;
   MB_InterruptVectorTable[0].CallBackRef = DataPtr;
}
