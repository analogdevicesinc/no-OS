/***************************************************************************//**
 *   @file   no_os_gnss.c
 *   @brief  Implementation of GNSS Interface
 *   @author Radu Etz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <stdlib.h>
#include <errno.h>
#include "no_os_gnss.h"
#include "no_os_alloc.h"
#include "no_os_mutex.h"

/* Public API Functions */

/**
 * @brief Initialize the GNSS communication peripheral.
 */
int32_t no_os_gnss_init(struct no_os_gnss_desc **desc,
			const struct no_os_gnss_init_param *param)
{
	if (!desc || !param || !param->platform_ops)
		return -EINVAL;

	return param->platform_ops->init(desc, param);
}

/**
 * @brief Free the resources allocated by no_os_gnss_init().
 */
int32_t no_os_gnss_remove(struct no_os_gnss_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	return desc->platform_ops->remove(desc);
}

/**
 * @brief Refresh timing data from GNSS device.
 */
int32_t no_os_gnss_refresh_timing_data(struct no_os_gnss_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	return desc->platform_ops->refresh_timing_data(desc);
}

/**
 * @brief Check if timing data is valid.
 */
int32_t no_os_gnss_is_timing_valid(struct no_os_gnss_desc *desc, bool *valid)
{
	if (!desc || !desc->platform_ops || !valid)
		return -EINVAL;

	return desc->platform_ops->is_timing_valid(desc, valid);
}

/**
 * @brief Get unified timing information with best available precision.
 */
int32_t no_os_gnss_get_unified_timing(struct no_os_gnss_desc *desc,
				      struct no_os_gnss_precise_time *timing)
{
	if (!desc || !desc->platform_ops || !timing)
		return -EINVAL;

	return desc->platform_ops->get_unified_timing(desc, timing);
}

/**
 * @brief Get Unix epoch timestamp with unified precision.
 */
int32_t no_os_gnss_get_unix_epoch_unified(struct no_os_gnss_desc *desc,
		uint32_t *epoch, uint32_t *fractional_seconds)
{
	if (!desc || !desc->platform_ops || !epoch || !fractional_seconds)
		return -EINVAL;

	return desc->platform_ops->get_unix_epoch_unified(desc, epoch,
			fractional_seconds);
}

/**
 * @brief Get GPS position and fix quality data.
 */
int32_t no_os_gnss_get_position_data(struct no_os_gnss_desc *desc,
				     struct no_os_gnss_position_data *position_data)
{
	if (!desc || !desc->platform_ops || !position_data)
		return -EINVAL;

	return desc->platform_ops->get_position_data(desc, position_data);
}
