/***************************************************************************//**
* @file   iio_ad738x.c
* @brief  Implementation of AD738X IIO interface
********************************************************************************
* Copyright (c) 2024-2026 Analog Devices, Inc.
* Copyright (c) 2024 BayLibre, SAS.
* All rights reserved.
*
* This software is proprietary to Analog Devices, Inc. and its licensors.
* By using this software you agree to the terms of the associated
* Analog Devices Software License Agreement.
*******************************************************************************/
#ifdef IIO_SUPPORT

#include <stdio.h>
#include <math.h>

#include "ad738x.h"
#include "iio_ad738x.h"
#include "iio.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#if !defined(USE_STANDARD_SPI)
#include "spi_engine.h"
#endif

#define AD738X_CH(_idx) {\
	.name = "voltage" #_idx, \
	.ch_type = IIO_VOLTAGE,\
	.ch_out = IIO_DIRECTION_INPUT,\
	.indexed = true,\
	.channel = _idx,\
	.scan_index = _idx,\
	.scan_type = &ad738x_iio_scan_type,\
	.attributes = ad738x_iio_ch_attributes,\
}

#define AD738X_CHN_STORAGE_BITS	16
#define AD738X_ADC_RESOLUTION		16
#define AD738X_ADC_MAX_COUNT_UNIPOLAR  (uint32_t)((1 << AD738X_ADC_RESOLUTION) - 1)
#define AD738X_DEFAULT_NUM_CH		2

/* Forward declarations */
static struct iio_attribute ad738x_iio_ch_attributes[];

struct ad738x_iio {
	struct ad738x_dev *drv_dev;
	struct iio_device *iio_dev;
};

/**
 * @brief Get the number of channels and storage bits from HDL configuration
 * @param dev - AD738x device structure (must be initialized)
 * @param num_channels - Pointer to store number of channels
 * @param storage_bits - Pointer to store storage bits per channel
 * @return 0 on success, negative error code otherwise
 *
 * This function reads the DMA source width from the SPI Engine offload
 * descriptor to determine how many spi_engine storage bits size samples fit in
 * each DMA source bus size. This determines the number of channels to be created
 * in the IIO device, as well as the storage bits per channel. If the SPI
 * Engine offload descriptor or DMA descriptor is not available, it defaults to
 * 2 channels with 16 storage bits each.
 */
static int ad738x_iio_get_hdl_config(struct ad738x_dev *dev,
				     uint8_t *num_channels,
				     uint8_t *storage_bits)
{
#if !defined(USE_STANDARD_SPI)
	struct spi_engine_desc *eng_desc;
	struct axi_dmac *rx_dma;
	uint32_t dma_width_bytes;
	uint32_t dma_width_bits;

	if (!dev || !dev->spi_desc || !dev->spi_desc->extra) {
		*num_channels = AD738X_DEFAULT_NUM_CH;
		*storage_bits = AD738X_CHN_STORAGE_BITS;
		return 0;
	}

	eng_desc = (struct spi_engine_desc *)dev->spi_desc->extra;
	rx_dma = eng_desc->offload_rx_dma;

	if (!rx_dma) {
		*num_channels = AD738X_DEFAULT_NUM_CH;
		*storage_bits = AD738X_CHN_STORAGE_BITS;
		return 0;
	}

	dma_width_bytes = rx_dma->width_src;
	dma_width_bits = dma_width_bytes * 8;

	/* Storage bits from SPI Engine data width (read from HDL register) */
	*storage_bits = eng_desc->max_data_width;
	if (dma_width_bits < *storage_bits)
		return -EINVAL;

	/* Calculate number of 16-bit channels that fit in DMA width */
	*num_channels = dma_width_bits / *storage_bits;

	return 0;
#else
	/* Standard SPI mode - default to 2 channels, 16-bit storage */
	*num_channels = AD738X_DEFAULT_NUM_CH;
	*storage_bits = AD738X_CHN_STORAGE_BITS;
	return 0;
#endif
}

/**
 * @brief Allocate and initialize IIO channels based on HDL configuration
 * @param num_channels - Number of channels to create
 * @param storage_bits - Storage bits per channel (from DMA width)
 * @param scan_type - Pointer to scan type structure
 * @return Pointer to allocated channel array, or NULL on error
 */
static struct iio_channel *ad738x_iio_alloc_channels(uint8_t num_channels,
		uint8_t storage_bits,
		struct scan_type *scan_type)
{
	struct iio_channel *channels;
	int i;

	channels = no_os_calloc(num_channels, sizeof(*channels));
	if (!channels)
		return NULL;

	/* Update scan_type with storage bits from HDL */
	scan_type->storagebits = storage_bits;

	for (i = 0; i < num_channels; i++) {
		channels[i].ch_type = IIO_VOLTAGE;
		channels[i].ch_out = IIO_DIRECTION_INPUT;
		channels[i].indexed = true;
		channels[i].channel = i;
		channels[i].scan_index = i;
		channels[i].scan_type = scan_type;
		channels[i].attributes = ad738x_iio_ch_attributes;
	}

	return channels;
}

/**
 * @brief Read the debug register value
 * @param device - Pointer to IIO device instance
 * @param reg - Register address to read from
 * @param val - Pointer to variable to read data into
 * @return 0 in case of success, negative value otherwise
 */
static int32_t iio_ad738x_debug_reg_read(void *device, uint32_t reg,
		uint32_t *val)
{
	struct ad738x_iio_dev *iio_dev = device;
	struct ad738x_dev *dev = iio_dev->ad738x_dev;
	int ret;
	uint16_t val16;

	ret = ad738x_spi_reg_read(dev, (uint8_t) reg, &val16);
	if (ret)
		return ret;

	*val = val16;
	return 0;
}

