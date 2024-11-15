/***************************************************************************//**
 *   @file   altera/altera_spi.h
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ALTERA_SPI_H_
#define ALTERA_SPI_H_

#include "no_os_spi.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum spi_type
 * @brief Altera platform architecture types
 */
enum spi_type {
	/** Nios II architecture */
	NIOS_II_SPI
};

/**
 * @struct altera_spi_init_param
 * @brief Structure holding the initialization parameters for Altera platform
 * specific SPI parameters.
 */
struct altera_spi_init_param {
	/** Altera architecture type */
	enum spi_type	type;
	/** SPI base address */
	uint32_t	base_address;
};

/**
 * @struct altera_spi_desc
 * @brief Altera platform specific SPI descriptor
 */
struct altera_spi_desc {
	/** Altera architecture type */
	enum spi_type	type;
	/** SPI base address */
	uint32_t	base_address;
};

/**
 * @brief Altera specific SPI platform ops structure
 */
extern const struct no_os_spi_platform_ops altera_spi_ops;

#endif /* ALTERA_SPI_H_ */
