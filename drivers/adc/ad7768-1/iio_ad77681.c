/***************************************************************************//**
*   @file   iio_ad77681.c
*   @brief  Source file of iio_ad77681
*   @author Celine Joy Capua (celinejoy.capua@analog.com)
********************************************************************************
* Copyright 2025(c) Analog Devices, Inc.
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
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

#include "iio_ad77681.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_units.h"
#include "no_os_alloc.h"


static const char *const common_mode_voltage_avail[8] = {
	[AD77681_VCM_HALF_VCC] = "(AVDD1-AVSS)/2",
	[AD77681_VCM_2_5V] = "2V5",
	[AD77681_VCM_2_05V] = "2V05",
	[AD77681_VCM_1_9V] = "1V9",
	[AD77681_VCM_1_65V] = "1V65",
	[AD77681_VCM_1_1V] = "1V1",
	[AD77681_VCM_0_9V] = "0V9",
	[AD77681_VCM_OFF] = "OFF",
};

static const char *const filter_type_avail[5] = {
	[AD77681_SINC5] =  "sinc5",
	[AD77681_SINC5_DECx8] = "sinc5",
	[AD77681_SINC5_DECx16] = "sinc5",
	[AD77681_SINC3] = "sinc3",
	[AD77681_FIR] = "wideband",
};

static const char *const sinc5_dec_rate_avail[8] = {
	[AD77681_SINC5_FIR_DECx32] = "32",
	[AD77681_SINC5_FIR_DECx64] = "64",
	[AD77681_SINC5_FIR_DECx128] = "128",
	[AD77681_SINC5_FIR_DECx256] = "256",
	[AD77681_SINC5_FIR_DECx512] = "512",
	[AD77681_SINC5_FIR_DECx1024] = "1024",
};

static const char *const mclk_div_avail[4] = {
	[AD77681_MCLK_DIV_16] = "16",
	[AD77681_MCLK_DIV_8] = "8",
	[AD77681_MCLK_DIV_4] = "4",
	[AD77681_MCLK_DIV_2] = "2",
};

static struct scan_type ad77681_iio_scan_type = {
	.sign = 's',
	.realbits = 24,
	.storagebits = 32,
	.shift = 8,
};

/**
 * @brief Fill the scale table for each PGA gain mode.
 * @param device - pointer to IIO device structure
 */
void ad77681_fill_scale_tbl(struct ad77681_iio_dev *device)
{
	struct ad77681_iio_dev *iio_ad77681;
	struct ad77681_dev *ad77681;
	int32_t val, val2, tmp0, tmp1, i;
	unsigned long denominator, numerator;
	uint64_t tmp2;

	iio_ad77681 = (struct ad77681_iio_dev *)device;
	ad77681 = iio_ad77681->ad77681_desc;

	//scan_type = ad77681_iio_scan_type;
	if (ad77681_iio_scan_type.sign == 's')
		val2 = ad77681_iio_scan_type.realbits - 1;
	else
		val2 = ad77681_iio_scan_type.realbits;

	for (i = 0; i < ad77681->num_pga_modes; i++) {
		/* Convert gain to a fraction format */
		numerator = ad77681->pga_gains[i];
		denominator = MILLI;
		if (ad77681->has_variable_aaf) {
			numerator *= ad7768_aaf_gains[ad77681->aaf_gain];
			denominator *= MILLI;
		}
		no_os_rational_best_approximation(numerator, denominator,
						  __INT_MAX__, __INT_MAX__,
						  &numerator, &denominator);

		val = ad77681->vref / 1000;
		/* Multiply by MILLI here to avoid losing precision */
		//val = mult_frac(val, denominator * MILLI, numerator);
		tmp0 = val / numerator;
		tmp2 = (uint64_t)val % (numerator);
		val = tmp0 * denominator + (tmp2 * denominator) / numerator;

		/* Would multiply by NANO here but we already multiplied by MILLI */
		tmp2 = no_os_shift_right((uint64_t)val * MICRO, val2);
		tmp0 = (int)no_os_div_s64_rem(tmp2, NANO, &tmp1);
		ad77681->scale_tbl[i][0] = tmp0; /* Integer part */
		ad77681->scale_tbl[i][1] = abs(tmp1); /* Fractional part */
	}
}

