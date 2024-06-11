/***************************************************************************//**
 *   @file   iio_adis.c
 *   @brief  Implementation of iio_adis.c
 *   @author RBolboac (ramona.bolboaca@analog.com)
 *******************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
 ******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "iio_adis_internals.h"
#include "no_os_delay.h"
#include "no_os_units.h"
#include <stdio.h>
#include <string.h>
#include "adis.h"
#include "adis_internals.h"
#include "iio_trigger.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define ADIS_BURST_DATA_SEL_0_CHN_MASK	NO_OS_GENMASK(5, 0)
#define ADIS_BURST_DATA_SEL_1_CHN_MASK	NO_OS_GENMASK(12, 7)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Wrapper for reading adis register.
 * @param device  - The iio device structure.
 * @param reg	  - Address of the register to be read from.
 * @param readval - Read data.
 * @return ret    - Result of the reading procedure.
 */
int adis_iio_read_reg(struct adis_iio_dev *device, uint32_t reg,
		      uint32_t *readval)
{
	return adis_read_reg(device->adis_dev, reg, readval, 2);
}

/**
 * @brief Wrapper for writing to adis register.
 * @param device   - The iio device structure.
 * @param reg	    - Address of the register to be written to.
 * @param writeval - Data to be written.
 * @return ret     - Result of the writing procedure.
 */
int adis_iio_write_reg(struct adis_iio_dev *device, uint32_t reg,
		       uint32_t writeval)
{
	return adis_write_reg(device->adis_dev, reg, writeval, 2);
}

/**
 * @brief Handles the read request for raw attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return the size of the read data in case of success, error code otherwise.
 */
int adis_iio_read_raw(void *dev, char *buf, uint32_t len,
		      const struct iio_ch_info *channel, intptr_t priv)
{
	struct adis_iio_dev *iio_adis;
	struct adis_dev *adis;
	int32_t res;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	switch(channel->address) {
	case ADIS_GYRO_X:
		ret = adis_read_x_gyro(adis, &res);
		break;
	case ADIS_GYRO_Y:
		ret = adis_read_y_gyro(adis, &res);
		break;
	case ADIS_GYRO_Z:
		ret = adis_read_z_gyro(adis, &res);
		break;
	case ADIS_ACCEL_X:
		ret = adis_read_x_accl(adis, &res);
		break;
	case ADIS_ACCEL_Y:
		ret = adis_read_y_accl(adis, &res);;
		break;
	case ADIS_ACCEL_Z:
		ret = adis_read_z_accl(adis, &res);
		break;
	case ADIS_TEMP:
		ret = adis_read_temp_out(adis, &res);
		break;
	case ADIS_DELTA_ANGL_X:
		ret = adis_read_x_deltang(adis, &res);
		break;
	case ADIS_DELTA_ANGL_Y:
		ret = adis_read_y_deltang(adis, &res);
		break;
	case ADIS_DELTA_ANGL_Z:
		ret = adis_read_z_deltang(adis, &res);
		break;
	case ADIS_DELTA_VEL_X:
		ret = adis_read_x_deltvel(adis, &res);
		break;
	case ADIS_DELTA_VEL_Y:
		ret = adis_read_y_deltvel(adis, &res);
		break;
	case ADIS_DELTA_VEL_Z:
		ret = adis_read_z_deltvel(adis, &res);
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &res);
}

/**
 * @brief Handles the read request for scale attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return the size of the read data in case of success, error code otherwise.
 */
int adis_iio_read_scale(void *dev, char *buf, uint32_t len,
			const struct iio_ch_info *channel, intptr_t priv)
{
	uint32_t vals[2];
	struct adis_iio_dev *iio_adis;
	struct adis_dev *adis;
	struct adis_scale_fractional scale_frac;
	struct adis_scale_fractional_log2 scale_frac_log2;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	switch (channel->type) {
	case IIO_ANGL_VEL:
		ret = adis_get_anglvel_scale(adis, &scale_frac);
		if (ret)
			return ret;
		vals[0] = scale_frac.dividend;
		vals[1] = scale_frac.divisor;
		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, (int32_t*)vals);
	case IIO_ACCEL:
		ret = adis_get_accl_scale(adis, &scale_frac);
		if (ret)
			return ret;
		vals[0] = scale_frac.dividend;
		vals[1] = scale_frac.divisor;
		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, (int32_t*)vals);
	case IIO_DELTA_ANGL:
		ret = adis_get_deltaangl_scale(adis, &scale_frac_log2);
		if (ret)
			return ret;
		vals[0] = scale_frac_log2.dividend;
		vals[1] = scale_frac_log2.power;
		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2, (int32_t*)vals);
	case IIO_DELTA_VELOCITY:
		ret = adis_get_deltavelocity_scale(adis, &scale_frac_log2);
		if (ret)
			return ret;
		vals[0] = scale_frac_log2.dividend;
		vals[1] = scale_frac_log2.power;
		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2, (int32_t*)vals);
	case IIO_TEMP:
		ret = adis_get_temp_scale(adis, &scale_frac);
		if (ret)
			return ret;
		vals[0] = scale_frac.dividend;
		vals[1] = scale_frac.divisor;
		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, (int32_t*)vals);
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Handles the read request for offset attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return the size of the read data in case of success, error code otherwise.
 */