/**
 * @brief Write the debug register value
 * @param device - Pointer to IIO device instance
 * @param reg - Register address to read from
 * @param val - Pointer to variable to write data into
 * @return 0 in case of success, negative value otherwise
 */
static int32_t iio_ad738x_debug_reg_write(void *device, uint32_t reg,
		uint32_t val)
{
	struct ad738x_iio_dev *iio_dev = device;
	struct ad738x_dev *dev = iio_dev->ad738x_dev;

	return ad738x_spi_reg_write(dev, (uint8_t)reg, (uint16_t)val);
}

static int get_raw(void *device, char *buf, uint32_t len,
		   const struct iio_ch_info *channel, intptr_t id)
{
	struct ad738x_iio_dev *iio_dev = device;
	struct ad738x_dev *dev = iio_dev->ad738x_dev;
	uint16_t results[2];
	int32_t ret;

	ret = ad738x_spi_single_conversion(dev, results);
	if (ret)
		return ret;

	return sprintf(buf, "%i", results[channel->ch_num]);
}

static int get_scale(void *device, char *buf, uint32_t len,
		     const struct iio_ch_info *channel, intptr_t id)
{
	struct ad738x_iio_dev *iio_dev = device;
	struct ad738x_dev *dev = iio_dev->ad738x_dev;
	int32_t vals[2];

	vals[0] = dev->ref_voltage_mv;
	vals[1] = AD738X_ADC_MAX_COUNT_UNIPOLAR;
	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 1, vals);
}

/**
 * @brief Read buffer data corresponding to AD738X IIO device
 * @param iio_dev_data - Pointer to IIO device data structure
 * @return 0 in case of success, negative error code otherwise
 */
static int32_t iio_ad738x_submit_buffer(struct iio_device_data *iio_dev_data)
{
	struct ad738x_iio_dev *iio_dev = iio_dev_data->dev;
	struct ad738x_dev *dev = iio_dev->ad738x_dev;
	struct iio_buffer *buffer = iio_dev_data->buffer;
	void *buff;
	int32_t ret;

	ret = iio_buffer_get_block(iio_dev_data->buffer, &buff);
	if (ret)
		return ret;

	ret = ad738x_read_data(dev, buff, buffer->samples);
	if (ret)
		return ret;

	return iio_buffer_block_done(buffer);
}

static struct scan_type ad738x_iio_scan_type = {
	.sign = 'u',
	.realbits = AD738X_ADC_RESOLUTION,
	.storagebits = AD738X_CHN_STORAGE_BITS, /* Updated dynamically from HDL */
};

static struct iio_attribute ad738x_iio_ch_attributes[] = {
	{ .name = "raw", .show = get_raw },
	{ .name = "scale", .show = get_scale, },
	END_ATTRIBUTES_ARRAY
};

/**
 * @brief Initialize AD738X for IIO interfacing
 * @param dev - The device structure.
 * @param init_param - IIO init parameter structure
 * @return 0 on success, an error code otherwise
 *
 * This function initializes the AD738x device and creates IIO channels
 * based on the HDL configuration. The number of channels and storage bits
 * are read from the DMA source width register.
 */
int ad738x_iio_init(struct ad738x_iio_dev **dev,
		    struct ad738x_init_param *init_param)
{
	struct ad738x_iio_dev *desc;
	struct iio_device *iio_dev;
	struct iio_channel *channels;
	uint8_t num_channels;
	uint8_t storage_bits;
	int ret;

	desc = no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	/* Initialize the AD738x hardware first */
	ret = ad738x_init(&desc->ad738x_dev, init_param);
	if (ret)
		goto error_desc;

	/* Read HDL configuration to get channel count and storage bits */
	ret = ad738x_iio_get_hdl_config(desc->ad738x_dev, &num_channels,
					&storage_bits);
	if (ret)
		goto error_dev;

	/* Allocate IIO device structure */
	iio_dev = no_os_calloc(1, sizeof(*iio_dev));
	if (!iio_dev) {
		ret = -ENOMEM;
		goto error_dev;
	}

	/* Allocate channels dynamically based on HDL config */
	channels = ad738x_iio_alloc_channels(num_channels, storage_bits,
					     &ad738x_iio_scan_type);
	if (!channels) {
		ret = -ENOMEM;
		goto error_iio_dev;
	}

	iio_dev->num_ch = num_channels;
	iio_dev->channels = channels;
	iio_dev->debug_reg_read = iio_ad738x_debug_reg_read;
	iio_dev->debug_reg_write = iio_ad738x_debug_reg_write;
	iio_dev->submit = iio_ad738x_submit_buffer;

	desc->iio_dev = iio_dev;
	*dev = desc;

	return 0;

error_iio_dev:
	no_os_free(iio_dev);
error_dev:
	ad738x_remove(desc->ad738x_dev);
error_desc:
	no_os_free(desc);
	return ret;
}

int ad738x_iio_remove(struct ad738x_iio_dev *dev)
{
	if (!dev)
		return -ENODEV;

	if (dev->iio_dev) {
		if (dev->iio_dev->channels)
			no_os_free(dev->iio_dev->channels);
		no_os_free(dev->iio_dev);
	}

	if (dev->ad738x_dev)
		ad738x_remove(dev->ad738x_dev);

	no_os_free(dev);
	return 0;
}

#endif
