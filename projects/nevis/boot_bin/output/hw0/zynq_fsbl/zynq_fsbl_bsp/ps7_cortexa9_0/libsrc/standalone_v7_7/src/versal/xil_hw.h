/******************************************************************************
* Copyright (c) 2021 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


/*****************************************************************************/
/**
*
* @file xil_hw.h
*
* This is the header file which contains definitions for the hardware
* registers.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- -------- -------------------------------------------------------
* 7.7   bsv   02/21/2017 Initial release
* 7.8   skd   03/09/2022 Compilation warning fix
*
* </pre>
*
* @note
*
******************************************************************************/

#ifndef XIL_HW_H
#define XIL_HW_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

/**@cond xil_internal
 * @{
 */

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/
/*
 * PMC_GLOBAL Base Address
 */
#define PMC_GLOBAL_BASEADDR     (0XF1110000U)

/*
 * Register: PMC_GLOBAL_DOMAIN_ISO_CNTRL
 */
#define PMC_GLOBAL_DOMAIN_ISO_CNTRL_PMC_PL_CFRAME_MASK   (0X00000400U)
#define PMC_GLOBAL_DOMAIN_ISO_CNTRL_PMC_PL_TEST_MASK   (0X00000800U)
/*
 * Definitions required from pmc_tap.h
 */
#define PMC_TAP_BASEADDR		(0XF11A0000U)
#define PMC_TAP_IDCODE		(PMC_TAP_BASEADDR + 0X00000000U)
#define PMC_TAP_VERSION		(PMC_TAP_BASEADDR + 0X00000004U)
#define PMC_TAP_VERSION_PMC_VERSION_MASK		(0X000000FFU)
/*
 * Definitions required from crp.h
 */
#define CRP_BASEADDR		(0XF1260000U)
#define CRP_RESET_REASON		(CRP_BASEADDR + 0X00000220U)
#define CRP_RST_NONPS		(CRP_BASEADDR + 0X00000320U)
#define CRP_RST_NONPS_NPI_RESET_MASK		(0X10U)
/*
 * Register: CRP_RST_PS
 */
#define CRP_RST_PS		(CRP_BASEADDR + 0x0000031CU)
/*
 * Register: PMC_IOU_SLCR
 */
#define PMC_IOU_SLCR_BASEADDR      (0XF1060000U)

/**
 * @}
 * @endcond
 */

#ifdef __cplusplus
}
#endif

#endif  /* XIL_HW_H */
