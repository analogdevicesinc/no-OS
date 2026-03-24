/*******************************************************************************
 *   @file   flash_storage.c
 *   @brief  Flash storage implementation for PQM calibration data
 *           Uses Zephyr NVS on MAX32650 internal flash
 *   @author Radu Etz (radu.etz@analog.com)
 ********************************************************************************
 * Copyright (c) 2026 Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#include "flash_storage.h"
#include "afe_calibration.h"
#include "pqlib_example.h"
#include "afe_config.h"
#include "ade9430.h"

#include <zephyr/fs/nvs.h>
#include <zephyr/storage/flash_map.h>
#include <zephyr/logging/log.h>
#include <string.h>
#include <stddef.h>

LOG_MODULE_REGISTER(flash_storage, LOG_LEVEL_INF);

/* NVS configuration */
#define NVS_PARTITION		storage_partition
#define NVS_PARTITION_DEVICE	FIXED_PARTITION_DEVICE(NVS_PARTITION)
#define NVS_PARTITION_OFFSET	FIXED_PARTITION_OFFSET(NVS_PARTITION)
#define NVS_CAL_DATA_ID		1  /* NVS item ID for calibration data */

/* CRC32 polynomial (IEEE 802.3) */
#define CRC32_POLYNOMIAL        0xEDB88320

static struct nvs_fs nvs;
static bool flash_initialized = false;

/* CRC32 lookup table */
static uint32_t crc32_table[256];
static bool crc32_table_initialized = false;

static void crc32_init_table(void)
{
	uint32_t i, j, crc;

	for (i = 0; i < 256; i++) {
		crc = i;
		for (j = 0; j < 8; j++) {
			if (crc & 1)
				crc = (crc >> 1) ^ CRC32_POLYNOMIAL;
			else
				crc = crc >> 1;
		}
		crc32_table[i] = crc;
	}
	crc32_table_initialized = true;
}

uint32_t flash_calculate_crc32(const void *data, uint32_t len)
{
	const uint8_t *buf = (const uint8_t *)data;
	uint32_t crc = 0xFFFFFFFF;
	uint32_t i;

	if (!crc32_table_initialized)
		crc32_init_table();

	for (i = 0; i < len; i++)
		crc = (crc >> 8) ^ crc32_table[(crc ^ buf[i]) & 0xFF];

	return crc ^ 0xFFFFFFFF;
}

int flash_storage_init(void)
{
	int rc;
	const struct device *flash_dev;

	if (flash_initialized)
		return FLASH_STATUS_OK;

	/* Initialize CRC table */
	if (!crc32_table_initialized)
		crc32_init_table();

	flash_dev = NVS_PARTITION_DEVICE;
	if (!device_is_ready(flash_dev)) {
		LOG_ERR("device not ready");
		return FLASH_STATUS_INIT_FAILED;
	}

	nvs.flash_device = flash_dev;
	nvs.offset = NVS_PARTITION_OFFSET;
	nvs.sector_size = 16384;  /* MAX32650 erase block = 16KB */
	nvs.sector_count = 2;

	rc = nvs_mount(&nvs);
	if (rc) {
		LOG_ERR("NVS mount failed: %d", rc);
		return FLASH_STATUS_INIT_FAILED;
	}

	flash_initialized = true;
	LOG_DBG("NVS initialized");
	return FLASH_STATUS_OK;
}

int flash_storage_remove(void)
{
	flash_initialized = false;
	return FLASH_STATUS_OK;
}

bool flash_storage_is_initialized(void)
{
	return flash_initialized;
}

int flash_read_calibration(FLASH_CALIBRATION_DATA *data)
{
	uint32_t calculated_crc;
	uint32_t crc_offset;
	int rc;

	if (!data)
		return FLASH_STATUS_READ_FAILED;

	rc = nvs_read(&nvs, NVS_CAL_DATA_ID, data, sizeof(FLASH_CALIBRATION_DATA));
	if (rc < 0 || rc != sizeof(FLASH_CALIBRATION_DATA))
		return FLASH_STATUS_NO_DATA;

	/* Validate magic number */
	if (data->magic != FLASH_MAGIC_CALIBRATION)
		return FLASH_STATUS_INVALID_MAGIC;

	/* Validate version */
	if (data->version != FLASH_VERSION_CALIBRATION)
		return FLASH_STATUS_INVALID_VERSION;

	/* Validate CRC (calculated over all data except the CRC field) */
	crc_offset = offsetof(FLASH_CALIBRATION_DATA, crc32);
	calculated_crc = flash_calculate_crc32(data, crc_offset);
	if (calculated_crc != data->crc32)
		return FLASH_STATUS_INVALID_CRC;

	return FLASH_STATUS_OK;
}

