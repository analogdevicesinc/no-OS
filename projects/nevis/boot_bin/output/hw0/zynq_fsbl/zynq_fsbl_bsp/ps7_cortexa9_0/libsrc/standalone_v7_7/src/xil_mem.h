/******************************************************************************/
/**
* Copyright (c) 2015 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/****************************************************************************/
/**
* @file xil_mem.h
*
* @addtogroup common_mem_operation_api Customized APIs for Memory Operations
*
* The xil_mem.h file contains prototype for functions related
* to memory operations. These APIs are applicable for all processors supported
* by Xilinx.
*
* @{
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 6.1   nsk      11/07/16 First release.
* 7.0   mus      01/07/19 Add cpp extern macro
*
* </pre>
*
*****************************************************************************/
#ifndef XIL_MEM_H		/* prevent circular inclusions */
#define XIL_MEM_H		/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/************************** Function Prototypes *****************************/

void Xil_MemCpy(void* dst, const void* src, u32 cnt);

#ifdef __cplusplus
}
#endif

#endif /* XIL_MEM_H */
/**
* @} End of "addtogroup common_mem_operation_api".
*/