/***************************************************************************//**
 *   @file   adis16607.c
 *   @brief  Implementation of adis16607.c
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

#include "adis.h"
#include "adis_internals.h"
#include "adis16607.h"
#include "no_os_units.h"
#include "no_os_delay.h"
#include <string.h>

#define ADIS16607_MSG_SIZE_16_BIT_BURST		28 /* in bytes: DIAG(2) + 12*2 + TEMP(2) + DATA_CNTR(2) + CRC(2) */
#define ADIS16607_MSG_SIZE_32_BIT_BURST		52 /* in bytes: 12*4 + TEMP(2) + DATA_CNTR(2) + CHECKSUM(2) */
#define ADIS16607_READ_BURST_DATA_CMD_SIZE	4
#define ADIS16607_READ_BURST_DATA_CMD_MSB	0x85

#define ADIS16607_USER_FIFO_CFG_REG		0x35
#define ADIS16607_FIFO_THR_MASK			NO_OS_GENMASK(10, 0)
#define ADIS16607_FIFO_DATA_REG			0x29
#define ADIS16607_FIFO_CH_INDEX_REG		0x2A
#define ADIS16607_FIFO_DATA_CMD			0xA9	/* FIFO_DATA (0x29) | read bit (0x80) */
#define ADIS16607_DIGITAL_STATUS_REG		0x4E
#define ADIS16607_BOOTLOADER_BUSY_MASK		NO_OS_BIT(0)

#define ADIS16607_SELF_TEST_REG(x)		((x) + 0x23)
#define ADIS16607_ST2_SELF_TEST_FORCE_MASK	NO_OS_BIT(7)
#define ADIS16607_ACCEL_XY_DELTA_CHECK_VAL_MAX	260
#define ADIS16607_ACCEL_Z_DELTA_CHECK_VAL_MAX	4000
#define ADIS16607_GYRO_DELTA_CHECK_VAL_MAX	2600

#define ADIS16607_LOCK_SPI_FULLDUPLEX		0xB3B3
#define ADIS16607_SPI_FULLDUPLEX_REG		0x31
#define ADIS16607_LOCK_SPI_HALFDUPLEX		0xB4B4
#define ADIS16607_SPI_HALFDUPLEX_REG		0x32
#define ADIS16607_GPIO_INIT_RESETB_ACTIVE	0x01
#define ADIS16607_GPIO_INIT_DATA_READY_ACTIVE	0x200

