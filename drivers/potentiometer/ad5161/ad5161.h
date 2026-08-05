/***************************************************************************//**
 *   @file   ad5161.h
 *   @brief  Header file for the ad5161 digital potentiometer drivers
********************************************************************************
Copyright 2025(c) Analog Devices, Inc.

SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef AD5161_H_
#define AD5161_H_

#include "../common/dpot.h"
#include "no_os_spi.h"
#include "no_os_i2c.h"
#include "no_os_gpio.h"
#include "no_os_util.h"

/* Control word for READ and WRITE to AD5161*/
#define AD5161_CTL_FOR_READ_WRITE 0x00
/* Control word for placing the AD5161 in shutdown condition*/
#define AD5161_CTL_FOR_EN_SHUTWOWN 0x20
/* Control word for placing the AD5161 in normal condition*/
#define AD5161_CTL_FOR_DIS_SHUTWOWN 0x80
/* Control word for placing the AD5161 in reset state(mid range ) */
#define AD5161_CTL_FOR_RESET_POSITION 0x40

/**
 * @struct ad516x_dpot_init_param.
 * @brief ad516x digital potentiometer init parameters.
 */
struct ad516x_dpot_init_param {
	/* Interface type */
	enum dpot_intf_type eintf_type;
	/* SPI init parameters */
	struct no_os_spi_init_param *spi_init;
	/* I2C init parameters */
	struct no_os_i2c_init_param *i2c_init;
	/* Digital interface select GPIO init parameters */
	struct no_os_gpio_init_param *dis_gpio_init;
};



/**
 * @struct ad5161_dpot_dev.
 * @brief ad5161 digital potentiometer device descriptor parameters.
 */
struct ad5161_dpot_dev {
	enum dpot_intf_type intf_type;
	/* I2C descriptor */
	struct no_os_i2c_desc *i2c_desc;

	struct no_os_spi_desc *spi_desc;
	/* Digital interface select GPIO descriptor */
	struct no_os_gpio_desc *dis_gpio_desc;
};
#endif	// ad5161_H_
