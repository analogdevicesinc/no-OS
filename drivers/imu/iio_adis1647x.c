/***************************************************************************//**
 *   @file   iio_adis1647x.c
 *   @brief  Implementation of iio_adis1647x.c
 *   @author RBolboac (ramona.gradinariu@analog.com)
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

#include "iio_adis1647x.h"
#include "no_os_alloc.h"
#include "no_os_units.h"

/******************************************************************************/
/************************** Variable Definitions ******************************/
/******************************************************************************/

static const char * const adis1647x_rang_mdl_txt[] = {
	[ADIS1647X_ID_NO_OFFSET(ADIS16477_1)] = "+/-125_degrees_per_sec",
	[ADIS1647X_ID_NO_OFFSET(ADIS16477_2)] = "+/-500_degrees_per_sec",
	[ADIS1647X_ID_NO_OFFSET(ADIS16477_3)] = "+/-2000_degrees_per_sec",
};

static struct scan_type adis1647x_iio_accel_scan_type = {
	.sign 		= 's',
	.realbits 	= 32,
	.storagebits 	= 32,
	.shift 		= 0,
	.is_big_endian 	= true
};

static struct scan_type adis1647x_iio_anglvel_scan_type = {
	.sign 		= 's',
	.realbits 	= 32,
	.storagebits 	= 32,
	.shift 		= 0,
	.is_big_endian 	= true
};

static struct scan_type adis1647x_iio_delta_vel_scan_type = {
	.sign 		= 's',
	.realbits 	= 32,
	.storagebits 	= 32,
	.shift 		= 0,
	.is_big_endian 	= true
};

static struct scan_type adis1647x_iio_delta_angl_scan_type = {
	.sign 		= 's',
	.realbits 	= 32,
	.storagebits 	= 32,
	.shift 		= 0,
	.is_big_endian 	= true
};

static struct scan_type adis1647x_iio_temp_scan_type = {
	.sign 		= 's',
	.realbits 	= 16,
	.storagebits 	= 16,
	.shift 		= 0,
	.is_big_endian 	= true
};

static struct iio_channel adis1647x_channels[] = {
	ADIS_GYRO_CHAN		(X, 	ADIS_GYRO_X, 		1647x, adis_iio_anglvel_attrs),
	ADIS_GYRO_CHAN		(Y, 	ADIS_GYRO_Y, 		1647x, adis_iio_anglvel_attrs),
	ADIS_GYRO_CHAN		(Z, 	ADIS_GYRO_Z, 		1647x, adis_iio_anglvel_attrs),
	ADIS_ACCEL_CHAN		(X,	ADIS_ACCEL_X, 		1647x, adis_iio_accel_attrs),
	ADIS_ACCEL_CHAN		(Y,	ADIS_ACCEL_Y, 		1647x, adis_iio_accel_attrs),
	ADIS_ACCEL_CHAN		(Z,	ADIS_ACCEL_Z, 		1647x, adis_iio_accel_attrs),
	ADIS_TEMP_CHAN		(ADIS_TEMP, 			1647x, adis_iio_temp_attrs),
	ADIS_DELTA_ANGL_CHAN	(X, 	ADIS_DELTA_ANGL_X, 	1647x, adis_iio_delta_angl_attrs),
	ADIS_DELTA_ANGL_CHAN	(Y, 	ADIS_DELTA_ANGL_Y, 	1647x, adis_iio_delta_angl_attrs),
	ADIS_DELTA_ANGL_CHAN	(Z, 	ADIS_DELTA_ANGL_Z, 	1647x, adis_iio_delta_angl_attrs),
	ADIS_DELTA_VEL_CHAN	(X, 	ADIS_DELTA_VEL_X, 	1647x, adis_iio_delta_vel_attrs),
	ADIS_DELTA_VEL_CHAN	(Y, 	ADIS_DELTA_VEL_Y, 	1647x, adis_iio_delta_vel_attrs),
	ADIS_DELTA_VEL_CHAN	(Z, 	ADIS_DELTA_VEL_Z, 	1647x, adis_iio_delta_vel_attrs),
};

static struct iio_attribute adis1647x_debug_attrs[] = {

