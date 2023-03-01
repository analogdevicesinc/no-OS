/*******************************************************************************
 *   @file   iio_adxl313.c
 *   @brief  Implementation of IIO ADXL313 Driver.
 *   @author GMois (george.mois@analog.com)
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
#include "iio_adxl313.h"
#include "adxl313.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define SCALE_FACTOR 1000000000UL
#define MIN_SHIFT    9

static const int adxl313_iio_odr_table[10][2] = {
	{6, 250000},
	{12, 500000},
	{25, 0},
	{50, 0},
	{100, 0},
	{200, 0},
	{400, 0},
	{800, 0},
	{1600, 0},
	{3200, 0}
};

static const int adxl312_iio_range_table[4][2] = {
	{1, 500000},
	{3, 0},
	{6, 0},
	{12, 0},
};

static const int adxl313_iio_range_table[4][2] = {
	{0, 500000},
	{1, 0},
	{2, 0},
	{4, 0}
};

static const int adxl314_iio_range = 200;

static const int adxl312_iio_scale_table[4] = { 28439285, 56878570, 113757140, 227514280 };

static const int adxl313_iio_scale_table[4] = { 9576806, 19153613, 38307226, 76614453 };

static const int adxl314_iio_scale = 478858719;

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
enum adxl313_iio_accel_chans {
	CHAN_X = 0,
	CHAN_Y = 1,
	CHAN_Z = 2
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
extern int adxl313_read(struct adxl313_dev *dev,
			uint8_t base_address, uint16_t size, uint8_t *read_data);
extern int adxl313_write(struct adxl313_dev *dev,
			 uint8_t base_address, uint16_t size, uint8_t *write_data);
static int adxl313_iio_find_2d_row(const int (*freq_tbl)[2], const int n,
				   const int val1, const int val2);
static int adxl313_iio_read_reg(struct adxl313_iio_dev *dev, uint32_t reg,
				uint32_t *readval);
static int adxl313_iio_write_reg(struct adxl313_iio_dev *dev, uint32_t reg,
				 uint32_t writeval);
static int adxl313_iio_read_raw(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv);
static int adxl313_iio_read_scale(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv);
static int adxl313_iio_write_scale(void *dev, char *buf,
				   uint32_t len, const struct iio_ch_info *channel, intptr_t priv);
static int adxl313_iio_read_calibbias(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv);
static int adxl313_iio_write_calibbias(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv);
static int adxl313_iio_read_sampling_freq(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int adxl313_iio_write_sampling_freq(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv);
static int adxl313_iio_read_range(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv);
static int adxl313_iio_write_range(void *dev, char *buf,
				   uint32_t len, const struct iio_ch_info *channel, intptr_t priv);
static int adxl313_iio_read_samp_freq_avail(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv);
static int adxl313_iio_read_range_avail(void *dev, char *buf,
					uint32_t len, const struct iio_ch_info *channel, intptr_t priv);
static int adxl313_iio_read_scale_avail(void *dev, char *buf,
					uint32_t len, const struct iio_ch_info *channel, intptr_t priv);
static int adxl313_iio_read_samples(void* dev, int16_t* buff, uint32_t samples);
static int adxl313_iio_update_channels(void* dev, uint32_t mask);
/******************************************************************************/
/************************ Variable Declarations *******************************/
/******************************************************************************/

static struct iio_attribute adxl313_iio_accel_attrs[] = {
	{
		.name  = "calibbias",
		.show  = adxl313_iio_read_calibbias,
		.store = adxl313_iio_write_calibbias,
	},
	{
		.name = "raw",
		.show = adxl313_iio_read_raw,
	},
	{
		.name   = "sampling_frequency",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adxl313_iio_read_sampling_freq,
		.store  = adxl313_iio_write_sampling_freq,
	},
	{
		.name   = "sampling_frequency_available",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adxl313_iio_read_samp_freq_avail,
	},
	{
		.name   = "range",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adxl313_iio_read_range,
		.store  = adxl313_iio_write_range,
	},
	{
		.name   = "range_available",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adxl313_iio_read_range_avail,
	},
	{
		.name   = "scale",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adxl313_iio_read_scale,
		.store  = adxl313_iio_write_scale,
	},
	{
		.name   = "scale_available",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adxl313_iio_read_scale_avail,
	},
	END_ATTRIBUTES_ARRAY
};

