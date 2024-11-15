/***************************************************************************//**
*   @file   adf4156_cfg.h
*   @brief  Header file of ADF4156 Driver Configuration.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
********************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __AD4156_CFG_H__
#define __AD4156_CFG_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "adf4156.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/
static struct adf4156_platform_data adf4156_pdata_lpc = {
	10000000,                   //clkin in Hz
	20000,                      //channel_spacing in Hz
	1,                          //ref_doubler_en
	0,                          //ref_div2_en
	ADF4156_INT_VAL(23),        //r0_user_settings
	ADF4156_MOD_WORD(2),        //r2_user_settings
	ADF4156_PD_POL(1),          //r3_user_settings
	0,                          //r4_user_settings
};

#endif // __AD4156_CFG_H__
