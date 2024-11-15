/***************************************************************************//**
 *   @file   chibios/chibios_spi.h
 *   @brief  Header file for the chibios spi driver.
 *   @author Robert Budai (robert.budai@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef CHIBIOS_SPI_H_
#define CHIBIOS_SPI_H_

#include <stdint.h>
#include "no_os_spi.h"
#include "hal.h"

#if (HAL_USE_SPI==TRUE)

/**
 * @struct chibios_spi_init_param
 * @brief Structure holding the initialization parameters for chibios platform
 * specific SPI parameters.
 */

/*
 * TO DO: extend SPI for more than 8 byte transfers (?)
 * 1/2 word configuration
 */

struct chibios_spi_init_param {
	/** SPI instance */
	SPIDriver *hspi;
	/**	SPI config struct*/
	SPIConfig *spicfg;
};

/**
 * @struct chibios_spi_desc
 * @brief chibios platform specific SPI descriptor
 */
struct chibios_spi_desc {
	/** SPI instance */
	SPIDriver *hspi;
	/**	SPI config struct*/
	SPIConfig *spicfg;
};

/**
 * @brief chibios specific SPI platform ops structure
 */
extern const struct no_os_spi_platform_ops chibios_spi_ops;

#endif // HAL_USE_SPI==TRUE

#endif // CHIBIOS_SPI_H_
