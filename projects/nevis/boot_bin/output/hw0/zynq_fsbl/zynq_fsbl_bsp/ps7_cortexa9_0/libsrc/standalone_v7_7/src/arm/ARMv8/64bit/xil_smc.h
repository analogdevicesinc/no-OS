/******************************************************************************
* Copyright (c) 2017 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xil_smc.h
*
* @addtogroup a53_64_smc_api Cortex A53 64bit EL1 Non-secure SMC Call
*
* Cortex A53 64bit EL1 Non-secure SMC Call provides a C wrapper for calling
* SMC from EL1 Non-secure application to request Secure monitor for secure
* services. SMC calling conventions should be followed.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 6.2 	pkp  	 02/16/17 First release
* 6.4   mus      08/17/17 Added constant define for SMC ID , which is
*                         intended to read the version/idcode of the
*                         platform
* 7.1  mus       07/31/19 Added support for Versal
*
*
* </pre>
*
******************************************************************************/

#ifndef XIL_SMC_H /* prevent circular inclusions */
#define XIL_SMC_H /* by using protection macros */

/***************************** Include Files ********************************/

#include "xil_types.h"
#include "bspconfig.h"

#ifdef __cplusplus
extern "C" {
#endif
#if EL1_NONSECURE
/************************** Constant Definitions ****************************/
#define SMC_FID_START	0xF2000000
#define SMC_FID_END	0xFF00FFFF

#define XILSP_INIT_DONE 0xF2000000
#define	ARITH_SMC_FID	0xF2000001

#define PM_IOCTL_SMC_FID	0xC2000022U
#define PM_IOCTL_OSPI_MUX_SELECT	0x15U
#define PIN_CONFIG_SCHMITT_CMOS		0x3U
#define PIN_CONFIG_TRI_STATE		0x6U
#define PM_OSPI_MUX_SEL_DMA		0x0
#define PM_OSPI_MUX_SEL_LINEAR	0x1
#define OSPI_NODE_ID	0x1822402a
#define PMC_GPIO_NODE_12_ID		0x14108027
#define PIN_REQUEST_SMC_FID		0xC200001CU
#define PIN_RELEASE_SMC_FID		0xC200001DU
#define PIN_SET_CONFIG_SMC_FID		0xC2000021U
#define PM_REQUEST_DEVICE_SMC_FID	0xC200000DU
#define PM_RELEASE_DEVICE_SMC_FID	0xC200000EU
#define PM_ASSERT_SMC_FID       0xC2000011U
#define PM_GETSTATUS_SMC_FID    0xC2000012U
#define MMIO_WRITE_SMC_FID	0xC2000013U
#define MMIO_READ_SMC_FID	0xC2000014U
#define GET_CHIPID_SMC_FID      0xC2000018U

/* GEM device IDs */
#define	DEV_GEM_0			0x18224019
#define	DEV_GEM_1			0x1822401a

/* GEM reference clock IDs */
#define	CLK_GEM0_REF			0x8208058
#define	CLK_GEM1_REF			0x8208059

/* PM API for setting clock divider */
#define PM_SET_DIVIDER_SMC_FID		0xC2000027U

/**************************** Type Definitions ******************************/
typedef struct {
	u64 Arg0;
	u64 Arg1;
	u64 Arg2;
	u64 Arg3;
} XSmc_OutVar;
/***************** Macros (Inline Functions) Definitions ********************/

#define XSave_X8toX17() \
	__asm__ __volatile__ ("stp X8, X9, [sp,#-0x10]!");\
	__asm__ __volatile__ ("stp X10, X11, [sp,#-0x10]!");\
	__asm__ __volatile__ ("stp X12, X13, [sp,#-0x10]!");\
	__asm__ __volatile__ ("stp X14, X15, [sp,#-0x10]!");\
	__asm__ __volatile__ ("stp X16, X17, [sp,#-0x10]!");

#define XRestore_X8toX17() \
	__asm__ __volatile__ ("ldp X16, X17, [sp], #0x10");\
	__asm__ __volatile__ ("ldp X14, X15, [sp], #0x10");\
	__asm__ __volatile__ ("ldp X12, X13, [sp], #0x10");\
	__asm__ __volatile__ ("ldp X10, X11, [sp], #0x10");\
	__asm__ __volatile__ ("ldp X8, X9, [sp], #0x10");

/************************** Variable Definitions ****************************/

/************************** Function Prototypes *****************************/
XSmc_OutVar Xil_Smc(u64 FunctionID, u64 Arg1, u64 Arg2, u64 Arg3, u64 Arg4,
					u64 Arg5, u64 Arg6, u64 Arg7);
#endif
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XIL_SMC_H */
/**
* @} End of "addtogroup a53_64_smc_api".
*/
