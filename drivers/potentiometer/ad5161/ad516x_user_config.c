/***************************************************************************//**
 * @file   ad516x_user_config.c
 * @brief  User configurations for AD5161 No-OS drivers
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

/* AD5161 No-OS driver init parameters */
struct ad516x_dpot_init_param ad5161_init_params = {
	/* Select interface type */
	.eintf_type = AD_I2C_INTERFACE,
	/* I2C Define interface init parameters */
	.i2c_init = &i2c_init_params,
	/* SPI Define interface init parameters */
	.spi_init = &spi_init_params,
	/* GPIO for Digital interface selection. */
	.dis_gpio_init = &dis_gpio_init_params,
};

/* AD5165 No-OS driver init parameters */
struct ad516x_dpot_init_param ad5165_init_params = {
	/* Select interface type */
	.eintf_type = AD_I2C_INTERFACE,
	/* SPI Define interface init parameters */
	.spi_init = &spi_init_params,
	/* GPIO for Digital interface selection. */
	.dis_gpio_init = &dis_gpio_init_params,
};
