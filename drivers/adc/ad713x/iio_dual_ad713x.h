/***************************************************************************//**
*   @file   iio_ad713x.h
*   @brief  Header file of iio_dual_ad713x
*   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
* Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef IIO_DUAL_AD713X
#define IIO_DUAL_AD713X

#ifdef IIO_SUPPORT

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdio.h>
#include "ad713x.h"
#include "iio_types.h"
#include "no_os_spi.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct iio_ad713x_init_par
 * @brief iio_ad713x configuration.
 */
struct iio_ad713x_init_par {
	/** Number of channels */
	uint8_t	num_channels;
	/* Device instance */
	struct ad713x_dev *dev;
	/** Spi engine descriptor */
	struct no_os_spi_desc *spi_eng_desc;
	/** Spi engine message descriptor */
	struct spi_engine_offload_message *spi_engine_offload_message;
	/** Invalidate the Data cache for the given address range */
	void (*dcache_invalidate_range)(uint32_t address, uint32_t bytes_count);
};

struct iio_ad713x {
	/* Mask of active ch */
	uint32_t mask;
	/** iio device descriptor */
	struct iio_device iio_dev_desc;
	/** Spi engine descriptor */
	struct no_os_spi_desc *spi_eng_desc;
	/** Spi engine message descriptor */
	struct spi_engine_offload_message *spi_engine_offload_message;
	/** Invalidate the Data cache for the given address range */
	void (*dcache_invalidate_range)(uint32_t address, uint32_t bytes_count);
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Init function. */
int32_t iio_dual_ad713x_init(struct iio_ad713x **desc,
			     struct iio_ad713x_init_par *param);
/* Get desciptor. */
void iio_dual_ad713x_get_dev_descriptor(struct iio_ad713x *desc,
					struct iio_device **dev_descriptor);
/* Free the resources allocated by iio_ad713x_init(). */
int32_t iio_dual_ad713x_remove(struct iio_ad713x *desc);

#endif /* IIO_SUPPORT */

#endif /* IIO_AD713X */
