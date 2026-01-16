/*******************************************************************************
 *   @file   flash_storage.c
 *   @brief  Flash storage implementation for PQM calibration data
 *           Uses internal flash on MAX32650
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
#include "flc.h"
#include "icc.h"
#include "mxc_device.h"
#include "mxc_delay.h"
#include <stdio.h>
#include <string.h>

/* Use last two pages of internal flash for calibration storage */
#define FLASH_CAL_PAGE_ADDR     ((MXC_FLASH_MEM_BASE + MXC_FLASH_MEM_SIZE) - (2 * MXC_FLASH_PAGE_SIZE))
#define FLASH_CAL_BACKUP_ADDR   ((MXC_FLASH_MEM_BASE + MXC_FLASH_MEM_SIZE) - (1 * MXC_FLASH_PAGE_SIZE))

/* CRC32 polynomial (IEEE 802.3) */
#define CRC32_POLYNOMIAL        0xEDB88320

static bool flash_initialized = false;

/* CRC32 lookup table */
static uint32_t crc32_table[256];
static bool crc32_table_initialized = false;

static void crc32_init_table(void);

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
	if (flash_initialized)
		return FLASH_STATUS_OK;

	/* Initialize CRC table */
	if (!crc32_table_initialized)
		crc32_init_table();

	flash_initialized = true;

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

	if (!data)
		return FLASH_STATUS_READ_FAILED;

	/* Read calibration data from internal flash */
	MXC_FLC_Read(FLASH_CAL_PAGE_ADDR, data, sizeof(FLASH_CALIBRATION_DATA));

	/* Validate magic number */
	if (data->magic != FLASH_MAGIC_CALIBRATION) {
		/* Try backup location */
		MXC_FLC_Read(FLASH_CAL_BACKUP_ADDR, data,
			     sizeof(FLASH_CALIBRATION_DATA));

		if (data->magic != FLASH_MAGIC_CALIBRATION)
			return FLASH_STATUS_NO_DATA;
	}

	/* Validate version */
	if (data->version != FLASH_VERSION_CALIBRATION)
		return FLASH_STATUS_INVALID_VERSION;

	/* Validate CRC (CRC is calculated over all data except the CRC field) */
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
	uint32_t *src;
	uint32_t addr;
	int ret;
	uint32_t i;

	if (!data)
		return FLASH_STATUS_WRITE_FAILED;

	/* Copy data and update header */
	memcpy(&write_data, data, sizeof(FLASH_CALIBRATION_DATA));
	write_data.magic = FLASH_MAGIC_CALIBRATION;
	write_data.version = FLASH_VERSION_CALIBRATION;

	/* Calculate CRC */
	crc_offset = offsetof(FLASH_CALIBRATION_DATA, crc32);
	write_data.crc32 = flash_calculate_crc32(&write_data, crc_offset);

	/* Disable instruction cache before modifying flash */
	MXC_ICC_Disable();

	/* Erase and write primary page */
	MXC_CRITICAL(
		ret = MXC_FLC_PageErase(FLASH_CAL_PAGE_ADDR);
	)
	if (ret != E_NO_ERROR) {
		MXC_ICC_Enable();
		return FLASH_STATUS_ERASE_FAILED;
	}

	src = (uint32_t *)&write_data;
	addr = FLASH_CAL_PAGE_ADDR;
	for (i = 0; i < sizeof(FLASH_CALIBRATION_DATA); i += 4) {
		MXC_CRITICAL(
			ret = MXC_FLC_Write32(addr + i, src[i / 4]);
		)
		if (ret != E_NO_ERROR) {
			MXC_ICC_Enable();
			return FLASH_STATUS_WRITE_FAILED;
		}
	}

	/* Erase and write backup page */
	MXC_CRITICAL(
		ret = MXC_FLC_PageErase(FLASH_CAL_BACKUP_ADDR);
	)
	if (ret != E_NO_ERROR) {
		MXC_ICC_Enable();
		return FLASH_STATUS_ERASE_FAILED;
	}

	addr = FLASH_CAL_BACKUP_ADDR;
	for (i = 0; i < sizeof(FLASH_CALIBRATION_DATA); i += 4) {
		MXC_CRITICAL(
			ret = MXC_FLC_Write32(addr + i, src[i / 4]);
		)
		if (ret != E_NO_ERROR) {
			MXC_ICC_Enable();
			return FLASH_STATUS_WRITE_FAILED;
		}
	}

	/* Re-enable instruction cache */
	MXC_ICC_Enable();

	return FLASH_STATUS_OK;
}

