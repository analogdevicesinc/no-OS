/***************************************************************************//**
 *   @file   iio_adis16607.c
 *   @brief  Implementation of iio_adis16607.c
 *   @author Radu Sabau (radu.sabau@analog.com)
 *******************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 ******************************************************************************/

#include "iio_adis16607.h"
#include "iio_types.h"
#include "iio_trigger.h"
#include "no_os_alloc.h"
#include "no_os_irq.h"
#include "no_os_units.h"

#define ADIS16607_FIFO_WORD_CNT_REG	0x2B
#define ADIS16607_FULL_FIFO_VAL		988

#define ADIS16607_MSW_MASK		NO_OS_GENMASK(31, 16)
#define ADIS16607_LSW_MASK		NO_OS_GENMASK(15, 0)
#define ADIS16607_FIFO_THR_MET		NO_OS_BIT(0)

static const char * const adis16607_rang_mdl_txt[] = {
	[ADIS16607_ID_NO_OFFSET(ADIS16607_2)] = "+/-450_degrees_per_sec",
	[ADIS16607_ID_NO_OFFSET(ADIS16607_3)] = "+/-2000_degrees_per_sec",
};

static struct scan_type adis16607_iio_accel_scan_type = {
	.sign 		= 's',
	.realbits 	= 24,
	.storagebits 	= 32,
	.shift 		= 0,
	.is_big_endian 	= true
};

static struct scan_type adis16607_iio_anglvel_scan_type = {
	.sign 		= 's',
	.realbits 	= 24,
	.storagebits 	= 32,
	.shift 		= 0,
	.is_big_endian 	= true
};

static struct scan_type adis16607_iio_delta_vel_scan_type = {
	.sign 		= 's',
	.realbits 	= 24,
	.storagebits 	= 32,
	.shift 		= 0,
	.is_big_endian 	= true
};

static struct scan_type adis16607_iio_delta_angl_scan_type = {
	.sign 		= 's',
	.realbits 	= 24,
	.storagebits 	= 32,
	.shift 		= 0,
	.is_big_endian 	= true
};

static struct scan_type adis16607_iio_temp_scan_type = {
	.sign 		= 's',
	.realbits 	= 16,
	.storagebits 	= 16,
	.shift 		= 0,
	.is_big_endian 	= true
};

