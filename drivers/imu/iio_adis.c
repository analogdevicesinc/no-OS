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

#define MAX_TXT_SIZE 50

static const char * const rang_mdl_txt[] = {
	"+/-125_degrees_per_sec",
	"+/-500_degrees_per_sec",
	"+/-2000_degrees_per_sec",
};

static const unsigned int adis_3db_freqs[] = {
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
	DIAG_CHECKSUM_ERR,
	DIAG_FLS_MEM_WR_CNT_EXCEED,
	DIAG_ACCL_SELF_TEST_ERR,
	DIAG_GYRO2_SELF_TEST_ERR,
	DIAG_GYRO1_SELF_TEST_ERR,
	DIAG_CLOCK_ERR,
	DIAG_FLS_MEM_TEST_ERR,
	DIAG_SNSR_SELF_TEST_ERR,
	DIAG_STANDBY_MODE,
	DIAG_SPI_COMM_ERR,
	DIAG_FLS_MEM_UPDATE_ERR,
	DIAG_DATA_PATH_OVERRUN,

	TIME_STAMP,
	DATA_CNTR,

	FILT_CTRL,
	RANG_MDL,

	BURST_SIZE_SEL,
	BURST_DATA_SEL,
	LINEAR_ACCL_COMP,
	PT_OF_PERC_ALGNMT,
	SENS_BW,
	SYNC_MODE,
	SYNC_POL,
	DR_POL,

	UP_SCALE,
	DEC_RATE,

	SOFTWARE_RESET_CMD,
	FLASH_MEM_TEST_CMD,
	FLASH_UPDATE_CMD,
	SENSOR_SELF_TEST_CMD,
	FACT_CALIB_RESTORE_CMD,

	FIRM_REV,
	FIRM_DATE,
	PRODUCT_ID,
	SERIAL_NUMBER,
	SCR_PAD_REG1,
	SCR_PAD_REG2,
	SCR_PAD_REG3,
	FLASH_COUNT,
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

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
	int res;

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

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t*)&res);
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
	int calibbias;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, (int32_t*)&calibbias, NULL);
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
	unsigned int filt_ctrl;
	int ret;
	int res;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	ret = adis_read_filt_ctrl(adis, &filt_ctrl);
	if (ret)
		return ret;

	res = adis_3db_freqs[filt_ctrl];

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t*)&res);
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
	unsigned int filt_ctrl;
	int i = NO_OS_ARRAY_SIZE(adis_3db_freqs);
	int ret;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	ret = iio_parse_value(buf, IIO_VAL_INT, (int32_t*)&filt_ctrl, NULL);
	if (ret)
		return ret;

	while (--i) {
		if (adis_3db_freqs[i] >= filt_ctrl)
			break;
	}

	return adis_write_filt_ctrl(adis, i);
}

/**
 * @brief Reads sync scale and decimation rate and computes the device sampling frequency in Hz.
 * @param dev  - The iio device structure.
 * @param freq - The sampling frequency of the device.
 * @return 0 in case of success, error code otherwise.
 */
static int adis_iio_get_freq(struct adis_dev* adis, unsigned int *freq)
{
	unsigned int sample_rate;
	unsigned int up_scale;
	unsigned int dec_rate;
	int ret;

	sample_rate = adis->clk_freq;
	if(adis->sync_mode == ADIS_SYNC_SCALED) {
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
	unsigned int freq;
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

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t*)&freq);
}

/**
 * @brief Sets sync scale and decimation rate based on the desired sampling frequency in Hz.
 * @param dev  - The iio device structure.
 * @param freq - The desired sampling frequency of the device.
 * @return 0 in case of success, error code otherwise.
 */
