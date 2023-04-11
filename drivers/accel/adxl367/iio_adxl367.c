/***************************************************************************//**
 *   @file   iio_adxl367.c
 *   @brief  IIO part of the ADXL367 driver.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
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
#include <stdlib.h>
#include <stdio.h>
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "iio_adxl367.h"
#include "adxl367.h"
#include <string.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
static const int adxl367_iio_odr_table[6][2] = {
	{12, 500000},
	{25,      0},
	{50,      0},
	{100,     0},
	{200,     0},
	{400,     0}
};

static const int adxl367_iio_scale_table[3][2] = {
	{0, 2394493},
	{0, 4788987},
	{0, 9577975}
};

static struct iio_device adxl367_iio_dev;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Wrapper for reading ADXL367 register.
 *
 * @param device  - The iio device structure.
 * @param reg	  - Address of the register to be read from.
 * @param readval - Read data.
 *
 * @return ret    - Result of the reading procedure.
*******************************************************************************/
static int adxl367_iio_read_reg(struct adxl367_iio_dev *dev, uint32_t reg,
				uint32_t *readval)
{
	return adxl367_get_register_value(dev->adxl367_dev, (uint8_t *)readval,
					  (uint8_t)reg, 1);
}

/***************************************************************************//**
 * @brief Wrapper for writing to ADXL367 register.
 *
 * @param device   - The iio device structure.
 * @param reg	   - Address of the register to be written to.
 * @param writeval - Data to be written.
 *
 * @return ret    - Result of the writing procedure.
*******************************************************************************/
static int adxl367_iio_write_reg(struct adxl367_iio_dev *dev, uint32_t reg,
				 uint32_t writeval)
{
	uint8_t val = writeval;
	return adxl367_set_register_value(dev->adxl367_dev, val, reg);
}