static struct scan_type adxl313_iio_accel_scan_type = {
	.sign = 's',
	.realbits = 13,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false
};

#define ADXL313_ACCEL_CHANNEL(index, reg, axis) { \
 	.ch_type = IIO_ACCEL,                         \
 	.channel = index,                             \
 	.address = reg,                               \
 	.modified = true,                             \
 	.channel2 = IIO_MOD_##axis,                   \
 	.scan_type = &adxl313_iio_accel_scan_type,    \
 	.scan_index = index,                          \
 	.attributes = adxl313_iio_accel_attrs,        \
 	.ch_out = false                               \
 }

static struct iio_channel adxl313_channels[] = {
	ADXL313_ACCEL_CHANNEL(0, CHAN_X, X),
	ADXL313_ACCEL_CHANNEL(1, CHAN_Y, Y),
	ADXL313_ACCEL_CHANNEL(2, CHAN_Z, Z),
};

static struct iio_device adxl313_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(adxl313_channels),
	.channels = adxl313_channels,
	.pre_enable = (int32_t (*)())adxl313_iio_update_channels,
	.read_dev = (int32_t (*)())adxl313_iio_read_samples,
	.debug_reg_read = (int32_t (*)())adxl313_iio_read_reg,
	.debug_reg_write = (int32_t (*)())adxl313_iio_write_reg
};

/*******************************************************************************
 * @brief Searches for match in the given 2-row table
 *
 * @param freq_tbl - The frequency table in which the search is performed
 * @param size     - The line index until which the search is performed
 * @param val1     - Value on the first row to be checked
 * @param val2     - Value on the second row to be checked
 *
 * @return ret     - Table index which contains the searched data
*******************************************************************************/
static int adxl313_iio_find_2d_row(const int (*freq_tbl)[2], const int size,
				   const int val1, const int val2)
{
	int i;

	for (i = 0; i < size; i++) {
		if ((freq_tbl[i][0] == val1) && (freq_tbl[i][1] == val2))
			return i;
	}

	return -EINVAL;
}

/*******************************************************************************
 * @brief Wrapper for reading ADXL314 register.
 *
 * @param device  - The iio device structure.
 * @param reg	  - Address of the register to be read from.
 * @param readval - Read data.
 *
 * @return ret    - Result of the reading procedure.
*******************************************************************************/
static int adxl313_iio_read_reg(struct adxl313_iio_dev *dev, uint32_t reg,
				uint32_t *readval)
{
	return adxl313_read(dev->adxl313_dev, reg, 1, (uint8_t *)readval);
}

/*******************************************************************************
 * @brief Wrapper for writing to ADXL314 register.
 *
 * @param device   - The iio device structure.
 * @param reg	    - Address of the register to be written to.
 * @param writeval - Data to be written.
 *
 * @return ret     - Result of the writing procedure.
*******************************************************************************/
static int adxl313_iio_write_reg(struct adxl313_iio_dev *dev, uint32_t reg,
				 uint32_t writeval)
{
	uint8_t val = writeval;
	return adxl313_write(dev->adxl313_dev, reg, 1, &val);
}

/*******************************************************************************
 * @brief Handles the read request for raw attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	   - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - Result of the reading procedure.
 * 					In case of success, the size of the read data is returned.
*******************************************************************************/
static int adxl313_iio_read_raw(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t ret;
	int16_t accel_x;
	int16_t accel_y;
	int16_t accel_z;
	struct adxl313_iio_dev *iio_adxl313;
	struct adxl313_dev *adxl313;

	if (!dev)
		return -EINVAL;

	iio_adxl313 = (struct adxl313_iio_dev *)dev;

	if (!iio_adxl313->adxl313_dev)
		return -EINVAL;

	adxl313 = iio_adxl313->adxl313_dev;

	if (channel->type == IIO_ACCEL) {
		ret = adxl313_get_raw_xyz(adxl313, &accel_x, &accel_y, &accel_z);
		if (ret)
			return ret;

		switch (channel->address) {
		case CHAN_X:
			ret = no_os_sign_extend16(accel_x, MIN_SHIFT + adxl313->resolution);
			break;
		case CHAN_Y:
			ret = no_os_sign_extend16(accel_y, MIN_SHIFT + adxl313->resolution);
			break;
		case CHAN_Z:
			ret = no_os_sign_extend16(accel_z, MIN_SHIFT + adxl313->resolution);
			break;
		default:
			return -EINVAL;
		}

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &ret);
	}

	return -EINVAL;
}

