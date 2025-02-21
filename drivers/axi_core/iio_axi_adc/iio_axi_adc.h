/***************************************************************************//**
 *   @file   iio_axi_adc.h
 *   @brief  Header file of iio_axi_adc.
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

#ifndef IIO_AXI_ADC_H_
#define IIO_AXI_ADC_H_

#include "iio_types.h"
#include "axi_adc_core.h"
#include "axi_dmac.h"

/**
 * @struct iio_axi_adc_desc
 * @brief iio_axi_adc_descriptor
 */
struct iio_axi_adc_desc {
	/** ADC device */
	struct axi_adc *adc;
	/** ADC mask */
	uint32_t mask;
	/** dma device */
	struct axi_dmac *dmac;
	/** Invalidate cache memory function pointer */
	void (*dcache_invalidate_range)(uint32_t address, uint32_t bytes_count);
	/** Custom implementation for get sampling frequency */
	int (*get_sampling_frequency)(struct axi_adc *dev, uint32_t chan,
				      uint64_t *sampling_freq_hz);
	/** iio device descriptor */
	struct iio_device dev_descriptor;
	/** Channel names */
	char (*ch_names)[20];
	/** Custom data format */
	struct scan_type *scan_type_common;
};

/**
 * @struct iio_axi_adc_init_param
 * @brief iio configuration.
 */
struct iio_axi_adc_init_param {
	/** ADC device */
	struct axi_adc *rx_adc;
	/** Receive DMA device */
	struct axi_dmac *rx_dmac;
	/** Invalidate the Data cache for the given address range */
	void (*dcache_invalidate_range)(uint32_t address, uint32_t bytes_count);
	/** Custom sampling frequency getter */
	int (*get_sampling_frequency)(struct axi_adc *dev, uint32_t chan,
				      uint64_t *sampling_freq_hz);
	/** Custom data format (unpopulated if not used, set to default)
	    Common to all channels */
	struct scan_type *scan_type_common;
};

/* Init iio. */
int32_t iio_axi_adc_init(struct iio_axi_adc_desc **desc,
			 struct iio_axi_adc_init_param *param);

/** Get device descriptor. */
void iio_axi_adc_get_dev_descriptor(struct iio_axi_adc_desc *desc,
				    struct iio_device **dev_descriptor);

/* Free the resources allocated by iio_axi_adc_init(). */
int32_t iio_axi_adc_remove(struct iio_axi_adc_desc *desc);

#endif // IIO_AXI_ADC_H_
