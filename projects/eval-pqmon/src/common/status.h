/*******************************************************************************
 *   @file   status.h
 *   @brief  Status state definition header file
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __STATUS_H__
#define __STATUS_H__

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define SYS_STATUS_SUCCESS 0
#define SYS_STATUS_PQLIB_ERROR 1
#define SYS_STATUS_NO_SIGNAL 2
#define SYS_STATUS_PQLIB_RUNNING 3
#define SYS_STATUS_PQLIB_CONFIG_ERROR 4
#define SYS_STATUS_AFE_CONFIG0_FAILED 5
#define SYS_STATUS_AFE_CONFIG1_FAILED 6
#define SYS_STATUS_AFE_CONFIG2_FAILED 7
#define SYS_STATUS_AFE_ACCMODE_FAILED 8
#define SYS_STATUS_AFE_ZX_LP_SEL_FAILED 9
#define SYS_STATUS_AFE_MASK0_FAILED 10
#define SYS_STATUS_AFE_RUN_FAILED 11
#define SYS_STATUS_AFE_WFB_CFG_FAILED 12
#define SYS_STATUS_AFE_READWAVEFORM_FAILED 13
#define SYS_STATUS_AFE_READRMS1012_FAILED 14
#define SYS_STATUS_AFE_READRMSONE_FAILED 15
#define SYS_STATUS_AFE_READPERIOD_FAILED 16
#define SYS_STATUS_AFE_READANGLE_FAILED 17
#define SYS_STATUS_AFE_READVERSION_FAILED 18
#define SYS_STATUS_AFE_STATUS1_FAILED 19
#define SYS_STATUS_AFE_STATUS0_FAILED 20
#define SYS_STATUS_TIMER_INIT_FAILED 21

#endif /* __AFE_CONFIG_H__ */
