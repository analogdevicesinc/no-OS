/***************************************************************************//**
 *   @file   iio_adis1655x.c
 *   @brief  Implementation of iio_adis1655x.c
 *   @author RBolboac (ramona.gradinariu@analog.com)
 *******************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#include "iio_adis1655x.h"
#include "no_os_alloc.h"
#include "no_os_units.h"

/******************************************************************************/
/************************** Variable Definitions ******************************/
/******************************************************************************/

static const char * const adis1655x_rang_mdl_txt[] = {
	[ADIS16550]   = "+/-300_degrees_per_sec",
};

static struct scan_type adis1655x_iio_accel_scan_type = {
	.sign 		= 's',
	.realbits 	= 32,
	.storagebits 	= 32,
	.shift 		= 0,
	.is_big_endian 	= true
};

static struct scan_type adis1655x_iio_anglvel_scan_type = {
	.sign 		= 's',
	.realbits 	= 32,
	.storagebits 	= 32,
	.shift 		= 0,
	.is_big_endian 	= true
};

static struct scan_type adis1655x_iio_delta_vel_scan_type = {
	.sign 		= 's',
	.realbits 	= 32,
	.storagebits 	= 32,
	.shift 		= 0,
	.is_big_endian 	= true
};

static struct scan_type adis1655x_iio_delta_angl_scan_type = {
	.sign 		= 's',
	.realbits 	= 32,
	.storagebits 	= 32,
	.shift 		= 0,
	.is_big_endian 	= true
};

static struct scan_type adis1655x_iio_temp_scan_type = {
	.sign 		= 's',
	.realbits 	= 16,
	.storagebits 	= 16,
	.shift 		= 0,
	.is_big_endian 	= true
};

