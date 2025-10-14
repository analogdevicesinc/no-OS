/******************************************************************************
* Copyright (c) 2009 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xil_assert.c
* @addtogroup common_assert_apis Assert APIs and Macros
* @{
*
* This file contains basic assert related functions for Xilinx software IP.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date   Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a hbm  07/14/09 Initial release
* 6.0   kvn  05/31/16 Make Xil_AsserWait a global variable
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xil_types.h"
#include "xil_assert.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/**
 * @brief This variable allows testing to be done easier with asserts. An assert
 * sets this variable such that a driver can evaluate this variable
 * to determine if an assert occurred.
 */
u32 Xil_AssertStatus;

/**
 * @brief This variable allows the assert functionality to be changed for testing
 * such that it does not wait infinitely. Use the debugger to disable the
 * waiting during testing of asserts.
 */
s32 Xil_AssertWait = 1;

/* The callback function to be invoked when an assert is taken */
static Xil_AssertCallback Xil_AssertCallbackRoutine = NULL;

/************************** Function Prototypes ******************************/

/*****************************************************************************/
/**
*
* @brief    Implement assert. Currently, it calls a user-defined callback
*           function if one has been set.  Then, it potentially enters an
*           infinite loop depending on the value of the Xil_AssertWait
*           variable.
*
* @param    File: filename of the source
* @param    Line: linenumber within File
*
* @return   None.
*
* @note     None.
*
******************************************************************************/
void Xil_Assert(const char8 *File, s32 Line)
{
	/* if the callback has been set then invoke it */
	if (Xil_AssertCallbackRoutine != 0) {
		(*Xil_AssertCallbackRoutine)(File, Line);
	}

	/* if specified, wait indefinitely such that the assert will show up
	 * in testing
	 */
	while (Xil_AssertWait != 0) {
	}
}

/*****************************************************************************/
/**
*
* @brief    Set up a callback function to be invoked when an assert occurs.
*           If a callback is already installed, then it will be replaced.
*
* @param    Routine: callback to be invoked when an assert is taken
*
* @return   None.
*
* @note     This function has no effect if NDEBUG is set
*
******************************************************************************/
void Xil_AssertSetCallback(Xil_AssertCallback Routine)
{
	Xil_AssertCallbackRoutine = Routine;
}

/*****************************************************************************/
/**
*
* @brief    Null handler function. This follows the XInterruptHandler
*           signature for interrupt handlers. It can be used to assign a null
*           handler (a stub) to an interrupt controller vector table.
*
* @param    NullParameter: arbitrary void pointer and not used.
*
* @return   None.
*
* @note     None.
*
******************************************************************************/
void XNullHandler(void *NullParameter)
{
	(void) NullParameter;
}
/**
* @} End of "addtogroup common_assert_apis".
*/
