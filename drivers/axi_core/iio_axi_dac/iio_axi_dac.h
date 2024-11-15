/***************************************************************************//**
 *   @file   iio_axi_dac.h
 *   @brief  Header file of iio_axi_dac.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IIO_AXI_DAC_H_
#define IIO_AXI_DAC_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "iio_types.h"
#include "axi_dac_core.h"
#include "axi_dmac.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct iio_basic_desc
 * @brief Application desciptor.
 */
struct iio_axi_dac_desc {
	struct axi_dac *dac;
	/** dma device */
	struct axi_dmac *dmac;
	/** ADC mask */
	uint32_t mask;
	/** flush contents of instruction and/or data cache */
	void (*dcache_flush_range)(uint32_t address, uint32_t bytes_count);
	/** iio device descriptor */
	struct iio_device dev_descriptor;
	/** Channel names */
	char (*ch_names)[20];
};

/**
 * @struct iio_basic_init_param
 * @brief Application configuration.
 */
struct iio_axi_dac_init_param {
	/** DAC device */
	struct axi_dac *tx_dac;
	/** Transmit DMA device */
	struct axi_dmac *tx_dmac;
	/** Function pointer to flush the data cache for the given address range */
	void (*dcache_flush_range)(uint32_t address, uint32_t bytes_count);
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Init application. */
int32_t iio_axi_dac_init(struct iio_axi_dac_desc **desc,
			 struct iio_axi_dac_init_param *param);
/** Get device descriptor. */
void iio_axi_dac_get_dev_descriptor(struct iio_axi_dac_desc *desc,
				    struct iio_device **dev_descriptor);
/* Free the resources allocated by iio_axi_dac_init(). */
int32_t iio_axi_dac_remove(struct iio_axi_dac_desc *desc);

#endif // IIO_AXI_DAC_H_
