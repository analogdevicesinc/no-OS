/***************************************************************************//**
 *   @file   iio_axi_dac.h
 *   @brief  Header file of iio_axi_dac.
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