/**
 * @brief Get the list of available values for VCM output voltage.
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int ad77681_iio_vcm_mode_avail(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	uint32_t avail_size = NO_OS_ARRAY_SIZE(common_mode_voltage_avail);
	uint32_t i;
	int length = 0;


	for (i = 0; i < avail_size; i++)
		length += sprintf(buf + length, "%s ", common_mode_voltage_avail[i]);

	return length;
}

/**
 * @brief Handles the read request for common mode voltage attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int ad77681_iio_show_vcm(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	struct ad77681_iio_dev *iio_ad77681;
	struct ad77681_dev *ad77681;

	iio_ad77681 = (struct ad77681_iio_dev *)dev;
	ad77681 = iio_ad77681->ad77681_desc;

	return sprintf(buf, "%s", common_mode_voltage_avail[ad77681->VCM_out]);
}

/**
 * @brief Handles the write request for common mode voltage attribute.
 *
 * @param dev - The iio device structure.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int ad77681_iio_store_vcm(void *dev, char *buf, uint32_t len,
				 const struct iio_ch_info *channel,
				 intptr_t priv)
{
	struct ad77681_iio_dev *iio_ad77681;
	struct ad77681_dev *ad77681;
	int ret;
	uint32_t avail_size = NO_OS_ARRAY_SIZE(common_mode_voltage_avail);
	uint32_t i;

	iio_ad77681 = (struct ad77681_iio_dev *)dev;
	ad77681 = iio_ad77681->ad77681_desc;

	for (i = 0; i < avail_size; i++)
		if (!strcmp(buf, common_mode_voltage_avail[i])) {
			ret = ad77681_set_VCM_output(ad77681, i);
			if (ret)
				return ret;
		}

	return len;
}

/**
 * @brief Get the list of available values for filter mode.
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int ad77681_iio_filter_mode_avail(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	uint32_t avail_size = NO_OS_ARRAY_SIZE(filter_type_avail);
	uint32_t i;
	int length = 0;

	for (i = 0; i < avail_size; i++)
		if (i != AD77681_SINC5_DECx8 && i != AD77681_SINC5_DECx16)
			length += sprintf(buf + length, "%s ", filter_type_avail[i]);

	return length;
}

/**
 * @brief Handles the read request for filter mode attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int ad77681_iio_show_filter_mode(void *dev, char *buf, uint32_t len,
					const struct iio_ch_info *channel,
					intptr_t priv)
{
	struct ad77681_iio_dev *iio_ad77681;
	struct ad77681_dev *ad77681;

	iio_ad77681 = (struct ad77681_iio_dev *)dev;
	ad77681 = iio_ad77681->ad77681_desc;

	return sprintf(buf, "%s", filter_type_avail[ad77681->filter]);
}

/**
 * @brief Handles the write request for filter mode attribute.
 *
 * @param dev - The iio device structure.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int ad77681_iio_store_filter_mode(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct ad77681_iio_dev *iio_ad77681;
	struct ad77681_dev *ad77681;
	int ret;
	uint32_t avail_size = NO_OS_ARRAY_SIZE(filter_type_avail);
	uint32_t i;

	iio_ad77681 = (struct ad77681_iio_dev *)dev;
	ad77681 = iio_ad77681->ad77681_desc;

	for (i = 0; i < avail_size; i++)
		if (!strcmp(buf, filter_type_avail[i])) {
			ret = ad77681_set_filter_type(dev, ad77681->decimate, i,
						      ad77681->sinc3_osr);
			if (ret)
				return ret;
		}

	ad77681_fill_scale_tbl(iio_ad77681);
	ret = ad77681_update_sample_rate(ad77681);
	if (ret)
		return ret;

	return len;

}

/**
 * @brief Get the list of available values for mclk division.
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int ad77681_iio_mclk_div_avail(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	uint32_t avail_size = NO_OS_ARRAY_SIZE(mclk_div_avail);
	uint32_t i;
	int length = 0;

	for (i = 0; i < avail_size; i++)
		length += sprintf(buf + length, "%s ", mclk_div_avail[i]);

	return length;
}

/**
 * @brief Handles the read request for mclk division attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int ad77681_iio_show_mclk_div(void *dev, char *buf, uint32_t len,
				     const struct iio_ch_info *channel,
				     intptr_t priv)
{
	struct ad77681_iio_dev *iio_ad77681;
	struct ad77681_dev *ad77681;

	iio_ad77681 = (struct ad77681_iio_dev *)dev;
	ad77681 = iio_ad77681->ad77681_desc;

	return sprintf(buf, "%s", mclk_div_avail[ad77681->mclk_div]);
}

/**
 * @brief Handles the write request for mclk division attribute.
 *
 * @param dev - The iio device structure.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int ad77681_iio_store_mclk_div(void *dev, char *buf, uint32_t len,
				      const struct iio_ch_info *channel,
				      intptr_t priv)
{
	struct ad77681_iio_dev *iio_ad77681;
	struct ad77681_dev *ad77681;
	int ret;
	uint32_t avail_size = NO_OS_ARRAY_SIZE(mclk_div_avail);
	uint32_t i;

	iio_ad77681 = (struct ad77681_iio_dev *)dev;
	ad77681 = iio_ad77681->ad77681_desc;

	for (i = 0; i < avail_size; i++)
		if (!strcmp(buf, mclk_div_avail[i])) {
			ret = ad77681_set_mclk_div(dev, i);
			if (ret)
				return ret;
		}

	ret = ad77681_update_sample_rate(ad77681);
	if (ret)
		return ret;

	return len;
}

/**
 * @brief Handles the read request for sampling frequency attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int ad77681_iio_show_sampling_freq(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct ad77681_iio_dev *iio_ad77681;
	struct ad77681_dev *ad77681;

	iio_ad77681 = (struct ad77681_iio_dev *)dev;
	ad77681 = iio_ad77681->ad77681_desc;

	return sprintf(buf, "%ld", ad77681->sample_rate);
}

/**
 * @brief Get the list of available values for decimation rate.
 * @param dev - The iio device structure.
 * @param buf - Buffer to be filled with requested data.
 * @param len - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv - Private descriptor
 * @return 0 in case of success, error code otherwise
 */