/*******************************************************************************
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
static int adxl313_iio_read_scale(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	struct adxl313_iio_dev *iio_adxl313;
	struct adxl313_dev *adxl313;
	int ret;
	uint8_t full_res = 0;

	if (!dev)
		return -EINVAL;

	iio_adxl313 = (struct adxl313_iio_dev *)dev;

	if (!iio_adxl313->adxl313_dev)
		return -EINVAL;

	adxl313 = iio_adxl313->adxl313_dev;

	vals[1] = SCALE_FACTOR;

	if (channel->type == IIO_ACCEL) {
		switch (adxl313->dev_type) {
		case ID_ADXL312:
			ret = adxl313_get_full_res_setting(adxl313, &full_res);
			if (ret)
				return ret;
			if (full_res)
				vals[0] = adxl312_iio_scale_table[0];
			else
				vals[0] = adxl312_iio_scale_table[adxl313->range - ADXL313_RANGE_FACTOR];
			return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, vals);
		case ID_ADXL313:
			ret = adxl313_get_full_res_setting(adxl313, &full_res);
			if (ret)
				return ret;
			if (full_res)
				vals[0] = adxl313_iio_scale_table[0];
			else
				vals[0] = adxl313_iio_scale_table[adxl313->range];
			return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, vals);
		case ID_ADXL314:
			vals[0] = adxl314_iio_scale;
			return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, vals);
		default:
			return -EINVAL;
		}
	}

	return -EINVAL;
}

/*******************************************************************************
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
static int adxl313_iio_write_scale(void *dev, char *buf,
				   uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	struct adxl313_iio_dev *iio_adxl313;
	struct adxl313_dev *adxl313;

	if (!dev)
		return -EINVAL;

	iio_adxl313 = (struct adxl313_iio_dev *)dev;

	if (!iio_adxl313->adxl313_dev)
		return -EINVAL;

	adxl313 = iio_adxl313->adxl313_dev;

	if (channel->type == IIO_ACCEL) {
		switch (adxl313->dev_type) {
		case ID_ADXL312:
			iio_parse_value(buf, IIO_VAL_FRACTIONAL, &vals[0], &vals[1]);
			/* Search for the given data in the range available data
			 * Enable/disable FULL_RES depending on selection */
			if (vals[1] == adxl312_iio_scale_table[0])
				return adxl313_enable_full_res(adxl313);
			break;
		case ID_ADXL313:
			iio_parse_value(buf, IIO_VAL_FRACTIONAL, &vals[0], &vals[1]);
			/* Enable/disable FULL_RES depending on selection */
			if (vals[1] == adxl313_iio_scale_table[0])
				return adxl313_enable_full_res(adxl313);
			break;
		case ID_ADXL314:
			return 0;
		default:
			return -EINVAL;
		}

		return adxl313_disable_full_res(adxl313);
	}

	return -EINVAL;
}