int adis_iio_read_offset(void *dev, char *buf, uint32_t len,
			 const struct iio_ch_info *channel, intptr_t priv)
{
	struct adis_iio_dev *iio_adis;
	struct adis_dev *adis;
	int temp_offset;
	int32_t vals;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	switch (channel->type) {
	case IIO_TEMP:
		ret = adis_get_temp_offset(adis, &temp_offset);
		if (ret)
			return ret;
		vals = temp_offset;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, &vals);
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Handles the read request for calibbias attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return the size of the read data in case of success, error code otherwise.
 */
int adis_iio_read_calibbias(void *dev, char *buf, uint32_t len,
			    const struct iio_ch_info *channel, intptr_t priv)
{
	struct adis_iio_dev *iio_adis;
	struct adis_dev *adis;
	int ret;
	int32_t res;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	switch (channel->address) {
	case ADIS_GYRO_X:
		ret = adis_read_xg_bias(adis, &res);
		break;
	case ADIS_GYRO_Y:
		ret = adis_read_yg_bias(adis, &res);
		break;
	case ADIS_GYRO_Z:
		ret = adis_read_zg_bias(adis, &res);
		break;
	case ADIS_ACCEL_X:
		ret = adis_read_xa_bias(adis, &res);
		break;
	case ADIS_ACCEL_Y:
		ret = adis_read_ya_bias(adis, &res);
		break;
	case ADIS_ACCEL_Z:
		ret = adis_read_za_bias(adis, &res);
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &res);
}

/**
 * @brief Handles the write request for calibbias attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with the data to be written.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return the result of the writing procedure.
 */
int adis_iio_write_calibbias(void *dev, char *buf, uint32_t len,
			     const struct iio_ch_info *channel, intptr_t priv)
{
	struct adis_iio_dev *iio_adis;
	struct adis_dev *adis;
	int32_t calibbias;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, &calibbias, NULL);
	if (ret)
		return ret;

	switch (channel->address) {
	case ADIS_GYRO_X:
		return adis_write_xg_bias(adis, calibbias);
	case ADIS_GYRO_Y:
		return adis_write_yg_bias(adis, calibbias);
	case ADIS_GYRO_Z:
		return adis_write_zg_bias(adis, calibbias);
	case ADIS_ACCEL_X:
		return adis_write_xa_bias(adis, calibbias);
	case ADIS_ACCEL_Y:
		return adis_write_ya_bias(adis, calibbias);
	case ADIS_ACCEL_Z:
		return adis_write_za_bias(adis, calibbias);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Handles the read request for calibscale attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return the size of the read data in case of success, error code otherwise.
 */
int adis_iio_read_calibscale(void *dev, char *buf, uint32_t len,
			     const struct iio_ch_info *channel, intptr_t priv)
{
	struct adis_iio_dev *iio_adis;
	struct adis_dev *adis;
	int ret;
	int32_t res;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	switch (channel->address) {
	case ADIS_GYRO_X:
		ret = adis_read_xg_scale(adis, &res);
		break;
	case ADIS_GYRO_Y:
		ret = adis_read_yg_scale(adis, &res);
		break;
	case ADIS_GYRO_Z:
		ret = adis_read_zg_scale(adis, &res);
		break;
	case ADIS_ACCEL_X:
		ret = adis_read_xa_scale(adis, &res);
		break;
	case ADIS_ACCEL_Y:
		ret = adis_read_ya_scale(adis, &res);
		break;
	case ADIS_ACCEL_Z:
		ret = adis_read_za_scale(adis, &res);
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &res);
}

/**
 * @brief Handles the write request for calibscale attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with the data to be written.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return the result of the writing procedure.
 */
int adis_iio_write_calibscale(void *dev, char *buf, uint32_t len,
			      const struct iio_ch_info *channel, intptr_t priv)
{
	struct adis_iio_dev *iio_adis;
	struct adis_dev *adis;
	int32_t calibscale;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, &calibscale, NULL);
	if (ret)
		return ret;

	switch (channel->address) {
	case ADIS_GYRO_X:
		return adis_write_xg_scale(adis, calibscale);
	case ADIS_GYRO_Y:
		return adis_write_yg_scale(adis, calibscale);
	case ADIS_GYRO_Z:
		return adis_write_zg_scale(adis, calibscale);
	case ADIS_ACCEL_X:
		return adis_write_xa_scale(adis, calibscale);
	case ADIS_ACCEL_Y:
		return adis_write_ya_scale(adis, calibscale);
	case ADIS_ACCEL_Z:
		return adis_write_za_scale(adis, calibscale);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Handles the read request for lpf attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return the size of the read data in case of success, error code otherwise.
 */
int adis_iio_read_lpf(void *dev, char *buf, uint32_t len,
		      const struct iio_ch_info *channel, intptr_t priv)
{
	struct adis_iio_dev *iio_adis;
	struct adis_dev *adis;
	uint32_t freq;
	int ret;
	int32_t res;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	switch(channel->address) {
	case ADIS_GYRO_X:
		ret = adis_read_lpf(adis, ADIS_GYRO_CHAN, ADIS_X_AXIS, &freq);
		break;
	case ADIS_GYRO_Y:
		ret = adis_read_lpf(adis, ADIS_GYRO_CHAN, ADIS_Y_AXIS, &freq);
		break;
	case ADIS_GYRO_Z:
		ret = adis_read_lpf(adis, ADIS_GYRO_CHAN, ADIS_Z_AXIS, &freq);
		break;
	case ADIS_ACCEL_X:
		ret = adis_read_lpf(adis, ADIS_ACCL_CHAN, ADIS_X_AXIS, &freq);
		break;
	case ADIS_ACCEL_Y:
		ret = adis_read_lpf(adis, ADIS_ACCL_CHAN, ADIS_Y_AXIS, &freq);
		break;
	case ADIS_ACCEL_Z:
		ret = adis_read_lpf(adis, ADIS_ACCL_CHAN, ADIS_Z_AXIS, &freq);
		break;
	default:
		/*
		 * Add default case for devices which do not have a filter per
		 * channel. The logic for retrieving the filter frequency is
		 * implemented in the lower level driver.
		 */
		ret = adis_read_lpf(adis, ADIS_GYRO_CHAN, ADIS_X_AXIS, &freq);
	}

	if (ret)
		return ret;

	res = freq;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, &res);
}

/**
 * @brief Handles the write request for lpf attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with the data to be written.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return the result of the writing procedure.
 */
int adis_iio_write_lpf(void *dev, char *buf, uint32_t len,
		       const struct iio_ch_info *channel, intptr_t priv)
{
	struct adis_iio_dev *iio_adis;
	struct adis_dev *adis;
	uint32_t freq;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, (int32_t*)&freq, NULL);
	if (ret)
		return ret;

	switch(channel->address) {
	case ADIS_GYRO_X:
		return adis_write_lpf(adis, ADIS_GYRO_CHAN, ADIS_X_AXIS, freq);
	case ADIS_GYRO_Y:
		return adis_write_lpf(adis, ADIS_GYRO_CHAN, ADIS_Y_AXIS, freq);
	case ADIS_GYRO_Z:
		return adis_write_lpf(adis, ADIS_GYRO_CHAN, ADIS_Z_AXIS, freq);
	case ADIS_ACCEL_X:
		return adis_write_lpf(adis, ADIS_ACCL_CHAN, ADIS_X_AXIS, freq);
	case ADIS_ACCEL_Y:
		return adis_write_lpf(adis, ADIS_ACCL_CHAN, ADIS_Y_AXIS, freq);
	case ADIS_ACCEL_Z:
		return adis_write_lpf(adis, ADIS_ACCL_CHAN, ADIS_Z_AXIS, freq);
	default:
		/*
		 * Add default case for devices which do not have a filter per
		 * channel. The logic for setting the filter frequency is
		 * implemented in the lower level driver.
		 */
		return adis_write_lpf(adis, ADIS_GYRO_CHAN, ADIS_X_AXIS, freq);
	}
}

/**
 * @brief Reads sync scale and decimation rate and computes the device sampling
 *        frequency in Hz.
 * @param dev  - The iio device structure.
 * @param freq - The sampling frequency of the device.
 * @return 0 in case of success, error code otherwise.
 */
static int adis_iio_get_freq(struct adis_dev* adis, uint32_t *freq)
{
	uint32_t sample_rate;
	uint32_t up_scale;
	uint32_t dec_rate;
	uint32_t sync_mode;
	uint32_t adis_sync_clk_freq;
	int ret;

	ret = adis_get_sync_clk_freq(adis, &adis_sync_clk_freq);
	if (ret)
		return ret;

	sample_rate = adis_sync_clk_freq;
	ret = adis_read_sync_mode(adis, &sync_mode);
	if (ret)
		return ret;

	if(sync_mode == ADIS_SYNC_SCALED) {
		ret = adis_read_up_scale(adis, &up_scale);
		if (ret)
			return ret;
		sample_rate = adis_sync_clk_freq * up_scale;
	}

	ret =  adis_read_dec_rate(adis, &dec_rate);
	if (ret)
		return ret;

	*freq = NO_OS_DIV_ROUND_CLOSEST(sample_rate, dec_rate + 1);

	return 0;
}

/**
 * @brief Handles the read request for sampling frequency attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return the size of the read data in case of success, error code otherwise.
 */
int adis_iio_read_sampling_freq(void *dev, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t priv)
{
	struct adis_iio_dev *iio_adis;
	struct adis_dev *adis;
	uint32_t freq;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	ret = adis_iio_get_freq(adis, &freq);
	if(ret)
		return ret;

	iio_adis->sampling_frequency = freq;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t*)&freq);
}