static int ad77681_iio_decimation_rate_avail(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct ad77681_iio_dev *iio_ad77681;
	struct ad77681_dev *ad77681;

	iio_ad77681 = (struct ad77681_iio_dev *)dev;
	ad77681 = iio_ad77681->ad77681_desc;

	if (ad77681->filter == AD77681_SINC5) {
		return sprintf(buf, "%s", "8 16 32 64 128 256 512 1024");
	} else if (ad77681->filter == AD77681_FIR) {
		return sprintf(buf, "%s", "32 64 128 256 512 1024");
	} else {
		return sprintf(buf, "%s", "[32 32 163840]");
	}
}

/**
 * @brief Handles the read request for decimation rate attribute.
 *
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 *
 * @return ret    - 0 in case of success, errno errors otherwise
*/
static int ad77681_iio_show_decimation_rate(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct ad77681_iio_dev *iio_ad77681;
	struct ad77681_dev *ad77681;
	int length = 0;

	iio_ad77681 = (struct ad77681_iio_dev *)dev;
	ad77681 = iio_ad77681->ad77681_desc;

	if (ad77681->filter == AD77681_SINC5 || ad77681->filter == AD77681_FIR) {
		return sprintf(buf, "%s", sinc5_dec_rate_avail[ad77681->decimate]);
	} else if (ad77681->filter == AD77681_SINC5_DECx8) {
		return sprintf(buf, "%s", "8");
	} else if (ad77681->filter == AD77681_SINC5_DECx16) {
		return sprintf(buf, "%s", "16");
	} else {
		length += sprintf(buf + length, "%d ", (ad77681->sinc3_osr + 1) * 32);
	}
	return length;
}