int flash_erase_calibration(void)
{
	int ret;

	/* Disable instruction cache */
	MXC_ICC_Disable();

	/* Erase primary page */
	MXC_CRITICAL(
		ret = MXC_FLC_PageErase(FLASH_CAL_PAGE_ADDR);
	)
	if (ret != E_NO_ERROR) {
		MXC_ICC_Enable();
		return FLASH_STATUS_ERASE_FAILED;
	}

	/* Erase backup page */
	MXC_CRITICAL(
		ret = MXC_FLC_PageErase(FLASH_CAL_BACKUP_ADDR);
	)
	if (ret != E_NO_ERROR) {
		MXC_ICC_Enable();
		return FLASH_STATUS_ERASE_FAILED;
	}

	MXC_ICC_Enable();

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
	FLASH_STATUS status;
	int ret;
	uint8_t ch;
	uint16_t run_reg = 0;

	/* Read calibration data from flash */
	status = flash_read_calibration(&cal_data);
	if (status != FLASH_STATUS_OK)
		return status;

	/* Stop AFE measurements before writing calibration registers */
	run_reg = 0;
	ret = afe_write_16bit_reg(REG_RUN, &run_reg);
	if (ret != 0)
		return FLASH_STATUS_WRITE_FAILED;

	/* Apply calibration coefficients to AFE registers for each channel */
	for (ch = 0; ch < FLASH_NUM_CHANNELS; ch++) {
		FLASH_CHANNEL_CALIBRATION *c = &cal_data.channel[ch];

		/* Apply gain coefficients if calibrated */
		if (c->gain_calibrated) {
			/* Use local copies */
			uint32_t igain_copy = (uint32_t)c->i_gain;
			uint32_t vgain_copy = (uint32_t)c->v_gain;

			uint16_t igain_reg = (ch == 0) ? REG_AIGAIN :
					     (ch == 1) ? REG_BIGAIN : REG_CIGAIN;
			afe_write_32bit_reg(igain_reg, &igain_copy);

			uint16_t vgain_reg = (ch == 0) ? REG_AVGAIN :
					     (ch == 1) ? REG_BVGAIN : REG_CVGAIN;
			afe_write_32bit_reg(vgain_reg, &vgain_copy);
		}

		/* Apply offset coefficients if calibrated */
		if (c->offset_calibrated) {
			/* Use local copies */
			uint32_t irmsos_copy = (uint32_t)c->i_rmsos;
			uint32_t vrmsos_copy = (uint32_t)c->v_rmsos;
			uint32_t ifrmsos_copy = (uint32_t)c->if_rmsos;
			uint32_t vfrmsos_copy = (uint32_t)c->vf_rmsos;

			uint16_t irmsos_reg = (ch == 0) ? REG_AIRMSOS :
					      (ch == 1) ? REG_BIRMSOS : REG_CIRMSOS;
			afe_write_32bit_reg(irmsos_reg, &irmsos_copy);

			uint16_t vrmsos_reg = (ch == 0) ? REG_AVRMSOS :
					      (ch == 1) ? REG_BVRMSOS : REG_CVRMSOS;
			afe_write_32bit_reg(vrmsos_reg, &vrmsos_copy);

			uint16_t ifrmsos_reg = (ch == 0) ? REG_AIFRMSOS :
					       (ch == 1) ? REG_BIFRMSOS : REG_CIFRMSOS;
			afe_write_32bit_reg(ifrmsos_reg, &ifrmsos_copy);

			uint16_t vfrmsos_reg = (ch == 0) ? REG_AVFRMSOS :
					       (ch == 1) ? REG_BVFRMSOS : REG_CVFRMSOS;
			afe_write_32bit_reg(vfrmsos_reg, &vfrmsos_copy);
		}
	}

	/* Restart AFE measurements */
	run_reg = 1;
	afe_write_16bit_reg(REG_RUN, &run_reg);

	return FLASH_STATUS_OK;
}