struct iio_attribute adis1655x_iio_anglvel_attrs[] = {
	{
		.name  = "calibbias",
		.show  = adis_iio_read_calibbias,
		.store = adis_iio_write_calibbias,
	},
	{
		.name  = "calibscale",
		.show  = adis_iio_read_calibscale,
		.store = adis_iio_write_calibscale,
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
	{
		.name   = "filter_low_pass_3db_frequency",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adis_iio_read_lpf,
		.store  = adis_iio_write_lpf,
	},
	END_ATTRIBUTES_ARRAY
};

struct iio_attribute adis1655x_iio_accel_attrs[] = {
	{
		.name  = "calibbias",
		.show  = adis_iio_read_calibbias,
		.store = adis_iio_write_calibbias,
	},
	{
		.name  = "calibscale",
		.show  = adis_iio_read_calibscale,
		.store = adis_iio_write_calibscale,
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
	{
		.name   = "filter_low_pass_3db_frequency",
		.shared = IIO_SHARED_BY_TYPE,
		.show   = adis_iio_read_lpf,
		.store  = adis_iio_write_lpf,
	},
	END_ATTRIBUTES_ARRAY
};

struct iio_attribute adis1655x_iio_temp_attrs[] = {
	{
		.name = "raw",
		.show = adis_iio_read_raw,
	},
	{
		.name = "scale",
		.show = adis_iio_read_scale,
	},
	{
		.name = "offset",
		.show = adis_iio_read_offset,
	},
	END_ATTRIBUTES_ARRAY
};

struct iio_attribute adis1655x_dev_attrs[] = {
	{
		.name   = "sampling_frequency",
		.shared = IIO_SHARED_BY_ALL,
		.show   = adis_iio_read_sampling_freq,
		.store  = adis_iio_write_sampling_freq,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_channel adis1655x_channels[] = {
	ADIS_GYRO_CHAN		(X, 	ADIS_GYRO_X, 		1655x, adis1655x_iio_anglvel_attrs),
	ADIS_GYRO_CHAN		(Y, 	ADIS_GYRO_Y, 		1655x, adis1655x_iio_anglvel_attrs),
	ADIS_GYRO_CHAN		(Z, 	ADIS_GYRO_Z, 		1655x, adis1655x_iio_anglvel_attrs),
	ADIS_ACCEL_CHAN		(X,	ADIS_ACCEL_X, 		1655x, adis1655x_iio_accel_attrs),
	ADIS_ACCEL_CHAN		(Y,	ADIS_ACCEL_Y, 		1655x, adis1655x_iio_accel_attrs),
	ADIS_ACCEL_CHAN		(Z,	ADIS_ACCEL_Z, 		1655x, adis1655x_iio_accel_attrs),
	ADIS_TEMP_CHAN		(ADIS_TEMP, 			1655x, adis1655x_iio_temp_attrs),
	ADIS_DELTA_ANGL_CHAN	(X, 	ADIS_DELTA_ANGL_X, 	1655x, adis_iio_delta_angl_attrs),
	ADIS_DELTA_ANGL_CHAN	(Y, 	ADIS_DELTA_ANGL_Y, 	1655x, adis_iio_delta_angl_attrs),
	ADIS_DELTA_ANGL_CHAN	(Z, 	ADIS_DELTA_ANGL_Z, 	1655x, adis_iio_delta_angl_attrs),
	ADIS_DELTA_VEL_CHAN	(X, 	ADIS_DELTA_VEL_X, 	1655x, adis_iio_delta_vel_attrs),
	ADIS_DELTA_VEL_CHAN	(Y, 	ADIS_DELTA_VEL_Y, 	1655x, adis_iio_delta_vel_attrs),
	ADIS_DELTA_VEL_CHAN	(Z, 	ADIS_DELTA_VEL_Z, 	1655x, adis_iio_delta_vel_attrs),
};

static struct iio_attribute adis1655x_debug_attrs[] = {
	{
		.name = "diag_flash_memory_test_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_MEM_FAILURE,
	},
	{
		.name = "diag_config_calib_crc_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_CONFIG_CALIB_CRC_ERR,
	},
	{
		.name = "diag_flash_memory_update_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_FLS_MEM_UPDATE_FAILURE,
	},
	{
		.name = "diag_overrange",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_OVERRANGE,
	},
	{
		.name = "diag_sensor_self_test_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_SNSR_FAILURE,
	},
	{
		.name = "diag_temperature_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_TEMP_ERR,
	},
	{
		.name = "diag_spi_communication_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_SPI_COMM_ERR,
	},
	{
		.name = "diag_data_path_overrun",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_DATA_PATH_OVERRUN,
	},
	{
		.name = "diag_power_supply_failure",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_POWER_SUPPLY_FAILURE,
	},
	{
		.name = "diag_boot_memory_failure",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_BOOT_MEMORY_FAILURE,
	},
	{
		.name = "diag_clock_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_CLK_ERR,
	},
	{
		.name = "diag_register_nvm_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_REG_NVM_ERR,
	},
	{
		.name = "diag_watchdog_timer_flag",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_WDG_TIMER_FLAG,
	},
	{
		.name = "diag_x_axis_gyroscope_failure",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_X_AXIS_GYRO_FAILURE,
	},
	{
		.name = "diag_y_axis_gyroscope_failure",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_Y_AXIS_GYRO_FAILURE,
	},
	{
		.name = "diag_z_axis_gyroscope_failure",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_Z_AXIS_GYRO_FAILURE,
	},
	{
		.name = "diag_x_axis_accelerometer_failure",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_X_AXIS_ACCL_FAILURE,
	},
	{
		.name = "diag_y_axis_accelerometer_failure",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_Y_AXIS_ACCL_FAILURE,
	},
	{
		.name = "diag_z_axis_accelerometer_failure",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_Z_AXIS_ACCL_FAILURE,
	},
	{
		.name = "diag_internal_processor_supply_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_INT_PROC_SUPPLY_ERR,
	},
	{
		.name = "diag_extenral_5v_supply_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_EXT_5V_SUPPLY_ERR,
	},
	{
		.name = "diag_internal_sensor_supply_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_INT_SNSR_SUPPLY_ERR,
	},
	{
		.name = "diag_internal_regulator_error",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_INT_REG_ERR,
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
		.name = "gyroscope_measurement_range",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_GYRO_MEAS_RANGE,
	},
	{
		.name = "point_of_percussion_alignment",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_PT_OF_PERC_ALGNMT,
	},
	{
		.name = "gyroscope_fir_filter_enable",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_GYRO_FIR_ENABLE,
	},
	{
		.name = "acceleroemeter_fir_filter_enable",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_ACCL_FIR_ENABLE,
	},
	{
		.name = "sync_mode_select",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_SYNC_MODE,
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
		.name = "software_reset",
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_CMD_SW_RES,
	},
	{
		.name = "write_lock",
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_CMD_WRITE_LOCK,
	},
	{
		.name = "processor_revision",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_PROC_REV,
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
		.name = "lot_number",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_LOT_NUM,
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
		.name = "scratch_pad_register4",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_USR_SCR_4,
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

static struct iio_device adis1655x_iio_dev = {
	.num_ch 		= NO_OS_ARRAY_SIZE(adis1655x_channels),
	.channels 		= adis1655x_channels,
	.debug_attributes 	= adis1655x_debug_attrs,
	.attributes		= adis1655x_dev_attrs,
	.pre_enable 		= (int32_t (*)())adis_iio_pre_enable,
	.trigger_handler 	= (int32_t (*)())adis_iio_trigger_handler,
	.debug_reg_read 	= (int32_t (*)())adis_iio_read_reg,
	.debug_reg_write 	= (int32_t (*)())adis_iio_write_reg,
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize adis1655x iio device.
 * @param iio_dev    - The adis1655x iio device.
 * @param init_param - The structure that contains the device initial parameters.
 * @return 0 in case of success, error code otherwise.
 */
int adis1655x_iio_init(struct adis_iio_dev **iio_dev,
		       struct adis_init_param *init_param)
{
	int ret;
	struct adis_iio_dev *desc;

	desc = (struct adis_iio_dev *)no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio_dev = &adis1655x_iio_dev;

	/* Update data based on the device id */
	desc->rang_mdl_txt = adis1655x_rang_mdl_txt[init_param->dev_id];

	ret = adis_init(&desc->adis_dev, init_param);
	if (ret)
		goto error_adis1655x_init;

	*iio_dev = desc;

	return 0;

error_adis1655x_init:
	no_os_free(desc);
	return ret;
}

/**
 * @brief Remove adis1655x iio device.
 * @param desc - The adis1655x iio device.
 */
void adis1655x_iio_remove(struct adis_iio_dev *desc)
{
	if (!desc)
		return;
	adis_remove(desc->adis_dev);
	no_os_free(desc);
}
