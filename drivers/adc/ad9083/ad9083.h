/***************************************************************************//**
 *   @file   ad9083.h
 *   @brief  Header file of ad9083 Driver.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __AD9083_H__
#define __AD9083_H__

#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "adi_ad9083.h"

/**
 * @struct ad9083_init_param
 * @brief Structure holding the parameters for ad9083 initialization.
 */
struct ad9083_init_param {
	/* SPI */
	struct no_os_spi_init_param	*spi_init;
	/* GPIO reset */
	struct no_os_gpio_init_param	*gpio_reset;
	/* GPIO power down */
	struct no_os_gpio_init_param	*gpio_pd;
	/* Settings selection */
	uint8_t uc;
	/* jesd receive clock descriptor */
	struct no_os_clk_desc *jesd_rx_clk_desc;
};

/**
 * @struct ad9083_phy
 * @brief Structure holding ad9083 descriptor.
 */
struct ad9083_phy {
	/* SPI */
	struct no_os_spi_desc 	*spi_desc;
	/* GPIO */
	struct no_os_gpio_desc	*gpio_reset;
	/* GPIO power down */
	struct no_os_gpio_desc	*gpio_pd;
	/* GPIO reference selection */
	struct no_os_gpio_desc	*gpio_ref_sel;
	/* adi ad9083 device*/
	adi_ad9083_device_t	adi_ad9083;
};

/* Initialize the device. */
int32_t ad9083_init(struct ad9083_phy **device,
		    struct ad9083_init_param *init_param);

/* Remove the device. */
int32_t ad9083_remove(struct ad9083_phy *device);

/* Read device register. */
int32_t ad9083_reg_get(struct ad9083_phy *device, uint32_t reg,
		       uint8_t *readval);

/* Write device register. */
int32_t ad9083_reg_set(struct ad9083_phy *device, uint32_t reg,
		       uint8_t writeval);

#endif // __AD9083_H__