static const struct adis_data_field_map_def adis16607_def = {
	.x_accl 		 = {.reg_addr = 0x06, .reg_size = 0x04, .field_mask = 0x00FFFFFF},
	.y_accl 		 = {.reg_addr = 0x08, .reg_size = 0x04, .field_mask = 0x00FFFFFF},
	.z_accl 		 = {.reg_addr = 0x0A, .reg_size = 0x04, .field_mask = 0x00FFFFFF},
	.x_gyro 		 = {.reg_addr = 0x0C, .reg_size = 0x04, .field_mask = 0x00FFFFFF},
	.y_gyro 		 = {.reg_addr = 0x0E, .reg_size = 0x04, .field_mask = 0x00FFFFFF},
	.z_gyro 		 = {.reg_addr = 0x10, .reg_size = 0x04, .field_mask = 0x00FFFFFF},
	.x_deltvel 		 = {.reg_addr = 0x12, .reg_size = 0x04, .field_mask = 0x00FFFFFF},
	.y_deltvel 		 = {.reg_addr = 0x14, .reg_size = 0x04, .field_mask = 0x00FFFFFF},
	.z_deltvel 		 = {.reg_addr = 0x16, .reg_size = 0x04, .field_mask = 0x00FFFFFF},
	.x_deltang 		 = {.reg_addr = 0x18, .reg_size = 0x04, .field_mask = 0x00FFFFFF},
	.y_deltang 		 = {.reg_addr = 0x1A, .reg_size = 0x04, .field_mask = 0x00FFFFFF},
	.z_deltang 		 = {.reg_addr = 0x1C, .reg_size = 0x04, .field_mask = 0x00FFFFFF},
	.temp_out 		 = {.reg_addr = 0x20, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.time_stamp 		 = {.reg_addr = 0x21, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.data_cntr 		 = {.reg_addr = 0x22, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.fifo_cnt		 = {.reg_addr = 0x2B, .reg_size = 0x02, .field_mask = 0x000003FF},
	.fifo_en		 = {.reg_addr = 0x35, .reg_size = 0x02, .field_mask = 0x000007FF},
	.write_lock		 = {.reg_addr = 0x2D, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.dr_enable		 = {.reg_addr = 0x2F, .reg_size = 0x02, .field_mask = 0x00000200},
	.sync_mode		 = {.reg_addr = 0x33, .reg_size = 0x02, .field_mask = 0x00008000},
	.up_scale		 = {.reg_addr = 0x33, .reg_size = 0x02, .field_mask = 0x00007FFF},
	.burst32		 = {.reg_addr = 0x34, .reg_size = 0x02, .field_mask = 0x00008000},
	.fifo_flush		 = {.reg_addr = 0x35, .reg_size = 0x02, .field_mask = 0x00008000},
	.sw_res			 = {.reg_addr = 0x36, .reg_size = 0x02, .field_mask = 0x00000001},
	.dec_rate		 = {.reg_addr = 0x3A, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.xg_bias 		 = {.reg_addr = 0x3E, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.yg_bias 		 = {.reg_addr = 0x3F, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.zg_bias 		 = {.reg_addr = 0x40, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.xa_bias 		 = {.reg_addr = 0x3B, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.ya_bias 		 = {.reg_addr = 0x3C, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.za_bias 		 = {.reg_addr = 0x3D, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.snsr_self_test		 = {.reg_addr = 0x39, .reg_size = 0x02, .field_mask = 0x00000040},
	.firm_rev		 = {.reg_addr = 0x01, .reg_size = 0x02, .field_mask = 0x0000FF00},
	.prod_id		 = {.reg_addr = 0x00, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.serial_num		 = {.reg_addr = 0x02, .reg_size = 0x04, .field_mask = 0xFFFFFFFF},
	.diag_stat		 = {.reg_addr = 0x05, .reg_size = 0x02, .field_mask = 0x0000FFFF},
	.diag_gyro1_failure_mask = NO_OS_BIT(13),
	.diag_accl_failure_mask	 = NO_OS_BIT(12),
	.diag_power_supply_failure_mask = NO_OS_BIT(11),
	.diag_boot_memory_failure_mask = NO_OS_BIT(9),
};

static const struct adis_timeout adis16607_timeouts = {
	.reset_ms 			= 130,
	.self_test_ms 			= 15,
	.sw_reset_ms 			= 50,
	.dec_rate_update_us		= 125,
};

static const struct adis_clk_freq_limit adis16607_sync_clk_freq_limits[] = {
	[ADIS_SYNC_DEFAULT] = { },
	[ADIS_SYNC_DIRECT] = { 401, 8000},
	[ADIS_SYNC_SCALED] = { 1, 400 },
};

static const struct adis_clk_freq_limit adis16607_sampling_clk_limits = {
	.min_freq = 1,
	.max_freq = 8000,
};

/* rad/s */
static const struct adis_scale_members adis16607_anglvel_scale[] = {
	[ADIS16607_ID_NO_OFFSET(ADIS16607_2)] = {DEGREE_TO_RAD(450), 30000 << 8},
	[ADIS16607_ID_NO_OFFSET(ADIS16607_3)] = {DEGREE_TO_RAD(2000), 31250 << 8},
};

/* m/s^2 */
static const struct adis_scale_members adis16607_accl_scale[] = {
	[ADIS16607_ID_NO_OFFSET(ADIS16607_2)] = {G_TO_M_S_2(40), 31250 << 8},
	[ADIS16607_ID_NO_OFFSET(ADIS16607_3)] = {G_TO_M_S_2(40), 31250 << 8},
};

/* Milli-degrees Celsius for temperature */
static const struct adis_scale_members adis16607_temp_scale[] = {
	[ADIS16607_ID_NO_OFFSET(ADIS16607_2)] = {1 * MILLIDEGREE_PER_DEGREE, 200},
	[ADIS16607_ID_NO_OFFSET(ADIS16607_3)] = {1 * MILLIDEGREE_PER_DEGREE, 200},
};

/* Milli-degrees Celsius for temperature */
static const int adis16607_temp_offset[] = {
	[ADIS16607_ID_NO_OFFSET(ADIS16607_2)] = 5000,
	[ADIS16607_ID_NO_OFFSET(ADIS16607_3)] = 5000,
};

/**
 * @brief Check if the delta between two values is within specified range.
 * @param val1 - First value to compare.
 * @param val2 - Second value to compare.
 * @param delta_max - Maximum allowed delta value.
 * @return true if delta is within range, false otherwise.
 */
static bool adis16607_delta_check(int16_t val1, int16_t val2, int16_t delta_max)
{
	int16_t delta_val;

	if (val1 > val2)
		delta_val = val1 - val2;
	else
		delta_val = val2 - val1;

	if (delta_val > delta_max)
		return false;

	return true;
}

/**
 * @brief Calculate CRC4 checksum for data array.
 * @param data - Pointer to data array.
 * @param len - Length of data array.
 * @return Calculated CRC4 value.
 */
static uint8_t adis16607_crc4(const uint8_t *data, uint8_t len)
{
	uint8_t crc = 0xA;
	int bit_length = 8;

	for (uint8_t i = 0; i < len; i++) {
		uint8_t current_byte = data[i];
		if (i == 3)
			bit_length = 4;

		for (int bit = 0; bit < bit_length; bit++) {
			uint8_t bit_in = (current_byte >> (7 - bit)) & 1;
			uint8_t bit_out = (crc >> 3) & 1;

			crc = (crc << 1) | bit_in;

			if (bit_out)
				crc ^= 0x01;
			crc &= 0xF;
		}
	}

	return crc & 0xF;
}

/**
 * @brief Perform sensor self-test for ADIS16607.
 * @param adis - The adis device.
 * @return 0 in case of success, error code otherwise.
 */
static int adis16607_snsr_self_test(struct adis_dev *adis)
{
	uint32_t st1_val[6], st2_val[6];
	int16_t delta_max[6] = {
		ADIS16607_ACCEL_XY_DELTA_CHECK_VAL_MAX,
		ADIS16607_ACCEL_XY_DELTA_CHECK_VAL_MAX,
		ADIS16607_ACCEL_Z_DELTA_CHECK_VAL_MAX,
		ADIS16607_GYRO_DELTA_CHECK_VAL_MAX,
		ADIS16607_GYRO_DELTA_CHECK_VAL_MAX,
		ADIS16607_GYRO_DELTA_CHECK_VAL_MAX,
	};
	int ret;

	ret = adis_write_field_u32(adis, adis->info->field_map->snsr_self_test, 1);
	if (ret)
		return ret;

	no_os_mdelay(adis->info->timeouts->self_test_ms);

	for (int i = 0; i < 6; i++) {
		ret = adis_read_reg(adis, ADIS16607_SELF_TEST_REG(i), &st1_val[i], 0x02);
		if (ret)
			return ret;
	}

	ret = adis_update_bits_base(adis,
				    adis->info->field_map->snsr_self_test.reg_addr,
				    ADIS16607_ST2_SELF_TEST_FORCE_MASK, 1, 0x02);
	if (ret)
		return ret;

	no_os_mdelay(adis->info->timeouts->self_test_ms);

	for (int i = 0; i < 6; i++) {
		ret = adis_read_reg(adis, ADIS16607_SELF_TEST_REG(i), &st2_val[i], 0x02);
		if (ret)
			return ret;

		if (!adis16607_delta_check((int16_t)st1_val[i], (int16_t)st2_val[i],
					   delta_max[i]))
			return -EINVAL;
	}

	return 0;
}

/**
 * @brief Get scale values for specified channel type.
 * @param adis - The adis device.
 * @param scale_m1 - Pointer to store scale multiplier 1.
 * @param scale_m2 - Pointer to store scale multiplier 2.
 * @param chan_type - Channel type to get scale for.
 * @return 0 in case of success, error code otherwise.
 */
static int adis16607_get_scale(struct adis_dev *adis,
			       uint32_t *scale_m1, uint32_t *scale_m2,
			       enum adis_chan_type chan_type)
{
	uint32_t dec_rate;
	int ret;

	switch (chan_type) {
	case ADIS_ACCL_CHAN:
		*scale_m1 = adis16607_accl_scale[ADIS16607_ID_NO_OFFSET(adis->dev_id)].scale_m1;
		*scale_m2 = adis16607_accl_scale[ADIS16607_ID_NO_OFFSET(adis->dev_id)].scale_m2;
		return 0;
	case ADIS_GYRO_CHAN:
		*scale_m1 = adis16607_anglvel_scale[ADIS16607_ID_NO_OFFSET(
				adis->dev_id)].scale_m1;
		*scale_m2 = adis16607_anglvel_scale[ADIS16607_ID_NO_OFFSET(
				adis->dev_id)].scale_m2;
		return 0;
	case ADIS_TEMP_CHAN:
		*scale_m1 = adis16607_temp_scale[ADIS16607_ID_NO_OFFSET(adis->dev_id)].scale_m1;
		*scale_m2 = adis16607_temp_scale[ADIS16607_ID_NO_OFFSET(adis->dev_id)].scale_m2;
		return 0;
	case ADIS_DELTAANGL_CHAN:
		ret = adis_read_dec_rate(adis, &dec_rate);
		if (ret)
			return ret;

		*scale_m1 = adis16607_anglvel_scale[ADIS16607_ID_NO_OFFSET(
				adis->dev_id)].scale_m1 * 8192ULL * (dec_rate + 1);
		*scale_m2 = 22032000ULL;

		return 0;
	case ADIS_DELTAVEL_CHAN:
		ret = adis_read_dec_rate(adis, &dec_rate);
		if (ret)
			return ret;

		*scale_m1 = adis16607_accl_scale[ADIS16607_ID_NO_OFFSET(
				adis->dev_id)].scale_m1 * 8192ULL * (dec_rate + 1);
		*scale_m2 = 22032000ULL;

		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Get offset value for specified channel type.
 * @param adis - The adis device.
 * @param offset - Pointer to store offset value.
 * @param chan_type - Channel type to get offset for.
 * @return 0 in case of success, error code otherwise.
 */
static int adis16607_get_offset(struct adis_dev *adis, int *offset,
				enum adis_chan_type chan_type)
{
	switch (chan_type) {
	case ADIS_TEMP_CHAN:
		*offset = adis16607_temp_offset[ADIS16607_ID_NO_OFFSET(adis->dev_id)];
		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Read burst data.
 * @param adis      - The adis device.
 * @param data      - The burst read data structure to be populated.
 * @param burst32   - True if 32-bit data is requested for accel
 *		      and gyro (or delta angle and delta velocity)
 *		      measurements, false if 16-bit data is requested.
 * @param burst_sel - 0 if accel and gyro data is requested, 1
 *		      if delta angle and delta velocity is requested.
 * @param fifo_pop  - In case FIFO is present, will pop the fifo if
 * 		      true. Unused if FIFO is not present.
 * @param crc_check - If true CRC will be checked, if false check will be skipped.
 * @return 0 in case of success, error code otherwise.
 * -EAGAIN in case the request has to be sent again due to data being unavailable
 * at the time of the request.
 */
static int adis16607_read_burst_data(struct adis_dev *adis,
				     struct adis_burst_data *data,
				     bool burst32, uint8_t burst_sel, bool fifo_pop, bool crc_check)
{
	uint16_t msw, lsw;
	uint8_t buffer[58] = { 0 };
	uint8_t msg_size = ADIS16607_MSG_SIZE_16_BIT_BURST;
	uint8_t axis_data_offset;
	uint8_t data_cntr_offset;
	uint8_t axis_data_size;
	uint8_t temp_offset;
	uint8_t idx;
	int ret = 0;

	/** SPI Full-Duplex Mode is not supported for burst read. */
	if (adis->comm_type == ADIS_SPI_COMM &&
	    adis->duplex_type == ADIS_SPI_FULL_DUPLEX)
		return -EOPNOTSUPP;

	if (adis->info->flags & ADIS_HAS_BURST32) {
		if (adis->burst32 != burst32) {
			ret = adis_write_burst32(adis, burst32);
			if (ret)
				return ret;
			ret = -EAGAIN;
		}
	}

	/* If burst32 or burst select has changed, wait for the next reading
	   request to actually read the data, because the according data will be available
	   only after the next data ready impulse. */
	if (ret == -EAGAIN)
		return ret;

	if (burst32)
		msg_size = ADIS16607_MSG_SIZE_32_BIT_BURST;

	if (fifo_pop)
		msg_size -= 2; /* First 2 bytes are actual data in FIFO mode. */

	/*
	 * Use FIFO_DATA command (0xA9) for FIFO pop, or regular burst
	 * command (0x85) for normal burst read.
	 */
	if (fifo_pop)
		buffer[0] = ADIS16607_FIFO_DATA_CMD;
	else
		buffer[0] = ADIS16607_READ_BURST_DATA_CMD_MSB;

	if (adis->comm_type == ADIS_I2C_COMM) {
		/* I2C burst read: send command byte, then read data */
		ret = no_os_i2c_write(adis->i2c_desc, buffer, 1, 0);
		if (ret)
			return ret;

		ret = no_os_i2c_read(adis->i2c_desc, buffer, msg_size, 1);
		if (ret)
			return ret;

		/* I2C burst data starts at buffer[0], no command offset */
		for (idx = 0; idx < msg_size; idx++)
			if (buffer[idx] != 0)
				break;

		if (idx == msg_size)
			return -EAGAIN;
	} else {
		/* SPI Half-Duplex: transfer needs to read diag stat and hold CSB pin
		down low for reading the rest of the registers. */
		ret = no_os_spi_write_and_read(adis->spi_desc, buffer,
					       msg_size + ADIS16607_READ_BURST_DATA_CMD_SIZE);
		if (ret)
			return ret;

		for (idx = ADIS16607_READ_BURST_DATA_CMD_SIZE;
		     idx < msg_size + ADIS16607_READ_BURST_DATA_CMD_SIZE; idx++)
			if (buffer[idx] != 0)
				break;

		if (idx == msg_size + ADIS16607_READ_BURST_DATA_CMD_SIZE)
			return -EAGAIN;
	}

	adis->diag_flags.checksum_err = false;

	if (adis->comm_type == ADIS_I2C_COMM) {
		/* I2C burst data starts at buffer[0], no command offset. */
		axis_data_offset = 0;
		axis_data_size = burst32 ? 48 : 24;
		temp_offset = axis_data_offset + axis_data_size;
		data_cntr_offset = temp_offset + 2;
	} else if (fifo_pop) {
		if (burst32) {
			axis_data_size = 48;
			axis_data_offset = ADIS16607_READ_BURST_DATA_CMD_SIZE - 2;
			temp_offset = axis_data_offset + axis_data_size;      /* 2 + 48 = 50 */
			data_cntr_offset = temp_offset + 2;                   /* 52 */
		} else {
			axis_data_size = 24;
			axis_data_offset = ADIS16607_READ_BURST_DATA_CMD_SIZE - 2;
			temp_offset = axis_data_offset + axis_data_size;      /* 2 + 24 = 26 */
			data_cntr_offset = temp_offset + 2;                   /* 28 */
		}
	} else {
		if (burst32) {
			axis_data_size = 48;
			axis_data_offset = ADIS16607_READ_BURST_DATA_CMD_SIZE;
			temp_offset = axis_data_offset + axis_data_size;      /* 4 + 48 = 52 */
			data_cntr_offset = temp_offset + 2;                   /* 54 */
		} else {
			axis_data_size = 24;
			axis_data_offset = ADIS16607_READ_BURST_DATA_CMD_SIZE;
			temp_offset = axis_data_offset + axis_data_size;      /* 4 + 24 = 28 */
			data_cntr_offset = temp_offset + 2;                   /* 30 */
		}
	}

	if (burst32) {
		msw = no_os_get_unaligned_be16(&buffer[axis_data_offset]);
		lsw = no_os_get_unaligned_be16(&buffer[axis_data_offset + 2]);
		data->x_accel_msb = (msw >> 8) & 0xFF;
		data->x_accel_lsb = ((lsw >> 8) & 0xFF) | ((msw & 0xFF) << 8);

		msw = no_os_get_unaligned_be16(&buffer[axis_data_offset + 4]);
		lsw = no_os_get_unaligned_be16(&buffer[axis_data_offset + 6]);
		data->y_accel_msb = (msw >> 8) & 0xFF;
		data->y_accel_lsb = ((lsw >> 8) & 0xFF) | ((msw & 0xFF) << 8);

		msw = no_os_get_unaligned_be16(&buffer[axis_data_offset + 8]);
		lsw = no_os_get_unaligned_be16(&buffer[axis_data_offset + 10]);
		data->z_accel_msb = (msw >> 8) & 0xFF;
		data->z_accel_lsb = ((lsw >> 8) & 0xFF) | ((msw & 0xFF) << 8);

		msw = no_os_get_unaligned_be16(&buffer[axis_data_offset + 12]);
		lsw = no_os_get_unaligned_be16(&buffer[axis_data_offset + 14]);
		data->x_gyro_msb = (msw >> 8) & 0xFF;
		data->x_gyro_lsb = ((lsw >> 8) & 0xFF) | ((msw & 0xFF) << 8);

		msw = no_os_get_unaligned_be16(&buffer[axis_data_offset + 16]);
		lsw = no_os_get_unaligned_be16(&buffer[axis_data_offset + 18]);
		data->y_gyro_msb = (msw >> 8) & 0xFF;
		data->y_gyro_lsb = ((lsw >> 8) & 0xFF) | ((msw & 0xFF) << 8);

		msw = no_os_get_unaligned_be16(&buffer[axis_data_offset + 20]);
		lsw = no_os_get_unaligned_be16(&buffer[axis_data_offset + 22]);
		data->z_gyro_msb = (msw >> 8) & 0xFF;
		data->z_gyro_lsb = ((lsw >> 8) & 0xFF) | ((msw & 0xFF) << 8);

		msw = no_os_get_unaligned_be16(&buffer[axis_data_offset + 24]);
		lsw = no_os_get_unaligned_be16(&buffer[axis_data_offset + 26]);
		data->x_deltvel_msb = (msw >> 8) & 0xFF;
		data->x_deltvel_lsb = ((lsw >> 8) & 0xFF) | ((msw & 0xFF) << 8);

		msw = no_os_get_unaligned_be16(&buffer[axis_data_offset + 28]);
		lsw = no_os_get_unaligned_be16(&buffer[axis_data_offset + 30]);
		data->y_deltvel_msb = (msw >> 8) & 0xFF;
		data->y_deltvel_lsb = ((lsw >> 8) & 0xFF) | ((msw & 0xFF) << 8);

		msw = no_os_get_unaligned_be16(&buffer[axis_data_offset + 32]);
		lsw = no_os_get_unaligned_be16(&buffer[axis_data_offset + 34]);
		data->z_deltvel_msb = (msw >> 8) & 0xFF;
		data->z_deltvel_lsb = ((lsw >> 8) & 0xFF) | ((msw & 0xFF) << 8);

		msw = no_os_get_unaligned_be16(&buffer[axis_data_offset + 36]);
		lsw = no_os_get_unaligned_be16(&buffer[axis_data_offset + 38]);
		data->x_deltang_msb = (msw >> 8) & 0xFF;
		data->x_deltang_lsb = ((lsw >> 8) & 0xFF) | ((msw & 0xFF) << 8);

		msw = no_os_get_unaligned_be16(&buffer[axis_data_offset + 40]);
		lsw = no_os_get_unaligned_be16(&buffer[axis_data_offset + 42]);
		data->y_deltang_msb = (msw >> 8) & 0xFF;
		data->y_deltang_lsb = ((lsw >> 8) & 0xFF) | ((msw & 0xFF) << 8);

		msw = no_os_get_unaligned_be16(&buffer[axis_data_offset + 44]);
		lsw = no_os_get_unaligned_be16(&buffer[axis_data_offset + 46]);
		data->z_deltang_msb = (msw >> 8) & 0xFF;
		data->z_deltang_lsb = ((lsw >> 8) & 0xFF) | ((msw & 0xFF) << 8);
	} else {
		data->x_accel_lsb = 0;
		data->x_accel_msb = no_os_get_unaligned_be16(&buffer[axis_data_offset]);
		data->y_accel_lsb = 0;
		data->y_accel_msb = no_os_get_unaligned_be16(&buffer[axis_data_offset + 2]);
		data->z_accel_lsb = 0;
		data->z_accel_msb = no_os_get_unaligned_be16(&buffer[axis_data_offset + 4]);
		data->x_gyro_lsb = 0;
		data->x_gyro_msb = no_os_get_unaligned_be16(&buffer[axis_data_offset + 6]);
		data->y_gyro_lsb = 0;
		data->y_gyro_msb = no_os_get_unaligned_be16(&buffer[axis_data_offset + 8]);
		data->z_gyro_lsb = 0;
		data->z_gyro_msb = no_os_get_unaligned_be16(&buffer[axis_data_offset + 10]);
		data->x_deltvel_lsb = 0;
		data->x_deltvel_msb = no_os_get_unaligned_be16(&buffer[axis_data_offset + 12]);
		data->y_deltvel_lsb = 0;
		data->y_deltvel_msb = no_os_get_unaligned_be16(&buffer[axis_data_offset + 14]);
		data->z_deltvel_lsb = 0;
		data->z_deltvel_msb = no_os_get_unaligned_be16(&buffer[axis_data_offset + 16]);
		data->x_deltang_lsb = 0;
		data->x_deltang_msb = no_os_get_unaligned_be16(&buffer[axis_data_offset + 18]);
		data->y_deltang_lsb = 0;
		data->y_deltang_msb = no_os_get_unaligned_be16(&buffer[axis_data_offset + 20]);
		data->z_deltang_lsb = 0;
		data->z_deltang_msb = no_os_get_unaligned_be16(&buffer[axis_data_offset + 22]);
	}

	data->temp_msb = 0;
	/* Temp data */
	data->temp_lsb = no_os_get_unaligned_be16(&buffer[temp_offset]);
	/* Counter data - aligned */
	data->data_cntr_lsb = no_os_get_unaligned_be16(&buffer[data_cntr_offset]);
	data->data_cntr_msb = 0;

	/*
	 * DIAG_STAT is returned during the SPI command phase at bytes 2-3;
	 * this framing does not apply to I2C.
	 */
	if (adis->comm_type == ADIS_SPI_COMM && !fifo_pop)
		adis_update_diag_flags(adis, no_os_get_unaligned_be16(&buffer[2]));

	return 0;
}

/**
 * @brief Read synchronization mode encoded value.
 * @param adis      - The adis device.
 * @param sync_mode - The synchronization mode encoded value.
 * @return 0 in case of success, error code otherwise.
 */
static int adis16607_read_sync_mode(struct adis_dev *adis, uint32_t *sync_mode)
{
	uint32_t reg_val;
	int ret;

	ret = adis_read_field_u32(adis, adis->info->field_map->sync_mode, sync_mode);
	if (ret)
		return ret;

	ret = adis_read_reg(adis, ADIS16607_USER_GPIO_CFG_REG, &reg_val, 0x02);
	if (ret)
		return ret;

	if (reg_val & ADIS16607_SYNC_GPIO_MASK)
		*sync_mode += 1;
	else
		*sync_mode = 0;

	return 0;
}

/**
 * @brief Update synchronization mode.
 * @param adis      - The adis device.
 * @param sync_mode - The synchronization mode encoded value to update.
 * @param ext_clk   - The external clock frequency to update.
 * @return 0 in case of success, error code otherwise.
 */
static int adis16607_write_sync_mode(struct adis_dev *adis, uint32_t sync_mode,
				     uint32_t ext_clk)
{
	int ret;

	/* Sync pulse is external */
	if (sync_mode != ADIS_SYNC_DEFAULT && sync_mode != ADIS_SYNC_OUTPUT
	    && (ext_clk < adis->info->sync_clk_freq_limits[sync_mode].min_freq
		|| ext_clk > adis->info->sync_clk_freq_limits[sync_mode].max_freq))
		return -EINVAL;

	adis->ext_clk = ext_clk;

	if (sync_mode > adis->info->sync_mode_max)
		return -EINVAL;

	if (sync_mode == ADIS_SYNC_DEFAULT) {
		ret = adis_write_reg(adis, ADIS16607_USER_SYNC_REG, 0, 0x02);
		if (ret)
			return ret;

		return adis_update_bits_base(adis, ADIS16607_USER_GPIO_CFG_REG,
					     ADIS16607_SYNC_GPIO_MASK, 0, 0x02);
	} else {
		ret = adis_update_bits_base(adis, ADIS16607_USER_GPIO_CFG_REG,
					    ADIS16607_SYNC_GPIO_MASK, 1, 0x02);
		if (ret)
			return ret;
	}

	if (sync_mode == ADIS_SYNC_SCALED) {
		/*
		* In sync scaled mode, the IMU sample rate is the
		* clk_freq * sync_scale.
		* Hence, default the IMU sample rate to the highest
		* multiple of the input clock lower than the IMU max
		* sample rate.
		*/
		ret = adis_write_up_scale(adis,
					  adis->info->sampling_clk_limits.max_freq / ext_clk);
		if (ret)
			return ret;
	}

	return adis_write_field_u32(adis, adis->info->field_map->sync_mode,
				    sync_mode - 1);
}

/**
 * @brief Read register via SPI interface.
 * @param dev - The adis device.
 * @param reg - Register address to read.
 * @param val - Pointer to store read value.
 * @param size - Size of register in bytes.
 * @return 0 in case of success, error code otherwise.
 */
static int adis16607_spi_read_reg(struct adis_dev *dev, uint32_t reg,
				  uint32_t *val,
				  uint32_t size)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = dev->tx,
		.rx_buff = dev->rx,
		.bytes_number = 4,
		.cs_change = 1,
	};
	uint32_t val_msw, val_lsw;
	uint8_t crc_data[4];
	int ret;

	switch (dev->duplex_type) {
	case ADIS_SPI_HALF_DUPLEX:
		switch (size) {
		case ADIS_2_BYTES_SIZE:
			dev->tx[0] = reg | NO_OS_BIT(7);
			dev->tx[1] = 0;
			dev->tx[2] = 0;
			dev->tx[3] = 0;
			ret = no_os_spi_transfer(dev->spi_desc, &xfer, 1);
			if (ret)
				return ret;

			ret = no_os_spi_transfer(dev->spi_desc, &xfer, 1);
			if (ret)
				return ret;

			*val = no_os_get_unaligned_be16(&dev->rx[2]);

			break;
		case ADIS_4_BYTES_SIZE:
			dev->tx[0] = reg | NO_OS_BIT(7);
			dev->tx[1] = 0;
			dev->tx[2] = 0;
			dev->tx[3] = 0;

			ret = no_os_spi_transfer(dev->spi_desc, &xfer, 1);
			if (ret)
				return ret;

			ret = no_os_spi_transfer(dev->spi_desc, &xfer, 1);
			if (ret)
				return ret;

			val_msw = no_os_get_unaligned_be16(&dev->rx[2]);

			dev->tx[0] = (reg + 1) | NO_OS_BIT(7);
			dev->tx[1] = 0;
			dev->tx[2] = 0;
			dev->tx[3] = 0;

			ret = no_os_spi_transfer(dev->spi_desc, &xfer, 1);
			if (ret)
				return ret;

			ret = no_os_spi_transfer(dev->spi_desc, &xfer, 1);
			if (ret)
				return ret;

			val_lsw = dev->rx[2];

			*val = ((val_msw << 8) & 0xFFFF00) | val_lsw;

			break;
		default:
			return -EINVAL;
		}

		return 0;
	case ADIS_SPI_FULL_DUPLEX:
		switch (size) {
		case ADIS_2_BYTES_SIZE:
			dev->tx[0] = 0;
			dev->tx[1] = 0;
			dev->tx[2] = reg;
			dev->tx[3] = 0x0;
			crc_data[0] = dev->tx[0];
			crc_data[1] = dev->tx[1];
			crc_data[2] = dev->tx[2];
			crc_data[3] = dev->tx[3] & 0xF0;
			dev->tx[3] = adis16607_crc4(crc_data, 4);

			ret = no_os_spi_transfer(dev->spi_desc, &xfer, 1);
			if (ret)
				return ret;

			ret = no_os_spi_transfer(dev->spi_desc, &xfer, 1);
			if (ret)
				return ret;

			crc_data[0] = dev->rx[0];
			crc_data[1] = dev->rx[1];
			crc_data[2] = dev->rx[2];
			crc_data[3] = dev->rx[3] & 0xF0;
			if (adis16607_crc4(crc_data, 4) != (dev->rx[3] & 0xF))
				return -EINVAL;

			*val = no_os_get_unaligned_be16(&dev->rx[0]);

			break;
		case ADIS_4_BYTES_SIZE:
			dev->tx[0] = 0;
			dev->tx[1] = 0;
			dev->tx[2] = reg;
			dev->tx[3] = 0x0;
			crc_data[0] = dev->tx[0];
			crc_data[1] = dev->tx[1];
			crc_data[2] = dev->tx[2];
			crc_data[3] = dev->tx[3] & 0xF0;
			dev->tx[3] = adis16607_crc4(crc_data, 4);

			ret = no_os_spi_transfer(dev->spi_desc, &xfer, 1);
			if (ret)
				return ret;

			ret = no_os_spi_transfer(dev->spi_desc, &xfer, 1);
			if (ret)
				return ret;

			crc_data[0] = dev->rx[0];
			crc_data[1] = dev->rx[1];
			crc_data[2] = dev->rx[2];
			crc_data[3] = dev->rx[3] & 0xF0;
			if (adis16607_crc4(crc_data, 4) != (dev->rx[3] & 0xF))
				return -EINVAL;

			val_msw = no_os_get_unaligned_be16(&dev->rx[0]);

			dev->tx[0] = 0;
			dev->tx[1] = 0;
			dev->tx[2] = reg + 1;
			dev->tx[3] = 0x0;
			crc_data[0] = dev->tx[0];
			crc_data[1] = dev->tx[1];
			crc_data[2] = dev->tx[2];
			crc_data[3] = dev->tx[3] & 0xF0;
			dev->tx[3] = adis16607_crc4(crc_data, 4);

			ret = no_os_spi_transfer(dev->spi_desc, &xfer, 1);
			if (ret)
				return ret;

			ret = no_os_spi_transfer(dev->spi_desc, &xfer, 1);
			if (ret)
				return ret;

			crc_data[0] = dev->rx[0];
			crc_data[1] = dev->rx[1];
			crc_data[2] = dev->rx[2];
			crc_data[3] = dev->rx[3] & 0xF0;
			if (adis16607_crc4(crc_data, 4) != (dev->rx[3] & 0xF))
				return -EINVAL;

			val_lsw = dev->rx[0];

			*val = ((val_msw << 8) & 0xFFFF00) | val_lsw;

			break;
		default:
			return -EINVAL;
		}

		return 0;
	default:
		return -EINVAL;
	}

}

/**
 * @brief Write register via SPI interface.
 * @param dev - The adis device.
 * @param reg - Register address to write.
 * @param val - Value to write to register.
 * @param size - Size of register in bytes.
 * @return 0 in case of success, error code otherwise.
 */
static int adis16607_spi_write_reg(struct adis_dev *dev, uint32_t reg,
				   uint32_t val,
				   uint32_t size)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = dev->tx,
		.rx_buff = dev->rx,
		.bytes_number = 4,
		.cs_change = 1,
	};
	uint8_t crc_data[4] = { 0 };

	if (size != ADIS_2_BYTES_SIZE || val >= NO_OS_BIT(16))
		return -EINVAL;

	switch (dev->duplex_type) {
	case ADIS_SPI_HALF_DUPLEX:
		xfer.bytes_number = 3;
		dev->tx[0] = reg;
		dev->tx[1] = (val >> 8) & 0xFF;
		dev->tx[2] = val & 0xFF;

		return no_os_spi_transfer(dev->spi_desc, &xfer, 1);
	case ADIS_SPI_FULL_DUPLEX:
		dev->tx[0] = (val >> 8) & 0xFF;
		dev->tx[1] = val & 0xFF;
		dev->tx[2] = reg;
		dev->tx[3] = NO_OS_BIT(7);
		crc_data[0] = dev->tx[0];
		crc_data[1] = dev->tx[1];
		crc_data[2] = dev->tx[2];
		crc_data[3] = dev->tx[3] & 0xF0;
		dev->tx[3] |= adis16607_crc4(crc_data, 4) & 0xF;

		return no_os_spi_transfer(dev->spi_desc, &xfer, 1);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Read register via I2C interface.
 * @param dev - The adis device.
 * @param reg - Register address to read.
 * @param val - Pointer to store read value.
 * @param size - Size of register in bytes.
 * @return 0 in case of success, error code otherwise.
 */
static int adis16607_i2c_read_reg(struct adis_dev *dev, uint32_t reg,
				  uint32_t *val,
				  uint32_t size)
{
	uint8_t tx_buf[1];
	uint8_t rx_buf[4];
	uint32_t val_msw, val_lsw;
	int ret;

	switch (size) {
	case ADIS_2_BYTES_SIZE:
		/* Send register address */
		tx_buf[0] = reg | NO_OS_BIT(7);
		ret = no_os_i2c_write(dev->i2c_desc, tx_buf, 1, 0);
		if (ret)
			return ret;

		/* Read 2 bytes */
		ret = no_os_i2c_read(dev->i2c_desc, rx_buf, 2, 1);
		if (ret)
			return ret;

		*val = no_os_get_unaligned_be16(rx_buf);

		break;
	case ADIS_4_BYTES_SIZE:
		/* Send register address for MSW */
		tx_buf[0] = reg | NO_OS_BIT(7);
		ret = no_os_i2c_write(dev->i2c_desc, tx_buf, 1, 0);
		if (ret)
			return ret;

		/* Read MSW (2 bytes) */
		ret = no_os_i2c_read(dev->i2c_desc, rx_buf, 2, 1);
		if (ret)
			return ret;

		val_msw = no_os_get_unaligned_be16(rx_buf);

		/* Send register address for LSW */
		tx_buf[0] = (reg + 1) | NO_OS_BIT(7);
		ret = no_os_i2c_write(dev->i2c_desc, tx_buf, 1, 0);
		if (ret)
			return ret;

		/* Read LSW (2 byte) */
		ret = no_os_i2c_read(dev->i2c_desc, rx_buf, 2, 1);
		if (ret)
			return ret;

		val_lsw = rx_buf[1];

		*val = ((val_msw << 8) & 0xFFFF00) | val_lsw;

		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Write register via I2C interface.
 * @param dev - The adis device.
 * @param reg - Register address to write.
 * @param val - Value to write to register.
 * @param size - Size of register in bytes.
 * @return 0 in case of success, error code otherwise.
 */
static int adis16607_i2c_write_reg(struct adis_dev *dev, uint32_t reg,
				   uint32_t val,
				   uint32_t size)
{
	uint8_t tx_buf[3];

	if (size != ADIS_2_BYTES_SIZE || val >= NO_OS_BIT(16))
		return -EINVAL;

	/* Prepare I2C write buffer: register address + 2 bytes of data */
	tx_buf[0] = reg;
	tx_buf[1] = (val >> 8) & 0xFF;
	tx_buf[2] = val & 0xFF;

	return no_os_i2c_write(dev->i2c_desc, tx_buf, 3, 1);
}

/**
 * @brief Read register value, dispatching to the active communication type.
 * @param dev  - The adis device.
 * @param reg  - The address of the lower of the two registers.
 * @param val  - The value read back from the device.
 * @param size - The size of the val buffer.
 * @return 0 in case of success, error code otherwise.
 */
static int adis16607_read_reg(struct adis_dev *dev, uint32_t reg,
			      uint32_t *val, uint32_t size)
{
	switch (dev->comm_type) {
	case ADIS_SPI_COMM:
		return adis16607_spi_read_reg(dev, reg, val, size);
	case ADIS_I2C_COMM:
		return adis16607_i2c_read_reg(dev, reg, val, size);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Write register value, dispatching to the active communication type.
 * @param dev  - The adis device.
 * @param reg  - The address of the lower of the two registers.
 * @param val  - The value to write.
 * @param size - Size of the register to update.
 * @return 0 in case of success, error code otherwise.
 */
static int adis16607_write_reg(struct adis_dev *dev, uint32_t reg,
			       uint32_t val, uint32_t size)
{
	switch (dev->comm_type) {
	case ADIS_SPI_COMM:
		return adis16607_spi_write_reg(dev, reg, val, size);
	case ADIS_I2C_COMM:
		return adis16607_i2c_write_reg(dev, reg, val, size);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Lock the ADIS16607 into the requested SPI duplex mode.
 * @param adis              - The adis device.
 * @param target_duplex_type - The duplex mode to lock into.
 * @return 0 in case of success, error code otherwise.
 *
 * The duplex lock command has to be issued in Half-Duplex framing
 * regardless of the target duplex mode, otherwise the device won't
 * acknowledge it. Only used once, during initial startup.
 */
static int adis16607_lock_duplex_mode(struct adis_dev *adis,
				      enum adis_spi_duplex_type target_duplex_type)
{
	int ret;

	adis->duplex_type = ADIS_SPI_HALF_DUPLEX;

	switch (target_duplex_type) {
	case ADIS_SPI_HALF_DUPLEX:
		ret = adis_write_reg(adis, ADIS16607_SPI_HALFDUPLEX_REG,
				     ADIS16607_LOCK_SPI_HALFDUPLEX, 0x02);
		break;
	case ADIS_SPI_FULL_DUPLEX:
		ret = adis_write_reg(adis, ADIS16607_SPI_FULLDUPLEX_REG,
				     ADIS16607_LOCK_SPI_FULLDUPLEX, 0x02);
		break;
	default:
		return -EINVAL;
	}

	if (!ret)
		adis->duplex_type = target_duplex_type;

	return ret;
}

/**
 * @brief Perform initial startup sequence for ADIS16607.
 * @param adis - The adis device.
 * @return 0 in case of success, error code otherwise.
 */
static int adis16607_initial_startup(struct adis_dev *adis)
{
	struct adis_diag_flags diag_flags;
	uint32_t temp_val;
	int ret;

	if (adis->comm_type != ADIS_SPI_COMM && adis->comm_type != ADIS_I2C_COMM)
		return -EINVAL;

	/* SPI-specific configuration */
	if (adis->comm_type == ADIS_SPI_COMM) {
		ret = adis16607_lock_duplex_mode(adis, adis->duplex_type);
		if (ret)
			return ret;
	}

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	ret = adis_read_reg(adis, adis->info->field_map->diag_stat.reg_addr, &temp_val,
			    adis->info->field_map->diag_stat.reg_size);
	if (ret)
		return ret;

	/** Check that the DIAG_STAT errors. */
	if (temp_val > 1)
		return -EINVAL;

	ret = adis_read_reg(adis, ADIS16607_DIGITAL_STATUS_REG, &temp_val, 0x02);
	if (ret)
		return ret;

	/** Ensure bootloader is not busy. */
	if (no_os_field_get(ADIS16607_BOOTLOADER_BUSY_MASK, temp_val))
		return -EINVAL;

	/** If RESETB GPIO is used, the device needs GPIO configuration for it. */
	if (adis->gpio_reset) {
		ret = adis_update_bits_base(adis, ADIS16607_USER_GPIO_CFG_REG,
					    ADIS16607_RESET_GPIO_MASK, 1, 2);
		if (ret)
			return ret;
	}

	ret = adis_update_bits_base(adis, ADIS16607_USER_GPIO_CFG_REG,
				    ADIS16607_DATA_READY_GPIO_MASK, 1, 2);
	if (ret)
		return ret;

	/* Enable DATA_CNTR for burst reads */
	ret = adis_update_bits_base(adis, ADIS16607_USER_DATA_CFG_REG,
				    ADIS16607_DATA_CNTR_EN_MASK, 1, 2);
	if (ret)
		return ret;

	ret = adis_write_dr_enable(adis, 0);
	if (ret)
		return ret;

	ret = adis_read_diag_stat(adis, &diag_flags);
	if (ret)
		return ret;

	return adis_read_diag_stat(adis, &diag_flags);
}

/**
 * @brief Enable or disable FIFO mode.
 * @param adis    - The adis device.
 * @param enable  - True to enable FIFO, false to disable.
 * @param thr     - FIFO watermark threshold (1-1024). Ignored if enable is false.
 * @param burst32 - True for 32-bit burst mode, false for 16-bit. Ignored if enable is false.
 * @return 0 in case of success, error code otherwise.
 */
int adis16607_fifo_enable(struct adis_dev *adis, bool enable, uint16_t thr,
			  bool burst32)
{
	uint16_t val;
	int ret;

	if (enable) {
		if (thr == 0 || thr > 2047)
			return -EINVAL;
		val = thr & ADIS16607_FIFO_THR_MASK;

		/* Enable DATA COUNTER. */
		ret = adis_update_bits_base(adis, ADIS16607_USER_DATA_CFG_REG,
					    ADIS16607_DATA_CNTR_EN_MASK, 1, 2);
		if (ret)
			return ret;

		ret = adis_write_burst32(adis, burst32);
		if (ret)
			return ret;

		/* Flush the FIFO for clean start. */
		ret = adis_cmd_fifo_flush(adis);
		if (ret)
			return ret;
	} else {
		val = 0;
	}

	return adis_write_reg(adis, ADIS16607_USER_FIFO_CFG_REG, val, 2);
}

/**
 * @brief Pop one sample from the FIFO (SPI Full-Duplex mode).
 * @param adis    - The adis device.
 * @param data    - The burst read data structure to be populated.
 * @param burst32 - True for 32-bit burst mode, false for 16-bit.
 * @return 0 in case of success, error code otherwise.
 *
 * In SPI Full-Duplex mode, issue repeated read commands to FIFO_DATA register.
 * Each command returns the next word. Full-duplex does not support address
 * auto-increment.
 */
static int adis16607_fifo_pop_full_duplex(struct adis_dev *adis,
		struct adis_burst_data *data,
		bool burst32)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = adis->tx,
		.rx_buff = adis->rx,
		.bytes_number = 4,
		.cs_change = 1,
	};
	uint8_t axis_data_offset = 0;
	uint16_t raw_data[26];
	uint8_t num_words;
	int ret;
	int i;

	/*
	 * Calculate number of 16-bit words to read.
	 * 32-bit mode: 26 words (12 channels × 2 + TEMP + DATA_CNTR)
	 * 16-bit mode: 14 words (12 channels × 1 + TEMP + DATA_CNTR)
	 */
	if (burst32)
		num_words = 26;
	else
		num_words = 14;

	/*
	 * Full-duplex FIFO read: response to command N comes in transfer N+1.
	 * We need num_words + 1 transfers total.
	 * Transfer 0: Send CMD → receive garbage (discard)
	 * Transfer 1: Send CMD → receive word 0
	 * Transfer N: Send CMD → receive word N-1
	 */
	for (i = 0; i < num_words + 1; i++) {
		/* Setup FIFO_DATA read command (no CRC) */
		adis->tx[0] = 0x00;
		adis->tx[1] = 0x00;
		if (i < num_words) {
			/* Send FIFO_DATA read command */
			adis->tx[2] = ADIS16607_FIFO_DATA_REG;
		} else {
			/* Last transfer: send NOP to get final word without popping another */
			adis->tx[2] = ADIS16607_DIAG_STAT_REG;
		}
		adis->tx[3] = 0x00;

		ret = no_os_spi_transfer(adis->spi_desc, &xfer, 1);
		if (ret)
			return ret;

		/* Skip first response (garbage), store the rest */
		if (i > 0)
			raw_data[i - 1] = no_os_get_unaligned_be16(&adis->rx[0]);
	}

	if (burst32) {
		/* ACCEL: 3 channels × 2 words each */
		data->x_accel_msb = (raw_data[axis_data_offset] >> 8) & 0xFF;
		data->x_accel_lsb = ((raw_data[axis_data_offset + 1] >> 8) & 0xFF) |
				    ((raw_data[axis_data_offset] & 0xFF) << 8);
		data->y_accel_msb = (raw_data[axis_data_offset + 2] >> 8) & 0xFF;
		data->y_accel_lsb = ((raw_data[axis_data_offset + 3] >> 8) & 0xFF) |
				    ((raw_data[axis_data_offset + 2] & 0xFF) << 8);
		data->z_accel_msb = (raw_data[axis_data_offset + 4] >> 8) & 0xFF;
		data->z_accel_lsb = ((raw_data[axis_data_offset + 5] >> 8) & 0xFF) |
				    ((raw_data[axis_data_offset + 4] & 0xFF) << 8);
		/* GYRO: 3 channels × 2 words each */
		data->x_gyro_msb = (raw_data[axis_data_offset + 6] >> 8) & 0xFF;
		data->x_gyro_lsb = ((raw_data[axis_data_offset + 7] >> 8) & 0xFF) |
				   ((raw_data[axis_data_offset + 6] & 0xFF) << 8);
		data->y_gyro_msb = (raw_data[axis_data_offset + 8] >> 8) & 0xFF;
		data->y_gyro_lsb = ((raw_data[axis_data_offset + 9] >> 8) & 0xFF) |
				   ((raw_data[axis_data_offset + 8] & 0xFF) << 8);
		data->z_gyro_msb = (raw_data[axis_data_offset + 10] >> 8) & 0xFF;
		data->z_gyro_lsb = ((raw_data[axis_data_offset + 11] >> 8) & 0xFF) |
				   ((raw_data[axis_data_offset + 10] & 0xFF) << 8);
		/* DELTVEL: 3 channels × 2 words each */
		data->x_deltvel_msb = (raw_data[axis_data_offset + 12] >> 8) & 0xFF;
		data->x_deltvel_lsb = ((raw_data[axis_data_offset + 13] >> 8) & 0xFF) |
				      ((raw_data[axis_data_offset + 12] & 0xFF) << 8);
		data->y_deltvel_msb = (raw_data[axis_data_offset + 14] >> 8) & 0xFF;
		data->y_deltvel_lsb = ((raw_data[axis_data_offset + 15] >> 8) & 0xFF) |
				      ((raw_data[axis_data_offset + 14] & 0xFF) << 8);
		data->z_deltvel_msb = (raw_data[axis_data_offset + 16] >> 8) & 0xFF;
		data->z_deltvel_lsb = ((raw_data[axis_data_offset + 17] >> 8) & 0xFF) |
				      ((raw_data[axis_data_offset + 16] & 0xFF) << 8);
		/* DELTANG: 3 channels × 2 words each */
		data->x_deltang_msb = (raw_data[axis_data_offset + 18] >> 8) & 0xFF;
		data->x_deltang_lsb = ((raw_data[axis_data_offset + 19] >> 8) & 0xFF) |
				      ((raw_data[axis_data_offset + 18] & 0xFF) << 8);
		data->y_deltang_msb = (raw_data[axis_data_offset + 20] >> 8) & 0xFF;
		data->y_deltang_lsb = ((raw_data[axis_data_offset + 21] >> 8) & 0xFF) |
				      ((raw_data[axis_data_offset + 20] & 0xFF) << 8);
		data->z_deltang_msb = (raw_data[axis_data_offset + 22] >> 8) & 0xFF;
		data->z_deltang_lsb = ((raw_data[axis_data_offset + 23] >> 8) & 0xFF) |
				      ((raw_data[axis_data_offset + 22] & 0xFF) << 8);
		/* TEMP, DATA_CNTR */
		data->temp_msb = 0;
		data->temp_lsb = raw_data[24];
		data->data_cntr_msb = 0;
		data->data_cntr_lsb = raw_data[25];
	} else {
		data->x_accel_lsb = 0;
		data->x_accel_msb = raw_data[axis_data_offset];
		data->y_accel_lsb = 0;
		data->y_accel_msb = raw_data[axis_data_offset + 1];
		data->z_accel_lsb = 0;
		data->z_accel_msb = raw_data[axis_data_offset + 2];
		data->x_gyro_lsb = 0;
		data->x_gyro_msb = raw_data[axis_data_offset + 3];
		data->y_gyro_lsb = 0;
		data->y_gyro_msb = raw_data[axis_data_offset + 4];
		data->z_gyro_lsb = 0;
		data->z_gyro_msb = raw_data[axis_data_offset + 5];
		data->x_deltvel_lsb = 0;
		data->x_deltvel_msb = raw_data[axis_data_offset + 6];
		data->y_deltvel_lsb = 0;
		data->y_deltvel_msb = raw_data[axis_data_offset + 7];
		data->z_deltvel_lsb = 0;
		data->z_deltvel_msb = raw_data[axis_data_offset + 8];
		data->x_deltang_lsb = 0;
		data->x_deltang_msb = raw_data[axis_data_offset + 9];
		data->y_deltang_lsb = 0;
		data->y_deltang_msb = raw_data[axis_data_offset + 10];
		data->z_deltang_lsb = 0;
		data->z_deltang_msb = raw_data[axis_data_offset + 11];
		data->temp_msb = 0;
		data->temp_lsb = raw_data[12];
		data->data_cntr_msb = 0;
		data->data_cntr_lsb = raw_data[13];
	}

	return 0;
}

/**
 * @brief Pop one sample from the FIFO.
 * @param adis - The adis device.
 * @param data - The burst read data structure to be populated.
 * @return 0 in case of success, error code otherwise.
 *
 * For I2C and SPI Half-Duplex: uses burst read with FIFO_DATA command.
 * For SPI Full-Duplex: issues repeated read commands to FIFO_DATA register.
 * Note: burst32 mode is configured via adis16607_fifo_enable().
 */
int adis16607_fifo_pop(struct adis_dev *adis, struct adis_burst_data *data)
{
	if (adis->comm_type == ADIS_I2C_COMM ||
	    adis->duplex_type == ADIS_SPI_HALF_DUPLEX)
		return adis16607_read_burst_data(adis, data, adis->burst32, 0,
						 true, false);
	else
		return adis16607_fifo_pop_full_duplex(adis, data, adis->burst32);
}

const struct adis_chip_info adis16607_chip_info = {
	.field_map		= &adis16607_def,
	.sync_clk_freq_limits	= adis16607_sync_clk_freq_limits,
	.sampling_clk_limits	= adis16607_sampling_clk_limits,
	.timeouts 		= &adis16607_timeouts,
	.read_delay 		= 0,
	.write_delay 		= 0,
	.cs_change_delay 	= 0,
	.has_lock		= true,
	.dec_rate_max 		= 0xFFFF,
	.sync_mode_max 		= ADIS_SYNC_SCALED,
	.int_clk		= 8000,
	.flags			= ADIS_HAS_BURST32 | ADIS_HAS_BURST_DELTA_DATA | ADIS_HAS_FIFO,
	.initial_startup	= &adis16607_initial_startup,
	.snsr_self_test		= &adis16607_snsr_self_test,
	.get_scale		= &adis16607_get_scale,
	.get_offset		= &adis16607_get_offset,
	.read_burst_data	= &adis16607_read_burst_data,
	.read_sync_mode		= &adis16607_read_sync_mode,
	.write_sync_mode	= &adis16607_write_sync_mode,
	.read_reg		= &adis16607_read_reg,
	.write_reg		= &adis16607_write_reg,
};
