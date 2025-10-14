/******************************************************************************
* Copyright (c) 2017 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xil_sleeptimer.h
*
* This header file contains ARM Cortex A53,A9,R5 specific sleep related APIs.
* For sleep related functions that can be used across all Xilinx supported
* processors, please use xil_sleeptimer.h.
*
*
* <pre>
* MODIFICATION HISTORY :
*
* Ver   Who  Date	 Changes
* ----- ---- -------- -------------------------------------------------------
* 6.6	srm  10/18/17 First Release.
* 7.0   mus  01/07/19 Add cpp extern macro
* 7.7	sk   01/10/22 Add explicit parentheses for EL3==1 to fix
* 		      misra_c_2012_rule_12_1 violation.
* 7.7	sk   01/10/22 Add void to XTime_StartTTCTimer function declaration
* 		      to fix misra_c_2012_rule_8_2 violation.
*
* </pre>
*****************************************************************************/

/**
 *@cond nocomments
 */
#ifndef XIL_SLEEPTIMER_H		/* prevent circular inclusions */
#define XIL_SLEEPTIMER_H		/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/****************************  Include Files  ********************************/

#include "xil_io.h"
#include "xparameters.h"
#include "bspconfig.h"

/************************** Constant Definitions *****************************/

#if defined (ARMR5) || (__aarch64__) || (ARMA53_32)
#define XSLEEP_TIMER_REG_SHIFT  32U
#define XSleep_ReadCounterVal   Xil_In32
#define XCntrVal 			    u32
#else
#define XSLEEP_TIMER_REG_SHIFT  16U
#define XSleep_ReadCounterVal   Xil_In16
#define XCntrVal 			    u16
#endif

#if defined(ARMR5) || (defined (__aarch64__) && (EL3==1)) || defined (ARMA53_32)
#if defined (versal)
#define CRL_TTC_RST    0xFF5E0344U
#define CRL_TTC_BASE_RST_MASK    0x1U
#else
#define RST_LPD_IOU2 					    0xFF5E0238U
#define RST_LPD_IOU2_TTC_BASE_RESET_MASK 	0x00000800U
#endif
#endif

#if defined (SLEEP_TIMER_BASEADDR)
/** @name Register Map
*
* Register offsets from the base address of the TTC device
*
* @{
*/
 #define XSLEEP_TIMER_TTC_CLK_CNTRL_OFFSET		0x00000000U
					     /**< Clock Control Register */
 #define XSLEEP_TIMER_TTC_CNT_CNTRL_OFFSET		0x0000000CU
	                                     /**< Counter Control Register*/
 #define XSLEEP_TIMER_TTC_COUNT_VALUE_OFFSET	0x00000018U
					     /**< Current Counter Value */
/* @} */
/** @name Clock Control Register
* Clock Control Register definitions of TTC
* @{
*/
 #define XSLEEP_TIMER_TTC_CLK_CNTRL_PS_EN_MASK		0x00000001U
						   /**< Prescale enable */
/* @} */
/** @name Counter Control Register
* Counter Control Register definitions of TTC
* @{
*/
#define XSLEEP_TIMER_TTC_CNT_CNTRL_DIS_MASK		0x00000001U
						/**< Disable the counter */
#define XSLEEP_TIMER_TTC_CNT_CNTRL_RST_MASK		0x00000010U
						  /**< Reset counter */
/* @} */

/**************************** Type Definitions *******************************/
/**
 *@endcond
 */
/************************** Function Prototypes ******************************/

void Xil_SleepTTCCommon(u32 delay, u64 frequency);
void XTime_StartTTCTimer(void);

#endif

#ifdef __cplusplus
}
#endif

#endif /* XIL_SLEEPTIMER_H */