/**
 * @brief Handles the write request for decimation rate attribute.
 *
 * @param dev - The iio device structure.
 * @param buf - Value to be written to attribute.
 * @param len - Length of the data in "buf".
 * @param channel - Channel properties.
 * @param priv    - Command attribute id.
 *
 * @return Number of bytes written to device, or negative value on failure.
 */
static int ad77681_iio_store_decimation_rate(void *dev, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv)
{
	struct ad77681_iio_dev *iio_ad77681;
	struct ad77681_dev *ad77681;
	int ret, temp;
	uint32_t avail_size = NO_OS_ARRAY_SIZE(sinc5_dec_rate_avail);
	uint32_t i;

	iio_ad77681 = (struct ad77681_iio_dev *)dev;
	ad77681 = iio_ad77681->ad77681_desc;

	for (i = 0; i < avail_size; i++)
		if (!strcmp(buf, "8")) {
			ret = ad77681_set_filter_type(dev, ad77681->decimate,
						      AD77681_SINC5_DECx8, ad77681->sinc3_osr);
			if (ret)
				return ret;
		} else if (!strcmp(buf, "16")) {
			ret = ad77681_set_filter_type(dev, ad77681->decimate,
						      AD77681_SINC5_DECx16, ad77681->sinc3_osr);
			if (ret)
				return ret;
		} else if (ad77681->filter == AD77681_SINC5 ||
			   ad77681->filter == AD77681_FIR) {
			for (i = 0; i < avail_size; i++)
				if (!strcmp(buf, sinc5_dec_rate_avail[i])) {
					ret = ad77681_set_filter_type(dev, i,
								      ad77681->filter,
								      ad77681->sinc3_osr);
				}
		} else {
			sscanf(buf, "%d", &temp);
			ret = ad77681_set_sinc3_dec_rate(ad77681, temp);
			if (ret)
				return ret;
		}

	ret = ad77681_update_sample_rate(ad77681);
	if (ret)
		return ret;

	return len;
}

/**
 * @brief	Getter for the raw attribute value
 * @param	device- pointer to IIO device structure
 * @param	buf- pointer to buffer holding attribute value
 * @param	len- length of buffer string data
 * @param	channel- pointer to IIO channel structure
 * @param	id- Attribute ID (optional)
 * @return	Number of characters read/written
 */
int ad77681_iio_read_raw(void *device, char *buf, uint32_t len,
			 const struct iio_ch_info *channel, intptr_t id)
{
	struct ad77681_iio_dev *iio_ad77681;
	struct ad77681_dev *ad77681;
	int ret;
	uint32_t adc_data_raw;
	uint32_t ad77681_sample_data = 0;
	uint8_t	 ad77681_sample_data_buff[AD77681_SAMPLE_DATA_BUFF_LEN] = { 0, 0, 0, 0, 0, 0 };

	iio_ad77681 = (struct ad77681_iio_dev *)device;
	ad77681 = iio_ad77681->ad77681_desc;

	ret = ad77681_set_conv_mode(ad77681, AD77681_CONV_SINGLE, AD77681_AIN_SHORT,
				    false);
	if (ret)
		return -EIO;

	ret = ad77681_spi_read_adc_data(ad77681, ad77681_sample_data_buff,
					AD77681_REGISTER_DATA_READ);
	if (ret) {
		return -EIO;
	}

	ad77681_sample_data =
		(
			(ad77681_sample_data_buff[1] << AD77681_2_BYTES_SHIFT) |
			(ad77681_sample_data_buff[2] << AD77681_1_BYTE_SHIFT) |
			(ad77681_sample_data_buff[3])
		);
	adc_data_raw = ad77681_sample_data & AD77681_24_BITS_SIGN_EXTENSION;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &adc_data_raw);
}

