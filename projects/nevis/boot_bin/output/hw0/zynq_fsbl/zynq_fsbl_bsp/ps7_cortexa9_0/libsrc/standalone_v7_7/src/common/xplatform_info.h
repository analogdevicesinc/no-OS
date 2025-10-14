/******************************************************************************
* Copyright (c) 2014 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xplatform_info.h
*
*
* @addtogroup common_platform_info APIs to Get Platform Information
*
*
* The xplatform_info.h file contains definitions for various available Xilinx&reg;
* platforms. Also, it contains prototype of APIs, which can be used to get the
* platform information.
*
* @{
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date    Changes
* ----- ---- --------- -------------------------------------------------------
* 6.4    ms   05/23/17 Added PSU_PMU macro to support XGetPSVersion_Info
*                      function for PMUFW.
* 7.2    adk  08/01/20 Added versal support for the XGetPSVersion_Info function.
* 7.6    mus  08/23/21 Updated prototypes for functions which are not taking any
*                      arguments with void keyword. This has been done to fix
*                      compilation warnings with "-Wstrict-prototypes" flag.
*                      It fixes CR#1108601.
* 7.6    mus  08/30/21 Updated flag checking to fix compilation warnings
*                      reported with "-Wundef" flag.
* 7.7	 sk   01/10/22 Update XPLAT_INFO_MASK from signed to unsigned to fix
*		       misra_c_2012_rule_10_4 violation.
* </pre>
*
******************************************************************************/

/**
 *@cond nocomments
 */

#ifndef XPLATFORM_INFO_H		/* prevent circular inclusions */
#define XPLATFORM_INFO_H		/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "xil_types.h"

/************************** Constant Definitions *****************************/
#if defined (versal)
#define XPAR_PMC_TAP_BASEADDR 0xF11A0000U
#define XPAR_PMC_TAP_VERSION_OFFSET 0x00000004U
#define XPLAT_PS_VERSION_ADDRESS (XPAR_PMC_TAP_BASEADDR + \
									XPAR_PMC_TAP_VERSION_OFFSET)
#else
#define XPAR_CSU_BASEADDR 0xFFCA0000U
#define	XPAR_CSU_VER_OFFSET 0x00000044U
#define XPLAT_PS_VERSION_ADDRESS (XPAR_CSU_BASEADDR + \
									XPAR_CSU_VER_OFFSET)
#endif
#define XPLAT_ZYNQ_ULTRA_MP_SILICON 0x0
#define XPLAT_ZYNQ_ULTRA_MP 0x1
#define XPLAT_ZYNQ_ULTRA_MPVEL 0x2
#define XPLAT_ZYNQ_ULTRA_MPQEMU 0x3
#define XPLAT_ZYNQ 0x4
#define XPLAT_MICROBLAZE 0x5
#define XPLAT_VERSAL 0x6U

#define XPS_VERSION_1 0x0
#define XPS_VERSION_2 0x1
#define XPLAT_INFO_MASK (0xFU)

#if defined (versal)
#define XPS_VERSION_INFO_MASK 0xFF00U
#define XPS_VERSION_INFO_SHIFT 0x8U
#define XPLAT_INFO_SHIFT 0x18U
#else
#define XPS_VERSION_INFO_MASK (0xF)
#define XPS_VERSION_INFO_SHIFT 0x0U
#define XPLAT_INFO_SHIFT 0xCU
#endif

/**************************** Type Definitions *******************************/
/**
 *@endcond
 */
/***************** Macros (Inline Functions) Definitions *********************/


u32 XGetPlatform_Info(void);

#if defined (ARMR5) || defined (__aarch64__) || defined (ARMA53_32) || defined (PSU_PMU) || defined (versal)
u32 XGetPSVersion_Info(void);
#endif

#if defined (ARMR5) || defined (__aarch64__) || defined (ARMA53_32)
u32 XGet_Zynq_UltraMp_Platform_info(void);
#endif
/************************** Function Prototypes ******************************/


#ifdef __cplusplus
}
#endif

#endif /* end of protection macro */
/**
* @} End of "addtogroup common_platform_info".
*/
