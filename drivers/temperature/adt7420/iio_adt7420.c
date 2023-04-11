/***************************************************************************//**
 *   @file   iio_adt7420.c
 *   @brief  Implementation of IIO ADT7420 Driver.
 *   @author RNechita (ramona.nechita@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "no_os_i2c.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "iio_adt7420.h"
#include "adt7420.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
static int adt7420_iio_read_temp(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t *priv);
static int adt7420_iio_read_max(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t *priv);
static int adt7420_iio_read_min(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t *priv);
static int adt7420_iio_read_crit(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t *priv);
static int adt7420_iio_read_hyst(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t *priv);
static int adt7420_iio_reg_read(struct adt7420_iio_dev *dev, uint32_t reg,
				uint32_t *readval);
static int adt7420_iio_reg_write(struct adt7420_iio_dev *dev, uint32_t reg,
				 uint32_t writeval);
static int adt7420_iio_update_channels(void *dev, uint32_t mask);
static int adt7420_iio_read_samples(void *dev, uint16_t *buff,
				    uint32_t samples);

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/
static struct iio_attribute adt7420_iio_attrs[] = {
	{
		.name = "temp",
		.show = adt7420_iio_read_temp
	},
	{
		.name = "temp_max",
		.show = adt7420_iio_read_max
	},
	{
		.name = "temp_min",
		.show = adt7420_iio_read_min
	},
	{
		.name = "temp_crit",
		.show = adt7420_iio_read_crit
	},
	{
		.name = "temp_hyst",
		.show = adt7420_iio_read_hyst
	},
	END_ATTRIBUTES_ARRAY
};

static struct scan_type adt7420_iio_scan_type = {
	.sign = 's',
	.realbits = 8,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false
};

static struct iio_channel adt7420_channels[] = {
	{
		.ch_type = IIO_TEMP,
		.channel = 0,
		.address = 0,
		.scan_index = 0,
		.scan_type = &adt7420_iio_scan_type,
		.attributes = adt7420_iio_attrs,
		.ch_out = false
	}
};

static struct iio_device adt7420_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(adt7420_channels),
	.channels = adt7420_channels,
	.pre_enable = (int32_t (*)()) adt7420_iio_update_channels,
	.read_dev = (int32_t (*)()) adt7420_iio_read_samples,
	.debug_reg_read = (int32_t (*)()) adt7420_iio_reg_read,
	.debug_reg_write = (int32_t (*)()) adt7420_iio_reg_write,
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief Initializes the adt7420 IIO driver.
 *
 * @param iio_dev  - The iio device structure.
 * @param init_param - structure containing initial device parameters
 *
 * @return ret - Result of the filling procedure.
*******************************************************************************/
int adt7420_iio_init(struct adt7420_iio_dev **iio_dev,
		     struct adt7420_iio_init_param *init_param)
{
	int ret;
	struct adt7420_iio_dev *desc;

	desc = (struct adt7420_iio_dev*)no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	ret = adt7420_init(&desc->adt7420_dev, *(init_param->adt7420_dev_init));
	if (ret)
		goto error_init;
	ret = adt7420_reset(desc->adt7420_dev);
	if (ret)
		goto error_init;
	desc->iio_dev = &adt7420_iio_dev;

	*iio_dev = desc;

