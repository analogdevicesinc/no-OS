/***************************************************************************//**
*   @file   ADF4157_cfg.h
*   @brief  Header file of ADF4157 Driver Configuration.
*   @author Lucian Sin (Lucian.Sin@analog.com)
*
********************************************************************************
* Copyright 2013(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __AD4157_CFG_H__
#define __AD4157_CFG_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "adf4157.h"

/******************************************************************************/
/************************ Constants Definitions *******************************/
/******************************************************************************/
static struct adf4157_platform_data adf4157_pdata_lpc = {
	10000000,                   //clkin
	0,                          //ref_doubler_en
	0,                          //ref_div2_en
	ADF4157_INT_VAL(23),        //r0_user_settings
	0,                          //r2_user_settings
	ADF4157_PD_POL(1),          //r3_user_settings
	0,                          //r4_user_settings
};

#endif // __AD4157_CFG_H__