int flash_write_calibration(const FLASH_CALIBRATION_DATA *data)
{
	FLASH_CALIBRATION_DATA write_data;
	uint32_t crc_offset;
	int rc;

	if (!data)
		return FLASH_STATUS_WRITE_FAILED;

	/* Copy data and update header */
	memcpy(&write_data, data, sizeof(FLASH_CALIBRATION_DATA));
	write_data.magic = FLASH_MAGIC_CALIBRATION;
	write_data.version = FLASH_VERSION_CALIBRATION;

	/* Calculate CRC */
	crc_offset = offsetof(FLASH_CALIBRATION_DATA, crc32);
	write_data.crc32 = flash_calculate_crc32(&write_data, crc_offset);

	/* Write to NVS (handles erase internally) */
	rc = nvs_write(&nvs, NVS_CAL_DATA_ID, &write_data,
		       sizeof(FLASH_CALIBRATION_DATA));
	if (rc < 0) {
		LOG_ERR("NVS write failed: %d", rc);
		return FLASH_STATUS_WRITE_FAILED;
	}

	return FLASH_STATUS_OK;
}

int flash_erase_calibration(void)
{
	int rc;

	rc = nvs_delete(&nvs, NVS_CAL_DATA_ID);
	if (rc < 0) {
		LOG_ERR("NVS delete failed: %d", rc);
		return FLASH_STATUS_ERASE_FAILED;
	}

	return FLASH_STATUS_OK;
}

bool flash_has_valid_calibration(void)
{
	FLASH_CALIBRATION_DATA data;
	return (flash_read_calibration(&data) == FLASH_STATUS_OK);
}

int flash_load_and_apply_calibration(void)
{
	FLASH_CALIBRATION_DATA cal_data;
	FLASH_STATUS fstatus;
	uint8_t ch;
	uint16_t run_reg = 0;
	uint32_t igain_copy, vgain_copy;
	uint16_t igain_reg, vgain_reg;
	uint32_t irmsos_copy, vrmsos_copy, ifrmsos_copy, vfrmsos_copy;
	uint16_t irmsos_reg, vrmsos_reg, ifrmsos_reg, vfrmsos_reg;
	int ret;

	fstatus = flash_read_calibration(&cal_data);
	if (fstatus != FLASH_STATUS_OK)
		return fstatus;

	/* Stop AFE measurements before writing calibration registers */
	run_reg = 0;
	ret = afe_write_16bit_reg(REG_RUN, &run_reg);
	if (ret != 0)
		return FLASH_STATUS_WRITE_FAILED;

	/* Apply calibration coefficients to AFE registers for each channel */
	for (ch = 0; ch < FLASH_NUM_CHANNELS; ch++) {
		FLASH_CHANNEL_CALIBRATION *c = &cal_data.channel[ch];

		if (c->gain_calibrated) {
			igain_copy = (uint32_t)c->i_gain;
			vgain_copy = (uint32_t)c->v_gain;

			igain_reg = (ch == 0) ? REG_AIGAIN :
				    (ch == 1) ? REG_BIGAIN : REG_CIGAIN;
			ret = afe_write_32bit_reg(igain_reg, &igain_copy);
			if (ret != 0)
				return FLASH_STATUS_WRITE_FAILED;

			vgain_reg = (ch == 0) ? REG_AVGAIN :
				    (ch == 1) ? REG_BVGAIN : REG_CVGAIN;
			ret = afe_write_32bit_reg(vgain_reg, &vgain_copy);
			if (ret != 0)
				return FLASH_STATUS_WRITE_FAILED;
		}

		if (c->offset_calibrated) {
			irmsos_copy = (uint32_t)c->i_rmsos;
			vrmsos_copy = (uint32_t)c->v_rmsos;
			ifrmsos_copy = (uint32_t)c->if_rmsos;
			vfrmsos_copy = (uint32_t)c->vf_rmsos;

			irmsos_reg = (ch == 0) ? REG_AIRMSOS :
				     (ch == 1) ? REG_BIRMSOS : REG_CIRMSOS;
			ret = afe_write_32bit_reg(irmsos_reg, &irmsos_copy);
			if (ret != 0)
				return FLASH_STATUS_WRITE_FAILED;

			vrmsos_reg = (ch == 0) ? REG_AVRMSOS :
				     (ch == 1) ? REG_BVRMSOS : REG_CVRMSOS;
			ret = afe_write_32bit_reg(vrmsos_reg, &vrmsos_copy);
			if (ret != 0)
				return FLASH_STATUS_WRITE_FAILED;

			ifrmsos_reg = (ch == 0) ? REG_AIFRMSOS :
				      (ch == 1) ? REG_BIFRMSOS : REG_CIFRMSOS;
			ret = afe_write_32bit_reg(ifrmsos_reg, &ifrmsos_copy);
			if (ret != 0)
				return FLASH_STATUS_WRITE_FAILED;

			vfrmsos_reg = (ch == 0) ? REG_AVFRMSOS :
				      (ch == 1) ? REG_BVFRMSOS : REG_CVFRMSOS;
			ret = afe_write_32bit_reg(vfrmsos_reg, &vfrmsos_copy);
			if (ret != 0)
				return FLASH_STATUS_WRITE_FAILED;
		}
	}

	/* Restart AFE measurements */
	run_reg = 1;
	ret = afe_write_16bit_reg(REG_RUN, &run_reg);
	if (ret != 0)
		return FLASH_STATUS_WRITE_FAILED;

	LOG_INF("calibration applied from NVS");
	return FLASH_STATUS_OK;
}