/*******************************************************************************
 * @brief Handles the read request for calibbias attribute.
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
static int adxl313_iio_read_calibbias(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val;
	struct adxl313_iio_dev *iio_adxl313;
	struct adxl313_dev *adxl313;

	if (!dev)
		return -EINVAL;

	iio_adxl313 = (struct adxl313_iio_dev *)dev;

	if (!iio_adxl313->adxl313_dev)
		return -EINVAL;

	adxl313 = iio_adxl313->adxl313_dev;

	if (channel->type == IIO_ACCEL) {
		adxl313_get_raw_offset(adxl313, &val, channel->address);

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
	}

	return -EINVAL;
}

/*******************************************************************************
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
static int adxl313_iio_write_calibbias(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t ret;
	int32_t val;
	enum adxl313_op_mode op_mode;
	struct adxl313_iio_dev *iio_adxl313;
	struct adxl313_dev *adxl313;

	if (!dev)
		return -EINVAL;

	iio_adxl313 = (struct adxl313_iio_dev *)dev;

	if (!iio_adxl313->adxl313_dev)
		return -EINVAL;

	adxl313 = iio_adxl313->adxl313_dev;



	if (channel->type == IIO_ACCEL) {
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);

		/* The device has to be in standby mode in order to be able to
		 * modify the offset.
		 * Obtain the current op mode */
		op_mode = adxl313->op_mode;
		/* Set the device in standby mode */
		ret = adxl313_set_op_mode(adxl313, ADXL313_STDBY);
		if (ret)
			return ret;

		/* Set the offset on the requested axis */
		switch (channel->address) {
		case CHAN_X:
			ret = adxl313_set_raw_offset(adxl313, val, ADXL313_X_AXIS);
			break;
		case CHAN_Y:
			ret = adxl313_set_raw_offset(adxl313, val, ADXL313_Y_AXIS);
			break;
		case CHAN_Z:
			ret = adxl313_set_raw_offset(adxl313, val, ADXL313_Z_AXIS);
			break;
		default:
			ret = -EINVAL;
			break;
		}

		if (ret) {
			adxl313_set_op_mode(adxl313, op_mode);
			return ret;
		}

		return adxl313_set_op_mode(adxl313, op_mode);
	}

	return -EINVAL;
}

/*******************************************************************************
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
static int adxl313_iio_read_sampling_freq(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2] = { 0 };
	struct adxl313_iio_dev *iio_adxl313;
	struct adxl313_dev *adxl313;

	if (!dev)
		return -EINVAL;

	iio_adxl313 = (struct adxl313_iio_dev *)dev;

	if (!iio_adxl313->adxl313_dev)
		return -EINVAL;

	adxl313 = iio_adxl313->adxl313_dev;

	if (channel->type == IIO_ACCEL) {
		vals[0] = adxl313_iio_odr_table[adxl313->odr][0];
		vals[1] = adxl313_iio_odr_table[adxl313->odr][1];
		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, vals);
	}

	return -EINVAL;
}

/*******************************************************************************
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
static int adxl313_iio_write_sampling_freq(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	int odr_idx;
	struct adxl313_iio_dev *iio_adxl313;
	struct adxl313_dev *adxl313;

	if (!dev)
		return -EINVAL;

	iio_adxl313 = (struct adxl313_iio_dev *)dev;

	if (!iio_adxl313->adxl313_dev)
		return -EINVAL;

	adxl313 = iio_adxl313->adxl313_dev;

	if (channel->type == IIO_ACCEL) {
		iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, &vals[0], &vals[1]);
		/* Search for the given data in the ODR available data */
		odr_idx = adxl313_iio_find_2d_row(adxl313_iio_odr_table,
						  NO_OS_ARRAY_SIZE(adxl313_iio_odr_table),
						  vals[0], vals[1]);
		if (odr_idx < 0)
			return odr_idx;

		/* Set the ODR value */
		return adxl313_set_odr(adxl313, odr_idx);
	}

	return -EINVAL;
}

/*******************************************************************************
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
static int adxl313_iio_read_samp_freq_avail(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	char buffer[20];

	if (channel->type == IIO_ACCEL) {
		/* Remove data existing in the buffer */
		strcpy(buf, "");

		/* Go through the values in ODR table and add them to the buffer */
		for (uint8_t i = 0; i < NO_OS_ARRAY_SIZE(adxl313_iio_odr_table); i++) {
			vals[0] = adxl313_iio_odr_table[i][0];
			vals[1] = adxl313_iio_odr_table[i][1];
			iio_format_value(buffer, sizeof(buffer), IIO_VAL_INT_PLUS_MICRO, 2, vals);
			strcat(buf, buffer);
			if (i != NO_OS_ARRAY_SIZE(adxl313_iio_odr_table) - 1)
				strcat(buf, " ");
		}
		return strlen(buf);
	}

	return -EINVAL;
}

