/***************************************************************************//**
*   @file   mdio_spi.h
*   @brief  Header file for MDIO implementation.
*   @author Johnson Ralph Perez (Johnsonralph.Perez@analog.com)
********************************************************************************
* Copyright 2026(c) Analog Devices, Inc.
*
* SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MDIO_SPI_H_
#define MDIO_SPI_H_

#include <stdint.h>
#include "no_os_spi.h"
#include "no_os_mdio.h"

/** MDIO SPI Preamble value (all 1s) */
#define MDIO_SPI_PREAMBLE		UINT32_MAX
/** Data field value for read operations */
#define MDIO_SPI_DATA_FIELD_READ	0xFFFF
/** Half buffer size in bytes */
#define MDIO_SPI_BUFF_SIZE_HALF_UINT8	4
/** Full buffer size in bytes */
#define MDIO_SPI_BUFF_SIZE_UINT8	8

/**
 * @struct mdio_spi_init_param
 * @brief Initialization parameter for the MDIO SPI interface.
 */
struct mdio_spi_init_param {
	/** SPI initialization parameters for the MDIO bus */
	struct no_os_spi_init_param mdio;
};

extern struct no_os_mdio_ops mdio_spi_ops;

#endif /* MDIO_SPI_H_ */