/**
 * @brief	Setter for the scale attribute value
 * @param	device- pointer to IIO device structure
 * @param	buf- pointer to buffer holding attribute value
 * @param	len- length of buffer string data
 * @param	channel- pointer to IIO channel structure
 * @param	id- Attribute ID (optional)
 * @return	Number of characters read/written
 */
int ad77681_iio_read_scale(void* device, char* buf, uint32_t len,
			   const struct iio_ch_info* channel, intptr_t id)
{
	struct ad77681_iio_dev *iio_ad77681;
	struct ad77681_dev *ad77681;
	int32_t vals[2];

	iio_ad77681 = (struct ad77681_iio_dev *)device;
	ad77681 = iio_ad77681->ad77681_desc;

	if (ad77681->has_pga) {
		vals[0] = ad77681->scale_tbl[ad77681->pga_gain_mode][0];
		vals[1] = ad77681->scale_tbl[ad77681->pga_gain_mode][1];

		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_NANO, 2, vals);
	}

	vals[0] = ad77681->vref / 1000;
	if (ad77681->has_variable_aaf)
		vals[0] = (vals[0] * MILLI) / ad7768_aaf_gains[ad77681->aaf_gain];
	/*
	 * ADC output code is two's complement so only (realbits - 1)
	 * bits express voltage magnitude
	 */
	vals[1] = ad77681_iio_scan_type.realbits - 1;

	return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2, vals);
}

int ad77681_iio_write_scale(void* device, char* buf, uint32_t len,
			    const struct iio_ch_info* channel, intptr_t id)
{
	struct ad77681_iio_dev *iio_ad77681;
	struct ad77681_dev *ad77681;
	int gain_mode, vals[2];
	float temp;

	iio_ad77681 = (struct ad77681_iio_dev *)device;
	ad77681 = iio_ad77681->ad77681_desc;

	if (!ad77681->has_pga)
		return -EOPNOTSUPP;

	sscanf(buf, "%f", &temp);
	vals[0] = (int)temp;
	vals[1] = (int)((temp - vals[0]) * NANO);

	gain_mode = ad77681_calc_pga_gain(ad77681, vals[0], vals[1],
					  ad77681_iio_scan_type.realbits);

	return ad77681_set_pga_gain(ad77681, gain_mode);
}

/**
 * @brief AD7768-1 reg read wrapper
 * @param dev - The iio device structure.
 * @param reg - Register address
 * @param readval - Register value
 *
 * @return 0 in case of success, errno errors otherwise
 */

static int ad77681_iio_reg_read(struct ad77681_iio_dev *dev, uint32_t reg,
				uint32_t *readval)
{
	int ret;
	uint8_t reg_data[3];

	ret = ad77681_spi_reg_read(dev->ad77681_desc, (uint8_t)reg,
				   reg_data);
	*readval = reg_data[1];

	return ret;
}

/**
 * @brief AD7768-1 reg write wrapper
 * @param dev - The iio device structure.
 * @param reg - Register address
 * @param readval - Register value
 *
 * @return 0 in case of success, errno errors otherwise
 */
static int ad77681_iio_reg_write(struct ad77681_iio_dev *dev, uint32_t reg,
				 uint32_t writeval)
{
	return ad77681_spi_reg_write(dev->ad77681_desc, (uint8_t)reg,
				     (uint8_t) writeval);
}