int flash_save_calibration_channel(uint8_t channel)
{
	FLASH_CALIBRATION_DATA cal_data;
	FLASH_STATUS status;
	FLASH_CHANNEL_CALIBRATION *c;

	if (channel >= FLASH_NUM_CHANNELS)
		return FLASH_STATUS_WRITE_FAILED;

	/* Try to read existing calibration data */
	status = flash_read_calibration(&cal_data);
	if (status != FLASH_STATUS_OK) {
		/* No existing data, initialize structure */
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

	/* Update calibration input parameters */
	cal_data.cal_nominal_current = calibrationCtx.input.nominal_current;
	cal_data.cal_nominal_voltage = calibrationCtx.input.nominal_voltage;
	cal_data.cal_offset_current = calibrationCtx.input.offset_current;
	cal_data.cal_offset_voltage = calibrationCtx.input.offset_voltage;

	/* Write updated calibration data */
	return flash_write_calibration(&cal_data);
}

int flash_save_all_calibration(void)
{
	FLASH_CALIBRATION_DATA cal_data;
	uint8_t ch;
	int ret;

	/* Initialize structure */
	memset(&cal_data, 0, sizeof(FLASH_CALIBRATION_DATA));

	/* Read current calibration coefficients from AFE for all channels */
	for (ch = 0; ch < FLASH_NUM_CHANNELS; ch++) {
		FLASH_CHANNEL_CALIBRATION *c = &cal_data.channel[ch];

		/* Read gain registers */
		uint16_t igain_reg = (ch == 0) ? REG_AIGAIN :
				     (ch == 1) ? REG_BIGAIN : REG_CIGAIN;
		ret = afe_read_32bit_buff(igain_reg, 1, (uint32_t *)&c->i_gain);
		if (ret == 0 && c->i_gain != 0)
			c->gain_calibrated = true;

		uint16_t vgain_reg = (ch == 0) ? REG_AVGAIN :
				     (ch == 1) ? REG_BVGAIN : REG_CVGAIN;
		ret = afe_read_32bit_buff(vgain_reg, 1, (uint32_t *)&c->v_gain);

		/* Read offset registers */
		uint16_t irmsos_reg = (ch == 0) ? REG_AIRMSOS :
				      (ch == 1) ? REG_BIRMSOS : REG_CIRMSOS;
		ret = afe_read_32bit_buff(irmsos_reg, 1, (uint32_t *)&c->i_rmsos);
		if (ret == 0 && c->i_rmsos != 0)
			c->offset_calibrated = true;

		uint16_t vrmsos_reg = (ch == 0) ? REG_AVRMSOS :
				      (ch == 1) ? REG_BVRMSOS : REG_CVRMSOS;
		ret = afe_read_32bit_buff(vrmsos_reg, 1, (uint32_t *)&c->v_rmsos);

		uint16_t ifrmsos_reg = (ch == 0) ? REG_AIFRMSOS :
				       (ch == 1) ? REG_BIFRMSOS : REG_CIFRMSOS;
		ret = afe_read_32bit_buff(ifrmsos_reg, 1, (uint32_t *)&c->if_rmsos);

		uint16_t vfrmsos_reg = (ch == 0) ? REG_AVFRMSOS :
				       (ch == 1) ? REG_BVFRMSOS : REG_CVFRMSOS;
		ret = afe_read_32bit_buff(vfrmsos_reg, 1, (uint32_t *)&c->vf_rmsos);
	}

	/* Save calibration input parameters from current context */
	cal_data.cal_nominal_current = calibrationCtx.input.nominal_current;
	cal_data.cal_nominal_voltage = calibrationCtx.input.nominal_voltage;
	cal_data.cal_offset_current = calibrationCtx.input.offset_current;
	cal_data.cal_offset_voltage = calibrationCtx.input.offset_voltage;

	/* Write calibration data */
	return flash_write_calibration(&cal_data);
}

/*******************************************************************************
 * Public Functions - Utility
 ******************************************************************************/

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
