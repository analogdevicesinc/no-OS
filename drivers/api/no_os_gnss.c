/***************************************************************************//**
 *   @file   no_os_gnss.c
 *   @brief  Implementation of GNSS Interface
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
uint32_t no_os_gnss_get_unix_epoch_unified(struct no_os_gnss_desc *desc,
		uint32_t *fractional_seconds)
{
	if (!desc || !desc->platform_ops || !fractional_seconds)
		return 0;

	return desc->platform_ops->get_unix_epoch_unified(desc, fractional_seconds);
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