	return 0;

error_reset:
	adt7420_remove(desc->adt7420_dev);
error_init:
	no_os_free(desc);
	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by adt7420_iio_init().
 *
 * @param desc - The IIO device structure.
 *
 * @return ret - Result of the remove procedure.
*******************************************************************************/
int adt7420_iio_remove(struct adt7420_iio_dev *desc)
{
	int ret;

	ret = adt7420_remove(desc->adt7420_dev);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}

/**
 * @brief adt7420 reg read wrapper
 *
 * @param dev - The iio device structure.
 * @param reg - Register address
 * @param readval - Register value
 *
 * @return 0 in case of success, errno errors otherwise
 */
static int adt7420_iio_reg_read(struct adt7420_iio_dev *dev, uint32_t reg,
				uint32_t *readval)
{
	if (reg > __UINT8_MAX__)
		return -EINVAL;

	return adt7420_reg_read(dev->adt7420_dev, reg, readval);
}

/**
 * @brief adt7420 reg write wrapper
 *
 * @param dev - The iio device structure.
 * @param reg - Register address
 * @param writeval - THe value to be written in register
 *
 * @return 0 in case of success, errno errors otherwise
 */
static int adt7420_iio_reg_write(struct adt7420_iio_dev *dev, uint32_t reg,
				 uint32_t writeval)
{
	if (!dev)
		return -ENODEV;

	if (writeval > __UINT16_MAX__)
		return -EINVAL;

	return adt7420_reg_write(dev->adt7420_dev, reg, writeval);
}

/**
 * @brief Handles the read request for temp attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int adt7420_iio_read_temp(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t *priv)
{
	struct adt7420_iio_dev *iio_adt7420;
	struct adt7420_dev *adt7420;
	int32_t val;
	if (!dev)
		return -ENODEV;

	iio_adt7420 = (struct adt7420_iio_dev *)dev;
	adt7420 = iio_adt7420->adt7420_dev;

	switch (channel->address) {
	case 0:
		val = adt7420_get_temperature(adt7420);
		break;
	default:
		return -EINVAL;
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
}

/**
 * @brief Handles the read request for max attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int adt7420_iio_read_max(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t *priv)
{
	struct adt7420_iio_dev *iio_adt7420;
	struct adt7420_dev *adt7420;

	uint16_t temp = 0;
	int32_t temp_c = 0;
	iio_adt7420 = (struct adt7420_iio_dev *)dev;
	adt7420 = iio_adt7420->adt7420_dev;

	switch (channel->address) {
	case 0:
		if (adt7420_is_spi(adt7420)) {
			adt7420_reg_read(adt7420, ADT7320_REG_T_HIGH, &temp);
		} else {
			uint16_t temp_msb = 0, temp_lsb = 0;
			adt7420_reg_read(adt7420, ADT7420_REG_T_HIGH_MSB, &temp_msb);
			adt7420_reg_read(adt7420, ADT7420_REG_T_HIGH_LSB, &temp_lsb);
			temp = (temp_msb << 8) | temp_lsb;
		}

		if (adt7420->resolution_setting) {
			if (temp & ADT7420_16BIT_SIGN)
				/*! Negative temperature */
				temp_c = ((int32_t)temp - ADT7420_16BIT_NEG) / ADT7420_16BIT_DIV;
			else
				/*! Positive temperature */
				temp_c = temp / ADT7420_16BIT_DIV;
		} else {
			temp >>= 3;
			if (temp & ADT7420_13BIT_SIGN)
				/*! Negative temperature */
				temp_c = ((int32_t)temp - ADT7420_13BIT_NEG) / ADT7420_13BIT_DIV;
			else
				/*! Positive temperature */
				temp_c = temp / ADT7420_13BIT_DIV;
		}
		break;
	default:
		return -EINVAL;
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &temp_c);
}

/**
 * @brief Handles the read request for min attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int adt7420_iio_read_min(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t *priv)
{
	struct adt7420_iio_dev *iio_adt7420;
	struct adt7420_dev *adt7420;

	uint16_t temp = 0;
	int32_t temp_c = 0;
	iio_adt7420 = (struct adt7420_iio_dev *)dev;
	adt7420 = iio_adt7420->adt7420_dev;

	switch (channel->address) {
	case 0:
		if (adt7420_is_spi(adt7420)) {
			adt7420_reg_read(adt7420, ADT7320_REG_T_LOW, &temp);
		} else {
			uint16_t temp_msb = 0, temp_lsb = 0;
			adt7420_reg_read(adt7420, ADT7420_REG_T_LOW_MSB, &temp_msb);
			adt7420_reg_read(adt7420, ADT7420_REG_T_LOW_LSB, &temp_lsb);
			temp = (temp_msb << 8) | temp_lsb;
		}

		if (adt7420->resolution_setting) {
			if (temp & ADT7420_16BIT_SIGN)
				/*! Negative temperature */
				temp_c = ((int32_t)temp - ADT7420_16BIT_NEG) / ADT7420_16BIT_DIV;
			else
				/*! Positive temperature */
				temp_c = temp / ADT7420_16BIT_DIV;
		} else {
			temp >>= 3;
			if (temp & ADT7420_13BIT_SIGN)
				/*! Negative temperature */
				temp_c = ((int32_t)temp - ADT7420_13BIT_NEG) / ADT7420_13BIT_DIV;
			else
				/*! Positive temperature */
				temp_c = temp / ADT7420_13BIT_DIV;
		}
		break;
	default:
		return -EINVAL;
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &temp_c);
}

