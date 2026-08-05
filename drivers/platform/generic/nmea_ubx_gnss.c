/***************************************************************************//**
 *   @file   nmea_ubx_gnss.c
 *   @brief  Platform-specific implementation for NMEA/UBX GNSS devices
 *   @author Radu Etz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <stdlib.h>
#include <errno.h>
#include "nmea_ubx_gnss.h"
#include "no_os_alloc.h"
#include "no_os_mutex.h"

/**
 * @brief Convert driver precise time to API precise time structure
 * @param src - Source driver precise time structure
 * @param dst - Destination API precise time structure
 */
static void convert_precise_time_to_api(const struct gnss_precise_time *src,
					struct no_os_gnss_precise_time *dst)
{
	if (!src || !dst)
		return;

	dst->unix_epoch = src->unix_epoch;
	dst->nanoseconds = src->nanoseconds;
	dst->time_accuracy = src->time_accuracy;
	dst->time_valid = src->time_valid;
	dst->time_fully_resolved = src->time_fully_resolved;
	dst->date_valid = src->date_valid;
	dst->year = src->year;
	dst->month = src->month;
	dst->day = src->day;
	dst->hour = src->hour;
	dst->minute = src->minute;
	dst->second = src->second;
}

/**
 * @brief Convert driver NMEA position to API position structure
 * @param src - Source driver NMEA position structure
 * @param dst - Destination API position structure
 */
static void convert_nmea_position_to_api(const struct gnss_nmea_position *src,
		struct no_os_gnss_position_data *dst)
{
	if (!src || !dst)
		return;

	dst->fix_quality = src->fix_quality;
	dst->satellites_used = src->satellites_used;
	dst->hdop = src->hdop;
	dst->altitude = src->altitude;
	dst->latitude = src->latitude;
	dst->longitude = src->longitude;
	dst->position_valid = src->position_valid;
	dst->fix_valid = src->fix_valid;
}

/**
 * @brief Initialize NMEA/UBX GNSS platform device
 */
static int32_t nmea_ubx_gnss_platform_init(struct no_os_gnss_desc **desc,
		const struct no_os_gnss_init_param *param)
{
	struct no_os_gnss_desc *gnss_desc;
	struct nmea_ubx_gnss_extra *extra_data;
	struct nmea_ubx_gnss_init_param *platform_param;
	struct gnss_dev *gnss_dev;
	struct gnss_init_param gnss_init_param;
	int32_t ret;

	if (!desc || !param || !param->extra)
		return -EINVAL;

	platform_param = (struct nmea_ubx_gnss_init_param *)param->extra;

	gnss_desc = (struct no_os_gnss_desc *)no_os_calloc(1, sizeof(*gnss_desc));
	if (!gnss_desc)
		return -ENOMEM;

	extra_data = (struct nmea_ubx_gnss_extra *)no_os_calloc(1, sizeof(*extra_data));
	if (!extra_data) {
		no_os_free(gnss_desc);
		return -ENOMEM;
	}

	/* Copy and modify the gnss_init_param with PPS settings */
	gnss_init_param = platform_param->gnss_init_param;
	gnss_init_param.pps_enable = param->pps_config.pps_enable;
	gnss_init_param.pps_frequency = param->pps_config.frequency;
	gnss_init_param.pps_pulse_length = param->pps_config.pulse_length;

	/* Initialize underlying GNSS driver */
	ret = gnss_init(&gnss_dev, gnss_init_param);
	if (ret) {
		no_os_free(extra_data);
		no_os_free(gnss_desc);
		return ret;
	}

	/* Set up the descriptor */
	gnss_desc->device_id = param->device_id;
	gnss_desc->platform_ops = param->platform_ops;
	extra_data->gnss_device = gnss_dev;
	gnss_desc->extra = extra_data;

	/* Initialize the PPS output */
	ret = gnss_init_pps(gnss_dev, param->pps_config.pps_enable);
	if (ret) {
		gnss_remove(gnss_dev);
		no_os_free(extra_data);
		no_os_free(gnss_desc);
		return ret;
	}

	/* Initialize mutex if needed */
	no_os_mutex_init(&gnss_desc->mutex);

	*desc = gnss_desc;

	return 0;
}

/**
 * @brief Refresh timing data from NMEA/UBX GNSS device
 */
static int32_t nmea_ubx_gnss_platform_refresh_timing_data(
	struct no_os_gnss_desc *desc)
{
	struct nmea_ubx_gnss_extra *extra_data;

	if (!desc || !desc->extra)
		return -EINVAL;