/**
 * @brief Sets sync scale and decimation rate based on the desired sampling
 *        frequency in Hz.
 * @param dev  - The iio device structure.
 * @param freq - The desired sampling frequency of the device.
 * @return 0 in case of success, error code otherwise.
 */
static int adis_iio_set_freq(struct adis_dev* adis, uint32_t freq)
{
	uint32_t scaled_rate;
	uint32_t up_scale;
	int ret;
	uint32_t sample_rate;
	uint32_t max_sampling_clk_freq = adis->info->sampling_clk_limits.max_freq;
	uint32_t dec_rate;
	uint32_t sync_mode;
	uint32_t adis_sync_clk_freq;

	ret = adis_get_sync_clk_freq(adis, &adis_sync_clk_freq);
	if (ret)
		return ret;
	sample_rate = adis_sync_clk_freq;

	ret = adis_read_sync_mode(adis, &sync_mode);
	if (ret)
		return ret;

	if(sync_mode == ADIS_SYNC_SCALED) {
		scaled_rate = no_os_lowest_common_multiple(adis_sync_clk_freq, freq);

		/*
		 * If lcm is bigger than the IMU maximum sampling rate there's
		 * no perfect solution. In this case, we get the highest
		 * multiple of the input clock lower than the IMU max sample
		 * rate.
		 */
		if (scaled_rate > max_sampling_clk_freq)
			scaled_rate = max_sampling_clk_freq / adis->ext_clk * adis->ext_clk;
		else
			scaled_rate = max_sampling_clk_freq / scaled_rate * scaled_rate;

		up_scale = scaled_rate / adis_sync_clk_freq;
		ret = adis_write_up_scale(adis, up_scale);
		if (ret)
			return ret;

		sample_rate = scaled_rate;
	}

	dec_rate = NO_OS_DIV_ROUND_CLOSEST(sample_rate, freq);

	if (dec_rate)
		dec_rate--;

	if (dec_rate > adis->info->dec_rate_max)
		dec_rate = adis->info->dec_rate_max;

	return adis_write_dec_rate(adis, dec_rate);
}

/**
 * @brief Handles the write request for sampling frequency attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with the data to be written.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return the result of the writing procedure.
 */
int adis_iio_write_sampling_freq(void *dev, char *buf,
				 uint32_t len, const struct iio_ch_info *channel, intptr_t priv)
{
	struct adis_iio_dev *iio_adis;
	struct adis_dev *adis;
	uint32_t freq;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, (int32_t*)&freq, NULL);
	if (ret)
		return ret;

	ret = adis_iio_set_freq(adis, freq);
	if (ret)
		return ret;

	/* Update sampling frequency */
	return adis_iio_get_freq(adis, &iio_adis->sampling_frequency);
}

/**
 * @brief Reads firmware date and returns it in char format.
 * @param adis - The adis device.
 * @param buf  - The read firmware date in char format.
 * @param size - The size of buf.
 * @return the size of the written data in buf in case of success, error code
 *         otherwise.
 */
static int adis_iio_read_firm_date(struct adis_dev* adis, char *buf,
				   uint8_t size)
{
	uint32_t firm_d;
	uint32_t firm_m;
	uint32_t firm_y;
	int ret;

	ret = adis_read_firm_d(adis, &firm_d);
	if (ret)
		return ret;

	ret = adis_read_firm_m(adis, &firm_m);
	if (ret)
		return ret;

	ret = adis_read_firm_y(adis, &firm_y);
	if (ret)
		return ret;

	return snprintf(buf, size, "%.2lx-%.2lx-%.4lx", firm_m, firm_d, firm_y);
}

/**
 * @brief Reads firmware revision and returns it in char format.
 * @param adis - The adis device.
 * @param buf  - The read firmware revision in char format.
 * @param size - The size of buf.
 * @return the size of the written data in buf in case of success, error code
 *         otherwise.
 */
static int adis_iio_read_firm_rev(struct adis_dev* adis, char *buf,
				  uint8_t size)
{
	uint32_t firm_rev;
	int ret;

	ret = adis_read_firm_rev(adis, &firm_rev);
	if (ret)
		return ret;

	return snprintf(buf, size, "%ld.%ld", firm_rev >> 8, firm_rev & 0xff);
}

/**
 * @brief Reads the serial number and returns it in char format.
 * @param adis - The adis device.
 * @param buf  - The read serial number in char format.
 * @param size - The size of buf.
 * @return the size of the written data in buf in case of success, error code
 *         otherwise.
 */
static int adis_iio_read_serial_num(struct adis_dev* adis, char *buf,
				    uint8_t size)
{
	uint32_t serial_num;
	int ret;

	ret = adis_read_serial_num(adis, &serial_num);
	if (ret)
		return ret;

	return snprintf(buf, size, "0x%.4lx", serial_num);
}

/**
 * @brief Reads gyroscope measurement range value and returns it in char format.
 * @param iio_adis - The iio adis device.
 * @param buf      - The gyroscope measurement range value in char format.
 * @return the size of the written data in buf in case of success, error code
 *         otherwise.
 */
int adis_iio_read_gyro_meas_range(struct adis_iio_dev* iio_adis, char *buf)
{
	return snprintf(buf, 50, "%s", iio_adis->rang_mdl_txt);
}

/**
 * @brief Handles the read request for debug attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return the size of the read data in case of success, error code otherwise.
 */
