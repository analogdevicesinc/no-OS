/***************************************************************************//**
*   @file   iio_ad713x.h
*   @brief  Header file of iio_axi_adc
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
#ifndef IIO_AD713X
#define IIO_AD713X

#ifdef IIO_SUPPORT

#include "iio_types.h"
#include "no_os_spi.h"

struct ad713x_iio;

/**
 * @struct ad713x_iio_init_param
 * @brief AD713x IIO initialization structure
 */
struct ad713x_iio_init_param {
	/** AD713x driver handler */
	struct ad713x_dev *drv_dev;
	/** Generic IIO device handler */
	struct iio_device *iio_dev;
	/** Integer part of the VREF */
	uint32_t vref_int;
	/** Decimal part of the VREF */
	uint32_t vref_micro;
	/** SPI Engine driver handler */
	struct no_os_spi_desc *spi_eng_desc;
	/** Invalidate the Data cache for the given address range */
	void (*dcache_invalidate_range)(uint32_t address, uint32_t bytes_count);
};

extern struct iio_device ad713x_iio_desc;

/** Allocate memory for AD713x IIO handler. */
int iio_ad713x_init(struct ad713x_iio **desc,
		    struct ad713x_iio_init_param *param);

/** Free memory allocated by iio_ad713x_init(). */
int iio_ad713x_remove(struct ad713x_iio *desc);

#endif /* IIO_SUPPORT */

#endif /* IIO_AD713X */
