/******************************************************************************
* Copyright (c) 2009 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xil_exception.h
*
* @addtogroup microblaze_exception_apis Microblaze Exception APIs
* @{
*
* The xil_exception.h file, available in the <install-directory>/src/microblaze folder,
* contains Microblaze specific exception related APIs and macros. Application programs
* can use these APIs for various exception related operations. For example, enable exception,
* disable exception, register exception hander.
*
* @note To use exception related functions, xil_exception.h must be added in source code
*
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00  hbm  07/28/09 Initial release
*
* </pre>
*
******************************************************************************/

/**
 *@cond nocomments
 */
#ifndef XIL_EXCEPTION_H /* prevent circular inclusions */
#define XIL_EXCEPTION_H /* by using protection macros */

#include "xil_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************** Constant Definitions *****************************/

/*
 * These constants are specific to Microblaze processor.
 */

#define XIL_EXCEPTION_ID_FIRST                0U
#define XIL_EXCEPTION_ID_FSL                  0U
#define XIL_EXCEPTION_ID_UNALIGNED_ACCESS     1U
#define XIL_EXCEPTION_ID_ILLEGAL_OPCODE       2U
#define XIL_EXCEPTION_ID_M_AXI_I_EXCEPTION    3U
#define XIL_EXCEPTION_ID_IPLB_EXCEPTION       3U
#define XIL_EXCEPTION_ID_M_AXI_D_EXCEPTION    4U
#define XIL_EXCEPTION_ID_DPLB_EXCEPTION       4U
#define XIL_EXCEPTION_ID_DIV_BY_ZERO          5U
#define XIL_EXCEPTION_ID_FPU                  6U
#define XIL_EXCEPTION_ID_STACK_VIOLATION      7U
#define XIL_EXCEPTION_ID_MMU                  7U
#define XIL_EXCEPTION_ID_LAST		      XIL_EXCEPTION_ID_MMU

/*
 * XIL_EXCEPTION_ID_INT is defined for all processors, but with different value.
 */
#define XIL_EXCEPTION_ID_INT		      16U /**
						  * exception ID for interrupt
						  */

/**************************** Type Definitions *******************************/

/**
 * This typedef is the exception handler function.
 */
typedef void (*Xil_ExceptionHandler)(void *Data);

/**
 * This data type defines an interrupt handler for a device.
 * The argument points to the instance of the component
 */
typedef void (*XInterruptHandler) (void *InstancePtr);

/***************** Macros (Inline Functions) Definitions *********************/


/************************** Function Prototypes ******************************/

extern void Xil_ExceptionRegisterHandler(u32 Id,
					 Xil_ExceptionHandler Handler,
					 void *Data);

extern void Xil_ExceptionRemoveHandler(u32 Id);

extern void Xil_ExceptionInit(void);
extern void Xil_ExceptionEnable(void);
extern void Xil_ExceptionDisable(void);

#ifdef __cplusplus
}
#endif

#endif

/**
 *@endcond
 */

/**
* @} End of "addtogroup microblaze_exception_apis".
*/
