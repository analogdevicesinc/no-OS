/***************************************************************************//**
 *   @file   maxim_spi.h
 *   @brief  maxim specific header for SPI driver
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MAXIM_SPI_H_
#define MAXIM_SPI_H_

#include <stdint.h>
#include "no_os_spi.h"

/**
 * @brief maxim specific SPI platform ops structure
 */
extern const struct no_os_spi_platform_ops max_spi_ops;

/**
 * @brief Slave Select pin polarity
 */
enum spi_ss_polarity {
	SPI_SS_POL_LOW,
	SPI_SS_POL_HIGH
};

/**
 * @brief Maxim specific SPI initialization parameters
 */
struct max_spi_init_param {
	uint32_t num_slaves;
	enum spi_ss_polarity polarity;
};

#endif
