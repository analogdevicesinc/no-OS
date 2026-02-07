/***************************************************************************//**
 *   @file   no_os_gnss.h
 *   @brief  Header file of GNSS Interface
 *   @author Radu Etz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#ifndef _NO_OS_GNSS_H_
#define _NO_OS_GNSS_H_

#include <stdint.h>
#include <stdbool.h>

#define GNSS_MAX_NUMBER 4

/**
 * @struct no_os_gnss_pps_config
 * @brief GNSS PPS configuration structure for timing synchronization.
 */
struct no_os_gnss_pps_config {
	/** Enable/disable PPS output */
	bool pps_enable;
	/** PPS output frequency in Hz (typically 1 for standard PPS) */
	uint32_t frequency;
	/** PPS pulse length in milliseconds */
	uint32_t pulse_length;
};

/**
 * @struct no_os_gnss_precise_time
 * @brief API-level precise time information structure
 */
struct no_os_gnss_precise_time {
	/** Unix epoch time in seconds */
	uint32_t unix_epoch;
	/** Nanosecond fraction (can be negative) */
	int32_t nanoseconds;
	/** Time accuracy estimate in nanoseconds */
	uint32_t time_accuracy;
	/** Time validity flag */
	bool time_valid;
	/** Time fully resolved flag */
	bool time_fully_resolved;
	/** Date validity flag */
	bool date_valid;
	/** Year (UTC) */
	uint16_t year;
	/** Month (UTC) */
	uint8_t month;
	/** Day (UTC) */
	uint8_t day;
	/** Hour (UTC) */
	uint8_t hour;
	/** Minute (UTC) */
	uint8_t minute;
	/** Second (UTC) */
	uint8_t second;
};

/**
 * @struct no_os_gnss_position_data
 * @brief API-level position and fix quality data structure
 */
struct no_os_gnss_position_data {
	/** Fix quality: 0=invalid, 1=GPS fix, 2=DGPS fix */
	uint8_t fix_quality;
	/** Number of satellites in use */
	uint8_t satellites_used;
	/** Horizontal dilution of precision */
	float hdop;
	/** Altitude above mean sea level (meters) */
	float altitude;
	/** Latitude in decimal degrees */
	float latitude;
	/** Longitude in decimal degrees */
	float longitude;
	/** Position data validity */
	bool position_valid;
	/** GPS fix validity */
	bool fix_valid;
};

/**
 * @struct no_os_gnss_platform_ops
 * @brief Structure holding GNSS function pointers that point to the platform
 * specific function
 */
struct no_os_gnss_platform_ops;

/**
 * @struct no_os_gnss_init_param
 * @brief Structure holding the parameters for GNSS initialization
 */
struct no_os_gnss_init_param {
	/** GNSS Device ID */
	uint32_t device_id;
	/** PPS configuration settings */
	struct no_os_gnss_pps_config pps_config;
	/** GNSS platform operations */
	const struct no_os_gnss_platform_ops *platform_ops;
	/** GNSS extra parameters (platform and device specific) */
	void *extra;
};

/**
 * @struct no_os_gnss_desc
 * @brief Structure holding the GNSS descriptor.
 */
struct no_os_gnss_desc {
	/** GNSS mutex */
	void *mutex;
	/** GNSS Device ID */
	uint32_t device_id;
	/** GNSS platform operations */
	const struct no_os_gnss_platform_ops *platform_ops;
	/** GNSS extra parameters (platform and device specific) */
	void *extra;
};

/**
 * @struct no_os_gnss_platform_ops
 * @brief Structure holding GNSS function pointers that point to the platform
 * specific function
 */
struct no_os_gnss_platform_ops {
	/** GNSS initialization function pointer */
	int32_t (*init)(struct no_os_gnss_desc **,
			const struct no_os_gnss_init_param *);
	/** GNSS refresh timing data function pointer */
	int32_t (*refresh_timing_data)(struct no_os_gnss_desc *);
	/** GNSS check timing validity function pointer */
	int32_t (*is_timing_valid)(struct no_os_gnss_desc *, bool *);
	/** GNSS get unified timing function pointer */
	int32_t (*get_unified_timing)(struct no_os_gnss_desc *,
				      struct no_os_gnss_precise_time *);
	/** GNSS get Unix epoch with unified precision function pointer */
	uint32_t (*get_unix_epoch_unified)(struct no_os_gnss_desc *,
					   uint32_t *);
	/** GNSS get position data function pointer */
	int32_t (*get_position_data)(struct no_os_gnss_desc *,
				     struct no_os_gnss_position_data *);
	/** GNSS removal function pointer */
	int32_t (*remove)(struct no_os_gnss_desc *);
};

/* GNSS Public API Functions */

/**
 * @brief Initialize the GNSS communication peripheral.
 * @param desc - The GNSS descriptor.
 * @param param - The structure that contains the GNSS parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_gnss_init(struct no_os_gnss_desc **desc,
			const struct no_os_gnss_init_param *param);

/**
 * @brief Free the resources allocated by no_os_gnss_init().
 * @param desc - The GNSS descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_gnss_remove(struct no_os_gnss_desc *desc);

/**
 * @brief Refresh timing data from GNSS device.
 * @param desc - The GNSS descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_gnss_refresh_timing_data(struct no_os_gnss_desc *desc);

/**
 * @brief Check if timing data is valid.
 * @param desc - The GNSS descriptor.
 * @param valid - Pointer to store timing validity status.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_gnss_is_timing_valid(struct no_os_gnss_desc *desc, bool *valid);

/**
 * @brief Get unified timing information with best available precision.
 * @param desc - The GNSS descriptor.
 * @param timing - Pointer to structure to store timing information.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_gnss_get_unified_timing(struct no_os_gnss_desc *desc,
				      struct no_os_gnss_precise_time *timing);

/**
 * @brief Get Unix epoch timestamp with unified precision.
 * @param desc - The GNSS descriptor.
 * @param fractional_seconds - Pointer to store fractional seconds (microseconds).
 * @return Unix epoch timestamp in seconds.
 */
uint32_t no_os_gnss_get_unix_epoch_unified(struct no_os_gnss_desc *desc,
		uint32_t *fractional_seconds);

/**
 * @brief Get GPS position and fix quality data.
 * @param desc - The GNSS descriptor.
 * @param position_data - Pointer to store position data.
 * @return 0 on success, negative error code otherwise.
 */
int32_t no_os_gnss_get_position_data(struct no_os_gnss_desc *desc,
				     struct no_os_gnss_position_data *position_data);

/* Platform-specific implementations */
extern const struct no_os_gnss_platform_ops nmea_ubx_gnss_ops;

#endif /* _NO_OS_GNSS_H_ */