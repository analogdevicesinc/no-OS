/***************************************************************************//**
 * @file   ad514x_user_config.c
 * @brief  User configurations for AD514x No-OS drivers
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

/* AD5141 No-OS driver init parameters */
struct ad5141_dpot_init_param ad5141_init_params = {
	/* Select interface type */
	.eintf_type = AD_SPI_INTERFACE,
	/* Define interface init parameters */
	.spi_init = &spi_init_params,
	.i2c_init = &i2c_init_params,

	/* Define gpio init parameters */
	.reset_gpio_init = &reset_gpio_init_params,
	.dis_gpio_init = &dis_gpio_init_params,
	.lrdac_gpio_init = &lrdac_gpio_init_params,
	.wp_gpio_init = &wp_gpio_init_params,
	.indep_gpio_init = &indep_gpio_init_params,
	.eoperating_mode = DPOT_DEFAULT_OPERATING_MODE,
	.shutdown_enable_pt = {
		DPOT_RDAC1_DEFAULT_SHUTDOWN
	},
	.shutdown_enable_lg = {
		DPOT_RAW1_DEFAULT_SHUTDOWN,
		DPOT_RWB1_DEFAULT_SHUTDOWN
	}
};

/* AD5142 No-OS driver init parameters */
struct ad5142_dpot_init_param ad5142_init_params = {
	.eintf_type = AD_SPI_INTERFACE,
	/* Define interface init parameters */
	.spi_init = &spi_init_params,
	.i2c_init = &i2c_init_params,
	/* Define gpio init parameters */
	.reset_gpio_init = &reset_gpio_init_params,
	.dis_gpio_init = &dis_gpio_init_params,
	.lrdac_gpio_init = &lrdac_gpio_init_params,
	.wp_gpio_init = &wp_gpio_init_params,
	.indep_gpio_init = &indep_gpio_init_params,
	.eoperating_mode = DPOT_DEFAULT_OPERATING_MODE,
	.shutdown_enable_pt = {
		DPOT_RDAC1_DEFAULT_SHUTDOWN,
		DPOT_RDAC2_DEFAULT_SHUTDOWN
	},
	.shutdown_enable_lg = {
		DPOT_RAW1_DEFAULT_SHUTDOWN,
		DPOT_RWB1_DEFAULT_SHUTDOWN,
		DPOT_RAW2_DEFAULT_SHUTDOWN,
		DPOT_RWB2_DEFAULT_SHUTDOWN
	}
};

/* AD5144 No-OS driver init parameters */
struct ad5144_dpot_init_param ad5144_init_params = {
	/* Define interface init parameters */
	.eintf_type = AD_SPI_INTERFACE,
	.spi_init = &spi_init_params,
	.i2c_init = &i2c_init_params,
	/* Define gpio init parameters */
	.reset_gpio_init = &reset_gpio_init_params,
	.indep_gpio_init = &indep_gpio_init_params,
	.dis_gpio_init  = &dis_gpio_init_params,
	.eoperating_mode = DPOT_DEFAULT_OPERATING_MODE,
	.shutdown_enable_pt = {
		DPOT_RDAC1_DEFAULT_SHUTDOWN,
		DPOT_RDAC2_DEFAULT_SHUTDOWN,
		DPOT_RDAC3_DEFAULT_SHUTDOWN,
		DPOT_RDAC4_DEFAULT_SHUTDOWN
	},
	.shutdown_enable_lg = {
		DPOT_RAW1_DEFAULT_SHUTDOWN,
		DPOT_RWB1_DEFAULT_SHUTDOWN,
		DPOT_RAW2_DEFAULT_SHUTDOWN,
		DPOT_RWB2_DEFAULT_SHUTDOWN,
		DPOT_RAW3_DEFAULT_SHUTDOWN,
		DPOT_RWB3_DEFAULT_SHUTDOWN,
		DPOT_RAW4_DEFAULT_SHUTDOWN,
		DPOT_RWB4_DEFAULT_SHUTDOWN
	}
};

/* AD5143 No-OS driver init parameters */
struct ad5143_dpot_init_param ad5143_init_params = {
	/* Define interface init parameters */
	.i2c_init = &i2c_init_params,
	.eoperating_mode = DPOT_DEFAULT_OPERATING_MODE,
	.shutdown_enable_pt = {
		DPOT_RDAC1_DEFAULT_SHUTDOWN,
		DPOT_RDAC2_DEFAULT_SHUTDOWN,
		DPOT_RDAC3_DEFAULT_SHUTDOWN,
		DPOT_RDAC4_DEFAULT_SHUTDOWN
	},
	.shutdown_enable_lg = {
		DPOT_RAW1_DEFAULT_SHUTDOWN,
		DPOT_RWB1_DEFAULT_SHUTDOWN,
		DPOT_RAW2_DEFAULT_SHUTDOWN,
		DPOT_RWB2_DEFAULT_SHUTDOWN,
		DPOT_RAW3_DEFAULT_SHUTDOWN,
		DPOT_RWB3_DEFAULT_SHUTDOWN,
		DPOT_RAW4_DEFAULT_SHUTDOWN,
		DPOT_RWB4_DEFAULT_SHUTDOWN,
	},
};