int adis_iio_read_debug_attrs(void *dev, char *buf, uint32_t len,
			      const struct iio_ch_info *channel, intptr_t priv)
{
	struct adis_iio_dev *iio_adis;
	struct adis_dev *adis;
	uint32_t res;
	int ret = 0;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	switch(priv) {
	case ADIS_DIAG_SNSR_INIT_FAILURE:
		ret = adis_read_diag_snsr_init_failure(adis, &res);
		break;
	case ADIS_DIAG_DATA_PATH_OVERRUN:
		ret = adis_read_diag_data_path_overrun(adis, &res);
		break;
	case ADIS_DIAG_FLS_MEM_UPDATE_FAILURE:
		ret = adis_read_diag_fls_mem_update_failure(adis, &res);
		break;
	case ADIS_DIAG_SPI_COMM_ERR:
		ret = adis_read_diag_spi_comm_err(adis, &res);
		break;
	case ADIS_DIAG_STANDBY_MODE:
		ret = adis_read_diag_standby_mode(adis, &res);
		break;
	case ADIS_DIAG_SNSR_FAILURE:
		ret = adis_read_diag_snsr_failure(adis, &res);
		break;
	case ADIS_DIAG_MEM_FAILURE:
		ret = adis_read_diag_mem_failure(adis, &res);
		break;
	case ADIS_DIAG_CLK_ERR:
		ret = adis_read_diag_clk_err(adis, &res);
		break;
	case ADIS_DIAG_GYRO1_FAILURE:
		ret = adis_read_diag_gyro1_failure(adis, &res);
		break;
	case ADIS_DIAG_GYRO2_FAILURE:
		ret = adis_read_diag_gyro2_failure(adis, &res);
		break;
	case ADIS_DIAG_ACCL_FAILURE:
		ret = adis_read_diag_accl_failure(adis, &res);
		break;
	case ADIS_DIAG_X_AXIS_GYRO_FAILURE:
		ret = adis_read_diag_x_axis_gyro_failure(adis, &res);
		break;
	case ADIS_DIAG_Y_AXIS_GYRO_FAILURE:
		ret = adis_read_diag_y_axis_gyro_failure(adis, &res);
		break;
	case ADIS_DIAG_Z_AXIS_GYRO_FAILURE:
		ret = adis_read_diag_z_axis_gyro_failure(adis, &res);
		break;
	case ADIS_DIAG_X_AXIS_ACCL_FAILURE:
		ret = adis_read_diag_x_axis_accl_failure(adis, &res);
		break;
	case ADIS_DIAG_Y_AXIS_ACCL_FAILURE:
		ret = adis_read_diag_y_axis_accl_failure(adis, &res);
		break;
	case ADIS_DIAG_Z_AXIS_ACCL_FAILURE:
		ret = adis_read_diag_z_axis_accl_failure(adis, &res);
		break;
	case ADIS_DIAG_ADUC_MCU_FAULT:
		ret = adis_read_diag_aduc_mcu_fault(adis, &res);
		break;
	case ADIS_DIAG_CONFIG_CALIB_CRC_ERR:
		ret = adis_read_diag_config_calib_crc_error(adis, &res);
		break;
	case ADIS_DIAG_OVERRANGE:
		ret = adis_read_diag_overrange(adis, &res);
		break;
	case ADIS_DIAG_TEMP_ERR:
		ret = adis_read_diag_temp_err(adis, &res);
		break;
	case ADIS_DIAG_POWER_SUPPLY_FAILURE:
		ret = adis_read_diag_power_supply_failure(adis, &res);
		break;
	case ADIS_DIAG_BOOT_MEMORY_FAILURE:
		ret = adis_read_diag_boot_memory_failure(adis, &res);
		break;
	case ADIS_DIAG_REG_NVM_ERR:
		ret = adis_read_diag_reg_nvm_err(adis, &res);
		break;
	case ADIS_DIAG_WDG_TIMER_FLAG:
		ret = adis_read_diag_wdg_timer_flag(adis, &res);
		break;
	case ADIS_DIAG_INT_PROC_SUPPLY_ERR:
		ret = adis_read_diag_int_proc_supply_err(adis, &res);
		break;
	case ADIS_DIAG_EXT_5V_SUPPLY_ERR:
		ret = adis_read_diag_ext_5v_supply_err(adis, &res);
		break;
	case ADIS_DIAG_INT_SNSR_SUPPLY_ERR:
		ret = adis_read_diag_int_snsr_supply_err(adis, &res);
		break;
	case ADIS_DIAG_INT_REG_ERR:
		ret = adis_read_diag_int_snsr_supply_err(adis, &res);
		break;
	case ADIS_DIAG_CHECKSUM_ERR:
		adis_read_diag_checksum_err(adis, &res);
		break;
	case ADIS_DIAG_FLS_MEM_WR_CNT_EXCEED:
		adis_read_diag_fls_mem_wr_cnt_exceed(adis, &res);
		break;
	case ADIS_DIAG_LOST_SAMPLES_COUNT:
		res = iio_adis->samples_lost;
		break;
	case ADIS_TIME_STAMP:
		ret = adis_read_time_stamp(adis, &res);
		break;
	case ADIS_DATA_CNTR:
		ret = adis_read_data_cntr(adis, &res);
		break;
	case ADIS_FIFO_CNT:
		ret = adis_read_fifo_cnt(adis, &res);
		break;
	case ADIS_SPI_CHKSUM:
		ret = adis_read_spi_chksum(adis, &res);
		break;
	case ADIS_FIFO_EN:
		ret = adis_read_fifo_en(adis, &res);
		break;
	case ADIS_FIFO_OVERFLOW:
		ret = adis_read_fifo_overflow(adis, &res);
		break;
	case ADIS_FIFO_WM_INT_EN:
		ret = adis_read_fifo_wm_int_en(adis, &res);
		break;
	case ADIS_FIFO_WM_INT_POL:
		ret = adis_read_fifo_wm_int_pol(adis, &res);
		break;
	case ADIS_FIFO_WM_LVL:
		ret = adis_read_fifo_wm_lvl(adis, &res);
		break;
	case ADIS_FILT_SIZE_VAR_B:
		ret = adis_read_filt_size_var_b(adis, &res);
		break;
	case ADIS_GYRO_MEAS_RANGE:
		return adis_iio_read_gyro_meas_range(iio_adis, buf);
	case ADIS_DR_ENABLE:
		ret = adis_read_dr_enable(adis, &res);
		break;
	case ADIS_DR_POLARITY:
		ret = adis_read_dr_polarity(adis, &res);
		break;
	case ADIS_DR_LINE_SEL:
		ret = adis_read_dr_selection(adis, &res);
		break;
	case ADIS_SYNC_POLARITY:
		ret = adis_read_sync_polarity(adis, &res);
		break;
	case ADIS_SYNC_LINE_SEL:
		ret = adis_read_sync_selection(adis, &res);
		break;
	case ADIS_SYNC_MODE:
		ret = adis_read_sync_mode(adis, &res);
		break;
	case ADIS_ALARM_IND_ENABLE:
		ret = adis_read_alarm_enable(adis, &res);
		break;
	case ADIS_ALARM_IND_POLARITY:
		ret = adis_read_alarm_polarity(adis, &res);
		break;
	case ADIS_ALARM_IND_LINE_SEL:
		ret = adis_read_alarm_selection(adis, &res);
		break;
	case ADIS_DIO_1_DIR:
		ret = adis_read_gpio_dir(adis, 0, &res);
		break;
	case ADIS_DIO_1_LVL:
		ret = adis_read_gpio_lvl(adis, 0, &res);
		break;
	case ADIS_DIO_2_DIR:
		ret = adis_read_gpio_dir(adis, 1, &res);
		break;
	case ADIS_DIO_2_LVL:
		ret = adis_read_gpio_lvl(adis, 1, &res);
		break;
	case ADIS_DIO_3_DIR:
		ret = adis_read_gpio_dir(adis, 2, &res);
		break;
	case ADIS_DIO_3_LVL:
		ret = adis_read_gpio_lvl(adis, 2, &res);
		break;
	case ADIS_DIO_4_DIR:
		ret = adis_read_gpio_dir(adis, 3, &res);
		break;
	case ADIS_DIO_4_LVL:
		ret = adis_read_gpio_lvl(adis, 3, &res);
		break;
	case ADIS_SENS_BW:
		ret = adis_read_sens_bw(adis, &res);
		break;
	case ADIS_PT_OF_PERC_ALGNMT:
		ret = adis_read_pt_of_perc_algnmt(adis, &res);
		break;
	case ADIS_LINEAR_ACCL_COMP:
		ret = adis_read_linear_accl_comp(adis, &res);
		break;
	case ADIS_BURST_SEL:
		ret = adis_read_burst_sel(adis, &res);
		break;
	case ADIS_BURST32:
		ret = adis_read_burst32(adis, &res);
		break;
	case ADIS_TIMESTAMP32:
		ret = adis_read_timestamp32(adis, &res);
		break;
	case ADIS_SYNC_4KHZ:
		ret = adis_read_sync_4khz(adis, &res);
		break;
	case ADIS_GYRO_FIR_ENABLE:
		ret = adis_read_gyro_fir_enable(adis, &res);
		break;
	case ADIS_ACCL_FIR_ENABLE:
		ret = adis_read_accl_fir_enable(adis, &res);
		break;
	case ADIS_FIR_EN_XG:
		ret = adis_read_fir_en_xg(adis, &res);
		break;
	case ADIS_FIR_EN_YG:
		ret = adis_read_fir_en_yg(adis, &res);
		break;
	case ADIS_FIR_EN_ZG:
		ret = adis_read_fir_en_zg(adis, &res);
		break;
	case ADIS_FIR_EN_XA:
		ret = adis_read_fir_en_xa(adis, &res);
		break;
	case ADIS_FIR_EN_YA:
		ret = adis_read_fir_en_ya(adis, &res);
		break;
	case ADIS_FIR_EN_ZA:
		ret = adis_read_fir_en_za(adis, &res);
		break;
	case ADIS_FIR_BANK_SEL_XG:
		ret = adis_read_fir_bank_sel_xg(adis, &res);
		break;
	case ADIS_FIR_BANK_SEL_YG:
		ret = adis_read_fir_bank_sel_yg(adis, &res);
		break;
	case ADIS_FIR_BANK_SEL_ZG:
		ret = adis_read_fir_bank_sel_zg(adis, &res);
		break;
	case ADIS_FIR_BANK_SEL_XA:
		ret = adis_read_fir_bank_sel_xa(adis, &res);
		break;
	case ADIS_FIR_BANK_SEL_YA:
		ret = adis_read_fir_bank_sel_ya(adis, &res);
		break;
	case ADIS_FIR_BANK_SEL_ZA:
		ret = adis_read_fir_bank_sel_za(adis, &res);
		break;
	case ADIS_UP_SCALE:
		ret = adis_read_up_scale(adis, &res);
		break;
	case ADIS_DEC_RATE:
		ret = adis_read_dec_rate(adis, &res);
		break;
	case ADIS_BIAS_CORR_TBC:
		ret = adis_read_bias_corr_tbc(adis, &res);
		break;
	case ADIS_BIAS_CORR_EN_XG:
		ret = adis_read_bias_corr_en_xg(adis, &res);
		break;
	case ADIS_BIAS_CORR_EN_YG:
		ret = adis_read_bias_corr_en_yg(adis, &res);
		break;
	case ADIS_BIAS_CORR_EN_ZG:
		ret = adis_read_bias_corr_en_zg(adis, &res);
		break;
	case ADIS_BIAS_CORR_EN_XA:
		ret = adis_read_bias_corr_en_xa(adis, &res);
		break;
	case ADIS_BIAS_CORR_EN_YA:
		ret = adis_read_bias_corr_en_ya(adis, &res);
		break;
	case ADIS_BIAS_CORR_EN_ZA:
		ret = adis_read_bias_corr_en_za(adis, &res);
		break;
	case ADIS_PROC_REV:
		ret = adis_read_proc_rev(adis, &res);
		break;
	case ADIS_BOOT_REV:
		ret = adis_read_boot_rev(adis, &res);
		break;
	case ADIS_FIRM_REV:
		return adis_iio_read_firm_rev(adis, buf, 7);
	case ADIS_FIRM_DATE:
		return adis_iio_read_firm_date(adis, buf, 12);
	case ADIS_PROD_ID:
		ret = adis_read_prod_id(adis, &res);
		break;
	case ADIS_SERIAL_NUM:
		return adis_iio_read_serial_num(adis, buf, 7);
	case ADIS_LOT_NUM:
		ret = adis_read_lot_num(adis, &res);
		break;
	case ADIS_USR_SCR_1:
		ret = adis_read_usr_scr_1(adis, &res);
		break;
	case ADIS_USR_SCR_2:
		ret = adis_read_usr_scr_2(adis, &res);
		break;
	case ADIS_USR_SCR_3:
		ret = adis_read_usr_scr_3(adis, &res);
		break;
	case ADIS_USR_SCR_4:
		ret = adis_read_usr_scr_4(adis, &res);
		break;
	case ADIS_FLS_MEM_WR_CNTR:
		ret = adis_read_fls_mem_wr_cntr(adis, &res);
		break;
	case ADIS_EXT_CLK_FREQ:
		res = adis->ext_clk;
		ret = 0;
		break;
	default:
		return -EINVAL;
	}

	if (ret)
		return ret;

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t*)&res);
}