/***************************************************************************//**
 * @brief Handles the read request for calibbias attribute.
 *
 * @param dev  - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - Result of the reading procedure.
 * 					In case of success, the size of the read data is returned.
*******************************************************************************/
static int adxl367_iio_read_calibbias(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val;
	struct adxl367_iio_dev *iio_adxl367;
	struct adxl367_dev *adxl367;

	if (!dev)
		return -EINVAL;

	iio_adxl367 = (struct adxl367_iio_dev *)dev;

	if (!iio_adxl367->adxl367_dev)
		return -EINVAL;

	adxl367 = iio_adxl367->adxl367_dev;

	switch (channel->type) {
	case IIO_ACCEL:
		switch (channel->ch_num) {
		case 0:
			val = no_os_sign_extend32(adxl367->x_offset,15);
			return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		case 1:
			val = no_os_sign_extend32(adxl367->y_offset,15);
			return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		case 2:
			val = no_os_sign_extend32(adxl367->z_offset,15);
			return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);

		default:
			return -EINVAL;;
		}

	default:
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Handles the write request for calibbias attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with the data to be written.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - Result of the writing procedure.
*******************************************************************************/
static int adxl367_iio_write_calibbias(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t ret;
	int32_t val;
	uint16_t calibbias;
	struct adxl367_iio_dev *iio_adxl367;
	struct adxl367_dev *adxl367;

	if (!dev)
		return -EINVAL;

	iio_adxl367 = (struct adxl367_iio_dev *)dev;

	if (!iio_adxl367->adxl367_dev)
		return -EINVAL;

	adxl367 = iio_adxl367->adxl367_dev;

	switch (channel->type) {
	case IIO_ACCEL:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		// Change this value to two's complement with sign bit = BIT15
		if(val < 0)
			calibbias = NO_OS_BIT(15) | (~abs(val) + 1);
		else
			calibbias = val;

		// Set the device in standby mode
		ret = adxl367_set_power_mode(adxl367, ADXL367_OP_STANDBY);
		if (ret)
			return ret;

		// Set the offset on the requested axis
		switch (channel->ch_num) {
		case 0:
			ret = adxl367_set_offset(adxl367, calibbias, adxl367->y_offset,
						 adxl367->z_offset);
			break;
		case 1:
			ret = adxl367_set_offset(adxl367, adxl367->x_offset, calibbias,
						 adxl367->z_offset);
			break;
		case 2:
			ret = adxl367_set_offset(adxl367, adxl367->x_offset, adxl367->y_offset,
						 calibbias);
			break;

		default:
			ret = -EINVAL;
			break;
		}
		if (ret)
			return ret;

		// Set the device in measuring mode
		return adxl367_set_power_mode(adxl367, ADXL367_OP_MEASURE);

	default:
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Handles the read request for offset attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - Result of the reading procedure.
 * 					In case of success, the size of the read data is returned.
*******************************************************************************/
static int adxl367_iio_read_offset(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals;

	switch (channel->type) {
	case IIO_TEMP:
		vals = -1185;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &vals);
	default:
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Handles the read request for raw attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - Result of the reading procedure.
 * 					In case of success, the size of the read data is returned.
*******************************************************************************/
static int adxl367_iio_read_raw(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t ret;
	int16_t temp_raw;
	int16_t accel_x;
	int16_t accel_y;
	int16_t accel_z;
	struct adxl367_iio_dev *iio_adxl367;
	struct adxl367_dev *adxl367;

	if (!dev)
		return -EINVAL;

	iio_adxl367 = (struct adxl367_iio_dev *)dev;

	if (!iio_adxl367->adxl367_dev)
		return -EINVAL;

	adxl367 = iio_adxl367->adxl367_dev;

	switch (channel->type) {
	case IIO_ACCEL:
		ret = adxl367_get_raw_xyz(adxl367, &accel_x, &accel_y, &accel_z);
		if (ret)
			return ret;

		switch (channel->ch_num) {
		case 0:
			ret = no_os_sign_extend32(accel_x, 15);
			break;
		case 1:
			ret = no_os_sign_extend32(accel_y, 15);
			break;
		case 2:
			ret = no_os_sign_extend32(accel_z, 15);
			break;

		default:
			return -EINVAL;
		}

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &ret);

	case IIO_TEMP:
		ret = adxl367_read_raw_temp(adxl367, &temp_raw);
		if (ret)
			return ret;

		ret = (int32_t)temp_raw;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &ret);

	default:
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Handles the read request for sampling_frequency attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - Result of the reading procedure.
 * 					In case of success, the size of the read data is returned.
*******************************************************************************/
static int adxl367_iio_read_sampling_freq(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	struct adxl367_iio_dev *iio_adxl367;
	struct adxl367_dev *adxl367;

	if (!dev)
		return -EINVAL;

	iio_adxl367 = (struct adxl367_iio_dev *)dev;

	if (!iio_adxl367->adxl367_dev)
		return -EINVAL;

	adxl367 = iio_adxl367->adxl367_dev;

	vals[0] = adxl367_iio_odr_table[adxl367->odr][0];
	vals[1] = adxl367_iio_odr_table[adxl367->odr][1];

	return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, vals);
}

/***************************************************************************//**
 * @brief Searches for match in the given 2-row table
 *
 * @param freq_tbl - The frequency table in which the search is performed
 * @param size     - The line index until which the search is performed
 * @param val1     - Value on the first row to be checked
 * @param val2     - Value on the second row to be checked
 *
 * @return ret     - Table index which contains the searched data
*******************************************************************************/
static int adxl367_iio_find_2d_row(const int (*freq_tbl)[2], const int size,
				   const int val1, const int val2)
{
	for (int i = 0; i < size; i++) {
		if (freq_tbl[i][0] == val1 && freq_tbl[i][1] == val2)
			return i;
	}

	return -EINVAL;
}

/***************************************************************************//**
 * @brief Handles the write request for sampling_frequency attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with the data to be written.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - Result of the writing procedure.
*******************************************************************************/
static int adxl367_iio_write_sampling_freq(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	int odr_idx;
	int ret;
	struct adxl367_iio_dev *iio_adxl367;
	struct adxl367_dev *adxl367;

	if (!dev)
		return -EINVAL;

	iio_adxl367 = (struct adxl367_iio_dev *)dev;

	if (!iio_adxl367->adxl367_dev)
		return -EINVAL;

	adxl367 = iio_adxl367->adxl367_dev;

	iio_parse_value(buf, IIO_VAL_FRACTIONAL, &vals[0], &vals[1]);
	// Search for the given data in the ODR available data
	odr_idx = adxl367_iio_find_2d_row(adxl367_iio_odr_table,
					  NO_OS_ARRAY_SIZE(adxl367_iio_odr_table),
					  vals[0], vals[1]);
	if (odr_idx < 0)
		return odr_idx;

	//switch to standby mode
	ret = adxl367_set_power_mode(adxl367, ADXL367_OP_STANDBY);
	if (ret)
		return ret;

	// Set the ODR value
	ret = adxl367_set_output_rate(adxl367, odr_idx);
	if (ret)
		return ret;

	//switch back to measure mode
	return adxl367_set_power_mode(adxl367, ADXL367_OP_MEASURE);
}

/***************************************************************************//**
 * @brief Handles the read request for sampling_frequency_available attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - Result of the reading procedure.
 * 					In case of success, the size of the read data is returned.
*******************************************************************************/
static int adxl367_iio_read_samp_freq_avail(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	char buffer[20];

	// Remove data existing in the buffer
	strcpy(buf, "");

	// Go through the values in ODR table and add them to the buffer
	for (uint8_t i = 0; i < NO_OS_ARRAY_SIZE(adxl367_iio_odr_table); i++) {
		vals[0] = adxl367_iio_odr_table[i][0];
		vals[1] = adxl367_iio_odr_table[i][1];
		iio_format_value(buffer, sizeof(buffer), IIO_VAL_INT_PLUS_MICRO, 2, vals);
		strcat(buf, buffer);
		strcat(buf, " ");
	}
	return strlen(buf);
}

/***************************************************************************//**
 * @brief Handles the read request for scale attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - Result of the reading procedure.
 * 					In case of success, the size of the read data is returned.
*******************************************************************************/
static int adxl367_iio_read_scale(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	struct adxl367_iio_dev *iio_adxl367;
	struct adxl367_dev *adxl367;

	if (!dev)
		return -EINVAL;

	iio_adxl367 = (struct adxl367_iio_dev *)dev;

	if (!iio_adxl367->adxl367_dev)
		return -EINVAL;

	adxl367 = iio_adxl367->adxl367_dev;

	switch (channel->type) {
	case IIO_ACCEL:
		switch (adxl367->range) {
		case ADXL367_2G_RANGE:
			vals[0] = 0;
			vals[1] = 2394493;
			break;
		case ADXL367_4G_RANGE:
			vals[0] = 0;
			vals[1] = 4788987;
			break;
		case ADXL367_8G_RANGE:
			vals[0] = 0;
			vals[1] = 9577975;
			break;
		}
		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_NANO, 2, vals);
	case IIO_TEMP:
		vals[0] = 0;
		vals[1] = 18447430;
		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_NANO, 2, vals);
	default:
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Handles the write request for scale attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with the data to be written.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - Result of the writing procedure.
*******************************************************************************/
static int adxl367_iio_store_scale(void *dev, char *buf,
				   uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	int ret;
	struct adxl367_iio_dev *iio_adxl367;
	struct adxl367_dev *adxl367;
	enum adxl367_range range;

	if (!dev)
		return -EINVAL;

	iio_adxl367 = (struct adxl367_iio_dev *)dev;

	if (!iio_adxl367->adxl367_dev)
		return -EINVAL;

	adxl367 = iio_adxl367->adxl367_dev;

	switch (channel->type) {
	case IIO_ACCEL:
		iio_parse_value(buf, IIO_VAL_INT_PLUS_NANO, &vals[0], &vals[1]);
		if (vals[0])
			return -EINVAL;
		switch (vals[1]) {
		case 239449300:
			//2g range
			range = ADXL367_2G_RANGE;
			break;
		case 478898700:
			//4g range
			range = ADXL367_4G_RANGE;
			break;
		case 957797500:
			//8g range
			range = ADXL367_8G_RANGE;
			break;
		default:
			return -EINVAL;
		}
		//switch to standby mode
		ret = adxl367_set_power_mode(adxl367, ADXL367_OP_STANDBY);
		if (ret)
			return ret;

		// Set the range value
		ret = adxl367_set_range(adxl367, range);
		if (ret)
			return ret;

		// switch back to measure mode
		return adxl367_set_power_mode(adxl367, ADXL367_OP_MEASURE);

	default:
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Handles the read request for scale_available attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - Result of the reading procedure.
 * 					In case of success, the size of the read data is returned.
*******************************************************************************/
static int adxl367_iio_read_scale_avail(void *dev, char *buf,
					uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	char buffer[20];

	switch (channel->type) {
	case IIO_ACCEL:
		// Remove data existing in the buffer
		strcpy(buf, "");

		// Go through the values in ODR table and add them to the buffer
		for (uint8_t i = 0; i < NO_OS_ARRAY_SIZE(adxl367_iio_scale_table); i++) {
			vals[0] = adxl367_iio_scale_table[i][0];
			vals[1] = adxl367_iio_scale_table[i][1];
			iio_format_value(buffer, sizeof(buffer), IIO_VAL_INT_PLUS_NANO, 2, vals);
			strcat(buf, buffer);
			strcat(buf, " ");
		}
		return strlen(buf);
	default:
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Updates the number of active channels and the total number of
 * 		  active channels
 *
 * @param dev  - The iio device structure.
 * @param mask - Mask of the active channels
 *
 * @return ret - Result of the updating procedure.
*******************************************************************************/
static int adxl367_iio_update_channels(void* dev, uint32_t mask)
{
	struct adxl367_iio_dev *iio_adxl367;
	uint8_t counter = 0;

	if (!dev)
		return -EINVAL;

	iio_adxl367 = (struct adxl367_iio_dev *)dev;

	iio_adxl367->active_channels = mask;

	if (mask & NO_OS_BIT(0))
		counter ++;
	if (mask & NO_OS_BIT(1))
		counter ++;
	if (mask & NO_OS_BIT(2))
		counter ++;
	if (mask & NO_OS_BIT(3))
		counter ++;

	iio_adxl367->no_of_active_channels = counter;

	return 0;
}

/***************************************************************************//**
 * @brief Reads the number of given samples for the selected channels
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param samples - Number of samples to be returned
 *
 * @return ret    - Result of the reading procedure.
 * 					In case of success, the size of the read data is returned.
*******************************************************************************/
static int adxl367_iio_read_samples(void* dev, int* buff, uint32_t samples)
{
	int16_t data_x;
	int16_t data_y;
	int16_t data_z;
	int16_t raw_temp;
	struct adxl367_iio_dev *iio_adxl367;
	struct adxl367_dev *adxl367;

	if (!dev)
		return -EINVAL;

	iio_adxl367 = (struct adxl367_iio_dev *)dev;

	if (!iio_adxl367->adxl367_dev)
		return -EINVAL;

	adxl367 = iio_adxl367->adxl367_dev;

	for(uint32_t i = 0; i < samples*iio_adxl367->no_of_active_channels;) {
		adxl367_get_raw_xyz(adxl367, &data_x, &data_y, &data_z);

		if (iio_adxl367->active_channels & NO_OS_BIT(0)) {
			buff[i] = no_os_sign_extend32(data_x, 15);
			i++;
		}
		if (iio_adxl367->active_channels & NO_OS_BIT(1)) {
			buff[i] = no_os_sign_extend32(data_y, 15);
			i++;
		}
		if (iio_adxl367->active_channels & NO_OS_BIT(2)) {
			buff[i] = no_os_sign_extend32(data_z, 15);
			i++;
		}
		if (iio_adxl367->active_channels & NO_OS_BIT(3)) {
			adxl367_read_raw_temp(adxl367, &raw_temp);
			buff[i] = raw_temp;
			i++;
		}
	}

	return samples;
}

/***************************************************************************//**
 * @brief Initializes the ADXL367 IIO driver
 *
 * @param iio_dev    - The iio device structure.
 * @param init_param - The structure that contains the device initial
 * 		       		   parameters.
 *
 * @return ret       - Result of the initialization procedure.
*******************************************************************************/
int adxl367_iio_init(struct adxl367_iio_dev **iio_dev,
		     struct adxl367_iio_init_param *init_param)
{
	int ret;
	struct adxl367_iio_dev *desc;

	desc = (struct adxl367_iio_dev *)no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio_dev = &adxl367_iio_dev;

	// Initialize ADXL367 driver
	ret = adxl367_init(&desc->adxl367_dev, *(init_param->adxl367_initial_param));
	if (ret)
		goto error_adxl367_init;

	// Enable temperature reading
	ret = adxl367_temp_read_en(desc->adxl367_dev, 1);
	if (ret)
		goto error_config;

	// Set ODR to 400Hz for iio live data feed
	ret = adxl367_set_output_rate(desc->adxl367_dev, ADXL367_ODR_400HZ);
	if (ret)
		goto error_config;

	// Enter measure mode
	ret = adxl367_set_power_mode(desc->adxl367_dev, ADXL367_OP_MEASURE);
	if (ret)
		goto error_config;

	*iio_dev = desc;

	return 0;

error_adxl367_init:
	no_os_free(desc);
	return ret;
error_config:
	adxl367_remove(desc->adxl367_dev);
	no_os_free(desc);
	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by adxl367_iio_init().
 *
 * @param desc - The IIO device structure.
 *
 * @return ret - Result of the remove procedure.
*******************************************************************************/
int adxl367_iio_remove(struct adxl367_iio_dev *desc)
{
	int ret;

	ret = adxl367_remove(desc->adxl367_dev);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/

static struct iio_attribute adxl367_iio_global_attributes[] = {
	{
		.name   = "sampling_frequency",
		.show   = adxl367_iio_read_sampling_freq,
		.store  = adxl367_iio_write_sampling_freq,
	},
	{
		.name   = "sampling_frequency_available",
		.show   = adxl367_iio_read_samp_freq_avail,
	},
	END_ATTRIBUTES_ARRAY,
};

static struct iio_attribute adxl367_iio_temp_attrs[] = {
	{
		.name = "offset",
		.show = adxl367_iio_read_offset,
	},
	{
		.name = "raw",
		.show = adxl367_iio_read_raw,
	},
	{
		.name = "scale",
		.show = adxl367_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adxl367_iio_accel_attrs[] = {
	{
		.name  = "calibbias",
		.show  = adxl367_iio_read_calibbias,
		.store = adxl367_iio_write_calibbias,
	},
	{
		.name = "raw",
		.show = adxl367_iio_read_raw,
	},
	{
		.name   = "scale",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adxl367_iio_read_scale,
		.store  = adxl367_iio_store_scale,
	},
	{
		.name   = "scale_available",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adxl367_iio_read_scale_avail,
	},
	END_ATTRIBUTES_ARRAY
};

static struct scan_type adxl367_iio_accel_scan_type = {
	.sign = 's',
	.realbits = 14,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false
};

static struct scan_type adxl367_iio_temp_scan_type = {
	.sign = 's',
	.realbits = 14,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false
};

#define ADXL367_ACCEL_CHANNEL(index, axis) 		{ \
	.ch_type = IIO_ACCEL,                         \
	.channel = index,                             \
	.scan_type = &adxl367_iio_accel_scan_type,    \
	.scan_index = index,                          \
	.attributes = adxl367_iio_accel_attrs,        \
	.ch_out = false,                              \
	.indexed = true,							  \
	.modified = true,                             \
	.channel2 = IIO_MOD_##axis					  \
}

static struct iio_channel adxl367_channels[] = {
	ADXL367_ACCEL_CHANNEL(0, X),
	ADXL367_ACCEL_CHANNEL(1, Y),
	ADXL367_ACCEL_CHANNEL(2, Z),
	{
		.ch_type = IIO_TEMP,
		.channel = 3,
		.scan_type = &adxl367_iio_temp_scan_type,
		.scan_index = 3,
		.attributes = adxl367_iio_temp_attrs,
		.ch_out = false,
	},
};

static struct iio_device adxl367_iio_dev = {
	.attributes = adxl367_iio_global_attributes,
	.num_ch = NO_OS_ARRAY_SIZE(adxl367_channels),
	.channels = adxl367_channels,
	.pre_enable = (int32_t (*)())adxl367_iio_update_channels,
	.read_dev = (int32_t (*)())adxl367_iio_read_samples,
	.debug_reg_read = (int32_t (*)())adxl367_iio_read_reg,
	.debug_reg_write = (int32_t (*)())adxl367_iio_write_reg
};
