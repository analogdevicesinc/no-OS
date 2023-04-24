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

#include "iio_adis.h"
#include "no_os_units.h"
#include <stdio.h>
#include <string.h>
#include "adis.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

static const uint32_t adis_3db_freqs[] = {
	720, /* Filter disabled, full BW (~720Hz) */
	360,
	164,
	80,
	40,
	20,
	10,
};

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

enum {
	ADIS_DIAG_DATA_PATH_OVERRUN,
	ADIS_DIAG_FLS_MEM_UPDATE_FAILURE,
	ADIS_DIAG_SPI_COMM_ERR,
	ADIS_DIAG_STANDBY_MODE,
	ADIS_DIAG_SNSR_FAILURE,
	ADIS_DIAG_MEM_FAILURE,
	ADIS_DIAG_CLK_ERR,
	ADIS_DIAG_GYRO1_FAILURE,
	ADIS_DIAG_GYRO2_FAILURE,
	ADIS_DIAG_ACCL_FAILURE,
	ADIS_DIAG_CHECKSUM_ERR,
	ADIS_DIAG_FLS_MEM_WR_CNT_EXCEED,
	ADIS_DIAG_LOST_SAMPLES_COUNT,

	ADIS_TIME_STAMP,
	ADIS_DATA_CNTR,

	ADIS_FILT_SIZE_VAR_B,
	ADIS_GYRO_MEAS_RANGE,

	ADIS_DR_POLARITY,
	ADIS_SYNC_POLARITY,
	ADIS_SYNC_MODE,
	ADIS_SENS_BW,
	ADIS_PT_OF_PERC_ALGNMT,
	ADIS_LINEAR_ACCL_COMP,
	ADIS_BURST_SEL,
	ADIS_BURST32,

	ADIS_UP_SCALE,
	ADIS_DEC_RATE,

	ADIS_CMD_FACT_CALIB_RESTORE,
	ADIS_CMD_SNSR_SELF_TEST,
	ADIS_CMD_FLS_MEM_UPDATE,
	ADIS_CMD_FLS_MEM_TEST,
	ADIS_CMD_SW_RES,

