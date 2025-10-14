/******************************************************************************
* Copyright (c) 2004 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/*****************************************************************************/
/**
*
* @file microblaze_exception_handler.c
*
* This file contains exception handler registration routines for
* the MicroBlaze processor.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Date     Changes
* ----- -------- -----------------------------------------------
* 1.00b 06/24/04 First release
* </pre>
*
******************************************************************************/


/***************************** Include Files *********************************/
#include "microblaze_exceptions_i.h"
#include "microblaze_exceptions_g.h"

#ifdef MICROBLAZE_EXCEPTIONS_ENABLED                    /* If exceptions are enabled in the processor */
/************************** Constant Definitions *****************************/


/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/
extern MB_ExceptionVectorTableEntry MB_ExceptionVectorTable[];
/****************************************************************************/

/*****************************************************************************/
/**
*
* Registers an exception handler for the MicroBlaze. The
* argument provided in this call as the DataPtr is used as the argument
* for the handler when it is called.
*
* @param    ExceptionId is the id of the exception to register this handler
*           for.
* @param    Top level handler.
* @param    DataPtr is a reference to data that will be passed to the handler
*           when it gets called.
* @return   None.
*
****************************************************************************/
void microblaze_register_exception_handler(u32 ExceptionId, Xil_ExceptionHandler Handler, void *DataPtr)
{
   MB_ExceptionVectorTable[ExceptionId].Handler = Handler;
   MB_ExceptionVectorTable[ExceptionId].CallBackRef = DataPtr;
}

#endif  /* MICROBLAZE_EXCEPTIONS_ENABLED */