struct iio_attribute adis16607_iio_temp_attrs[] = {
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

static struct iio_channel adis16607_channels[] = {
	ADIS_GYRO_CHAN(X, 	ADIS_GYRO_X, 		16607, adis_iio_anglvel_attrs),
	ADIS_GYRO_CHAN(Y, 	ADIS_GYRO_Y, 		16607, adis_iio_anglvel_attrs),
	ADIS_GYRO_CHAN(Z, 	ADIS_GYRO_Z, 		16607, adis_iio_anglvel_attrs),
	ADIS_ACCEL_CHAN(X,	ADIS_ACCEL_X, 		16607, adis_iio_accel_attrs),
	ADIS_ACCEL_CHAN(Y,	ADIS_ACCEL_Y, 		16607, adis_iio_accel_attrs),
	ADIS_ACCEL_CHAN(Z,	ADIS_ACCEL_Z, 		16607, adis_iio_accel_attrs),
	ADIS_TEMP_CHAN(ADIS_TEMP, 			16607, adis16607_iio_temp_attrs),
	ADIS_DELTA_ANGL_CHAN(X, 	ADIS_DELTA_ANGL_X, 	16607, adis_iio_delta_angl_attrs),
	ADIS_DELTA_ANGL_CHAN(Y, 	ADIS_DELTA_ANGL_Y, 	16607, adis_iio_delta_angl_attrs),
	ADIS_DELTA_ANGL_CHAN(Z, 	ADIS_DELTA_ANGL_Z, 	16607, adis_iio_delta_angl_attrs),
	ADIS_DELTA_VEL_CHAN(X, 	ADIS_DELTA_VEL_X, 	16607, adis_iio_delta_vel_attrs),
	ADIS_DELTA_VEL_CHAN(Y, 	ADIS_DELTA_VEL_Y, 	16607, adis_iio_delta_vel_attrs),
	ADIS_DELTA_VEL_CHAN(Z, 	ADIS_DELTA_VEL_Z, 	16607, adis_iio_delta_vel_attrs),
};

static int adis16607_iio_trigger_handler_no_burst(struct iio_device_data
		*dev_data)
{
	struct adis_iio_dev *iio_adis;
	struct adis_burst_data data;
	struct adis_dev *adis;
	int32_t temp_val;
	uint8_t buff[52];
	uint32_t mask;
	uint8_t i = 0;
	uint8_t chan;
	int ret;

	if (!dev_data)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev_data->dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	iio_trig_disable(iio_adis->hw_trig_desc);

	mask = dev_data->buffer->active_mask;
	for (chan = 0; chan < ADIS_NUM_CHAN; chan++) {
		if (mask & (1 << chan)) {
			switch (chan) {
			case ADIS_TEMP:
				ret = adis_read_temp_out(adis, &temp_val);
				if (ret)
					goto trig_enable;

				data.temp_msb = no_os_field_get(ADIS16607_MSW_MASK, temp_val);
				data.temp_lsb = no_os_field_get(ADIS16607_LSW_MASK, temp_val);

				if (iio_adis->iio_dev->channels[chan].scan_type->storagebits == 32) {
					buff[i++] = (data.temp_msb >> 8) & 0xFF;
					buff[i++] = data.temp_msb & 0xFF;
				}

				buff[i++] = (data.temp_lsb >> 8) & 0xFF;
				buff[i++] = data.temp_lsb & 0xFF;
				/*
				 * The temperature channel has 16-bit storage size.
				 * We need to perform the padding to have the buffer
				 * elements naturally aligned in case there are any
				 * 32-bit storage size channels enabled which have a
				 * scan index higher than the temperature channel scan
				 * index.
				 */
				if (mask & NO_OS_GENMASK(ADIS_DELTA_VEL_Z, ADIS_DELTA_ANGL_X)
				    && iio_adis->iio_dev->channels[chan].scan_type->storagebits == 16) {
					buff[i++] = 0;
					buff[i++] = 0;
				}

				break;
			case ADIS_GYRO_X:
				ret = adis_read_x_gyro(adis, &temp_val);
				if (ret)
					goto trig_enable;

				data.x_gyro_msb = (temp_val >> 24) & 0xFF;
				data.x_gyro_lsb = (temp_val >> 8) & 0xFFFF;

				buff[i++] = (data.x_gyro_msb >> 8) & 0xFF;
				buff[i++] = data.x_gyro_msb & 0xFF;
				buff[i++] = (data.x_gyro_lsb >> 8) & 0xFF;
				buff[i++] = data.x_gyro_lsb & 0xFF;

				break;
			case ADIS_GYRO_Y:
				ret = adis_read_y_gyro(adis, &temp_val);
				if (ret)
					goto trig_enable;

				data.y_gyro_msb = (temp_val >> 24) & 0xFF;
				data.y_gyro_lsb = (temp_val >> 8) & 0xFFFF;

				buff[i++] = (data.y_gyro_msb >> 8) & 0xFF;
				buff[i++] = data.y_gyro_msb & 0xFF;
				buff[i++] = (data.y_gyro_lsb >> 8) & 0xFF;
				buff[i++] = data.y_gyro_lsb & 0xFF;

				break;
			case ADIS_GYRO_Z:
				ret = adis_read_z_gyro(adis, &temp_val);
				if (ret)
					goto trig_enable;

				data.z_gyro_msb = (temp_val >> 24) & 0xFF;
				data.z_gyro_lsb = (temp_val >> 8) & 0xFFFF;

				buff[i++] = (data.z_gyro_msb >> 8) & 0xFF;
				buff[i++] = data.z_gyro_msb & 0xFF;
				buff[i++] = (data.z_gyro_lsb >> 8) & 0xFF;
				buff[i++] = data.z_gyro_lsb & 0xFF;

				break;
			case ADIS_ACCEL_X:
				ret = adis_read_x_accl(adis, &temp_val);
				if (ret)
					goto trig_enable;

				data.x_accel_msb = (temp_val >> 24) & 0xFF;
				data.x_accel_lsb = (temp_val >> 8) & 0xFFFF;

				buff[i++] = (data.x_accel_msb >> 8) & 0xFF;
				buff[i++] = data.x_accel_msb & 0xFF;
				buff[i++] = (data.x_accel_lsb >> 8) & 0xFF;
				buff[i++] = data.x_accel_lsb & 0xFF;

				break;
			case ADIS_ACCEL_Y:
				ret = adis_read_y_accl(adis, &temp_val);
				if (ret)
					goto trig_enable;

				data.y_accel_msb = (temp_val >> 24) & 0xFF;
				data.y_accel_lsb = (temp_val >> 8) & 0xFFFF;

				buff[i++] = (data.y_accel_msb >> 8) & 0xFF;
				buff[i++] = data.y_accel_msb & 0xFF;
				buff[i++] = (data.y_accel_lsb >> 8) & 0xFF;
				buff[i++] = data.y_accel_lsb & 0xFF;

				break;
			case ADIS_ACCEL_Z:
				ret = adis_read_z_accl(adis, &temp_val);
				if (ret)
					goto trig_enable;

				data.z_accel_msb = (temp_val >> 24) & 0xFF;
				data.z_accel_lsb = (temp_val >> 8) & 0xFFFF;

				buff[i++] = (data.z_accel_msb >> 8) & 0xFF;
				buff[i++] = data.z_accel_msb & 0xFF;
				buff[i++] = (data.z_accel_lsb >> 8) & 0xFF;
				buff[i++] = data.z_accel_lsb & 0xFF;

				break;
			case ADIS_DELTA_ANGL_X:
				ret = adis_read_x_deltang(adis, &temp_val);
				if (ret)
					goto trig_enable;

				data.x_deltang_msb = (temp_val >> 24) & 0xFF;
				data.x_deltang_lsb = (temp_val >> 8) & 0xFFFF;

				buff[i++] = (data.x_deltang_msb >> 8) & 0xFF;
				buff[i++] = data.x_deltang_msb & 0xFF;
				buff[i++] = (data.x_deltang_lsb >> 8) & 0xFF;
				buff[i++] = data.x_deltang_lsb & 0xFF;

				break;
			case ADIS_DELTA_ANGL_Y:
				ret = adis_read_y_deltang(adis, &temp_val);
				if (ret)
					goto trig_enable;

				data.y_deltang_msb = (temp_val >> 24) & 0xFF;
				data.y_deltang_lsb = (temp_val >> 8) & 0xFFFF;

				buff[i++] = (data.y_deltang_msb >> 8) & 0xFF;
				buff[i++] = data.y_deltang_msb & 0xFF;
				buff[i++] = (data.y_deltang_lsb >> 8) & 0xFF;
				buff[i++] = data.y_deltang_lsb & 0xFF;

				break;
			case ADIS_DELTA_ANGL_Z:
				ret = adis_read_z_deltang(adis, &temp_val);
				if (ret)
					goto trig_enable;

				data.z_deltang_msb = (temp_val >> 24) & 0xFF;
				data.z_deltang_lsb = (temp_val >> 8) & 0xFFFF;

				buff[i++] = (data.z_deltang_msb >> 8) & 0xFF;
				buff[i++] = data.z_deltang_msb & 0xFF;
				buff[i++] = (data.z_deltang_lsb >> 8) & 0xFF;
				buff[i++] = data.z_deltang_lsb & 0xFF;

				break;
			case ADIS_DELTA_VEL_X:
				ret = adis_read_x_deltvel(adis, &temp_val);
				if (ret)
					goto trig_enable;

				data.x_deltvel_msb = (temp_val >> 24) & 0xFF;
				data.x_deltvel_lsb = (temp_val >> 8) & 0xFFFF;

				buff[i++] = (data.x_deltvel_msb >> 8) & 0xFF;
				buff[i++] = data.x_deltvel_msb & 0xFF;
				buff[i++] = (data.x_deltvel_lsb >> 8) & 0xFF;
				buff[i++] = data.x_deltvel_lsb & 0xFF;

				break;
			case ADIS_DELTA_VEL_Y:
				ret = adis_read_y_deltvel(adis, &temp_val);
				if (ret)
					goto trig_enable;

				data.y_deltvel_msb = (temp_val >> 24) & 0xFF;
				data.y_deltvel_lsb = (temp_val >> 8) & 0xFFFF;

				buff[i++] = (data.y_deltvel_msb >> 8) & 0xFF;
				buff[i++] = data.y_deltvel_msb & 0xFF;
				buff[i++] = (data.y_deltvel_lsb >> 8) & 0xFF;
				buff[i++] = data.y_deltvel_lsb & 0xFF;

				break;
			case ADIS_DELTA_VEL_Z:
				ret = adis_read_z_deltvel(adis, &temp_val);
				if (ret)
					goto trig_enable;

				data.z_deltvel_msb = (temp_val >> 24) & 0xFF;
				data.z_deltvel_lsb = (temp_val >> 8) & 0xFFFF;

				buff[i++] = (data.z_deltvel_msb >> 8) & 0xFF;
				buff[i++] = data.z_deltvel_msb & 0xFF;
				buff[i++] = (data.z_deltvel_lsb >> 8) & 0xFF;
				buff[i++] = data.z_deltvel_lsb & 0xFF;

				break;
			default:
				break;
			}
		}
	}

	iio_buffer_push_scan(dev_data->buffer, buff);

	ret = 0;

trig_enable:
	iio_trig_enable(iio_adis->hw_trig_desc);

	return ret;
}

static int adis16607_iio_pre_enable_no_burst(void *dev, uint32_t mask)
{
	struct adis_iio_dev *iio_adis;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	ret = adis_read_sync_mode(iio_adis->adis_dev, &iio_adis->sync_mode);
	if (ret)
		return ret;

	return adis_write_dr_enable(iio_adis->adis_dev, 1);
}

static int adis16607_iio_post_disable_no_burst(void *dev)
{
	struct adis_iio_dev *iio_adis;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	return adis_write_dr_enable(iio_adis->adis_dev, 0);
}

static int adis16607_iio_push_single_sample(struct adis_iio_dev *iio_adis,
		uint32_t mask, struct iio_buffer *buffer)
{
	struct adis_burst_data data;
	struct adis_dev *adis;
	uint8_t buff[52];
	uint8_t i = 0;
	uint8_t chan;
	int ret;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	ret = adis16607_fifo_pop(adis, &data);
	/* If ret ==  EAGAIN then no data is available to read (will happen
	for a burst request or in case burst32 or burst select has been changed) */
	if (ret == -EAGAIN)
		return 0;

	if (ret)
		return ret;

	/* Update samples lost counter based on data counter */
	uint32_t current_data_cntr = data.data_cntr_lsb | data.data_cntr_msb << 16;

	if (iio_adis->data_cntr) {
		if (current_data_cntr > iio_adis->data_cntr)
			iio_adis->samples_lost += current_data_cntr -
						  iio_adis->data_cntr - 1;
		else if (current_data_cntr < iio_adis->data_cntr)
			/* Data counter overflow occurred */
			iio_adis->samples_lost += NO_OS_U16_MAX -
						  iio_adis->data_cntr +
						  current_data_cntr;
	}

	iio_adis->data_cntr = current_data_cntr;

	/* Assign data from burst_data structure to buffer byte by byte (big-endian) */
	for (chan = 0; chan < ADIS_NUM_CHAN; chan++) {
		if (mask & (1 << chan)) {
			switch (chan) {
			case ADIS_TEMP:
				if (iio_adis->iio_dev->channels[chan].scan_type->storagebits == 32) {
					buff[i++] = (data.temp_msb >> 8) & 0xFF;
					buff[i++] = data.temp_msb & 0xFF;
				}

				buff[i++] = (data.temp_lsb >> 8) & 0xFF;
				buff[i++] = data.temp_lsb & 0xFF;
				/*
				 * The temperature channel has 16-bit storage size.
				 * We need to perform the padding to have the buffer
				 * elements naturally aligned in case there are any
				 * 32-bit storage size channels enabled which have a
				 * scan index higher than the temperature channel scan
				 * index.
				 */
				if (mask & NO_OS_GENMASK(ADIS_DELTA_VEL_Z, ADIS_DELTA_ANGL_X)
				    && iio_adis->iio_dev->channels[chan].scan_type->storagebits == 16) {
					buff[i++] = 0;
					buff[i++] = 0;
				}

				break;
			case ADIS_GYRO_X:
				buff[i++] = (data.x_gyro_msb >> 8) & 0xFF;
				buff[i++] = data.x_gyro_msb & 0xFF;
				buff[i++] = (data.x_gyro_lsb >> 8) & 0xFF;
				buff[i++] = data.x_gyro_lsb & 0xFF;
				break;
			case ADIS_GYRO_Y:
				buff[i++] = (data.y_gyro_msb >> 8) & 0xFF;
				buff[i++] = data.y_gyro_msb & 0xFF;
				buff[i++] = (data.y_gyro_lsb >> 8) & 0xFF;
				buff[i++] = data.y_gyro_lsb & 0xFF;
				break;
			case ADIS_GYRO_Z:
				buff[i++] = (data.z_gyro_msb >> 8) & 0xFF;
				buff[i++] = data.z_gyro_msb & 0xFF;
				buff[i++] = (data.z_gyro_lsb >> 8) & 0xFF;
				buff[i++] = data.z_gyro_lsb & 0xFF;
				break;
			case ADIS_ACCEL_X:
				buff[i++] = (data.x_accel_msb >> 8) & 0xFF;
				buff[i++] = data.x_accel_msb & 0xFF;
				buff[i++] = (data.x_accel_lsb >> 8) & 0xFF;
				buff[i++] = data.x_accel_lsb & 0xFF;
				break;
			case ADIS_ACCEL_Y:
				buff[i++] = (data.y_accel_msb >> 8) & 0xFF;
				buff[i++] = data.y_accel_msb & 0xFF;
				buff[i++] = (data.y_accel_lsb >> 8) & 0xFF;
				buff[i++] = data.y_accel_lsb & 0xFF;
				break;
			case ADIS_ACCEL_Z:
				buff[i++] = (data.z_accel_msb >> 8) & 0xFF;
				buff[i++] = data.z_accel_msb & 0xFF;
				buff[i++] = (data.z_accel_lsb >> 8) & 0xFF;
				buff[i++] = data.z_accel_lsb & 0xFF;
				break;
			case ADIS_DELTA_ANGL_X:
				buff[i++] = (data.x_deltang_msb >> 8) & 0xFF;
				buff[i++] = data.x_deltang_msb & 0xFF;
				buff[i++] = (data.x_deltang_lsb >> 8) & 0xFF;
				buff[i++] = data.x_deltang_lsb & 0xFF;
				break;
			case ADIS_DELTA_ANGL_Y:
				buff[i++] = (data.y_deltang_msb >> 8) & 0xFF;
				buff[i++] = data.y_deltang_msb & 0xFF;
				buff[i++] = (data.y_deltang_lsb >> 8) & 0xFF;
				buff[i++] = data.y_deltang_lsb & 0xFF;
				break;
			case ADIS_DELTA_ANGL_Z:
				buff[i++] = (data.z_deltang_msb >> 8) & 0xFF;
				buff[i++] = data.z_deltang_msb & 0xFF;
				buff[i++] = (data.z_deltang_lsb >> 8) & 0xFF;
				buff[i++] = data.z_deltang_lsb & 0xFF;
				break;
			case ADIS_DELTA_VEL_X:
				buff[i++] = (data.x_deltvel_msb >> 8) & 0xFF;
				buff[i++] = data.x_deltvel_msb & 0xFF;
				buff[i++] = (data.x_deltvel_lsb >> 8) & 0xFF;
				buff[i++] = data.x_deltvel_lsb & 0xFF;
				break;
			case ADIS_DELTA_VEL_Y:
				buff[i++] = (data.y_deltvel_msb >> 8) & 0xFF;
				buff[i++] = data.y_deltvel_msb & 0xFF;
				buff[i++] = (data.y_deltvel_lsb >> 8) & 0xFF;
				buff[i++] = data.y_deltvel_lsb & 0xFF;
				break;
			case ADIS_DELTA_VEL_Z:
				buff[i++] = (data.z_deltvel_msb >> 8) & 0xFF;
				buff[i++] = data.z_deltvel_msb & 0xFF;
				buff[i++] = (data.z_deltvel_lsb >> 8) & 0xFF;
				buff[i++] = data.z_deltvel_lsb & 0xFF;
				break;
			default:
				break;
			}
		}
	}

	return iio_buffer_push_scan(buffer, buff);
}

static int adis16607_iio_trigger_handler_with_fifo(struct iio_device_data
		*dev_data)
{
	struct adis_iio_dev *iio_adis;
	struct adis_dev *adis;
	uint32_t diag_stat;
	uint32_t fifo_cnt;
	uint32_t fifo_thr;
	uint32_t mask;
	uint32_t i = 0;
	int ret = 0;

	if (!dev_data)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev_data->dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	iio_trig_disable(iio_adis->hw_trig_desc);

	adis = iio_adis->adis_dev;

	ret = adis_read_reg(adis, ADIS16607_DIAG_STAT_REG, &diag_stat, 2);
	if (ret)
		goto trig_enable;

	if (!(diag_stat & ADIS16607_FIFO_THR_MET))
		goto trig_enable;

	ret = adis16607_fifo_enable(iio_adis->adis_dev, false, 0, false);
	if (ret)
		goto fifo_enable;

	ret = adis_read_reg(adis, ADIS16607_FIFO_WORD_CNT_REG, &fifo_cnt, 2);
	if (ret)
		goto fifo_enable;

	/* 1 samples of data = 26 word count in the FIFO. */
	fifo_cnt /= 26;
	/* From data-sheet, minimum time between reads */
	no_os_udelay(10);
	if (fifo_cnt > dev_data->buffer->samples)
		fifo_cnt = dev_data->buffer->samples;

	if (fifo_cnt) {
		for (i = 0; i < fifo_cnt; i++) {
			ret = adis16607_iio_push_single_sample(iio_adis, dev_data->buffer->active_mask,
							       dev_data->buffer);
			if (ret)
				goto fifo_enable;

			/* From data-sheet, minimum time between reads */
			no_os_udelay(10);
		}
	}

fifo_enable:
	/*
	 * Request number of samples from FIFO.
	 * Cap to ADIS16607_FULL_FIFO_VAL if requested samples is greater than
	 * the FIFO word size.
	 */
	fifo_thr = 26u * (dev_data->buffer->samples - 1);
	adis16607_fifo_enable(iio_adis->adis_dev, true,
			      (fifo_thr && fifo_thr <= ADIS16607_FULL_FIFO_VAL) ?
			      fifo_thr : ADIS16607_FULL_FIFO_VAL, true);
trig_enable:
	iio_trig_enable(iio_adis->hw_trig_desc);
	return ret;
}

static int adis16607_iio_pre_enable_with_fifo(void *dev, uint32_t mask)
{
	struct adis_iio_dev *iio_adis;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	iio_adis->samples_lost = 0;
	iio_adis->data_cntr = 0;

	ret = adis_write_dr_enable(iio_adis->adis_dev, 1);
	if (ret)
		return ret;

	/* Request full FIFO first. */
	return adis16607_fifo_enable(iio_adis->adis_dev, true, ADIS16607_FULL_FIFO_VAL,
				     true);
}

static int adis16607_iio_post_disable_with_fifo(void *dev)
{
	struct adis_iio_dev *iio_adis;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	ret = adis16607_fifo_enable(iio_adis->adis_dev, false, 0, false);
	if (ret)
		return ret;

	return adis_write_dr_enable(iio_adis->adis_dev, 0);
}

static int adis16607_iio_trigger_handler(struct iio_device_data *dev_data)
{
	struct adis_iio_dev *iio_adis;
	struct adis_burst_data data;
	struct adis_dev *adis;
	uint8_t buff[52];
	uint32_t mask;
	uint8_t i = 0;
	uint8_t chan;
	int ret;

	if (!dev_data)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev_data->dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	adis = iio_adis->adis_dev;

	iio_trig_disable(iio_adis->hw_trig_desc);

	ret = adis_read_burst_data(adis, &data, 1, 0, false, false);
	if (ret)
		goto trig_enable;

	/* Update samples lost counter based on data counter */
	uint32_t current_data_cntr = data.data_cntr_lsb | data.data_cntr_msb << 16;

	if (iio_adis->data_cntr) {
		if (current_data_cntr > iio_adis->data_cntr)
			iio_adis->samples_lost += current_data_cntr -
						  iio_adis->data_cntr - 1;
		else if (current_data_cntr < iio_adis->data_cntr)
			/* Data counter overflow occurred */
			iio_adis->samples_lost += NO_OS_U16_MAX -
						  iio_adis->data_cntr +
						  current_data_cntr;
	}

	iio_adis->data_cntr = current_data_cntr;

	/* Assign data from burst_data structure to buffer byte by byte (big-endian) */
	mask = dev_data->buffer->active_mask;
	for (chan = 0; chan < ADIS_NUM_CHAN; chan++) {
		if (mask & (1 << chan)) {
			switch (chan) {
			case ADIS_TEMP:
				if (iio_adis->iio_dev->channels[chan].scan_type->storagebits == 32) {
					buff[i++] = (data.temp_msb >> 8) & 0xFF;
					buff[i++] = data.temp_msb & 0xFF;
				}

				buff[i++] = (data.temp_lsb >> 8) & 0xFF;
				buff[i++] = data.temp_lsb & 0xFF;
				/*
				 * The temperature channel has 16-bit storage size.
				 * We need to perform the padding to have the buffer
				 * elements naturally aligned in case there are any
				 * 32-bit storage size channels enabled which have a
				 * scan index higher than the temperature channel scan
				 * index.
				 */
				if (mask & NO_OS_GENMASK(ADIS_DELTA_VEL_Z, ADIS_DELTA_ANGL_X)
				    && iio_adis->iio_dev->channels[chan].scan_type->storagebits == 16) {
					buff[i++] = 0;
					buff[i++] = 0;
				}

				break;
			case ADIS_GYRO_X:
				buff[i++] = (data.x_gyro_msb >> 8) & 0xFF;
				buff[i++] = data.x_gyro_msb & 0xFF;
				buff[i++] = (data.x_gyro_lsb >> 8) & 0xFF;
				buff[i++] = data.x_gyro_lsb & 0xFF;
				break;
			case ADIS_GYRO_Y:
				buff[i++] = (data.y_gyro_msb >> 8) & 0xFF;
				buff[i++] = data.y_gyro_msb & 0xFF;
				buff[i++] = (data.y_gyro_lsb >> 8) & 0xFF;
				buff[i++] = data.y_gyro_lsb & 0xFF;
				break;
			case ADIS_GYRO_Z:
				buff[i++] = (data.z_gyro_msb >> 8) & 0xFF;
				buff[i++] = data.z_gyro_msb & 0xFF;
				buff[i++] = (data.z_gyro_lsb >> 8) & 0xFF;
				buff[i++] = data.z_gyro_lsb & 0xFF;
				break;
			case ADIS_ACCEL_X:
				buff[i++] = (data.x_accel_msb >> 8) & 0xFF;
				buff[i++] = data.x_accel_msb & 0xFF;
				buff[i++] = (data.x_accel_lsb >> 8) & 0xFF;
				buff[i++] = data.x_accel_lsb & 0xFF;
				break;
			case ADIS_ACCEL_Y:
				buff[i++] = (data.y_accel_msb >> 8) & 0xFF;
				buff[i++] = data.y_accel_msb & 0xFF;
				buff[i++] = (data.y_accel_lsb >> 8) & 0xFF;
				buff[i++] = data.y_accel_lsb & 0xFF;
				break;
			case ADIS_ACCEL_Z:
				buff[i++] = (data.z_accel_msb >> 8) & 0xFF;
				buff[i++] = data.z_accel_msb & 0xFF;
				buff[i++] = (data.z_accel_lsb >> 8) & 0xFF;
				buff[i++] = data.z_accel_lsb & 0xFF;
				break;
			case ADIS_DELTA_ANGL_X:
				buff[i++] = (data.x_deltang_msb >> 8) & 0xFF;
				buff[i++] = data.x_deltang_msb & 0xFF;
				buff[i++] = (data.x_deltang_lsb >> 8) & 0xFF;
				buff[i++] = data.x_deltang_lsb & 0xFF;
				break;
			case ADIS_DELTA_ANGL_Y:
				buff[i++] = (data.y_deltang_msb >> 8) & 0xFF;
				buff[i++] = data.y_deltang_msb & 0xFF;
				buff[i++] = (data.y_deltang_lsb >> 8) & 0xFF;
				buff[i++] = data.y_deltang_lsb & 0xFF;
				break;
			case ADIS_DELTA_ANGL_Z:
				buff[i++] = (data.z_deltang_msb >> 8) & 0xFF;
				buff[i++] = data.z_deltang_msb & 0xFF;
				buff[i++] = (data.z_deltang_lsb >> 8) & 0xFF;
				buff[i++] = data.z_deltang_lsb & 0xFF;
				break;
			case ADIS_DELTA_VEL_X:
				buff[i++] = (data.x_deltvel_msb >> 8) & 0xFF;
				buff[i++] = data.x_deltvel_msb & 0xFF;
				buff[i++] = (data.x_deltvel_lsb >> 8) & 0xFF;
				buff[i++] = data.x_deltvel_lsb & 0xFF;
				break;
			case ADIS_DELTA_VEL_Y:
				buff[i++] = (data.y_deltvel_msb >> 8) & 0xFF;
				buff[i++] = data.y_deltvel_msb & 0xFF;
				buff[i++] = (data.y_deltvel_lsb >> 8) & 0xFF;
				buff[i++] = data.y_deltvel_lsb & 0xFF;
				break;
			case ADIS_DELTA_VEL_Z:
				buff[i++] = (data.z_deltvel_msb >> 8) & 0xFF;
				buff[i++] = data.z_deltvel_msb & 0xFF;
				buff[i++] = (data.z_deltvel_lsb >> 8) & 0xFF;
				buff[i++] = data.z_deltvel_lsb & 0xFF;
				break;
			default:
				break;
			}
		}
	}

	iio_buffer_push_scan(dev_data->buffer, buff);

	ret = 0;

trig_enable:
	iio_trig_enable(iio_adis->hw_trig_desc);

	return ret;
}

static int adis16607_iio_pre_enable(void *dev, uint32_t mask)
{
	struct adis_iio_dev *iio_adis;
	int ret;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	ret = adis_write_burst32(iio_adis->adis_dev, 1);
	if (ret)
		return ret;

	ret = adis_update_bits_base(iio_adis->adis_dev, ADIS16607_USER_DATA_CFG_REG,
				    ADIS16607_DATA_CNTR_EN_MASK, 1, 2);
	if (ret)
		return ret;

	return adis_write_dr_enable(iio_adis->adis_dev, 1);
}

static int adis16607_iio_post_disable(void *dev)
{
	struct adis_iio_dev *iio_adis;

	if (!dev)
		return -EINVAL;

	iio_adis = (struct adis_iio_dev *)dev;

	if (!iio_adis->adis_dev)
		return -EINVAL;

	return adis_write_dr_enable(iio_adis->adis_dev, 0);
}

struct iio_attribute adis16607_debug_attrs[] = {
	{
		.name = "diag_gyro_failure",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_GYRO1_FAILURE,
	},
	{
		.name = "diag_accel_failure",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_ACCL_FAILURE,
	},
	{
		.name = "diag_power_supply_failure",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_POWER_SUPPLY_FAILURE,
	},
	{
		.name = "diag_bootloader_failure",
		.show = adis_iio_read_debug_attrs,
		.priv = ADIS_DIAG_BOOT_MEMORY_FAILURE,
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
		.priv = ADIS_GYRO_MEAS_RANGE,
	},
	{
		.name = "sync_mode_select",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_SYNC_MODE,
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
		.name = "sensor_self_test",
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_CMD_SNSR_SELF_TEST,
	},
	{
		.name = "fifo_flush",
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_CMD_FIFO_FLUSH,
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
		.name = "external_clock_frequency",
		.show = adis_iio_read_debug_attrs,
		.store = adis_iio_write_debug_attrs,
		.priv = ADIS_EXT_CLK_FREQ,
	},
	END_ATTRIBUTES_ARRAY
};

static struct iio_device adis16607_iio_dev = {
	.num_ch 		= NO_OS_ARRAY_SIZE(adis16607_channels),
	.channels 		= adis16607_channels,
	.debug_attributes 	= adis16607_debug_attrs,
	.attributes		= adis_dev_attrs,
	.pre_enable 		= (int32_t (*)())adis16607_iio_pre_enable,
	.post_disable 		= (int32_t (*)())adis16607_iio_post_disable,
	.trigger_handler 	= (int32_t (*)())adis16607_iio_trigger_handler,
	.debug_reg_read 	= (int32_t (*)())adis_iio_read_reg,
	.debug_reg_write 	= (int32_t (*)())adis_iio_write_reg,
};

/**
 * @brief Initialize adis16607 iio device.
 * @param iio_dev    - The adis16607 iio device.
 * @param init_param - The structure that contains the device initial parameters.
 * @param adis16607_trig_desc - Trigger descriptor for data ready pin.
 * @return 0 in case of success, error code otherwise.
 */
int adis16607_iio_init(struct adis_iio_dev **iio_dev,
		       struct adis_init_param *init_param,
		       struct iio_hw_trig *adis16607_trig_desc)
{
	int ret;
	struct adis_iio_dev *desc;

