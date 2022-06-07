/***************************************************************************//**
 * @file dac_demo.h
 * @brief Header file of DAC Demo Driver.
 * @author RNechita (ramona.nechita@analog.com)
 ********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * - Neither the name of Analog Devices, Inc. nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * - The use of this software may or may not infringe the patent rights
 * of one or more patent holders. This license does not release you
 * from the requirement that you obtain separate licenses from these
 * patent holders to use this software.
 * - Use of the software either in source or binary form, must be run
 * on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

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
