/***************************************************************************//**
 * @file   ad524x_user_config.c
 * @brief  User configurations for AD524x No-OS drivers
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

/* AD5242 No-OS driver init parameters */

struct ad5242_dpot_init_param ad5242_init_params = {
	/* Select interface type */
	.eIntfType = AD_I2C_INTERFACE,
	/* Define interface init parameters */
	.i2c_init = &i2c_init_params,
	.shdn_gpio_init = &shdn_gpio_init_params
};

/* AD5246 No-OS driver init parameters */
struct ad5246_dpot_init_param ad5246_init_params = {
	/* Select interface type */
	.eIntfType = AD_I2C_INTERFACE,
	/* Define interface init parameters */
	.i2c_init = &i2c_init_params,
};