/**
 * @brief Handles the write request for debug attributes.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with the data to be written.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return the result of the writing procedure.
 */
int adis_iio_write_debug_attrs(void *dev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv)
{
	struct adis_iio_dev *iio_adis;
	struct adis_dev *adis;
	uint32_t val;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, (int32_t*)&val, NULL);
	if (ret)
		return ret;

	switch(priv) {
	case ADIS_FIFO_EN:
		return adis_write_fifo_en(adis, val);
	case ADIS_FIFO_OVERFLOW:
		return adis_write_fifo_overflow(adis, val);
	case ADIS_FIFO_WM_INT_EN:
		return adis_write_fifo_wm_int_en(adis, val);
	case ADIS_FIFO_WM_INT_POL:
		return adis_write_fifo_wm_int_pol(adis, val);
	case ADIS_FIFO_WM_LVL:
		return adis_write_fifo_wm_lvl(adis, val);
	case ADIS_FILT_SIZE_VAR_B:
		return adis_write_filt_size_var_b(adis, val);
	case ADIS_DR_ENABLE:
		return adis_write_dr_enable(adis, val);
	case ADIS_DR_POLARITY:
		return adis_write_dr_polarity(adis, val);
	case ADIS_DR_LINE_SEL:
		return adis_write_dr_selection(adis, val);
	case ADIS_SYNC_POLARITY:
		return adis_write_sync_polarity(adis, val);
	case ADIS_SYNC_LINE_SEL:
		return adis_write_sync_selection(adis, val);
	case ADIS_SYNC_MODE:
		ret = adis_write_sync_mode(adis, val, adis->ext_clk);
		if (ret)
			return ret;

		/* Update sampling frequency */
		return adis_iio_get_freq(adis, &iio_adis->sampling_frequency);
	case ADIS_ALARM_IND_ENABLE:
		return adis_write_alarm_enable(adis, val);
	case ADIS_ALARM_IND_POLARITY:
		return adis_write_alarm_polarity(adis, val);
	case ADIS_ALARM_IND_LINE_SEL:
		return adis_write_alarm_selection(adis, val);
	case ADIS_DIO_1_DIR:
		return adis_write_gpio_dir(adis, 0, val);
	case ADIS_DIO_2_DIR:
		return adis_write_gpio_dir(adis, 1, val);
	case ADIS_DIO_3_DIR:
		return adis_write_gpio_dir(adis, 2, val);
	case ADIS_DIO_4_DIR:
		return adis_write_gpio_dir(adis, 3, val);
	case ADIS_DIO_1_LVL:
		return adis_write_gpio_lvl(adis, 0, val);
	case ADIS_DIO_2_LVL:
		return adis_write_gpio_lvl(adis, 1, val);
	case ADIS_DIO_3_LVL:
		return adis_write_gpio_lvl(adis, 2, val);
	case ADIS_DIO_4_LVL:
		return adis_write_gpio_lvl(adis, 3, val);
	case ADIS_SENS_BW:
		return adis_write_sens_bw(adis, val);
	case ADIS_PT_OF_PERC_ALGNMT:
		return adis_write_pt_of_perc_algnmt(adis, val);
	case ADIS_LINEAR_ACCL_COMP:
		return adis_write_linear_accl_comp(adis, val);
	case ADIS_BURST_SEL:
		return adis_write_burst_sel(adis, val);
	case ADIS_BURST32:
		return adis_write_burst32(adis, val);
	case ADIS_TIMESTAMP32:
		return adis_write_timestamp32(adis, val);
	case ADIS_SYNC_4KHZ:
		return adis_write_sync_4khz(adis, val);
	case ADIS_GYRO_FIR_ENABLE:
		return adis_write_gyro_fir_enable(adis, val);
	case ADIS_ACCL_FIR_ENABLE:
		return adis_write_accl_fir_enable(adis, val);
	case ADIS_FIR_EN_XG:
		return adis_write_fir_en_xg(adis, val);
	case ADIS_FIR_EN_YG:
		return adis_write_fir_en_yg(adis, val);
	case ADIS_FIR_EN_ZG:
		return adis_write_fir_en_zg(adis, val);
	case ADIS_FIR_EN_XA:
		return adis_write_fir_en_xa(adis, val);
	case ADIS_FIR_EN_YA:
		return adis_write_fir_en_ya(adis, val);
	case ADIS_FIR_EN_ZA:
		return adis_write_fir_en_za(adis, val);
	case ADIS_FIR_BANK_SEL_XG:
		return adis_write_fir_bank_sel_xg(adis, val);
	case ADIS_FIR_BANK_SEL_YG:
		return adis_write_fir_bank_sel_yg(adis, val);
	case ADIS_FIR_BANK_SEL_ZG:
		return adis_write_fir_bank_sel_zg(adis, val);
	case ADIS_FIR_BANK_SEL_XA:
		return adis_write_fir_bank_sel_xa(adis, val);
	case ADIS_FIR_BANK_SEL_YA:
		return adis_write_fir_bank_sel_ya(adis, val);
	case ADIS_FIR_BANK_SEL_ZA:
		return adis_write_fir_bank_sel_za(adis, val);
	case ADIS_UP_SCALE:
		ret = adis_write_up_scale(adis, val);
		if (ret)
			return ret;

		/* Update sampling frequency */
		return adis_iio_get_freq(adis, &iio_adis->sampling_frequency);
	case ADIS_DEC_RATE:
		ret = adis_write_dec_rate(adis, val);
		if (ret)
			return ret;

		/* Update sampling frequency */
		return adis_iio_get_freq(adis, &iio_adis->sampling_frequency);
	case ADIS_BIAS_CORR_TBC:
		return adis_write_bias_corr_tbc(adis, val);
	case ADIS_BIAS_CORR_EN_XG:
		return adis_write_bias_corr_en_xg(adis, val);
	case ADIS_BIAS_CORR_EN_YG:
		return adis_write_bias_corr_en_yg(adis, val);
	case ADIS_BIAS_CORR_EN_ZG:
		return adis_write_bias_corr_en_zg(adis, val);
	case ADIS_BIAS_CORR_EN_XA:
		return adis_write_bias_corr_en_xa(adis, val);
	case ADIS_BIAS_CORR_EN_YA:
		return adis_write_bias_corr_en_ya(adis, val);
	case ADIS_BIAS_CORR_EN_ZA:
		return adis_write_bias_corr_en_za(adis, val);
	case ADIS_CMD_BIAS_CORR_UPDATE:
		return adis_cmd_bias_corr_update(adis);
	case ADIS_CMD_FACT_CALIB_RESTORE:
		ret = adis_cmd_fact_calib_restore(adis);
		if (ret)
			return ret;

		/* Update sampling frequency */
		return adis_iio_get_freq(adis, &iio_adis->sampling_frequency);
	case ADIS_CMD_SNSR_SELF_TEST:
		return adis_cmd_snsr_self_test(adis);
	case ADIS_CMD_FLS_MEM_UPDATE:
		return adis_cmd_fls_mem_update(adis);
	case ADIS_CMD_FLS_MEM_TEST:
		return adis_cmd_fls_mem_test(adis);
	case ADIS_CMD_FIFO_FLUSH:
		return adis_cmd_fifo_flush(adis);
	case ADIS_CMD_SW_RES:
		ret = adis_cmd_sw_res(adis);
		if (ret)
			return ret;

		/* Update sampling frequency */
		return adis_iio_get_freq(adis, &iio_adis->sampling_frequency);
	case ADIS_CMD_WRITE_LOCK:
		return adis_cmd_write_lock(adis);
	case ADIS_USR_SCR_1:
		return adis_write_usr_scr_1(adis, val);
	case ADIS_USR_SCR_2:
		return adis_write_usr_scr_2(adis, val);
	case ADIS_USR_SCR_3:
		return adis_write_usr_scr_3(adis, val);
	case ADIS_USR_SCR_4:
		return adis_write_usr_scr_4(adis, val);
	case ADIS_EXT_CLK_FREQ:
		return adis_update_ext_clk_freq(adis, val);
	default:
		return -EINVAL;
	}
}

