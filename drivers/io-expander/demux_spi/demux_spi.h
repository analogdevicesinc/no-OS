/***************************************************************************//**
 *   @file   demux_spi.h
 *   @brief  Header file of SPI Demux Interface
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SRC_DEMUX_SPI_H_
#define SRC_DEMUX_SPI_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define CS_OFFSET 0x80

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct no_os_spi_desc
 * @brief Structure initialization with the platform specific SPI functions
 */
extern const struct no_os_spi_platform_ops demux_spi_platform_ops;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initialize the SPI communication peripheral. */
int32_t demux_spi_init(struct no_os_spi_desc **desc,
		       const struct no_os_spi_init_param *param);

/* Free the resources allocated by no_os_spi_init(). */
int32_t demux_spi_remove(struct no_os_spi_desc *desc);

/* Write and read data to/from SPI. */
int32_t demux_spi_write_and_read(struct no_os_spi_desc *desc, uint8_t *data,
				 uint16_t bytes_number);

#endif /* SRC_DEMUX_SPI_H_ */