/*******************************************************************************
 * @brief Handles the read request for range attribute.
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
static int adxl313_iio_read_range(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2] = { 0 };
	struct adxl313_iio_dev *iio_adxl313;
	struct adxl313_dev *adxl313;

	if (!dev)
		return -EINVAL;

	iio_adxl313 = (struct adxl313_iio_dev *)dev;

	if (!iio_adxl313->adxl313_dev)
		return -EINVAL;

	adxl313 = iio_adxl313->adxl313_dev;

	if (channel->type == IIO_ACCEL) {
		switch (adxl313->dev_type) {
		case ID_ADXL312:
			vals[0] = adxl312_iio_range_table[adxl313->range - ADXL313_RANGE_FACTOR][0];
			vals[1] = adxl312_iio_range_table[adxl313->range - ADXL313_RANGE_FACTOR][1];
			break;
		case ID_ADXL313:
			vals[0] = adxl313_iio_range_table[adxl313->range][0];
			vals[1] = adxl313_iio_range_table[adxl313->range][1];
			break;
		case ID_ADXL314:
			vals[0] = adxl314_iio_range;
			vals[1] = 0; /* fractional part */
			break;
		default:
			return -EINVAL;
		}

		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 1, vals);
	}

	return -EINVAL;
}

/*******************************************************************************
 * @brief Handles the write request for range attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with the data to be written.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - Result of the writing procedure.
*******************************************************************************/
static int adxl313_iio_write_range(void *dev, char *buf,
				   uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	int range_idx;
	struct adxl313_iio_dev *iio_adxl313;
	struct adxl313_dev *adxl313;

	if (!dev)
		return -EINVAL;

	iio_adxl313 = (struct adxl313_iio_dev *)dev;

	if (!iio_adxl313->adxl313_dev)
		return -EINVAL;

	adxl313 = iio_adxl313->adxl313_dev;

	if (channel->type == IIO_ACCEL) {
		switch (adxl313->dev_type) {
		case ID_ADXL312:
			iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, &vals[0], &vals[1]);
			/* Search for the given data in the range available data */
			range_idx = adxl313_iio_find_2d_row(adxl312_iio_range_table,
							    NO_OS_ARRAY_SIZE(adxl312_iio_range_table),
							    vals[0], vals[1]);
			if (range_idx < 0)
				return range_idx;

			/* Set the ODR value */
			return adxl313_set_range(adxl313, range_idx + ADXL313_RANGE_FACTOR);
		case ID_ADXL313:
			iio_parse_value(buf, IIO_VAL_INT_PLUS_MICRO, &vals[0], &vals[1]);
			/* Search for the given data in the range available data */
			range_idx = adxl313_iio_find_2d_row(adxl313_iio_range_table,
							    NO_OS_ARRAY_SIZE(adxl313_iio_range_table),
							    vals[0], vals[1]);
			if (range_idx < 0)
				return range_idx;

			/* Set the ODR value */
			return adxl313_set_range(adxl313, range_idx);
		case ID_ADXL314:
			return 0;
		default:
			return -EINVAL;
		}
	}

	return -EINVAL;
}

/*******************************************************************************
 * @brief Handles the read request for range_available attribute.
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
static int adxl313_iio_read_range_avail(void *dev, char *buf,
					uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	char buffer[20];
	struct adxl313_iio_dev *iio_adxl313;
	struct adxl313_dev *adxl313;

	if (!dev)
		return -EINVAL;

	iio_adxl313 = (struct adxl313_iio_dev *)dev;

	if (!iio_adxl313->adxl313_dev)
		return -EINVAL;

	adxl313 = iio_adxl313->adxl313_dev;

	if (channel->type == IIO_ACCEL) {
		/* Remove data existing in the buffer */
		strcpy(buf, "");

		switch (adxl313->dev_type) {
		case ID_ADXL312:
			/* Go through the values in corresponding range table and add them to the buffer */
			for (uint8_t i = 0; i < NO_OS_ARRAY_SIZE(adxl312_iio_range_table); i++) {
				vals[0] = adxl312_iio_range_table[i][0];
				vals[1] = adxl312_iio_range_table[i][1];
				iio_format_value(buffer, sizeof(buffer), IIO_VAL_INT_PLUS_MICRO, 2, vals);
				strcat(buf, buffer);
				if (i != NO_OS_ARRAY_SIZE(adxl313_iio_range_table) - 1)
					strcat(buf, " ");
			}
			break;
		case ID_ADXL313:
			/* Go through the values in corresponding range table and add them to the buffer */
			for (uint8_t i = 0; i < NO_OS_ARRAY_SIZE(adxl313_iio_range_table); i++) {
				vals[0] = adxl313_iio_range_table[i][0];
				vals[1] = adxl313_iio_range_table[i][1];
				iio_format_value(buffer, sizeof(buffer), IIO_VAL_INT_PLUS_MICRO, 2, vals);
				strcat(buf, buffer);
				if (i != NO_OS_ARRAY_SIZE(adxl313_iio_range_table) - 1)
					strcat(buf, " ");
			}
			break;
		case ID_ADXL314:
			vals[0] = adxl314_iio_range;
			vals[1] = 0; /* fractional part */
			iio_format_value(buffer, sizeof(buffer), IIO_VAL_INT_PLUS_MICRO, 2, vals);
			strcat(buf, buffer);
			break;
		default:
			return -EINVAL;
		}
		return strlen(buf);
	}

	return -EINVAL;
}

