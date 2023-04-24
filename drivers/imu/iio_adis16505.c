/***************************************************************************//**
 *   @file   iio_adis16505.c
 *   @brief  Implementation of iio_adis16505.c
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

#include "iio_adis16505.h"
#include "no_os_alloc.h"
#include "no_os_units.h"

/******************************************************************************/
/************************** Variable Definitions ******************************/
/******************************************************************************/

/* Values from datasheet for 32-bit data */
static const struct adis_iio_scale_fractional adis16505_gyro_scale[] = {
	[ADIS16505_1] = {1, RAD_TO_DEGREE(10485760)},
	[ADIS16505_2] = {1, RAD_TO_DEGREE(2621440)},
	[ADIS16505_3] = {1, RAD_TO_DEGREE(655360)},
};

static const struct adis_iio_scale_fractional adis16505_accl_scale[] = {
	[ADIS16505_1] = {1, 26756268},
	[ADIS16505_2] = {1, 26756268},
	[ADIS16505_3] = {1, 26756268},
};

static const struct adis_iio_scale_fractional_log2 adis16505_rot_scale[] = {
	[ADIS16505_1] = {360, 31},
	[ADIS16505_2] = {720, 31},
	[ADIS16505_3] = {2160, 31},
};

static const struct adis_iio_scale_fractional_log2 adis16505_vel_scale[] = {
	[ADIS16505_1] = {100, 31},
	[ADIS16505_2] = {100, 31},
	[ADIS16505_3] = {100, 31},
};

/* IIO uses milli-degrees Celsius for temperature */
static const struct adis_iio_scale_fractional adis16505_temp_scale[] = {
	[ADIS16505_1] = {1 * MILLIDEGREE_PER_DEGREE, 10},
	[ADIS16505_2] = {1 * MILLIDEGREE_PER_DEGREE, 10},
	[ADIS16505_3] = {1 * MILLIDEGREE_PER_DEGREE, 10},
};

static const char * const adis16505_rang_mdl_txt[] = {
	[ADIS16505_1] = "+/-125_degrees_per_sec",
	[ADIS16505_2] = "+/-500_degrees_per_sec",
	[ADIS16505_3] = "+/-2000_degrees_per_sec",
};

static struct scan_type adis16505_iio_accel_scan_type = {
	.sign 		= 's',
	.realbits 	= 32,
	.storagebits 	= 32,
	.shift 		= 0,
	.is_big_endian 	= true
};

static struct scan_type adis16505_iio_anglvel_scan_type = {
	.sign 		= 's',
	.realbits 	= 32,
	.storagebits 	= 32,
	.shift 		= 0,
	.is_big_endian 	= true
};

static struct scan_type adis16505_iio_delta_vel_scan_type = {
	.sign 		= 's',
	.realbits 	= 32,
	.storagebits 	= 32,
	.shift 		= 0,
	.is_big_endian 	= true
};

static struct scan_type adis16505_iio_delta_angl_scan_type = {
	.sign 		= 's',
	.realbits 	= 32,
	.storagebits 	= 32,
	.shift 		= 0,
	.is_big_endian 	= true
};

static struct scan_type adis16505_iio_temp_scan_type = {
	.sign 		= 's',
	.realbits 	= 32,
	.storagebits 	= 32,
	.shift 		= 0,
	.is_big_endian 	= true
};

static struct scan_type adis16505_iio_data_counter_scan_type = {
	.sign 		= 'u',
	.realbits 	= 32,
	.storagebits 	= 32,
	.shift 		= 0,
	.is_big_endian 	= true
};

static struct iio_channel adis16505_channels[] = {
	ADIS_GYRO_CHAN		(X, 	ADIS_GYRO_X, 		16505),
	ADIS_GYRO_CHAN		(Y, 	ADIS_GYRO_Y, 		16505),
	ADIS_GYRO_CHAN		(Z, 	ADIS_GYRO_Z, 		16505),
	ADIS_ACCEL_CHAN		(X,	ADIS_ACCEL_X, 		16505),
	ADIS_ACCEL_CHAN		(Y,	ADIS_ACCEL_Y, 		16505),
	ADIS_ACCEL_CHAN		(Z,	ADIS_ACCEL_Z, 		16505),
	ADIS_TEMP_CHAN		(ADIS_TEMP, 			16505),
	ADIS_DELTA_ANGL_CHAN	(X, 	ADIS_DELTA_ANGL_X, 	16505),
	ADIS_DELTA_ANGL_CHAN	(Y, 	ADIS_DELTA_ANGL_Y, 	16505),
	ADIS_DELTA_ANGL_CHAN	(Z, 	ADIS_DELTA_ANGL_Z, 	16505),
	ADIS_DELTA_VEL_CHAN	(X, 	ADIS_DELTA_VEL_X, 	16505),
	ADIS_DELTA_VEL_CHAN	(Y, 	ADIS_DELTA_VEL_Y, 	16505),
	ADIS_DELTA_VEL_CHAN	(Z, 	ADIS_DELTA_VEL_Z, 	16505),
	ADIS_DATA_COUNTER_CHAN	(ADIS_DATA_COUNTER,		16505),
};

static struct iio_attribute adis16505_debug_attrs[] = {

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
		.name = "diag_gyroscope1_self_test_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_GYRO1_FAILURE,
	},
	{
		.name = "diag_gyroscope2_self_test_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_GYRO2_FAILURE,
	},
	{
		.name = "diag_acceleration_self_test_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_ACCL_FAILURE,
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
		.name = "internal_sensor_bandwidth",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_SENS_BW,
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

static struct iio_device adis16505_iio_dev = {
	.num_ch 		= NO_OS_ARRAY_SIZE(adis16505_channels),
	.channels 		= adis16505_channels,
	.debug_attributes 	= adis16505_debug_attrs,
	.attributes		= adis_dev_attrs,
	.pre_enable 		= (int32_t (*)())adis_iio_pre_enable,
	.trigger_handler 	= (int32_t (*)())adis_iio_trigger_handler,
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize adis16505 iio device.
 * @param iio_dev    - The adis16505 iio device.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, error code otherwise.
 */
int adis16505_iio_init(struct adis_iio_dev **iio_dev,
		       struct adis_init_param *init_param)
{
	int ret;
	struct adis_iio_dev *desc;

	desc = (struct adis_iio_dev *)no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio_dev = &adis16505_iio_dev;

	adis16505_chip_info.ip = init_param;

	/* Update scales based on the device id */
	desc->gyro_scale = adis16505_gyro_scale[init_param->dev_id];
	desc->accl_scale = adis16505_accl_scale[init_param->dev_id];
	desc->rot_scale = adis16505_rot_scale[init_param->dev_id];
	desc->vel_scale = adis16505_vel_scale[init_param->dev_id];
	desc->temp_scale = adis16505_temp_scale[init_param->dev_id];
	desc->rang_mdl_txt = adis16505_rang_mdl_txt[init_param->dev_id];

	ret = adis_init(&desc->adis_dev, &adis16505_chip_info);
	if (ret)
		goto error_adis16505_init;

	*iio_dev = desc;

	return 0;

error_adis16505_init:
	no_os_free(desc);
	return ret;
}

/**
 * @brief Remove adis16505 iio device.
 * @param desc - The adis16505 iio device.
 */
void adis16505_iio_remove(struct adis_iio_dev *desc)
{
	adis_remove(desc->adis_dev);
	no_os_free(desc);
}