/**
 * @brief Handles the read request for crit attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int adt7420_iio_read_crit(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t *priv)
{
	struct adt7420_iio_dev *iio_adt7420;
	struct adt7420_dev *adt7420;

	uint16_t temp = 0;
	int32_t temp_c = 0;
	iio_adt7420 = (struct adt7420_iio_dev *)dev;
	adt7420 = iio_adt7420->adt7420_dev;

	switch (channel->address) {
	case 0:
		if (adt7420_is_spi(adt7420)) {
			adt7420_reg_read(adt7420, ADT7320_REG_T_CRIT, &temp);
		} else {
			uint16_t temp_msb = 0, temp_lsb = 0;
			adt7420_reg_read(adt7420, ADT7420_REG_T_CRIT_MSB, &temp_msb);
			adt7420_reg_read(adt7420, ADT7420_REG_T_CRIT_LSB, &temp_lsb);
			temp = (temp_msb << 8) | temp_lsb;
		}

		if (adt7420->resolution_setting) {
			if (temp & ADT7420_16BIT_SIGN)
				/*! Negative temperature */
				temp_c = ((int32_t)temp - ADT7420_16BIT_NEG) / ADT7420_16BIT_DIV;
			else
				/*! Positive temperature */
				temp_c = temp / ADT7420_16BIT_DIV;
		} else {
			temp >>= 3;
			if (temp & ADT7420_13BIT_SIGN)
				/*! Negative temperature */
				temp_c = ((int32_t)temp - ADT7420_13BIT_NEG) / ADT7420_13BIT_DIV;
			else
				/*! Positive temperature */
				temp_c = temp / ADT7420_13BIT_DIV;
		}
		break;
	default:
		return -EINVAL;
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &temp_c);
}

/**
 * @brief Handles the read request for hyst attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int adt7420_iio_read_hyst(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t *priv)
{
	struct adt7420_iio_dev *iio_adt7420;
	struct adt7420_dev *adt7420;

	int16_t temp = 0;
	int32_t temp_c = 0;
	iio_adt7420 = (struct adt7420_iio_dev *)dev;
	adt7420 = iio_adt7420->adt7420_dev;

	switch (channel->address) {
	case 0:
		if (adt7420_is_spi(adt7420))
			adt7420_reg_read(adt7420, ADT7320_REG_HIST, &temp);
		else
			adt7420_reg_read(adt7420, ADT7420_REG_HIST, &temp);

		if (adt7420->resolution_setting) {
			if (temp & ADT7420_16BIT_SIGN)
				/*! Negative temperature */
				temp_c = ((int32_t)temp - ADT7420_16BIT_NEG) / ADT7420_16BIT_DIV;
			else
				/*! Positive temperature */
				temp_c = temp / ADT7420_16BIT_DIV;
		} else {
			temp >>= 3;
			if (temp & ADT7420_13BIT_SIGN)
				/*! Negative temperature */
				temp_c = ((int32_t)temp - ADT7420_13BIT_NEG) / ADT7420_13BIT_DIV;
			else
				/*! Positive temperature */
				temp_c = temp / ADT7420_13BIT_DIV;
		}
		break;
	default:
		return -EINVAL;
	}

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &temp_c);
}

/**
 * @brief Updates the number of active channels
 *
 * @param dev - The iio device structure.
 * @param mask - Bit mask containing active channels
 *
 * @return 0 in case of success, errno errors otherwise
 */
static int adt7420_iio_update_channels(void *dev, uint32_t mask)
{
	struct adt7420_iio_dev *iio_adt7420;

	if (!dev)
		return -ENODEV;

	iio_adt7420 = dev;

	iio_adt7420->active_channels = mask;
	iio_adt7420->no_active_channels = no_os_hweight8((uint8_t)mask);

	return 0;
}

/**
 * @brief Reads the number of given samples for the selected channels
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param samples - Number of samples to be returned
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int adt7420_iio_read_samples(void *dev, uint16_t *buff,
				    uint32_t samples)
{
	struct adt7420_iio_dev *iio_adt7420;
	struct adt7420_dev *adt7420;

	iio_adt7420 = (struct adt7420_iio_dev *)dev;
	adt7420 = iio_adt7420->adt7420_dev;

	for (uint32_t i = 0; i < samples; i++)
		buff[i] = (int16_t) adt7420_get_temperature(adt7420);

	return samples;
}