/**
 * @brief API to be called before trigger is enabled.
 * @param dev  - The iio device structure.
 * @param mask - The active channels mask.
 * @return 0 in case of success, error code otherwise.
 */
int adis_iio_pre_enable(void* dev, uint32_t mask)
{
	struct adis_iio_dev *iio_adis;
	struct adis_dev *adis;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	if (adis->info->flags & ADIS_HAS_BURST_DELTA_DATA) {
		/* Check mask */
		if ((mask & ADIS_BURST_DATA_SEL_0_CHN_MASK)
		    && (mask & ADIS_BURST_DATA_SEL_1_CHN_MASK))
			return -EINVAL;

		if (mask & ADIS_BURST_DATA_SEL_1_CHN_MASK)
			iio_adis->burst_sel = 1;
		else
			iio_adis->burst_sel = 0;
	}

	if (adis->info->flags & ADIS_HAS_BURST32)
		iio_adis->burst_size = 1;
	else
		iio_adis->burst_size = 0;

	iio_adis->samples_lost = 0;
	iio_adis->data_cntr = 0;

	if (iio_adis->has_fifo) {
		/* Set FIFO overflow behavior to overwrite old data when FIFO is full. */
		ret = adis_cmd_fifo_flush(adis);
		if (ret)
			return ret;
		/* From data-sheet, wait time to finalize the fifo flush command */
		no_os_udelay(500);
		ret = adis_write_fifo_overflow(adis, 1);
		if (ret)
			return ret;
		ret = adis_write_fifo_en(adis, 1);
		if (ret)
			return ret;
	}

	return adis_read_sync_mode(adis, &iio_adis->sync_mode);
}

