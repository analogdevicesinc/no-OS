/***************************************************************************//**
 *   @file   ltc4332.h
 *   @brief  Header file of SPI ltc4332 Interface
 *   @author Paul Benoit (paul.benoit@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SRC_LTC4332_SPI_H_
#define SRC_LTC4332_SPI_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_spi.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct no_os_spi_desc
 * @brief Structure initialization with the platform specific SPI functions
 */
extern const struct no_os_spi_platform_ops ltc4332_spi_platform_ops;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initialize the SPI communication peripheral. */
int32_t ltc4332_spi_init(struct no_os_spi_desc **desc,
			 const struct no_os_spi_init_param *param);

/* Free the resources allocated by no_os_spi_init(). */
int32_t ltc4332_spi_remove(struct no_os_spi_desc *desc);

/* Write and read data to/from SPI. */
int32_t ltc4332_spi_write_and_read(struct no_os_spi_desc *desc, uint8_t *data,
				   uint16_t bytes_number);

#endif /* SRC_LTC4332_SPI_H_ */
