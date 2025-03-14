/***************************************************************************//**
*   @file   iio_ad4134.h
*   @brief  Header file of iio_dual_ad4134
*   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
* Copyright 2019(c) Analog Devices, Inc.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef IIO_DUAL_AD4134
#define IIO_DUAL_AD4134

#ifdef IIO_SUPPORT

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdio.h>
#include "ad4134.h"
#include "iio_types.h"
#include "no_os_spi.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct iio_ad4134_init_par
 * @brief iio_ad4134 configuration.
 */
struct iio_ad4134_init_par {
	/** Number of channels */
	uint8_t	num_channels;
	/* Device instance */
	struct ad4134_dev *dev;
	/** Spi engine descriptor */
	struct no_os_spi_desc *spi_eng_desc;
	/** Spi engine message descriptor */
	struct spi_engine_offload_message *spi_engine_offload_message;
	/** Invalidate the Data cache for the given address range */
	void (*dcache_invalidate_range)(uint32_t address, uint32_t bytes_count);
};

struct iio_ad4134 {
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
int32_t iio_dual_ad4134_init(struct iio_ad4134 **desc,
			     struct iio_ad4134_init_par *param);
/* Get desciptor. */
void iio_dual_ad4134_get_dev_descriptor(struct iio_ad4134 *desc,
					struct iio_device **dev_descriptor);
/* Free the resources allocated by iio_ad4134_init(). */
int32_t iio_dual_ad4134_remove(struct iio_ad4134 *desc);

#endif /* IIO_SUPPORT */

#endif /* IIO_AD4134 */