/**
 * @brief API to be called after trigger is disabled.
 * @param dev  - The iio device structure.
 * @param mask - The active channels mask.
 * @return 0 in case of success, error code otherwise.
 */
int adis_iio_post_disable(void* dev, uint32_t mask)
{
	struct adis_iio_dev *iio_adis;
	struct adis_dev *adis;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	if (iio_adis->has_fifo)
		return adis_write_fifo_en(adis, 0);

	return 0;
}

/**
 * @brief API to be called to get one single sample-set based on the given mask.
 * @param iio_adis - The iio adis structure.
 * @param mask     - The active channels mask.
 * @param buffer   - IIO buffer to push the sample set to.
 * @return 0 in case of success, error code otherwise.
 */
static int adis_iio_trigger_push_single_sample(struct adis_iio_dev *iio_adis,
		uint32_t mask, struct iio_buffer *buffer, bool pop)
{
	struct adis_dev *adis;
	int ret;
	struct adis_burst_data data;
	uint8_t i = 0;
	uint32_t res1;
	uint32_t res2;
	uint8_t chan;

	adis = iio_adis->adis_dev;

	ret = adis_read_burst_data(adis, &data, iio_adis->burst_size,
				   iio_adis->burst_sel, pop, false);

	/* If ret ==  EAGAIN then no data is available to read (will happen
	for a burst request or in case burst32 or burst select has been changed) */
	if (ret == -EAGAIN)
		return 0;

	if (ret)
		return ret;

	uint32_t current_data_cntr = data.data_cntr_lsb | data.data_cntr_msb << 16;

	if (iio_adis->data_cntr) {
		if(current_data_cntr > iio_adis->data_cntr) {
			if (iio_adis->sync_mode != ADIS_SYNC_SCALED)
				iio_adis->samples_lost += current_data_cntr - iio_adis->data_cntr - 1;
			else {
				res1 = (current_data_cntr - iio_adis->data_cntr) * 49;
				res2 = NO_OS_DIV_ROUND_CLOSEST(1000000, iio_adis->sampling_frequency);

				if(res1 > res2) {
					iio_adis->samples_lost += res1 / res2;
					if(res1 % res2 < res2 / 2)
						iio_adis->samples_lost--;
				}
			}

		} else if (current_data_cntr == iio_adis->data_cntr) {
			/* No new data, nothing else to do */
			return 0;
		}

		else { /* data counter overflowed occurred */
			if (iio_adis->sync_mode != ADIS_SYNC_SCALED)
				iio_adis->samples_lost += NO_OS_U16_MAX - iio_adis->data_cntr +
							  current_data_cntr;
		}
	}

	iio_adis->data_cntr = current_data_cntr;

	for (chan = 0; chan < ADIS_NUM_CHAN; chan++) {
		if (mask & (1 << chan)) {
			switch(chan) {
			case ADIS_TEMP:

				if (iio_adis->iio_dev->channels[chan].scan_type->storagebits == 32)
					iio_adis->data[i++] = data.temp_msb;

				iio_adis->data[i++] = data.temp_lsb;
				/*
				 * The temperature channel has 16-bit storage size.
				 * We need to perform the padding to have the buffer
				 * elements naturally aligned in case there are any
				 * 32-bit storage size channels enabled which have a
				 * scan index higher than the temperature channel scan
				 * index.
				 */
				if (mask & NO_OS_GENMASK(ADIS_DELTA_VEL_Z, ADIS_DELTA_ANGL_X)
				    && iio_adis->iio_dev->channels[chan].scan_type->storagebits == 16)
					iio_adis->data[i++] = 0;
				break;
			case ADIS_GYRO_X:
				if(iio_adis->burst_sel) {
					iio_adis->data[i++] = 0;
					iio_adis->data[i++] = 0;
				} else {
					/* upper 16 */
					iio_adis->data[i++] = data.x_gyro_msb;
					/* lower 16 */
					iio_adis->data[i++] =  data.x_gyro_lsb;
				}
				break;
			case ADIS_GYRO_Y:
				if(iio_adis->burst_sel) {
					iio_adis->data[i++] = 0;
					iio_adis->data[i++] = 0;
				} else {
					/* upper 16 */
					iio_adis->data[i++] = data.y_gyro_msb;
					/* lower 16 */
					iio_adis->data[i++] =  data.y_gyro_lsb;
				}
				break;
			case ADIS_GYRO_Z:
				if(iio_adis->burst_sel) {
					iio_adis->data[i++] = 0;
					iio_adis->data[i++] = 0;
				} else {
					/* upper 16 */
					iio_adis->data[i++] = data.z_gyro_msb;
					/* lower 16 */
					iio_adis->data[i++] =  data.z_gyro_lsb;
				}
				break;
			case ADIS_ACCEL_X:
				if(iio_adis->burst_sel) {
					iio_adis->data[i++] = 0;
					iio_adis->data[i++] = 0;
				} else {
					/* upper 16 */
					iio_adis->data[i++] = data.x_accel_msb;
					/* lower 16 */
					iio_adis->data[i++] =  data.x_accel_lsb;
				}
				break;
			case ADIS_ACCEL_Y:
				if(iio_adis->burst_sel) {
					iio_adis->data[i++] = 0;
					iio_adis->data[i++] = 0;
				} else {
					/* upper 16 */
					iio_adis->data[i++] = data.y_accel_msb;
					/* lower 16 */
					iio_adis->data[i++] =  data.y_accel_lsb;
				}
				break;
			case ADIS_ACCEL_Z:
				if(iio_adis->burst_sel) {
					iio_adis->data[i++] = 0;
					iio_adis->data[i++] = 0;
				} else {
					/* upper 16 */
					iio_adis->data[i++] = data.z_accel_msb;
					/* lower 16 */
					iio_adis->data[i++] =  data.z_accel_lsb;
				}
				break;
			case ADIS_DELTA_ANGL_X:
				if(!iio_adis->burst_sel) {
					iio_adis->data[i++] = 0;
					iio_adis->data[i++] = 0;
				} else {
					/* upper 16 */
					iio_adis->data[i++] = data.x_gyro_msb;
					/* lower 16 */
					iio_adis->data[i++] =  data.x_gyro_lsb;
				}
				break;
			case ADIS_DELTA_ANGL_Y:
				if(!iio_adis->burst_sel) {
					iio_adis->data[i++] = 0;
					iio_adis->data[i++] = 0;
				} else {
					/* upper 16 */
					iio_adis->data[i++] = data.y_gyro_msb;
					/* lower 16 */
					iio_adis->data[i++] =  data.y_gyro_lsb;
				}
				break;
			case ADIS_DELTA_ANGL_Z:
				if(!iio_adis->burst_sel) {
					iio_adis->data[i++] = 0;
					iio_adis->data[i++] = 0;
				} else {
					/* upper 16 */
					iio_adis->data[i++] = data.z_gyro_msb;
					/* lower 16 */
					iio_adis->data[i++] =  data.z_gyro_lsb;
				}
				break;
			case ADIS_DELTA_VEL_X:
				if(!iio_adis->burst_sel) {
					iio_adis->data[i++] = 0;
					iio_adis->data[i++] = 0;
				} else {
					/* upper 16 */
					iio_adis->data[i++] = data.x_accel_msb;
					/* lower 16 */
					iio_adis->data[i++] =  data.x_accel_lsb;
				}
				break;
			case ADIS_DELTA_VEL_Y:
				if(!iio_adis->burst_sel) {
					iio_adis->data[i++] = 0;
					iio_adis->data[i++] = 0;
				} else {
					/* upper 16 */
					iio_adis->data[i++] = data.y_accel_msb;
					/* lower 16 */
					iio_adis->data[i++] =  data.y_accel_lsb;
				}
				break;
			case ADIS_DELTA_VEL_Z:
				if(!iio_adis->burst_sel) {
					iio_adis->data[i++] = 0;
					iio_adis->data[i++] = 0;
				} else {
					/* upper 16 */
					iio_adis->data[i++] = data.z_accel_msb;
					/* lower 16 */
					iio_adis->data[i++] =  data.z_accel_lsb;
				}
				break;
			default:
				break;
			}
		}
	}

	return iio_buffer_push_scan(buffer, &iio_adis->data[0]);
}