	{
		.name = "diag_data_path_overrun",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_DATA_PATH_OVERRUN,
	},
	{
		.name = "diag_flash_memory_update_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_FLS_MEM_UPDATE_FAILURE,
	},
	{
		.name = "diag_spi_communication_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_SPI_COMM_ERR,
	},
	{
		.name = "diag_standby_mode",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_STANDBY_MODE,
	},
	{
		.name = "diag_sensor_self_test_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_SNSR_FAILURE,
	},
	{
		.name = "diag_flash_memory_test_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_MEM_FAILURE,
	},
	{
		.name = "diag_clock_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_CLK_ERR,
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
		.name = "lost_samples_count",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_LOST_SAMPLES_COUNT,
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
		.name = "data_ready_polarity",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_DR_POLARITY,
	},
	{
		.name = "sync_polarity",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_SYNC_POLARITY,
	},
	{
		.name = "sync_mode_select",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_SYNC_MODE,
	},
	{
		.name = "point_of_percussion_alignment",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_PT_OF_PERC_ALGNMT,
	},
	{
		.name = "linear_acceleration_compensation",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_LINEAR_ACCL_COMP,
	},
	{
		.name = "burst_data_selection",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_BURST_SEL,
	},
	{
		.name = "burst_size_selection",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_BURST32,
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
		.name = "bias_correction_time_base_control",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_BIAS_CORR_TBC,
	},
	{
		.name = "x_axis_gyroscope_bias_correction_enable",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_BIAS_CORR_EN_XG,
	},
	{
		.name = "y_axis_gyroscope_bias_correction_enable",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_BIAS_CORR_EN_YG,
	},
	{
		.name = "z_axis_gyroscope_bias_correction_enable",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_BIAS_CORR_EN_ZG,
	},
	{
		.name = "x_axis_accelerometer_bias_correction_enable",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_BIAS_CORR_EN_XA,
	},
	{
		.name = "y_axis_accelerometer_bias_correction_enable",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_BIAS_CORR_EN_YA,
	},
	{
		.name = "z_axis_accelerometer_bias_correction_enable",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_BIAS_CORR_EN_ZA,
	},
	{
		.name = "bias_correction_update",
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_CMD_BIAS_CORR_UPDATE,
	},
	{
		.name = "factory_calibration_restore",
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_CMD_FACT_CALIB_RESTORE,
	},
	{
		.name = "sensor_self_test",
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_CMD_SNSR_SELF_TEST,
	},
	{
		.name = "flash_memory_update",
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_CMD_FLS_MEM_UPDATE,
	},
	{
		.name = "flash_memory_test",
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_CMD_FLS_MEM_TEST,
	},
	{
		.name = "software_reset",
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_CMD_SW_RES,
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
		.name = "flash_count",
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

static struct iio_device adis1647x_iio_dev = {
	.num_ch 		= NO_OS_ARRAY_SIZE(adis1647x_channels),
	.channels 		= adis1647x_channels,
	.debug_attributes 	= adis1647x_debug_attrs,
	.attributes		= adis_dev_attrs,
	.pre_enable 		= (int32_t (*)())adis_iio_pre_enable,
	.trigger_handler 	= (int32_t (*)())adis_iio_trigger_handler,
	.debug_reg_read 	= (int32_t (*)())adis_iio_read_reg,
	.debug_reg_write 	= (int32_t (*)())adis_iio_write_reg,
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize adis1647x iio device.
 * @param iio_dev    - The adis1647x iio device.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, error code otherwise.
 */
int adis1647x_iio_init(struct adis_iio_dev **iio_dev,
		       struct adis_init_param *init_param)
{
	int ret;
	struct adis_iio_dev *desc;

	desc = (struct adis_iio_dev *)no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio_dev = &adis1647x_iio_dev;

	/* Update data based on the device id */
	desc->rang_mdl_txt = adis1647x_rang_mdl_txt[ADIS1647X_ID_NO_OFFSET(
				     init_param->dev_id)];

	ret = adis_init(&desc->adis_dev, init_param);
	if (ret)
		goto error_adis1647x_init;

	*iio_dev = desc;

	return 0;

error_adis1647x_init:
	no_os_free(desc);
	return ret;
}

/**
 * @brief Remove adis1647x iio device.
 * @param desc - The adis1647x iio device.
 */
void adis1647x_iio_remove(struct adis_iio_dev *desc)
{
	if (!desc)
		return;
	adis_remove(desc->adis_dev);
	no_os_free(desc);
}
