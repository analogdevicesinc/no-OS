/***************************************************************************//**
 *   @file   iio_adxl355.c
 *   @brief  Implementation of IIO ADXL355 Driver.
 *   @author RBolboac (ramona.bolboaca@analog.com)
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
#include "iio_adxl355.h"
#include "adxl355.h"
#include "no_os_units.h"
#include "no_os_alloc.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define ACCEL_AXIS_X (uint32_t) 0
#define ACCEL_AXIS_Y (uint32_t) 1
#define ACCEL_AXIS_Z (uint32_t) 2

static const int adxl355_iio_odr_table[11][2] = {
	{4000, 0},
	{2000, 0},
	{1000, 0},
	{500, 0},
	{250, 0},
	{125, 0},
	{62, 500000},
	{31, 250000},
	{15, 625000},
	{7, 813000},
	{3, 906000},
};

static const int adxl355_iio_hpf_3db_multipliers[] = {
	0,
	247000,
	62084,
	15545,
	3862,
	954,
	238,
};

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
enum adxl355_iio_accel_chans {
	chan_x,
	chan_y,
	chan_z,
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
extern int adxl355_read_device_data(struct adxl355_dev *dev,
				    uint8_t base_address, uint16_t size, uint8_t *read_data);
extern int adxl355_write_device_data(struct adxl355_dev *dev,
				     uint8_t base_address, uint16_t size, uint8_t *write_data);
static int adxl355_iio_fill_3db_frequency_table(void *dev);
static int adxl355_iio_find_2d_row(const int (*freq_tbl)[2], const int n,
				   const int val1, const int val2);
static int adxl355_iio_read_reg(struct adxl355_iio_dev *dev, uint32_t reg,
				uint32_t *readval);
static int adxl355_iio_write_reg(struct adxl355_iio_dev *dev, uint32_t reg,
				 uint32_t writeval);
static int adxl355_iio_read_raw(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv);
static int adxl355_iio_read_offset(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel, intptr_t priv);
static int adxl355_iio_read_scale(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv);
static int adxl355_iio_read_calibbias(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv);
static int adxl355_iio_write_calibbias(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv);
static int adxl355_iio_read_hpf(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv);
static int adxl355_iio_write_hpf(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv);
static int adxl355_iio_read_sampling_freq(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int adxl355_iio_write_sampling_freq(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv);
static int adxl355_iio_read_hpf_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv);
static int adxl355_iio_read_samp_freq_avail(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv);
static int adxl355_iio_read_samples(void* dev, int* buff, uint32_t samples);
static int adxl355_iio_update_channels(void* dev, uint32_t mask);
static int32_t adxl355_trigger_handler(struct iio_device_data *dev_data);
/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/
static struct iio_attribute adxl355_iio_temp_attrs[] = {
	{
		.name = "offset",
		.show = adxl355_iio_read_offset,
	},
	{
		.name = "raw",
		.show = adxl355_iio_read_raw,
	},
	{
		.name = "scale",
		.show = adxl355_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute adxl355_iio_accel_attrs[] = {
	{
		.name  = "calibbias",
		.show  = adxl355_iio_read_calibbias,
		.store = adxl355_iio_write_calibbias,
	},
	{
		.name   = "filter_high_pass_3db_frequency",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adxl355_iio_read_hpf,
		.store  = adxl355_iio_write_hpf,
	},
	{
		.name   = "filter_high_pass_3db_frequency_available",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adxl355_iio_read_hpf_available,
	},
	{
		.name = "raw",
		.show = adxl355_iio_read_raw,
	},
	{
		.name   = "sampling_frequency",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adxl355_iio_read_sampling_freq,
		.store  = adxl355_iio_write_sampling_freq,
	},
	{
		.name   = "sampling_frequency_available",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adxl355_iio_read_samp_freq_avail,
	},
	{
		.name   = "scale",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adxl355_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

static struct scan_type adxl355_iio_accel_scan_type = {
	.sign = 's',
	.realbits = 20,
	.storagebits = 32,
	.shift = 0,
	.is_big_endian = false
};

#define ADXL355_ACCEL_CHANNEL(index, reg, axis) { \
	.ch_type = IIO_ACCEL,                         \
	.channel = index,                             \
	.address = reg,                               \
	.modified = true,                             \
	.channel2 = IIO_MOD_##axis,                   \
	.scan_type = &adxl355_iio_accel_scan_type,    \
	.scan_index = index,                          \
	.attributes = adxl355_iio_accel_attrs,        \
	.ch_out = false                               \
}

static struct iio_channel adxl355_channels[] = {
	ADXL355_ACCEL_CHANNEL(0, chan_x, X),
	ADXL355_ACCEL_CHANNEL(1, chan_y, Y),
	ADXL355_ACCEL_CHANNEL(2, chan_z, Z),
	{
		.ch_type = IIO_TEMP,
		.channel = 3,
		.attributes = adxl355_iio_temp_attrs,
		.ch_out = false,
	},
};

static struct iio_device adxl355_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(adxl355_channels),
	.channels = adxl355_channels,
	.pre_enable = (int32_t (*)())adxl355_iio_update_channels,
	.trigger_handler = (int32_t (*)())adxl355_trigger_handler,
	.read_dev = (int32_t (*)())adxl355_iio_read_samples,
	.debug_reg_read = (int32_t (*)())adxl355_iio_read_reg,
	.debug_reg_write = (int32_t (*)())adxl355_iio_write_reg
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief Fills the 3db frequency table based on the current ODR setting.
 *
 * @param dev  - The iio device structure.
 *
 * @return ret - Result of the filling procedure.
*******************************************************************************/
static int adxl355_iio_fill_3db_frequency_table(void *dev)
{
	uint32_t multiplier;
	uint64_t div, rem;
	uint64_t odr;
	struct adxl355_iio_dev *iio_adxl355;
	struct adxl355_dev *adxl355;

	if (!dev)
		return -EINVAL;

	iio_adxl355 = (struct adxl355_iio_dev *)dev;

	if (!iio_adxl355->adxl355_dev)
		return -EINVAL;

	adxl355 = iio_adxl355->adxl355_dev;

	// Obtained odr value multiplied by 10^6
	odr = no_os_mul_u64_u32_shr(adxl355_iio_odr_table[adxl355->odr_lpf][0], MEGA,
				    0) + adxl355_iio_odr_table[adxl355->odr_lpf][1];

	for (uint8_t i = 0; i < NO_OS_ARRAY_SIZE(adxl355_iio_hpf_3db_multipliers);
	     i++) {
		// Obtain multiplier value multiplied by 10^8
		multiplier = adxl355_iio_hpf_3db_multipliers[i];
		// hpf_value = odr * multiplier / 10^14
		div = no_os_div64_u64_rem(no_os_mul_u64_u32_shr(odr, multiplier, 0),
					  TERA * 100, &rem);

		iio_adxl355->adxl355_hpf_3db_table[i][0] = div;
		iio_adxl355->adxl355_hpf_3db_table[i][1] = no_os_div_u64(rem, MEGA * 100);
	}

	return 0;
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
static int adxl355_iio_find_2d_row(const int (*freq_tbl)[2], const int size,
				   const int val1, const int val2)
{
	int i;

	for (i = 0; i < size; i++) {
		if (freq_tbl[i][0] == val1 && freq_tbl[i][1] == val2)
			return i;
	}

	return -EINVAL;
}

/***************************************************************************//**
 * @brief Wrapper for reading ADXL355 register.
 *
 * @param device  - The iio device structure.
 * @param reg	  - Address of the register to be read from.
 * @param readval - Read data.
 *
 * @return ret    - Result of the reading procedure.
*******************************************************************************/
static int adxl355_iio_read_reg(struct adxl355_iio_dev *dev, uint32_t reg,
				uint32_t *readval)
{
	return adxl355_read_device_data(dev->adxl355_dev, reg, 1, (uint8_t *)readval);
}

/***************************************************************************//**
 * @brief Wrapper for writing to ADXL355 register.
 *
 * @param device   - The iio device structure.
 * @param reg	   - Address of the register to be written to.
 * @param writeval - Data to be written.
 *
 * @return ret    - Result of the writing procedure.
*******************************************************************************/
static int adxl355_iio_write_reg(struct adxl355_iio_dev *dev, uint32_t reg,
				 uint32_t writeval)
{
	uint8_t val = writeval;
	return adxl355_write_device_data(dev->adxl355_dev, reg, 1, &val);
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
static int adxl355_iio_read_raw(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t ret;
	uint16_t temp_raw;
	uint32_t accel_x;
	uint32_t accel_y;
	uint32_t accel_z;
	struct adxl355_iio_dev *iio_adxl355;
	struct adxl355_dev *adxl355;

	if (!dev)
		return -EINVAL;

	iio_adxl355 = (struct adxl355_iio_dev *)dev;

	if (!iio_adxl355->adxl355_dev)
		return -EINVAL;

	adxl355 = iio_adxl355->adxl355_dev;

	switch (channel->type) {
	case IIO_ACCEL:
		ret = adxl355_get_raw_xyz(adxl355, &accel_x, &accel_y, &accel_z);
		if (ret)
			return ret;

		switch (channel->address) {
		case ACCEL_AXIS_X:
			ret = no_os_sign_extend32(accel_x, 19);
			break;
		case ACCEL_AXIS_Y:
			ret = no_os_sign_extend32(accel_y, 19);
			break;
		case ACCEL_AXIS_Z:
			ret = no_os_sign_extend32(accel_z, 19);
			break;

		default:
			return -EINVAL;
		}

		return iio_format_value(buf, len, IIO_VAL_INT, 1, &ret);

	case IIO_TEMP:
		ret = adxl355_get_raw_temp(adxl355, &temp_raw);
		if (ret)
			return ret;

		ret = (int32_t)temp_raw;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &ret);

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
static int adxl355_iio_read_offset(void *dev, char *buf, uint32_t len,
				   const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	struct adxl355_iio_dev *iio_adxl355;
	struct adxl355_dev *adxl355;

	if (!dev)
		return -EINVAL;

	iio_adxl355 = dev;

	if (!iio_adxl355->adxl355_dev)
		return -EINVAL;

	adxl355 = iio_adxl355->adxl355_dev;

	switch (channel->type) {
	case IIO_TEMP:
		switch(adxl355->dev_type) {
		case ID_ADXL355:
		case ID_ADXL357:
			vals[0] = -2111;
			vals[1] = 250000;
			break;
		case ID_ADXL359:
			vals[0] = -2078;
			vals[1] = 250000;
			break;
		default:
			return -EINVAL;
		}
		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, vals);
	default:
		return -EINVAL;
	}
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
static int adxl355_iio_read_scale(void *dev, char *buf, uint32_t len,
				  const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	struct adxl355_iio_dev *iio_adxl355;
	struct adxl355_dev *adxl355;

	if (!dev)
		return -EINVAL;

	iio_adxl355 = dev;

	if (!iio_adxl355->adxl355_dev)
		return -EINVAL;

	adxl355 = iio_adxl355->adxl355_dev;

	switch (channel->type) {
	case IIO_ACCEL:
		switch(adxl355->dev_type) {
		case ID_ADXL355:
			vals[0] = 0;
			vals[1] = 38245;
			break;
		case ID_ADXL357:
		case ID_ADXL359:
			vals[0] = 0;
			vals[1] = 191229;
			break;
		default:
			return -EINVAL;
		}
		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_NANO, 2, vals);
	case IIO_TEMP:
		vals[0] = -110;
		vals[1] = 497238;
		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, vals);
	default:
		return -EINVAL;
	}
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
static int adxl355_iio_read_calibbias(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t val;
	struct adxl355_iio_dev *iio_adxl355;
	struct adxl355_dev *adxl355;

	if (!dev)
		return -EINVAL;

	iio_adxl355 = (struct adxl355_iio_dev *)dev;

	if (!iio_adxl355->adxl355_dev)
		return -EINVAL;

	adxl355 = iio_adxl355->adxl355_dev;

	switch (channel->type) {
	case IIO_ACCEL:
		switch (channel->address) {
		case ACCEL_AXIS_X:
			val = no_os_sign_extend32(adxl355->x_offset,15);
			return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		case ACCEL_AXIS_Y:
			val = no_os_sign_extend32(adxl355->y_offset,15);
			return iio_format_value(buf, len, IIO_VAL_INT, 1, &val);
		case ACCEL_AXIS_Z:
			val = no_os_sign_extend32(adxl355->z_offset,15);
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
static int adxl355_iio_write_calibbias(void *dev, char *buf, uint32_t len,
				       const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t ret;
	int32_t val;
	uint16_t calibbias;
	enum adxl355_op_mode op_mode;
	struct adxl355_iio_dev *iio_adxl355;
	struct adxl355_dev *adxl355;

	if (!dev)
		return -EINVAL;

	iio_adxl355 = (struct adxl355_iio_dev *)dev;

	if (!iio_adxl355->adxl355_dev)
		return -EINVAL;

	adxl355 = iio_adxl355->adxl355_dev;

	switch (channel->type) {
	case IIO_ACCEL:
		iio_parse_value(buf, IIO_VAL_INT, &val, NULL);
		// Change this value to two's complement with sign bit = BIT15
		if(val < 0)
			calibbias = NO_OS_BIT(15) | (~abs(val) + 1);
		else
			calibbias = val;

		// The device has to be in standby mode in order to be able to
		// modify the offset.
		// Obtain the current op mode
		op_mode = adxl355->op_mode;
		// Set the device in standby mode
		ret = adxl355_set_op_mode(adxl355, ADXL355_STDBY_TEMP_OFF_DRDY_OFF);
		if (ret)
			return ret;

		// Set the offset on the requested axis
		switch (channel->address) {
		case ACCEL_AXIS_X:
			ret = adxl355_set_offset(adxl355, calibbias, adxl355->y_offset,
						 adxl355->z_offset);
			break;
		case ACCEL_AXIS_Y:
			ret = adxl355_set_offset(adxl355, adxl355->x_offset, calibbias,
						 adxl355->z_offset);
			break;
		case ACCEL_AXIS_Z:
			ret = adxl355_set_offset(adxl355, adxl355->x_offset, adxl355->y_offset,
						 calibbias);
			break;

		default:
			ret = -EINVAL;
			break;
		}

		if (ret) {
			adxl355_set_op_mode(adxl355, op_mode);
			return ret;
		}

		return adxl355_set_op_mode(adxl355, op_mode);

	default:
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Handles the read request for filter_low attribute.
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
static int adxl355_iio_read_hpf(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	struct adxl355_iio_dev *iio_adxl355;
	struct adxl355_dev *adxl355;

	if (!dev)
		return -EINVAL;

	iio_adxl355 = (struct adxl355_iio_dev *)dev;

	if (!iio_adxl355->adxl355_dev)
		return -EINVAL;

	adxl355 = iio_adxl355->adxl355_dev;

	switch (channel->type) {
	case IIO_ACCEL:
		// Write the value corresponding to the current ODR and HPF settings
		vals[0] = iio_adxl355->adxl355_hpf_3db_table[adxl355->hpf_corner][0];
		vals[1] = iio_adxl355->adxl355_hpf_3db_table[adxl355->hpf_corner][1];
		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, vals);

	default:
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Handles the write request for filter_low attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with the data to be written.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - Result of the writing procedure.
*******************************************************************************/
static int adxl355_iio_write_hpf(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t priv)
{
	int hpf_idx;
	int32_t vals[2];
	struct adxl355_iio_dev *iio_adxl355;
	struct adxl355_dev *adxl355;

	if (!dev)
		return -EINVAL;

	iio_adxl355 = (struct adxl355_iio_dev *)dev;

	if (!iio_adxl355->adxl355_dev)
		return -EINVAL;

	adxl355 = iio_adxl355->adxl355_dev;

	switch (channel->type) {
	case IIO_ACCEL:
		// Search for the given data in the HPF available data for the current ODR
		iio_parse_value(buf, IIO_VAL_FRACTIONAL, &vals[0], &vals[1]);
		hpf_idx = adxl355_iio_find_2d_row(iio_adxl355->adxl355_hpf_3db_table,
						  NO_OS_ARRAY_SIZE(iio_adxl355->adxl355_hpf_3db_table),vals[0], vals[1]);
		if (hpf_idx < 0)
			return hpf_idx;
		// Set the HPF value
		return adxl355_set_hpf_corner(adxl355, hpf_idx);

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
static int adxl355_iio_read_sampling_freq(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	struct adxl355_iio_dev *iio_adxl355;
	struct adxl355_dev *adxl355;

	if (!dev)
		return -EINVAL;

	iio_adxl355 = (struct adxl355_iio_dev *)dev;

	if (!iio_adxl355->adxl355_dev)
		return -EINVAL;

	adxl355 = iio_adxl355->adxl355_dev;

	switch (channel->type) {
	case IIO_ACCEL:
		vals[0] = adxl355_iio_odr_table[adxl355->odr_lpf][0];
		vals[1] = adxl355_iio_odr_table[adxl355->odr_lpf][1];
		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2, vals);
	default:
		return -EINVAL;
	}
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
static int adxl355_iio_write_sampling_freq(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	int odr_idx;
	int ret;
	struct adxl355_iio_dev *iio_adxl355;
	struct adxl355_dev *adxl355;

	if (!dev)
		return -EINVAL;

	iio_adxl355 = (struct adxl355_iio_dev *)dev;

	if (!iio_adxl355->adxl355_dev)
		return -EINVAL;

	adxl355 = iio_adxl355->adxl355_dev;

	switch (channel->type) {
	case IIO_ACCEL:
		iio_parse_value(buf, IIO_VAL_FRACTIONAL, &vals[0], &vals[1]);
		// Search for the given data in the ODR available data
		odr_idx = adxl355_iio_find_2d_row(adxl355_iio_odr_table,
						  NO_OS_ARRAY_SIZE(adxl355_iio_odr_table),
						  vals[0], vals[1]);
		if (odr_idx < 0)
			return odr_idx;

		// Set the ODR value
		ret = adxl355_set_odr_lpf(adxl355, odr_idx);
		if (ret)
			return ret;

		// Update 3db frequency table for new ODR value
		return adxl355_iio_fill_3db_frequency_table(iio_adxl355);

	default:
		return -EINVAL;
	}
}

/***************************************************************************//**
 * @brief Handles the read request for filter_high_pass_3db_frequency_available
 * 		  attribute.
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
static int adxl355_iio_read_hpf_available(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	char buffer[20];
	struct adxl355_iio_dev *iio_adxl355;

	if (!dev)
		return -EINVAL;

	iio_adxl355 = (struct adxl355_iio_dev *)dev;

	switch (channel->type) {
	case IIO_ACCEL:
		// Remove data existing in the buffer
		strcpy(buf, "");

		// Go through the values in HPF table for the current ODR and add them to the buffer
		for (uint8_t i = 0; i < NO_OS_ARRAY_SIZE(iio_adxl355->adxl355_hpf_3db_table);
		     i++) {

			vals[0] = iio_adxl355->adxl355_hpf_3db_table[i][0];
			vals[1] = iio_adxl355->adxl355_hpf_3db_table[i][1];
			iio_format_value(buffer, len, IIO_VAL_INT_PLUS_MICRO, 2, vals);
			strcat(buf, buffer);
			if (i < NO_OS_ARRAY_SIZE(iio_adxl355->adxl355_hpf_3db_table) - 1)
				strcat(buf, " ");
		}
		return strlen(buf);
	default:
		return -EINVAL;
	}
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
static int adxl355_iio_read_samp_freq_avail(void *dev, char *buf,
		uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	int32_t vals[2];
	char buffer[20];

	switch (channel->type) {
	case IIO_ACCEL:
		// Remove data existing in the buffer
		strcpy(buf, "");

		// Go through the values in ODR table and add them to the buffer
		for (uint8_t i = 0; i < NO_OS_ARRAY_SIZE(adxl355_iio_odr_table); i++) {
			vals[0] = adxl355_iio_odr_table[i][0];
			vals[1] = adxl355_iio_odr_table[i][1];
			iio_format_value(buffer, sizeof(buffer), IIO_VAL_INT_PLUS_MICRO, 2, vals);
			strcat(buf, buffer);
			if (i < NO_OS_ARRAY_SIZE(adxl355_iio_odr_table) - 1)
				strcat(buf, " ");
		}
		return strlen(buf);
	default:
		return -EINVAL;
	}
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
static int adxl355_iio_read_samples(void* dev, int* buff, uint32_t samples)
{
	uint32_t data_x;
	uint32_t data_y;
	uint32_t data_z;
	uint16_t raw_temp;
	struct adxl355_iio_dev *iio_adxl355;
	struct adxl355_dev *adxl355;

	if (!dev)
		return -EINVAL;

	iio_adxl355 = (struct adxl355_iio_dev *)dev;

	if (!iio_adxl355->adxl355_dev)
		return -EINVAL;

	adxl355 = iio_adxl355->adxl355_dev;

	for(uint32_t i = 0; i < samples*iio_adxl355->no_of_active_channels;) {
		adxl355_get_raw_xyz(adxl355, &data_x, &data_y, &data_z);

		if (iio_adxl355->active_channels & NO_OS_BIT(0)) {
			buff[i] = no_os_sign_extend32(data_x, 19);
			i++;
		}
		if (iio_adxl355->active_channels & NO_OS_BIT(1)) {
			buff[i] = no_os_sign_extend32(data_y, 19);
			i++;
		}
		if (iio_adxl355->active_channels & NO_OS_BIT(2)) {
			buff[i] = no_os_sign_extend32(data_z, 19);
			i++;
		}
		if (iio_adxl355->active_channels & NO_OS_BIT(3)) {
			adxl355_get_raw_temp(adxl355, &raw_temp);
			buff[i] = raw_temp;
			i++;
		}
	}

	return samples;
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
static int adxl355_iio_update_channels(void* dev, uint32_t mask)
{
	struct adxl355_iio_dev *iio_adxl355;
	uint8_t counter = 0;

	if (!dev)
		return -EINVAL;

	iio_adxl355 = (struct adxl355_iio_dev *)dev;

	iio_adxl355->active_channels = mask;

	if (mask & NO_OS_BIT(0))
		counter ++;
	if (mask & NO_OS_BIT(1))
		counter ++;
	if (mask & NO_OS_BIT(2))
		counter ++;
	if (mask & NO_OS_BIT(3))
		counter ++;

	iio_adxl355->no_of_active_channels = counter;

	return 0;
}

/***************************************************************************//**
 * @brief Handles trigger: reads one data-set and writes it to the buffer.
 *
 * @param dev_data  - The iio device data structure.
 *
 * @return ret - Result of the handling procedure. In case of success, the size
 * 				 of the written data is returned.
*******************************************************************************/
static int32_t adxl355_trigger_handler(struct iio_device_data *dev_data)
{
	int32_t data_buff[3];
	uint32_t x,y,z;
	uint8_t i = 0;

	struct adxl355_iio_dev *iio_adxl355;
	struct adxl355_dev *adxl355;

	if (!dev_data)
		return -EINVAL;

	iio_adxl355 = (struct adxl355_iio_dev *)dev_data->dev;

	if (!iio_adxl355->adxl355_dev)
		return -EINVAL;

	adxl355 = iio_adxl355->adxl355_dev;

	adxl355_get_raw_xyz(adxl355, &x, &y, &z);

	if (dev_data->buffer->active_mask & NO_OS_BIT(0)) {
		data_buff[0] = no_os_sign_extend32(x, 19);
		i++;
	}
	if (dev_data->buffer->active_mask & NO_OS_BIT(1)) {
		data_buff[i] = no_os_sign_extend32(y, 19);
		i++;
	}
	if (dev_data->buffer->active_mask & NO_OS_BIT(2)) {
		data_buff[i] = no_os_sign_extend32(z, 19);
		i++;
	}

	return iio_buffer_push_scan(dev_data->buffer, &data_buff[0]);
}

/***************************************************************************//**
 * @brief Initializes the ADXL355 IIO driver
 *
 * @param iio_dev    - The iio device structure.
 * @param init_param - The structure that contains the device initial
 * 		       		   parameters.
 *
 * @return ret       - Result of the initialization procedure.
*******************************************************************************/
int adxl355_iio_init(struct adxl355_iio_dev **iio_dev,
		     struct adxl355_iio_dev_init_param *init_param)
{
	int ret;
	struct adxl355_iio_dev *desc;

	desc = (struct adxl355_iio_dev *)no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio_dev = &adxl355_iio_dev;

	// Initialize ADXL355 driver
	ret = adxl355_init(&desc->adxl355_dev, *(init_param->adxl355_dev_init));
	if (ret)
		goto error_adxl355_init;

	// Perform soft reset
	// In some cases it not might be successfull at first try
	// Retrial mechanism can be implemented
	ret = adxl355_soft_reset(desc->adxl355_dev);
	if (ret)
		goto error_config;

	// Set operation mode
	ret = adxl355_set_op_mode(desc->adxl355_dev, ADXL355_MEAS_TEMP_ON_DRDY_ON);
	if (ret)
		goto error_config;

	// Fill HPF frequency table based on the current setting
	ret = adxl355_iio_fill_3db_frequency_table(desc);
	if (ret)
		goto error_config;

	*iio_dev = desc;

	return 0;

error_adxl355_init:
	no_os_free(desc);
	return ret;
error_config:
	adxl355_remove(desc->adxl355_dev);
	no_os_free(desc);
	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by adxl355_iio_init().
 *
 * @param desc - The IIO device structure.
 *
 * @return ret - Result of the remove procedure.
*******************************************************************************/
int adxl355_iio_remove(struct adxl355_iio_dev *desc)
{
	int ret;

	ret = adxl355_remove(desc->adxl355_dev);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}