/**
 * @brief Handles trigger: reads one data-set and writes it to the buffer.
 * @param dev_data  - The iio device data structure.
 * @return the size of the written data in case of success, error code otherwise.
 */
int adis_iio_trigger_handler(struct iio_device_data *dev_data)
{
	struct adis_iio_dev *iio_adis;

	if (!dev_data)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev_data->dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	return adis_iio_trigger_push_single_sample(iio_adis,
			dev_data->buffer->active_mask, dev_data->buffer, false);
}

/**
 * @brief Handles trigger: reads all available samples in FIFO and writes them
 *                         to the buffer.
 * @param dev_data  - The iio device data structure.
 * @return the size of the written data in case of success, error code otherwise.
 */
int adis_iio_trigger_handler_with_fifo(struct iio_device_data *dev_data)
{
	struct adis_iio_dev *iio_adis;
	struct adis_dev *adis;
	int ret;
	uint32_t fifo_cnt;
	uint16_t j;

	if (!dev_data)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev_data->dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	iio_trig_disable(iio_adis->hw_trig_desc);

	adis = iio_adis->adis_dev;

	ret = adis_read_fifo_cnt(adis, &fifo_cnt);
	if (ret)
		goto trig_enable;

	/* From data-sheet, minimum time between reads */
	no_os_udelay(10);
	if (fifo_cnt > dev_data->buffer->samples)
		fifo_cnt = dev_data->buffer->samples;

	if (fifo_cnt > 2) {
		/* Burst request */
		ret = adis_iio_trigger_push_single_sample(iio_adis,
				dev_data->buffer->active_mask, dev_data->buffer, true);
		if (ret)
			goto trig_enable;

		/* From data-sheet, minimum time between reads */
		no_os_udelay(10);

		for (j = 0; j < fifo_cnt - 1; j++) {
			ret = adis_iio_trigger_push_single_sample(iio_adis,
					dev_data->buffer->active_mask, dev_data->buffer, true);
			if (ret)
				goto trig_enable;

			/* From data-sheet, minimum time between reads */
			no_os_udelay(10);
		}
		ret = adis_iio_trigger_push_single_sample(iio_adis,
				dev_data->buffer->active_mask, dev_data->buffer, false);
		/* From data-sheet, minimum time between reads */
		no_os_udelay(10);
	}

trig_enable:
	iio_trig_enable(iio_adis->hw_trig_desc);
	return ret;
}

struct iio_attribute adis_dev_attrs[] = {
	{
		.name   = "filter_low_pass_3db_frequency",
		.shared = IIO_SHARED_BY_ALL,
		.show   = adis_iio_read_lpf,
		.store  = adis_iio_write_lpf,
	},
	{
		.name   = "sampling_frequency",
		.shared = IIO_SHARED_BY_ALL,
		.show   = adis_iio_read_sampling_freq,
		.store  = adis_iio_write_sampling_freq,
	},
	END_ATTRIBUTES_ARRAY
};

struct iio_attribute adis_iio_anglvel_attrs[] = {
	{
		.name  = "calibbias",
		.show  = adis_iio_read_calibbias,
		.store = adis_iio_write_calibbias,
	},
	{
		.name = "raw",
		.show = adis_iio_read_raw,
	},
	{
		.name   = "scale",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adis_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

struct iio_attribute adis_iio_accel_attrs[] = {
	{
		.name  = "calibbias",
		.show  = adis_iio_read_calibbias,
		.store = adis_iio_write_calibbias,
	},
	{
		.name = "raw",
		.show = adis_iio_read_raw,
	},
	{
		.name   = "scale",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adis_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

struct iio_attribute adis_iio_temp_attrs[] = {
	{
		.name = "raw",
		.show = adis_iio_read_raw,
	},
	{
		.name = "scale",
		.show = adis_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

struct iio_attribute adis_iio_delta_angl_attrs[] = {
	{
		.name = "raw",
		.show = adis_iio_read_raw,
	},
	{
		.name   = "scale",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adis_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};

struct iio_attribute adis_iio_delta_vel_attrs[] = {
	{
		.name = "raw",
		.show = adis_iio_read_raw,
	},
	{
		.name   = "scale",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adis_iio_read_scale,
	},
	END_ATTRIBUTES_ARRAY
};
