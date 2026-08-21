/*******************************************************************************
 *   @file   ftd2xx_spi.h
 *   @brief  Implementation of SPI driver for ftd2xx platform.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef FTD2XX_SPI_H_
#define FTD2XX_SPI_H_

#include "no_os_spi.h"

#include "ftd2xx.h"

/**
 * @struct ftd2xx_spi_init
 * @brief Structure holding the initialization parameters for ftd2xx platform
 * 	  specific SPI parameters.
 */
struct ftd2xx_spi_init {
	/** Selected device specific PIN configuration.
	 *  BIT7-BIT0: Initial direction of the pins
	 *  BIT15-BIT8: Initial values of the pins
	 *  BIT23-BIT16: Final direction of the pins
	 *  BIT31-BIT24: Final values of the pins
	 */
	uint32_t channel_config_pin;
};

/**
 * @struct ftd2xx_spi_desc
 * @brief ftd2xx platform specific SPI descriptor
 */
struct ftd2xx_spi_desc {
	/** Specific device HANDLE. */
	FT_HANDLE ftHandle;
};

/**
 * @brief ftd2xx specific SPI platform ops structure
 */
extern const struct no_os_spi_platform_ops ftd2xx_spi_ops;

#endif
