/***************************************************************************//**
 *   @file   iio_axi_adc.h
 *   @brief  Header file of iio_axi_adc.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IIO_AXI_ADC_H_
#define IIO_AXI_ADC_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "iio_types.h"
#include "axi_adc_core.h"
#include "axi_dmac.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

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

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Init iio. */
int32_t iio_axi_adc_init(struct iio_axi_adc_desc **desc,
			 struct iio_axi_adc_init_param *param);

/** Get device descriptor. */
void iio_axi_adc_get_dev_descriptor(struct iio_axi_adc_desc *desc,
				    struct iio_device **dev_descriptor);

/* Free the resources allocated by iio_axi_adc_init(). */
int32_t iio_axi_adc_remove(struct iio_axi_adc_desc *desc);

#endif // IIO_AXI_ADC_H_