int flash_save_calibration_channel(uint8_t channel)
{
	FLASH_CALIBRATION_DATA cal_data;
	FLASH_STATUS fstatus;
	FLASH_CHANNEL_CALIBRATION *c;

	if (channel >= FLASH_NUM_CHANNELS)
		return FLASH_STATUS_WRITE_FAILED;

	/* Try to read existing calibration data */
	fstatus = flash_read_calibration(&cal_data);
	if (fstatus != FLASH_STATUS_OK) {
		memset(&cal_data, 0, sizeof(FLASH_CALIBRATION_DATA));
	}

	/* Update channel data from calibration context */
	c = &cal_data.channel[channel];

	if (calibrationCtx.type == CALIBRATION_TYPE_GAIN) {
		c->i_gain = calibrationCtx.result.i_gain;
		c->v_gain = calibrationCtx.result.v_gain;
		c->gain_calibrated = true;
		c->gain_i_error = calibrationCtx.result.gain_i_error_after;
		c->gain_v_error = calibrationCtx.result.gain_v_error_after;
	} else {
		c->i_rmsos = calibrationCtx.result.i_rmsos;
		c->v_rmsos = calibrationCtx.result.v_rmsos;
		c->if_rmsos = calibrationCtx.result.if_rmsos;
		c->vf_rmsos = calibrationCtx.result.vf_rmsos;
		c->offset_calibrated = true;
		c->offset_i_error = calibrationCtx.result.offset_i_error_after;
		c->offset_v_error = calibrationCtx.result.offset_v_error_after;
	}

	cal_data.cal_nominal_current = calibrationCtx.input.nominal_current;
	cal_data.cal_nominal_voltage = calibrationCtx.input.nominal_voltage;
	cal_data.cal_offset_current = calibrationCtx.input.offset_current;
	cal_data.cal_offset_voltage = calibrationCtx.input.offset_voltage;

	return flash_write_calibration(&cal_data);
}