	desc = (struct adis_iio_dev *)no_os_calloc(1, sizeof(*desc));
	if (!desc)
		return -ENOMEM;

	desc->iio_dev = (struct iio_device *)no_os_calloc(1, sizeof(*desc->iio_dev));
	if (!desc->iio_dev) {
		ret = -ENOMEM;
		goto error_adis16607_init;
	}
	*desc->iio_dev = adis16607_iio_dev;

	if (init_param->use_fifo) {
		desc->iio_dev->trigger_handler = (int32_t (*)())
						 adis16607_iio_trigger_handler_with_fifo;
		desc->iio_dev->pre_enable = (int32_t (*)())adis16607_iio_pre_enable_with_fifo;
		desc->iio_dev->post_disable = (int32_t (*)())
					      adis16607_iio_post_disable_with_fifo;
		desc->has_fifo = true;
	} else {
		if (init_param->comm_type == ADIS_SPI_COMM
		    && init_param->duplex_type == ADIS_SPI_FULL_DUPLEX) {
			desc->iio_dev->trigger_handler = (int32_t (*)())
							 adis16607_iio_trigger_handler_no_burst;
			desc->iio_dev->pre_enable = (int32_t (*)())adis16607_iio_pre_enable_no_burst;
			desc->iio_dev->post_disable = (int32_t (*)())
						      adis16607_iio_post_disable_no_burst;
		}
		desc->has_fifo = false;
	}
	desc->hw_trig_desc = adis16607_trig_desc;

	/* Update data based on the device id */
	desc->rang_mdl_txt = adis16607_rang_mdl_txt[ADIS16607_ID_NO_OFFSET(
				     init_param->dev_id)];

	ret = adis_init(&desc->adis_dev, init_param);
	if (ret)
		goto error_adis16607_init;

	*iio_dev = desc;

	return 0;

error_adis16607_init:
	no_os_free(desc->iio_dev);
	no_os_free(desc);
	return ret;
}

/**
 * @brief Remove adis16607 iio device.
 * @param desc - The adis16607 iio device.
 */
void adis16607_iio_remove(struct adis_iio_dev *desc)
{
	if (!desc)
		return;
	adis_remove(desc->adis_dev);
	no_os_free(desc->iio_dev);
	no_os_free(desc);
}