	ADIS_FIRM_REV,
	ADIS_FIRM_DATE,
	ADIS_PROD_ID,
	ADIS_SERIAL_NUM,
	ADIS_USR_SCR_1,
	ADIS_USR_SCR_2,
	ADIS_USR_SCR_3,
	ADIS_FLS_MEM_WR_CNTR,
	ADIS_EXT_CLK_FREQ,
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Handles the read request for raw attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return the size of the read data in case of success, error code otherwise.
 */
static int adis_iio_read_raw(void *dev, char *buf, uint32_t len,
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

	switch(channel->ch_num) {
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
static int adis_iio_read_scale(void *dev, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t priv)
{
	uint32_t vals[2];
	struct adis_iio_dev *iio_adis;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	switch (channel->type) {
	case IIO_ANGL_VEL:
		vals[0] = iio_adis->gyro_scale.dividend;
		vals[1] = iio_adis->gyro_scale.divisor;
		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, (int32_t*)vals);
	case IIO_ACCEL:
		vals[0] = iio_adis->accl_scale.dividend;
		vals[1] = iio_adis->accl_scale.divisor;
		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, (int32_t*)vals);
	case IIO_ROT:
		vals[0] = iio_adis->rot_scale.dividend;
		vals[1] = iio_adis->rot_scale.power;
		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2, (int32_t*)vals);
	case IIO_VELOCITY:
		vals[0] = iio_adis->vel_scale.dividend;
		vals[1] = iio_adis->vel_scale.power;
		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2, (int32_t*)vals);
	case IIO_TEMP:
		vals[0] = iio_adis->temp_scale.dividend;
		vals[1] = iio_adis->temp_scale.divisor;
		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, (int32_t*)vals);
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
static int adis_iio_read_calibbias(void *dev, char *buf, uint32_t len,
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

	switch (channel->ch_num) {
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
static int adis_iio_write_calibbias(void *dev, char *buf, uint32_t len,
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

	switch (channel->ch_num) {
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
 * @brief Handles the read request for lpf attribute.
 * @param dev     - The iio device structure.
 * @param buf	  - Command buffer to be filled with requested data.
 * @param len     - Length of the received command buffer in bytes.
 * @param channel - Command channel info.
 * @param priv    - Command attribute id.
 * @return the size of the read data in case of success, error code otherwise.
 */
static int adis_iio_read_lpf(void *dev, char *buf, uint32_t len,
			     const struct iio_ch_info *channel, intptr_t priv)
{
	struct adis_iio_dev *iio_adis;
	struct adis_dev *adis;
	uint32_t filt_size_var_b;
	int ret;
	int32_t res;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	ret = adis_read_filt_size_var_b(adis, &filt_size_var_b);
	if (ret)
		return ret;

	res = adis_3db_freqs[filt_size_var_b];

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
static int adis_iio_write_lpf(void *dev, char *buf, uint32_t len,
			      const struct iio_ch_info *channel, intptr_t priv)
{
	struct adis_iio_dev *iio_adis;
	struct adis_dev *adis;
	uint32_t filt_size_var_b;
	int i = NO_OS_ARRAY_SIZE(adis_3db_freqs);
	int ret;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, (int32_t*)&filt_size_var_b, NULL);
	if (ret)
		return ret;

	while (--i) {
		if (adis_3db_freqs[i] >= filt_size_var_b)
			break;
	}

	return adis_write_filt_size_var_b(adis, i);
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
	int ret;

	sample_rate = adis->clk_freq;
	ret = adis_read_sync_mode(adis, &sync_mode);
	if (ret)
		return ret;

	if(sync_mode == ADIS_SYNC_SCALED) {
		ret = adis_read_up_scale(adis, &up_scale);
		if (ret)
			return ret;
		sample_rate = adis->clk_freq * up_scale;
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
static int adis_iio_read_sampling_freq(void *dev, char *buf, uint32_t len,
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
	uint32_t sample_rate = adis->clk_freq;
	uint32_t dec_rate;
	uint32_t sync_mode;

	ret = adis_read_sync_mode(adis, &sync_mode);
	if (ret)
		return ret;

	if(sync_mode == ADIS_SYNC_SCALED) {
		scaled_rate = no_os_lowest_common_multiple(adis->clk_freq, freq);

		/*
		 * If lcm is bigger than the IMU maximum sampling rate there's
		 * no perfect solution. In this case, we get the highest
		 * multiple of the input clock lower than the IMU max sample
		 * rate.
		 */
		if (scaled_rate > 2100)
			scaled_rate = 2100 / adis->clk_freq * adis->clk_freq;
		else
			scaled_rate = 2100 / scaled_rate * scaled_rate;

		up_scale = scaled_rate / adis->clk_freq;
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
static int adis_iio_write_sampling_freq(void *dev, char *buf,
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
static int adis_read_fw_date(struct adis_dev* adis, char *buf, uint8_t size)
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

	return snprintf(buf, size, "%.2lx-%.2lx-%.4lx", firm_d, firm_m, firm_y);
}

/**
 * @brief Reads firmware revision and returns it in char format.
 * @param adis - The adis device.
 * @param buf  - The read firmware revision in char format.
 * @param size - The size of buf.
 * @return the size of the written data in buf in case of success, error code
 *         otherwise.
 */
static int adis_read_fw_rev(struct adis_dev* adis, char *buf, uint8_t size)
{
	uint32_t firm_rev;
	int ret;

	ret = adis_read_firm_rev(adis, &firm_rev);
	if (ret)
		return ret;

	return snprintf(buf, size, "%ld.%ld", firm_rev >> 8, firm_rev & 0xff);
}

/**
 * @brief Reads gyroscope measurement range value and returns it in char format.
 * @param adis - The adis device.
 * @param buf  - The gyroscope measurement range value in char format.
 * @return the size of the written data in buf in case of success, error code
 *         otherwise.
 */
int iio_adis_read_gyro_meas_range(struct adis_iio_dev* iio_adis, char *buf)
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
static int adis_iio_read_debug_attrs(void *dev, char *buf, uint32_t len,
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
	case ADIS_FILT_SIZE_VAR_B:
		ret = adis_read_filt_size_var_b(adis, &res);
		break;
	case ADIS_GYRO_MEAS_RANGE:
		return iio_adis_read_gyro_meas_range(iio_adis, buf);
	case ADIS_DR_POLARITY:
		ret = adis_read_dr_polarity(adis, &res);
		break;
	case ADIS_SYNC_POLARITY:
		ret = adis_read_sync_polarity(adis, &res);
		break;
	case ADIS_SYNC_MODE:
		ret = adis_read_sync_mode(adis, &res);
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
	case ADIS_UP_SCALE:
		ret = adis_read_up_scale(adis, &res);
		break;
	case ADIS_DEC_RATE:
		ret = adis_read_dec_rate(adis, &res);
		break;
	case ADIS_FIRM_REV:
		return adis_read_fw_rev(adis, buf, 7);
	case ADIS_FIRM_DATE:
		return adis_read_fw_date(adis, buf, 12);
	case ADIS_PROD_ID:
		ret = adis_read_prod_id(adis, &res);
		break;
	case ADIS_SERIAL_NUM:
		ret = adis_read_serial_num(adis, &res);
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
static int adis_iio_write_debug_attrs(void *dev, char *buf, uint32_t len,
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
	case ADIS_FILT_SIZE_VAR_B:
		return adis_write_filt_size_var_b(adis, val);
	case ADIS_DR_POLARITY:
		return adis_write_dr_polarity(adis, val);
	case ADIS_SYNC_POLARITY:
		return adis_write_sync_polarity(adis, val);
	case ADIS_SYNC_MODE:
		ret = adis_write_sync_mode(adis, val, adis->ext_clk);
		if (ret)
			return ret;

		/* Update sampling frequency */
		return adis_iio_get_freq(adis, &iio_adis->sampling_frequency);
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
	case ADIS_CMD_SW_RES:
		ret = adis_cmd_sw_res(adis);
		if (ret)
			return ret;

		/* Update sampling frequency */
		return adis_iio_get_freq(adis, &iio_adis->sampling_frequency);
	case ADIS_USR_SCR_1:
		return adis_write_usr_scr_1(adis, val);
	case ADIS_USR_SCR_2:
		return adis_write_usr_scr_2(adis, val);
	case ADIS_USR_SCR_3:
		return adis_write_usr_scr_3(adis, val);
	case ADIS_EXT_CLK_FREQ:
		return adis_update_ext_clk_freq(adis, val);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Updates active channels iio adis parameter.
 * @param dev  - The iio device structure.
 * @param mask - The active channels mask.
 * @return 0 in case of success, error code otherwise.
 */
int adis_iio_update_channels(void* dev, uint32_t mask)
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

	iio_adis->samples_lost = 0;
	iio_adis->data_cntr = 0;
	ret  = adis_read_burst_sel(adis, &iio_adis->burst_sel);
	if (ret)
		return ret;

	ret = adis_read_burst32(adis, &iio_adis->burst_size);
	if (ret)
		return ret;

	return adis_read_sync_mode(adis, &iio_adis->sync_mode);
}

/**
 * @brief Handles trigger: reads one data-set and writes it to the buffer.
 * @param dev_data  - The iio device data structure.
 * @return the size of the written data in case of success, error code otherwise.
 */
int adis_iio_trigger_handler(struct iio_device_data *dev_data)
{
	uint16_t buff[15];
	uint8_t i = 0;
	uint8_t buff_idx;

	struct adis_iio_dev *iio_adis;
	struct adis_dev *adis;
	int ret;
	uint8_t temp_offset;
	uint8_t data_cntr_offset;
	uint16_t current_data_cntr;
	uint32_t res1;
	uint32_t res2;
	uint32_t mask;
	uint8_t chan;

	if (!dev_data)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev_data->dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	ret = adis_read_burst_data(adis, sizeof(buff), buff, iio_adis->burst_size);
	if (ret)
		return ret;

	temp_offset = iio_adis->burst_size ? 13 : 7;
	data_cntr_offset = iio_adis->burst_size ? 14 : 8;

	current_data_cntr = no_os_bswap_constant_16(buff[data_cntr_offset]);

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

		} else /* data counter overflowed occurred */
			if (iio_adis->sync_mode != ADIS_SYNC_SCALED)
				iio_adis->samples_lost += NO_OS_U16_MAX - iio_adis->data_cntr +
							  current_data_cntr;
	}

	iio_adis->data_cntr = current_data_cntr;

	mask = dev_data->buffer->active_mask;

	for (chan = 0; chan < ADIS_NUM_CHAN; chan++) {
		if (mask & (1 << chan)) {
			switch(chan) {
			case ADIS_TEMP:
				iio_adis->data[i++] = 0;
				iio_adis->data[i++] = buff[temp_offset];
				break;
			case ADIS_GYRO_X ... ADIS_ACCEL_Z:
				/*
				* The first 2 bytes on the received data are the
				* DIAG_STAT reg, hence the +1 offset here...
				*/
				if(iio_adis->burst_sel) {
					iio_adis->data[i++] = 0;
					iio_adis->data[i++] = 0;
				} else {
					if (iio_adis->burst_size) {
						/* upper 16 */
						iio_adis->data[i++] = buff[chan * 2 + 2];
						/* lower 16 */
						iio_adis->data[i++] = buff[chan * 2 + 1];
					} else {
						iio_adis->data[i++] = buff[chan + 1];
						/* lower not used */
						iio_adis->data[i++] = 0;
					}
				}
				break;
			case ADIS_DELTA_ANGL_X ... ADIS_DELTA_VEL_Z:
				if(!iio_adis->burst_sel) {
					iio_adis->data[i++] = 0;
					iio_adis->data[i++] = 0;
				} else {
					buff_idx = chan - ADIS_DELTA_ANGL_X;
					if (iio_adis->burst_size) {
						/* upper 16 */
						iio_adis->data[i++] = buff[buff_idx * 2 + 2];
						/* lower 16 */
						iio_adis->data[i++] = buff[buff_idx * 2 + 1];
					} else {
						iio_adis->data[i++] = buff[buff_idx + 1];
						/* lower not used */
						iio_adis->data[i++] = 0;
					}
				}
				break;
			case ADIS_DATA_COUNTER:
				iio_adis->data[i++] = 0;
				iio_adis->data[i++] = buff[data_cntr_offset];
				break;
			default:
				break;
			}
		}
	}

	return iio_buffer_push_scan(dev_data->buffer, &iio_adis->data[0]);
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

struct iio_attribute adis_debug_attrs[] = {
	{
		.name = "lost_samples_count",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_LOST_SAMPLES_COUNT,
	},
	{
		.name = "diag_checksum_error_flag",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_CHECKSUM_ERR,
	},
	{
		.name = "diag_flash_memory_write_count_exceeded_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_FLS_MEM_WR_CNT_EXCEED,
	},
	{
		.name = "diag_acceleration_self_test_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_ACCL_FAILURE,
	},
	{
		.name = "diag_gyroscope2_self_test_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_GYRO2_FAILURE,
	},
	{
		.name = "diag_gyroscope1_self_test_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_GYRO1_FAILURE,
	},
	{
		.name = "diag_clock_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_CLK_ERR,
	},
	{
		.name = "diag_flash_memory_test_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_MEM_FAILURE,
	},
	{
		.name = "diag_sensor_self_test_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_SNSR_FAILURE,
	},
	{
		.name = "diag_standby_mode",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_STANDBY_MODE,
	},
	{
		.name = "diag_spi_communication_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_SPI_COMM_ERR,
	},
	{
		.name = "diag_flash_memory_update_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_FLS_MEM_UPDATE_FAILURE,
	},
	{
		.name = "diag_data_path_overrun",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_DATA_PATH_OVERRUN,
	},
	{
		.name = "time_stamp",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_TIME_STAMP,
	},
	{
		.name = "data_counter",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DATA_CNTR,
	},
	{
		.name = "filter_size",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_FILT_SIZE_VAR_B,
	},
	{
		.name = "gyroscope_measurement_range",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_GYRO_MEAS_RANGE,
	},
	{
		.name = "burst_size_selection",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_BURST32,
	},
	{
		.name = "burst_data_selection",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_BURST_SEL,
	},
	{
		.name = "linear_acceleration_compensation",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_LINEAR_ACCL_COMP,
	},
	{
		.name = "point_of_percussion_alignment",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_PT_OF_PERC_ALGNMT,
	},
	{
		.name = "internal_sensor_bandwidth",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_SENS_BW,
	},
	{
		.name = "sync_mode_select",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_SYNC_MODE,
	},
	{
		.name = "sync_polarity",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_SYNC_POLARITY,
	},
	{
		.name = "data_ready_polarity",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_DR_POLARITY,
	},
	{
		.name = "sync_signal_scale",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_UP_SCALE,
	},
	{
		.name = "decimation_filter",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_DEC_RATE,
	},
	{
		.name = "software_reset",
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_CMD_SW_RES,
	},
	{
		.name = "flash_memory_test",
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_CMD_FLS_MEM_TEST,
	},
	{
		.name = "flash_memory_update",
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_CMD_FLS_MEM_UPDATE,
	},
	{
		.name = "sensor_self_test",
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_CMD_SNSR_SELF_TEST,
	},
	{
		.name = "factory_calibration_restore",
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_CMD_FACT_CALIB_RESTORE,
	},
	{
		.name = "firmware_revision",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_FIRM_REV,
	},
	{
		.name = "firmware_date",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_FIRM_DATE,
	},
	{
		.name = "product_id",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_PROD_ID,
	},
	{
		.name = "serial_number",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_SERIAL_NUM,
	},
	{
		.name = "scratch_pad_register1",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_USR_SCR_1,
	},
	{
		.name = "scratch_pad_register2",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_USR_SCR_2,
	},
	{
		.name = "scratch_pad_register3",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_USR_SCR_3,
	},
	{
		.name = "flash_counter",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_FLS_MEM_WR_CNTR,
	},
	{
		.name = "external_clock_frequency",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_EXT_CLK_FREQ,
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