	extra_data = (struct nmea_ubx_gnss_extra *)desc->extra;
	if (!extra_data->gnss_device)
		return -EINVAL;

	return gnss_refresh_timing_data(extra_data->gnss_device);
}

/**
 * @brief Check if timing data is valid for NMEA/UBX GNSS device
 */
static int32_t nmea_ubx_gnss_platform_is_timing_valid(struct no_os_gnss_desc
		*desc,
		bool *valid)
{
	struct nmea_ubx_gnss_extra *extra_data;

	if (!desc || !desc->extra || !valid)
		return -EINVAL;

	extra_data = (struct nmea_ubx_gnss_extra *)desc->extra;
	if (!extra_data->gnss_device)
		return -EINVAL;

	return gnss_is_timing_valid(extra_data->gnss_device, valid);
}

/**
 * @brief Get unified timing from NMEA/UBX GNSS device
 */
static int32_t nmea_ubx_gnss_platform_get_unified_timing(
	struct no_os_gnss_desc *desc,
	struct no_os_gnss_precise_time *timing)
{
	struct nmea_ubx_gnss_extra *extra_data;
	struct gnss_precise_time driver_timing;
	int32_t ret;

	if (!desc || !desc->extra || !timing)
		return -EINVAL;

	extra_data = (struct nmea_ubx_gnss_extra *)desc->extra;
	if (!extra_data->gnss_device)
		return -EINVAL;

	ret = gnss_get_unified_timing(extra_data->gnss_device, &driver_timing);
	if (ret)
		return ret;

	/* Convert driver structure to API structure */
	convert_precise_time_to_api(&driver_timing, timing);

	return 0;
}

/**
 * @brief Get Unix epoch with unified precision from NMEA/UBX GNSS device
 */
static int32_t nmea_ubx_gnss_platform_get_unix_epoch_unified(
	struct no_os_gnss_desc *desc,
	uint32_t *epoch, uint32_t *fractional_seconds)
{
	struct nmea_ubx_gnss_extra *extra_data;

	if (!desc || !desc->extra || !epoch || !fractional_seconds)
		return -EINVAL;

	extra_data = (struct nmea_ubx_gnss_extra *)desc->extra;
	if (!extra_data->gnss_device)
		return -EINVAL;

	return gnss_get_unix_epoch_unified(extra_data->gnss_device, epoch,
					   fractional_seconds);
}

/**
 * @brief Get position data from NMEA/UBX GNSS device
 */
static int32_t nmea_ubx_gnss_platform_get_position_data(
	struct no_os_gnss_desc *desc,
	struct no_os_gnss_position_data *position_data)
{
	struct nmea_ubx_gnss_extra *extra_data;
	struct gnss_nmea_position driver_position;
	int32_t ret;

	if (!desc || !desc->extra || !position_data)
		return -EINVAL;

	extra_data = (struct nmea_ubx_gnss_extra *)desc->extra;
	if (!extra_data->gnss_device)
		return -EINVAL;

	ret = gnss_get_nmea_position_data(extra_data->gnss_device, &driver_position);
	if (ret)
		return ret;

	/* Convert driver structure to API structure */
	convert_nmea_position_to_api(&driver_position, position_data);

	return 0;
}

/**
 * @brief Remove NMEA/UBX GNSS platform device
 */
static int32_t nmea_ubx_gnss_platform_remove(struct no_os_gnss_desc *desc)
{
	struct nmea_ubx_gnss_extra *extra_data;

	if (!desc)
		return -EINVAL;

	/* Remove underlying GNSS device */
	if (desc->extra) {
		extra_data = (struct nmea_ubx_gnss_extra *)desc->extra;
		if (extra_data->gnss_device)
			gnss_remove(extra_data->gnss_device);
		no_os_free(extra_data);
	}

	/* Remove mutex */
	if (desc->mutex)
		no_os_mutex_remove(desc->mutex);

	no_os_free(desc);

	return 0;
}

/**
 * @brief Platform operations structure for NMEA/UBX GNSS devices
 */
const struct no_os_gnss_platform_ops nmea_ubx_gnss_ops = {
	.init = nmea_ubx_gnss_platform_init,
	.refresh_timing_data = nmea_ubx_gnss_platform_refresh_timing_data,
	.is_timing_valid = nmea_ubx_gnss_platform_is_timing_valid,
	.get_unified_timing = nmea_ubx_gnss_platform_get_unified_timing,
	.get_unix_epoch_unified = nmea_ubx_gnss_platform_get_unix_epoch_unified,
	.get_position_data = nmea_ubx_gnss_platform_get_position_data,
	.remove = nmea_ubx_gnss_platform_remove
};