static int adis_iio_set_freq(struct adis_dev* adis, unsigned int freq)
{
	uint32_t scaled_rate;
	unsigned int up_scale;
	int ret;
	unsigned int sample_rate = adis->clk_freq;
	unsigned int dec_rate;

	if (adis->sync_mode == ADIS_SYNC_SCALED) {
		scaled_rate = no_os_lowest_common_multiple(adis->clk_freq, freq);

		/*
		 * If lcm is bigger than the IMU maximum sampling rate there's no perfect
		 * solution. In this case, we get the highest multiple of the input clock
		 * lower than the IMU max sample rate.
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

	if (dec_rate > adis->data->dec_rate_max)
		dec_rate = adis->data->dec_rate_max;

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
	unsigned int freq;
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

	return adis_iio_set_freq(adis, freq);
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

	switch (channel->type) {
	case IIO_ANGL_VEL:
		vals[0] = 1;
		vals[1] = RAD_TO_DEGREE(40 << 16);
		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, (int32_t*)vals);
	case IIO_ACCEL:
		vals[0] = 78;
		vals[1] = 32000 << 16;
		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL, 2, (int32_t*)vals);
	case IIO_ROT:
		vals[0] = 720;
		vals[1] = 31;
		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2, (int32_t*)vals);
	case IIO_VELOCITY:
		vals[0] = 100;
		vals[1] = 31;
		return iio_format_value(buf, len, IIO_VAL_FRACTIONAL_LOG2, 2, (int32_t*)vals);
	case IIO_TEMP:
		vals[0] = 100;
		return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t*)vals);
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Reads firmware date and returns it in char format.
 * @param adis - The adis device.
 * @param buf  - The read firmware date in char format.
 * @param size - The size of buf.
 * @return the size of the written data in buf in case of success, error code otherwise.
 */
static int adis_read_fw_date(struct adis_dev* adis, char *buf, uint8_t size)
{
	unsigned int firm_dm;
	unsigned int firm_y;
	int ret;

	ret = adis_read_firm_dm(adis, &firm_dm);
	if (ret)
		return ret;
	ret = adis_read_firm_y(adis, &firm_y);
	if (ret)
		return ret;

	return snprintf(buf, size, "%.2x-%.2x-%.4x", firm_dm >> 8, firm_dm & 0xff,
			firm_y);
}

/**
 * @brief Reads firmware revision and returns it in char format.
 * @param adis - The adis device.
 * @param buf  - The read firmware revision in char format.
 * @param size - The size of buf.
 * @return the size of the written data in buf in case of success, error code otherwise.
 */
static int adis_read_fw_rev(struct adis_dev* adis, char *buf, uint8_t size)
{
	unsigned int firm_rev;
	int ret;

	ret = adis_read_firm_rev(adis, &firm_rev);
	if (ret)
		return ret;

	return snprintf(buf, size, "%d.%d", firm_rev >> 8, firm_rev & 0xff);
}

/**
 * @brief Reads gyroscope measurement range value and returns it in char format.
 * @param adis - The adis device.
 * @param buf  - The gyroscope measurement range value in char format.
 * @return the size of the written data in buf in case of success, error code otherwise.
 */
int iio_adis_read_rang_mdl(struct adis_dev* adis, char *buf)
{
	int ret;
	unsigned int rang_mdl;
	ret = adis_read_rang_mdl(adis, &rang_mdl);
	if (ret)
		return ret;

	return snprintf(buf, strlen(rang_mdl_txt[rang_mdl]) + 1, "%s\n",
			rang_mdl_txt[rang_mdl]);
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
	unsigned int res;
	int ret = 0;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	switch(priv) {
	case DIAG_CHECKSUM_ERR:
		adis_read_diag_checksum_err(adis, &res);
		break;
	case DIAG_FLS_MEM_WR_CNT_EXCEED:
		adis_read_diag_fls_mem_wr_cnt_exceed(adis, &res);
		break;
	case DIAG_ACCL_SELF_TEST_ERR:
		ret = adis_read_diag_accl_self_test_err(adis, &res);
		break;
	case DIAG_GYRO2_SELF_TEST_ERR:
		ret = adis_read_diag_gyro2_self_test_err(adis, &res);
		break;
	case DIAG_GYRO1_SELF_TEST_ERR:
		ret = adis_read_diag_gyro1_self_test_err(adis, &res);
		break;
	case DIAG_CLOCK_ERR:
		ret = adis_read_diag_clock_err(adis, &res);
		break;
	case DIAG_FLS_MEM_TEST_ERR:
		ret = adis_read_diag_fls_mem_test_err(adis, &res);
		break;
	case DIAG_SNSR_SELF_TEST_ERR:
		ret = adis_read_diag_snsr_self_test_err(adis, &res);
		break;
	case DIAG_STANDBY_MODE:
		ret = adis_read_diag_standby_mode(adis, &res);
		break;
	case DIAG_SPI_COMM_ERR:
		ret = adis_read_diag_spi_comm_err(adis, &res);
		break;
	case DIAG_FLS_MEM_UPDATE_ERR:
		ret = adis_read_diag_fls_mem_update_err(adis, &res);
		break;
	case DIAG_DATA_PATH_OVERRUN:
		ret = adis_read_diag_data_path_overrun(adis, &res);
		break;
	case TIME_STAMP:
		ret = adis_read_time_stamp(adis, &res);
		break;
	case DATA_CNTR:
		ret = adis_read_data_cntr(adis, &res);
		break;
	case FILT_CTRL:
		ret = adis_read_filt_ctrl(adis, &res);
		break;
	case RANG_MDL:
		return iio_adis_read_rang_mdl(adis, buf);
	case BURST_SIZE_SEL:
		ret = adis_read_burst_size(adis, &res);
		break;
	case BURST_DATA_SEL:
		ret = adis_read_burst_sel(adis, &res);
		break;
	case LINEAR_ACCL_COMP:
		ret = adis_read_linear_accl_comp(adis, &res);
		break;
	case PT_OF_PERC_ALGNMT:
		ret = adis_read_pt_of_perc_algnmt(adis, &res);
		break;
	case SENS_BW:
		ret = adis_read_sens_bw(adis, &res);
		break;
	case SYNC_MODE:
		ret = adis_read_sync_mode(adis, &res);
		break;
	case SYNC_POL:
		ret = adis_read_sync_polarity(adis, &res);
		break;
	case DR_POL:
		ret = adis_read_dr_polarity(adis, &res);
		break;
	case UP_SCALE:
		ret = adis_read_up_scale(adis, &res);
		break;
	case DEC_RATE:
		ret = adis_read_dec_rate(adis, &res);
		break;
	case FIRM_REV:
		return adis_read_fw_rev(adis, buf, 7);
	case FIRM_DATE:
		return adis_read_fw_date(adis, buf, 12);
	case PRODUCT_ID:
		ret = adis_read_prod_id(adis, &res);
		break;
	case SERIAL_NUMBER:
		ret = adis_read_serial_num(adis, &res);
		break;
	case SCR_PAD_REG1:
		ret = adis_read_usr_scr_1(adis, &res);
		break;
	case SCR_PAD_REG2:
		ret = adis_read_usr_scr_2(adis, &res);
		break;
	case SCR_PAD_REG3:
		ret = adis_read_usr_scr_3(adis, &res);
		break;
	case FLASH_COUNT:
		ret = adis_read_flshcnt(adis, &res);
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
	unsigned int val;
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
	case FILT_CTRL:
		return adis_write_filt_ctrl(adis, val);
	case BURST_SIZE_SEL:
		return adis_write_burst_size(adis, val);
	case BURST_DATA_SEL:
		return adis_write_burst_sel(adis, val);
	case LINEAR_ACCL_COMP:
		return adis_write_linear_accl_comp(adis, val);
	case PT_OF_PERC_ALGNMT:
		return adis_write_pt_of_perc_algnmt(adis, val);
	case SENS_BW:
		return adis_write_sens_bw(adis, val);
	case SYNC_MODE:
		return adis_write_sync_mode(adis, val);
	case SYNC_POL:
		return adis_write_sync_polarity(adis, val);
	case DR_POL:
		return adis_write_dr_polarity(adis, val);
	case UP_SCALE:
		return adis_write_up_scale(adis, val);
	case DEC_RATE:
		return adis_write_dec_rate(adis, val);
	case SOFTWARE_RESET_CMD:
		return adis_cmd_sw_res(adis);
	case FLASH_MEM_TEST_CMD:
		return adis_cmd_fls_mem_test(adis);
	case FLASH_UPDATE_CMD:
		return adis_cmd_fls_mem_update(adis);
	case SENSOR_SELF_TEST_CMD:
		return adis_cmd_snsr_self_test(adis);
	case FACT_CALIB_RESTORE_CMD:
		return adis_cmd_fact_calib_restore(adis);
	case SCR_PAD_REG1:
		return adis_write_usr_scr_1(adis, val);
	case SCR_PAD_REG2:
		return adis_write_usr_scr_2(adis, val);
	case SCR_PAD_REG3:
		return adis_write_usr_scr_3(adis, val);
	default:
		return -EINVAL;
	}
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
static int adis_iio_read_raw(void *dev, char *buf, uint32_t len,
			     const struct iio_ch_info *channel, intptr_t priv)
{
	struct adis_iio_dev *iio_adis;
	struct adis_dev *adis;
	int res;
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

	return iio_format_value(buf, len, IIO_VAL_INT, 1, (int32_t*)&res);
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

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	iio_adis->active_channels = mask;
	iio_adis->no_of_active_channels = no_os_hweight32(mask);

	return 0;
}

/**
 * @brief Reads the number of given samples for the active channels.
 * @param dev     - The iio device structure.
 * @param samples - The number of samples to be read.
 * @param buff    - The buffer to be filled with the requested number of samples.
 * @return the size of the buffer in case of success, error code otherwise.
 */
int adis_iio_read_samples(void* dev, int* buff, uint32_t samples)
{
	struct adis_iio_dev *iio_adis;
	struct adis_dev *adis;
	struct adis_burst_data burst_data;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	for(uint32_t i = 0; i < samples*iio_adis->no_of_active_channels;) {
		ret = adis_read_burst_data(adis, &burst_data);
		if (ret)
			return ret;

		if (iio_adis->active_channels & NO_OS_BIT(ADIS_GYRO_X))
			buff[i++] = adis->burst_sel ? 0 : burst_data.x_gyro;
		if (iio_adis->active_channels & NO_OS_BIT(ADIS_GYRO_Y))
			buff[i++] = adis->burst_sel ? 0 : burst_data.y_gyro;
		if (iio_adis->active_channels & NO_OS_BIT(ADIS_GYRO_Z))
			buff[i++] = adis->burst_sel ? 0 : burst_data.z_gyro;
		if (iio_adis->active_channels & NO_OS_BIT(ADIS_ACCEL_X))
			buff[i++] = adis->burst_sel ? 0 : burst_data.x_accl;
		if (iio_adis->active_channels & NO_OS_BIT(ADIS_ACCEL_Y))
			buff[i++] = adis->burst_sel ? 0 : burst_data.y_accl;
		if (iio_adis->active_channels & NO_OS_BIT(ADIS_ACCEL_Z))
			buff[i++] = adis->burst_sel ? 0 : burst_data.z_accl;
		if (iio_adis->active_channels & NO_OS_BIT(ADIS_TEMP))
			buff[i++] = burst_data.temp_out;
		if (iio_adis->active_channels & NO_OS_BIT(ADIS_DELTA_ANGL_X))
			buff[i++] = adis->burst_sel ?  burst_data.x_gyro : 0;
		if (iio_adis->active_channels & NO_OS_BIT(ADIS_DELTA_ANGL_Y))
			buff[i++] = adis->burst_sel ? burst_data.y_gyro : 0;
		if (iio_adis->active_channels & NO_OS_BIT(ADIS_DELTA_ANGL_Z))
			buff[i++] = adis->burst_sel ? burst_data.z_gyro : 0;
		if (iio_adis->active_channels & NO_OS_BIT(ADIS_DELTA_VEL_X))
			buff[i++] = adis->burst_sel ? burst_data.x_accl : 0;
		if (iio_adis->active_channels & NO_OS_BIT(ADIS_DELTA_VEL_Y))
			buff[i++] = adis->burst_sel ? burst_data.y_accl : 0;
		if (iio_adis->active_channels & NO_OS_BIT(ADIS_DELTA_VEL_Z))
			buff[i++] = adis->burst_sel ? burst_data.z_accl : 0;
	}

	return samples;
}

/**
 * @brief Handles trigger: reads one data-set and writes it to the buffer.
 * @param dev_data  - The iio device data structure.
 * @return the size of the written data in case of success, error code otherwise.
 */
int adis_iio_trigger_handler(struct iio_device_data *dev_data)
{
	int buff[13];
	uint8_t i = 0;

	struct adis_iio_dev *iio_adis;
	struct adis_dev *adis;
	struct adis_burst_data burst_data;
	int ret;

	if (!dev_data)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev_data->dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	ret = adis_read_burst_data(adis, &burst_data);
	if (ret)
		return ret;

	if (dev_data->buffer->active_mask & NO_OS_BIT(ADIS_GYRO_X))
		buff[i++] = adis->burst_sel ? 0 : burst_data.x_gyro;
	if (dev_data->buffer->active_mask & NO_OS_BIT(ADIS_GYRO_Y))
		buff[i++] = adis->burst_sel ? 0 : burst_data.y_gyro;
	if (dev_data->buffer->active_mask & NO_OS_BIT(ADIS_GYRO_Z))
		buff[i++] = adis->burst_sel ? 0 : burst_data.z_gyro;
	if (dev_data->buffer->active_mask & NO_OS_BIT(ADIS_ACCEL_X))
		buff[i++] = adis->burst_sel ? 0 : burst_data.x_accl;
	if (dev_data->buffer->active_mask & NO_OS_BIT(ADIS_ACCEL_Y))
		buff[i++] = adis->burst_sel ? 0 : burst_data.y_accl;
	if (dev_data->buffer->active_mask & NO_OS_BIT(ADIS_ACCEL_Z))
		buff[i++] = adis->burst_sel ? 0 : burst_data.z_accl;
	if (dev_data->buffer->active_mask & NO_OS_BIT(ADIS_TEMP))
		buff[i++] = burst_data.temp_out;
	if (dev_data->buffer->active_mask & NO_OS_BIT(ADIS_DELTA_ANGL_X))
		buff[i++] = adis->burst_sel ? burst_data.x_gyro : 0;
	if (dev_data->buffer->active_mask & NO_OS_BIT(ADIS_DELTA_ANGL_Y))
		buff[i++] = adis->burst_sel ? burst_data.y_gyro : 0;
	if (dev_data->buffer->active_mask & NO_OS_BIT(ADIS_DELTA_ANGL_Z))
		buff[i++] = adis->burst_sel ? burst_data.z_gyro : 0;
	if (dev_data->buffer->active_mask & NO_OS_BIT(ADIS_DELTA_VEL_X))
		buff[i++] = adis->burst_sel ? burst_data.x_accl : 0;
	if (dev_data->buffer->active_mask & NO_OS_BIT(ADIS_DELTA_VEL_Y))
		buff[i++] = adis->burst_sel ? burst_data.y_accl : 0;
	if (dev_data->buffer->active_mask & NO_OS_BIT(ADIS_DELTA_VEL_Z))
		buff[i++] = adis->burst_sel ? burst_data.z_accl : 0;

	return iio_buffer_push_scan(dev_data->buffer, &buff[0]);
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
		.name = "diag_checksum_error_flag",
		.show = adis_iio_read_debug_attrs,
		.priv = DIAG_CHECKSUM_ERR,
	},
	{
		.name = "diag_flash_memory_write_count_exceeded_error",
		.show = adis_iio_read_debug_attrs,
		.priv = DIAG_FLS_MEM_WR_CNT_EXCEED,
	},
	{
		.name = "diag_acceleration_self_test_error",
		.show = adis_iio_read_debug_attrs,
		.priv = DIAG_ACCL_SELF_TEST_ERR,
	},
	{
		.name = "diag_gyroscope2_self_test_error",
		.show = adis_iio_read_debug_attrs,
		.priv = DIAG_GYRO2_SELF_TEST_ERR,
	},
	{
		.name = "diag_gyroscope1_self_test_error",
		.show = adis_iio_read_debug_attrs,
		.priv = DIAG_GYRO1_SELF_TEST_ERR,
	},
	{
		.name = "diag_clock_error",
		.show = adis_iio_read_debug_attrs,
		.priv = DIAG_CLOCK_ERR,
	},
	{
		.name = "diag_flash_memory_test_error",
		.show = adis_iio_read_debug_attrs,
		.priv = DIAG_FLS_MEM_TEST_ERR,
	},
	{
		.name = "diag_sensor_self_test_error",
		.show = adis_iio_read_debug_attrs,
		.priv = DIAG_SNSR_SELF_TEST_ERR,
	},
	{
		.name = "diag_standby_mode",
		.show = adis_iio_read_debug_attrs,
		.priv = DIAG_STANDBY_MODE,
	},
	{
		.name = "diag_spi_communication_error",
		.show = adis_iio_read_debug_attrs,
		.priv = DIAG_SPI_COMM_ERR,
	},
	{
		.name = "diag_flash_memory_update_error",
		.show = adis_iio_read_debug_attrs,
		.priv = DIAG_FLS_MEM_UPDATE_ERR,
	},
	{
		.name = "diag_data_path_overrun",
		.show = adis_iio_read_debug_attrs,
		.priv = DIAG_DATA_PATH_OVERRUN,
	},
	{
		.name = "time_stamp",
		.show = adis_iio_read_debug_attrs,
		.priv = TIME_STAMP,
	},
	{
		.name = "data_counter",
		.show = adis_iio_read_debug_attrs,
		.priv = DATA_CNTR,
	},
	{
		.name = "filter_size",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = FILT_CTRL,
	},
	{
		.name = "gyroscope_measurement_range",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = RANG_MDL,
	},
	{
		.name = "burst_size_selection",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = BURST_SIZE_SEL,
	},
	{
		.name = "burst_data_selection",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = BURST_DATA_SEL,
	},
	{
		.name = "linear_acceleration_compensation",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = LINEAR_ACCL_COMP,
	},
	{
		.name = "point_of_percussion_alignment",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = PT_OF_PERC_ALGNMT,
	},
	{
		.name = "internal_sensor_bandwidth",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = SENS_BW,
	},
	{
		.name = "sync_mode_select",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = SYNC_MODE,
	},
	{
		.name = "sync_polarity",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = SYNC_POL,
	},
	{
		.name = "data_ready_polarity",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = DR_POL,
	},
	{
		.name = "sync_signal_scale",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = UP_SCALE,
	},
	{
		.name = "decimation_filter",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = DEC_RATE,
	},
	{
		.name = "software_reset",
		.store = adis_iio_write_debug_attrs,
		.priv = SOFTWARE_RESET_CMD,
	},
	{
		.name = "flash_memory_test",
		.store = adis_iio_write_debug_attrs,
		.priv = FLASH_MEM_TEST_CMD,
	},
	{
		.name = "flash_memory_update",
		.store = adis_iio_write_debug_attrs,
		.priv = FLASH_UPDATE_CMD,
	},
	{
		.name = "sensor_self_test",
		.store = adis_iio_write_debug_attrs,
		.priv = SENSOR_SELF_TEST_CMD,
	},
	{
		.name = "factory_calibration_restore",
		.store = adis_iio_write_debug_attrs,
		.priv = FACT_CALIB_RESTORE_CMD,
	},
	{
		.name = "firmware_revision",
		.show = adis_iio_read_debug_attrs,
		.priv = FIRM_REV,
	},
	{
		.name = "firmware_date",
		.show = adis_iio_read_debug_attrs,
		.priv = FIRM_DATE,
	},
	{
		.name = "product_id",
		.show = adis_iio_read_debug_attrs,
		.priv = PRODUCT_ID,
	},
	{
		.name = "serial_number",
		.show = adis_iio_read_debug_attrs,
		.priv = SERIAL_NUMBER,
	},
	{
		.name = "scratch_pad_register1",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = SCR_PAD_REG1,
	},
	{
		.name = "scratch_pad_register2",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = SCR_PAD_REG2,
	},
	{
		.name = "scratch_pad_register3",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = SCR_PAD_REG3,
	},
	{
		.name = "flash_counter",
		.show = adis_iio_read_debug_attrs,
		.priv = FLASH_COUNT,
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