/*******************************************************************************
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
static int adxl313_iio_read_scale_avail(void *dev, char *buf,
					uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	char buffer[20];
	struct adxl313_iio_dev *iio_adxl313;
	struct adxl313_dev *adxl313;

	if (!dev)
		return -EINVAL;

	iio_adxl313 = (struct adxl313_iio_dev *)dev;

	if (!iio_adxl313->adxl313_dev)
		return -EINVAL;

	adxl313 = iio_adxl313->adxl313_dev;

	vals[1] = SCALE_FACTOR;

	if (channel->type == IIO_ACCEL) {
		/* Remove data existing in the buffer */
		strcpy(buf, "");

		switch (adxl313->dev_type) {
		case ID_ADXL312:
			/* Go through the values in corresponding range table and add them to the buffer
			 * one for FULL_RES and one for 10-bit resolution, if not minimum range */
			vals[0] = adxl312_iio_scale_table[0];
			iio_format_value(buffer, sizeof(buffer), IIO_VAL_FRACTIONAL, 2, vals);
			strcat(buf, buffer);
			/* FULL_RES and 10-bit resolution if range larger than minimum value */
			if (adxl313->range != ADXL313_1_5G_RANGE) {
				strcat(buf, " ");
				vals[0] = adxl312_iio_scale_table[adxl313->range - ADXL313_RANGE_FACTOR];
				iio_format_value(buffer, sizeof(buffer), IIO_VAL_FRACTIONAL, 2, vals);
				strcat(buf, buffer);
			}
			break;
		case ID_ADXL313:
			/* Go through the values in corresponding range table and add them to the buffer
			 * one for FULL_RES and one for 10-bit resolution, if not minimum range */
			vals[0] = adxl313_iio_scale_table[0];
			iio_format_value(buffer, sizeof(buffer), IIO_VAL_FRACTIONAL, 2, vals);
			strcat(buf, buffer);
			/* FULL_RES and 10-bit resolution if range larger than minimum value */
			if (adxl313->range != ADXL313_0_5G_RANGE) {
				strcat(buf, " ");
				vals[0] = adxl313_iio_scale_table[adxl313->range];
				iio_format_value(buffer, sizeof(buffer), IIO_VAL_FRACTIONAL, 2, vals);
				strcat(buf, buffer);
			}
			break;
		case ID_ADXL314:
			vals[0] = adxl314_iio_scale;
			iio_format_value(buffer, sizeof(buffer), IIO_VAL_FRACTIONAL, 2, vals);
			strcat(buf, buffer);
			break;
		default:
			return -EINVAL;
		}
		return strlen(buf);
	}

	return -EINVAL;
}

