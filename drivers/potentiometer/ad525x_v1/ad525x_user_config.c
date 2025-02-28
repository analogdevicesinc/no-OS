/***************************************************************************//**
 * @file   ad5259_user_config.c
 * @brief  User configurations for AD5259 No-OS drivers
********************************************************************************
* Copyright (c) 2024 Analog Devices, Inc.
* All rights reserved.
*
* This software is proprietary to Analog Devices, Inc. and its licensors.
* By using this software you agree to the terms of the associated
* Analog Devices Software License Agreement.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "dpot_user_config.h"

/******************************************************************************/
/********************* Macros and Constants Definition ************************/
/******************************************************************************/

/******************************************************************************/
/******************** Variables and User Defined Data Types *******************/
/******************************************************************************/

/* AD5259 No-OS driver init parameters */

struct ad5259_dpot_init_param ad5259_init_params = {
	/* Select interface type */
	.eIntfType = AD_I2C_INTERFACE,
	/* Define interface init parameters */
	.i2c_init = &i2c_init_params,
	/* Define gpio init parameters */
	.operating_mode = DPOT_DEFAULT_OPERATING_MODE,
};