static struct iio_attribute ad77681_ch_attrs[] = {
	{
		.name = "raw",
		.show = ad77681_iio_read_raw,
	},
	{
		.name = "scale",
		.show = ad77681_iio_read_scale,
		.store = ad77681_iio_write_scale,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_attribute ad77681_global_attrs[] = {
	{
		.name = "common_mode_voltage",
		.show = ad77681_iio_show_vcm,
		.store = ad77681_iio_store_vcm,
		.shared = IIO_SHARED_BY_ALL,
	},
	{
		.name = "common_mode_voltage_available",
		.show = ad77681_iio_vcm_mode_avail,
		.shared = IIO_SHARED_BY_ALL,
	},
	{
		.name = "sampling_frequency",
		.show = ad77681_iio_show_sampling_freq,
		.shared = IIO_SHARED_BY_ALL,
	},
	{
		.name = "filter_mode",
		.show = ad77681_iio_show_filter_mode,
		.store = ad77681_iio_store_filter_mode,
		.shared = IIO_SHARED_BY_ALL,
	},
	{
		.name = "filter_mode_available",
		.show = ad77681_iio_filter_mode_avail,
		.shared = IIO_SHARED_BY_ALL,
	},
	{
		.name = "mclk_division",
		.show = ad77681_iio_show_mclk_div,
		.store = ad77681_iio_store_mclk_div,
		.shared = IIO_SHARED_BY_ALL,
	},
	{
		.name = "mclk_division_available",
		.show = ad77681_iio_mclk_div_avail,
		.shared = IIO_SHARED_BY_ALL,
	},
	{
		.name = "decimation_rate",
		.show = ad77681_iio_show_decimation_rate,
		.store = ad77681_iio_store_decimation_rate,
		.shared = IIO_SHARED_BY_ALL,
	},
	{
		.name = "decimation_rate_available",
		.show = ad77681_iio_decimation_rate_avail,
		.shared = IIO_SHARED_BY_ALL,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel ad77681_channels[] = {
	{
		.name = "voltage0",
		.ch_type = IIO_VOLTAGE,
		.channel = 0,
		.indexed = true,
		.attributes = ad77681_ch_attrs,
		.scan_type = &ad77681_iio_scan_type,
		.scan_index = 0,
		.ch_out = false,
	},
};

static struct iio_device ad77681_iio_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(ad77681_channels),
	.channels = ad77681_channels,
	.attributes = ad77681_global_attrs,
	.debug_reg_read = (int32_t (*)())ad77681_iio_reg_read,
	.debug_reg_write = (int32_t (*)())ad77681_iio_reg_write,
};


/**
 * @brief Initializes the AD7768-1 IIO driver
 * @param iio_dev - The iio device structure.
 * @param init_param - Parameters for the initialization of iio_dev
 *
 * @return 0 in case of success, errno errors otherwise
 */
int ad77681_iio_init(struct ad77681_iio_dev **iio_dev,
		     struct ad77681_iio_dev_init_param *init_param)
{
	int ret;
	struct ad77681_iio_dev *iio_descriptor;
	struct ad77681_status_registers *adc_status;

	if (!init_param)
		return -EINVAL;

	iio_descriptor = (struct ad77681_iio_dev *)no_os_calloc(1,
			 sizeof(*iio_descriptor));
	if (!iio_descriptor)
		return -ENOMEM;

	ret = ad77681_setup(&iio_descriptor->ad77681_desc,
			    *init_param->ad77681_init_param, &adc_status);
	if (ret)
		goto init_err;

	ad77681_fill_scale_tbl(iio_descriptor);
	iio_descriptor->iio_dev = &ad77681_iio_dev;

	*iio_dev = iio_descriptor;

	return 0;

init_err:
	no_os_free(iio_descriptor);

	return ret;
}

/**
 * @brief Free resources allocated by the init function
 * @param dev - Pointer to the IIO driver structure.
 *
 * @return 0 in case of success, errno errors otherwise
 */
int ad77681_iio_remove(struct ad77681_iio_dev *dev)
{
	if (!dev)
		return -ENODEV;

	no_os_free(dev);

	return 0;
}