/******************************************************************************
* Copyright (c) 2014 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xplatform_info.c
* @addtogroup common_platform_info Hardware Platform Information
* @{
* This file contains information about hardware for which the code is built
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date   Changes
* ----- ---- -------- -------------------------------------------------------
* 5.00  pkp  12/15/14 Initial release
* 5.04  pkp  01/12/16 Added platform information support for Cortex-A53 32bit
*					  mode
* 6.00  mus  17/08/16 Removed unused variable from XGetPlatform_Info
* 6.4   ms   05/23/17 Added PSU_PMU macro to support XGetPSVersion_Info
*                     function for PMUFW.
*       ms   06/13/17 Added PSU_PMU macro to provide support of
*                     XGetPlatform_Info function for PMUFW.
*       mus  08/17/17 Add EL1 NS mode support for
*                     XGet_Zynq_UltraMp_Platform_info and XGetPSVersion_Info
*                     APIs.
* 7.0	aru 03/15/19  Check for versal before aarch64 and armr5
*		      in XGetPlatform_Info()
* 7.2   adk 08/01/20  Added versal support for the XGetPSVersion_Info function.
* 7.6   mus 08/23/21  Updated prototypes for functions which are not taking any
*                     arguments with void keyword. This has been done to fix
*                     compilation warnings with "-Wstrict-prototypes" flag.
*                     It fixes CR#1108601.
* 7.6    mus 08/30/21 Updated flag checking to fix compilation warnings
*                     reported with "-Wundef" flag. It fixes CR#1108601.

* 7.7    mus 11/02/21 Updated XGet_Zynq_UltraMp_Platform_info and
*                     XGetPSVersion_Info to fix compilation warning
*                     reported with "-Wundef" flag CR#1111453
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xil_types.h"
#include "xil_io.h"
#include "xplatform_info.h"
#if defined (__aarch64__)
#include "bspconfig.h"
#include "xil_smc.h"
#endif
/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/


/************************** Function Prototypes ******************************/

/*****************************************************************************/
/**
*
* @brief    This API is used to provide information about platform
*
* @return   The information about platform defined in xplatform_info.h
*
******************************************************************************/
u32 XGetPlatform_Info(void)
{
#if defined (versal)
	return XPLAT_VERSAL;
#elif defined (ARMR5) || defined (__aarch64__) || defined (ARMA53_32) || defined (PSU_PMU)
	return XPLAT_ZYNQ_ULTRA_MP;
#elif defined (__microblaze__)
	return XPLAT_MICROBLAZE;
#else
	return XPLAT_ZYNQ;
#endif
}

/*****************************************************************************/
/**
*
* @brief    This API is used to provide information about zynq ultrascale MP platform
*
* @return   The information about zynq ultrascale MP platform defined in
*			xplatform_info.h
*
******************************************************************************/
#if defined (ARMR5) || defined (__aarch64__) || defined (ARMA53_32)
u32 XGet_Zynq_UltraMp_Platform_info(void)
{
#if defined (__aarch64__) && (EL1_NONSECURE == 1)
	XSmc_OutVar reg;
    /*
	 * This SMC call will return,
     *  idcode - upper 32 bits of reg.Arg0
     *  version - lower 32 bits of reg.Arg1
	 */
	reg = Xil_Smc(GET_CHIPID_SMC_FID,0,0, 0, 0, 0, 0, 0);
	return (u32)((reg.Arg1 >> XPLAT_INFO_SHIFT) & XPLAT_INFO_MASK);
#else
	u32 reg;
	reg = ((Xil_In32(XPLAT_PS_VERSION_ADDRESS) >> XPLAT_INFO_SHIFT )
		& XPLAT_INFO_MASK);
	return reg;
#endif
}
#endif

/*****************************************************************************/
/**
*
* @brief    This API is used to provide information about PS Silicon version
*
* @return   The information about PS Silicon version.
*
******************************************************************************/
#if defined (ARMR5) || defined (__aarch64__) || defined (ARMA53_32) || defined (PSU_PMU) || defined (versal)
u32 XGetPSVersion_Info(void)
{
#if defined (__aarch64__) && (EL1_NONSECURE == 1)
        /*
         * This SMC call will return,
         *  idcode - upper 32 bits of reg.Arg0
         *  version - lower 32 bits of reg.Arg1
         */
        XSmc_OutVar reg;
        reg = Xil_Smc(GET_CHIPID_SMC_FID,0,0, 0, 0, 0, 0, 0);
        return (u32)((reg.Arg1 &  XPS_VERSION_INFO_MASK) >>
		XPS_VERSION_INFO_SHIFT);
#else
	u32 reg;
	reg = (Xil_In32(XPLAT_PS_VERSION_ADDRESS)
			& XPS_VERSION_INFO_MASK);
	return (reg >> XPS_VERSION_INFO_SHIFT);
#endif
}
#endif
/** @} */