/*******************************************************************************
 * @brief Reads the number of given samples for the selected channels
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param samples - Number of samples to be returned
 *
 * @return ret    - Result of the reading procedure.
 * 					In case of success, the size of the read data is returned.
*******************************************************************************/
static int adxl313_iio_read_samples(void* dev, int16_t* buff, uint32_t samples)
{
	int ret;
	int16_t data_x;
	int16_t data_y;
	int16_t data_z;
	struct adxl313_iio_dev *iio_adxl313;
	struct adxl313_dev *adxl313;

	if (!dev)
		return -EINVAL;

	iio_adxl313 = (struct adxl313_iio_dev *)dev;

	if (!iio_adxl313->adxl313_dev)
		return -EINVAL;

	adxl313 = iio_adxl313->adxl313_dev;

	for(uint32_t i = 0; i < samples*iio_adxl313->no_of_active_channels;) {
		ret = adxl313_get_raw_xyz(adxl313, &data_x, &data_y, &data_z);
		if (ret)
			return ret;

		if (iio_adxl313->active_channels & NO_OS_BIT(0)) {
			buff[i] = no_os_sign_extend16(data_x, MIN_SHIFT + adxl313->resolution);
			i++;
		}
		if (iio_adxl313->active_channels & NO_OS_BIT(1)) {
			buff[i] = no_os_sign_extend16(data_y, MIN_SHIFT + adxl313->resolution);
			i++;
		}
		if (iio_adxl313->active_channels & NO_OS_BIT(2)) {
			buff[i] = no_os_sign_extend16(data_z, MIN_SHIFT + adxl313->resolution);
			i++;
		}
	}

	return samples;
}

/*******************************************************************************
 * @brief Updates the number of active channels and the total number of
 * 		  active channels
 *
 * @param dev  - The iio device structure.
 * @param mask - Mask of the active channels
 *
 * @return ret - Result of the updating procedure.
*******************************************************************************/
static int adxl313_iio_update_channels(void* dev, uint32_t mask)
{
	struct adxl313_iio_dev *iio_adxl313;

	if (!dev)
		return -EINVAL;

	iio_adxl313 = (struct adxl313_iio_dev *)dev;

	iio_adxl313->active_channels = mask;

	iio_adxl313->no_of_active_channels = no_os_hweight32(mask);

	return 0;
}

/*******************************************************************************
 * @brief Initializes the ADXL313 IIO driver
 *
 * @param iio_dev    - The iio device structure.
 * @param init_param - The structure that contains the device initial
 * 		       		   parameters.
 *
 * @return ret       - Result of the initialization procedure.
*******************************************************************************/
int adxl313_iio_init(struct adxl313_iio_dev **iio_dev,
		     struct adxl313_iio_dev_init_param *init_param)
{
	int ret;
	struct adxl313_iio_dev *desc;

	desc = (struct adxl313_iio_dev *)no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio_dev = &adxl313_iio_dev;

	/* Initialize ADXL314 driver */
	ret = adxl313_init(&desc->adxl313_dev, *(init_param->adxl313_dev_init));
	if (ret)
		goto error_adxl313_init;

	/* Set the range to +/- 12 g for ADXL312 and +/- 4 g for ADXL313 (maximum range) */
	if (desc->adxl313_dev->dev_type == ID_ADXL312) {
		ret = adxl313_set_range(desc->adxl313_dev, ADXL313_12G_RANGE);
		if (ret)
			goto error_config;
	} else {
		if (desc->adxl313_dev->dev_type == ID_ADXL313) {
			ret = adxl313_set_range(desc->adxl313_dev, ADXL313_4G_RANGE);
			if (ret)
				goto error_config;
		}
	}

	/* Enables full resolution for ADXL312 and ADXL313 */
	if (desc->adxl313_dev->dev_type != ID_ADXL314) {
		ret = adxl313_enable_full_res(desc->adxl313_dev);
		if (ret)
			goto error_config;
	}

	/* Set FIFO mode to BYPASS */
	ret = adxl313_set_fifo_mode(desc->adxl313_dev, ADXL313_BYPAS_MODE);
	if (ret)
		goto error_config;

	/* Set operation mode */
	ret = adxl313_set_op_mode(desc->adxl313_dev, ADXL313_MEAS);
	if (ret)
		goto error_config;

	*iio_dev = desc;

	return 0;

error_adxl313_init:
	no_os_free(desc);
	return ret;
error_config:
	adxl313_remove(desc->adxl313_dev);
	no_os_free(desc);
	return ret;
}

/*******************************************************************************
 * @brief Free the resources allocated by adxl313_iio_init().
 *
 * @param desc - The IIO device structure.
 *
 * @return ret - Result of the remove procedure.
*******************************************************************************/
int adxl313_iio_remove(struct adxl313_iio_dev *desc)
{
	int ret;

	ret = adxl313_remove(desc->adxl313_dev);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}
