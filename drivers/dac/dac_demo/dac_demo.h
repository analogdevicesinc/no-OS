/***************************************************************************//**
 * @file dac_demo.h
 * @brief Header file of DAC Demo Driver.
 * @author RNechita (ramona.nechita@analog.com)
 ********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef IIO_DEMO_DAC_
#define IIO_DEMO_DAC_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include "iio_types.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

#define MAX_DAC_ADDR		16
/* For testing a maximum of 16 channels can be enabled */
#ifndef TOTAL_DAC_CHANNELS
#define TOTAL_DAC_CHANNELS 2
#endif

/**
 * @struct iio_demo_dac_desc
 * @brief Desciptor.
 */
struct dac_demo_desc {
	/** Dummy registers of device for testing */
	uint8_t reg[MAX_DAC_ADDR];
	/** Demo global device attribute */
	uint32_t dac_global_attr;
	/** Demo device channel attribute */
	uint32_t dac_ch_attr[MAX_DAC_ADDR];
	/** Active channel**/
	uint32_t active_ch;
	/** Number of samples in each buffer */
	uint32_t loopback_buffer_len;
	/** Array of buffers for each channel*/
	uint16_t **loopback_buffers;
};

/**
 * @struct iio_demo_dac_init_param
 * @brief iio demo dac configuration.
 */
struct dac_demo_init_param {
	/** Demo global dac attribute */
	uint32_t dev_global_attr;
	/** Demo dac channel attribute */
	uint32_t dev_ch_attr[MAX_DAC_ADDR];
	/** Number of samples in each buffer */
	uint32_t loopback_buffer_len;
	/** Buffer for adc/dac communication*/
	uint16_t **loopback_buffers;
};

enum iio_dac_demo_attributes {
	DAC_CHANNEL_ATTR,
	DAC_GLOBAL_ATTR,
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t dac_demo_init(struct dac_demo_desc **desc,
		      struct dac_demo_init_param *param);

int32_t dac_demo_remove(struct dac_demo_desc *desc);

int32_t update_dac_channels(void *dev, int32_t mask);

int32_t close_dac_channels(void* dev);

int32_t dac_submit_samples(struct iio_device_data *dev_data);

int get_dac_demo_attr(void *device, char *buf, uint32_t len,
		      const struct iio_ch_info *channel, intptr_t priv);

int set_dac_demo_attr(void *device, char *buf, uint32_t len,
		      const struct iio_ch_info *channel, intptr_t priv);

int32_t dac_demo_reg_read(struct dac_demo_desc *desc, uint8_t reg_index,
			  uint8_t *readval);

int32_t dac_demo_reg_write(struct dac_demo_desc *desc, uint8_t reg_index,
			   uint8_t writeval);

int32_t dac_demo_trigger_handler(struct iio_device_data *dev_data);

#endif /*IIO_DEMO_DAC_*/
