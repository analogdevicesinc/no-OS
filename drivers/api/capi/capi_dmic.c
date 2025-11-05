/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_dmic.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the DMIC driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the DMIC functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_dmic.h>

int capi_dmic_init(struct capi_dmic_handle **handle, const struct capi_dmic_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->init != NULL) {
		int result = config->ops->init(handle, config);
		return (result < 0) ? result : 0;
	}

	return -EINVAL;
}

int capi_dmic_deinit(const struct capi_dmic_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->deinit != NULL) {
		int32_t result = handle->ops->deinit(handle);
		return (result < 0) ? result : 0;
	}

	return -EINVAL;
}

int capi_dmic_configure_chan_pair(const struct capi_dmic_handle *handle,
				  enum capi_dmic_chan_pair ch_pair,
				  const struct capi_dmic_chan_pair_config *ch_pair_config)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->configure_chan_pair != NULL) {
		int32_t result = handle->ops->configure_chan_pair(handle, ch_pair, ch_pair_config);
		return (result < 0) ? result : 0;
	}

	return -EINVAL;
}

int capi_dmic_configure_chan_pairs(const struct capi_dmic_handle *handle, uint8_t ch_pair_mask,
				   const struct capi_dmic_chan_pair_config *ch_pair_config)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->configure_chan_pairs != NULL) {
		int32_t result =
			handle->ops->configure_chan_pairs(handle, ch_pair_mask, ch_pair_config);
		return (result < 0) ? result : 0;
	}

	return -EINVAL;
}

int capi_dmic_enable_hpf(const struct capi_dmic_handle *handle, enum capi_dmic_chan_pair ch_pair,
			 bool en, enum capi_dmic_hpf_cf cut_off_freq)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->enable_hpf != NULL) {
		int32_t result = handle->ops->enable_hpf(handle, ch_pair, en, cut_off_freq);
		return (result < 0) ? result : 0;
	}

	return -EINVAL;
}

int capi_dmic_configure_vol_param(const struct capi_dmic_handle *handle,
				  enum capi_dmic_vol_param vol_param, bool en)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->configure_vol_param != NULL) {
		int32_t result = handle->ops->configure_vol_param(handle, vol_param, en);
		return (result < 0) ? result : 0;
	}

	return -EINVAL;
}

int capi_dmic_set_chan_volume(const struct capi_dmic_handle *handle, enum capi_dmic_ch ch,
			      uint8_t vol)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->set_chan_volume != NULL) {
		int32_t result = handle->ops->set_chan_volume(handle, ch, vol);
		return (result < 0) ? result : 0;
	}

	return -EINVAL;
}

int capi_dmic_enable_channels(const struct capi_dmic_handle *handle, uint8_t ch_mask)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->enable_channels != NULL) {
		int32_t result = handle->ops->enable_channels(handle, ch_mask);
		return (result < 0) ? result : 0;
	}

	return -EINVAL;
}

int capi_dmic_disable_channels(const struct capi_dmic_handle *handle, uint8_t ch_mask)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->disable_channels != NULL) {
		int32_t result = handle->ops->disable_channels(handle, ch_mask);
		return (result < 0) ? result : 0;
	}

	return -EINVAL;
}

int capi_dmic_configure_clk(const struct capi_dmic_handle *handle, bool clk_map, bool en,
			    enum capi_dmic_clk_rate clk_rate)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->configure_clk != NULL) {
		int32_t result = handle->ops->configure_clk(handle, clk_map, en, clk_rate);
		return (result < 0) ? result : 0;
	}

	return -EINVAL;
}

int capi_dmic_mute_channels(const struct capi_dmic_handle *handle, uint8_t ch_mask)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->mute_channels != NULL) {
		int32_t result = handle->ops->mute_channels(handle, ch_mask);
		return (result < 0) ? result : 0;
	}

	return -EINVAL;
}

int capi_dmic_unmute_channels(const struct capi_dmic_handle *handle, uint8_t ch_mask)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->unmute_channels != NULL) {
		int32_t result = handle->ops->unmute_channels(handle, ch_mask);
		return (result < 0) ? result : 0;
	}

	return -EINVAL;
}