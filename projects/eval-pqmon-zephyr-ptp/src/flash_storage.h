/*******************************************************************************
 *   @file   flash_storage.h
 *   @brief  Flash storage interface for PQM calibration data header file
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

#ifndef __FLASH_STORAGE_H__
#define __FLASH_STORAGE_H__

#include <stdint.h>
#include <stdbool.h>

/* Magic number for data validation */
#define FLASH_MAGIC_CALIBRATION         0x43414C49  /* "CALI" */

/* Data structure version (increment when structure changes) */
#define FLASH_VERSION_CALIBRATION       0x0001

/* Number of channels (phases) */
#define FLASH_NUM_CHANNELS              3

/**
 * @brief Calibration coefficients for a single channel (phase)
 */
typedef struct {
	/* Gain coefficients (from gain calibration) */
	int32_t i_gain;             /* Current gain coefficient */
	int32_t v_gain;             /* Voltage gain coefficient */
	/* Offset coefficients (from offset calibration) */
	int32_t i_rmsos;            /* Current RMS offset */
	int32_t v_rmsos;            /* Voltage RMS offset */
	int32_t if_rmsos;           /* Current fundamental RMS offset */
	int32_t vf_rmsos;           /* Voltage fundamental RMS offset */
	/* Calibration status */
	bool gain_calibrated;       /* Gain calibration done flag */
	bool offset_calibrated;     /* Offset calibration done flag */
	/* Calibration errors (for reference) */
	float gain_i_error;         /* Current error after gain cal (%) */
	float gain_v_error;         /* Voltage error after gain cal (%) */
	float offset_i_error;       /* Current error after offset cal (%) */
	float offset_v_error;       /* Voltage error after offset cal (%) */
	/* Reserved for future use */
	uint8_t reserved[16];
} FLASH_CHANNEL_CALIBRATION;

/**
 * @brief Complete calibration data structure
 */
typedef struct {
	/* Header */
	uint32_t magic;             /* Magic number for validation */
	uint16_t version;           /* Structure version */
	uint16_t reserved_hdr;      /* Reserved */
	/* Per-channel calibration data */
	FLASH_CHANNEL_CALIBRATION channel[FLASH_NUM_CHANNELS];
	/* Calibration input parameters used */
	float cal_nominal_current;  /* Nominal current used for gain cal */
	float cal_nominal_voltage;  /* Nominal voltage used for gain cal */
	float cal_offset_current;   /* Offset current used for offset cal */
	float cal_offset_voltage;   /* Offset voltage used for offset cal */
	/* Timestamp of last calibration */
	uint32_t timestamp;         /* Unix timestamp of last calibration */
	/* CRC for data integrity */
	uint32_t crc32;             /* CRC32 of all data above */
} FLASH_CALIBRATION_DATA;

/**
 * @brief Flash storage status
 */
typedef enum {
	FLASH_STATUS_OK = 0,
	FLASH_STATUS_NO_DATA = -9,
	FLASH_STATUS_INVALID_CRC,
	FLASH_STATUS_INVALID_VERSION,
	FLASH_STATUS_INVALID_MAGIC,
	FLASH_STATUS_ERASE_FAILED,
	FLASH_STATUS_WRITE_FAILED,
	FLASH_STATUS_READ_FAILED,
	FLASH_STATUS_INIT_FAILED,
	FLASH_STATUS_NOT_INITIALIZED
} FLASH_STATUS;

int flash_storage_init(void);
int flash_storage_remove(void);
bool flash_storage_is_initialized(void);
int flash_read_calibration(FLASH_CALIBRATION_DATA *data);
int flash_write_calibration(const FLASH_CALIBRATION_DATA *data);
int flash_erase_calibration(void);
int flash_load_and_apply_calibration(void);
int flash_save_calibration_channel(uint8_t channel);
int flash_save_all_calibration(void);
bool flash_has_valid_calibration(void);
const char *flash_status_to_string(FLASH_STATUS status);
uint32_t flash_calculate_crc32(const void *data, uint32_t len);

#endif /* __FLASH_STORAGE_H__ */