int flash_save_all_calibration(void)
{
	FLASH_CALIBRATION_DATA cal_data;
	uint8_t ch;
	uint16_t igain_reg, vgain_reg, irmsos_reg, vrmsos_reg;
	uint16_t ifrmsos_reg, vfrmsos_reg;
	int ret;

	memset(&cal_data, 0, sizeof(FLASH_CALIBRATION_DATA));

	/* Read current calibration coefficients from AFE for all channels */
	for (ch = 0; ch < FLASH_NUM_CHANNELS; ch++) {
		FLASH_CHANNEL_CALIBRATION *c = &cal_data.channel[ch];

		igain_reg = (ch == 0) ? REG_AIGAIN :
			    (ch == 1) ? REG_BIGAIN : REG_CIGAIN;
		ret = afe_read_32bit_buff(igain_reg, 1, (uint32_t *)&c->i_gain);
		if (ret != 0)
			return FLASH_STATUS_READ_FAILED;

		vgain_reg = (ch == 0) ? REG_AVGAIN :
			    (ch == 1) ? REG_BVGAIN : REG_CVGAIN;
		ret = afe_read_32bit_buff(vgain_reg, 1, (uint32_t *)&c->v_gain);
		if (ret != 0)
			return FLASH_STATUS_READ_FAILED;

		if (c->i_gain != 0 || c->v_gain != 0)
			c->gain_calibrated = true;

		irmsos_reg = (ch == 0) ? REG_AIRMSOS :
			     (ch == 1) ? REG_BIRMSOS : REG_CIRMSOS;
		ret = afe_read_32bit_buff(irmsos_reg, 1, (uint32_t *)&c->i_rmsos);
		if (ret != 0)
			return FLASH_STATUS_READ_FAILED;

		vrmsos_reg = (ch == 0) ? REG_AVRMSOS :
			     (ch == 1) ? REG_BVRMSOS : REG_CVRMSOS;
		ret = afe_read_32bit_buff(vrmsos_reg, 1, (uint32_t *)&c->v_rmsos);
		if (ret != 0)
			return FLASH_STATUS_READ_FAILED;

		if (c->i_rmsos != 0 || c->v_rmsos != 0)
			c->offset_calibrated = true;

		ifrmsos_reg = (ch == 0) ? REG_AIFRMSOS :
			      (ch == 1) ? REG_BIFRMSOS : REG_CIFRMSOS;
		ret = afe_read_32bit_buff(ifrmsos_reg, 1, (uint32_t *)&c->if_rmsos);
		if (ret != 0)
			return FLASH_STATUS_READ_FAILED;

		vfrmsos_reg = (ch == 0) ? REG_AVFRMSOS :
			      (ch == 1) ? REG_BVFRMSOS : REG_CVFRMSOS;
		ret = afe_read_32bit_buff(vfrmsos_reg, 1, (uint32_t *)&c->vf_rmsos);
		if (ret != 0)
			return FLASH_STATUS_READ_FAILED;

		c->gain_i_error = calibrationCtx.result.gain_i_error_after;
		c->gain_v_error = calibrationCtx.result.gain_v_error_after;
		c->offset_i_error = calibrationCtx.result.offset_i_error_after;
		c->offset_v_error = calibrationCtx.result.offset_v_error_after;
	}

	cal_data.cal_nominal_current = calibrationCtx.input.nominal_current;
	cal_data.cal_nominal_voltage = calibrationCtx.input.nominal_voltage;
	cal_data.cal_offset_current = calibrationCtx.input.offset_current;
	cal_data.cal_offset_voltage = calibrationCtx.input.offset_voltage;

	return flash_write_calibration(&cal_data);
}

const char *flash_status_to_string(FLASH_STATUS status)
{
	switch (status) {
	case FLASH_STATUS_OK:
		return "OK";
	case FLASH_STATUS_NOT_INITIALIZED:
		return "Not initialized";
	case FLASH_STATUS_INIT_FAILED:
		return "Init failed";
	case FLASH_STATUS_READ_FAILED:
		return "Read failed";
	case FLASH_STATUS_WRITE_FAILED:
		return "Write failed";
	case FLASH_STATUS_ERASE_FAILED:
		return "Erase failed";
	case FLASH_STATUS_INVALID_MAGIC:
		return "Invalid magic";
	case FLASH_STATUS_INVALID_VERSION:
		return "Invalid version";
	case FLASH_STATUS_INVALID_CRC:
		return "Invalid CRC";
	case FLASH_STATUS_NO_DATA:
		return "No data";
	default:
		return "Unknown";
	}
}
