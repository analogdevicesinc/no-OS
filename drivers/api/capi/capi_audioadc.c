/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_audioadc.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the Audio ADC driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the Audio ADC functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_audioadc.h>

int capi_audioadc_init(struct capi_audioadc_handle **handle,
		       const struct capi_audioadc_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->init != NULL) {
		return config->ops->init(handle, config);
	}

	return -EINVAL;
}

int capi_audioadc_deinit(const struct capi_audioadc_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->deinit != NULL) {
		return handle->ops->deinit(handle);
	}

	return -EINVAL;
}

int capi_audioadc_clk_config(const struct capi_audioadc_handle *handle,
			     enum capi_audioadc_mclk_freq mclk_freq)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->clk_config != NULL) {
		return handle->ops->clk_config(handle, mclk_freq);
	}

	return -EINVAL;
}

int capi_audioadc_hw_domain_enable(const struct capi_audioadc_handle *handle,
				   enum capi_audioadc_hw_domain domain)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->hw_domain_enable != NULL) {
		return handle->ops->hw_domain_enable(handle, domain);
	}

	return -EINVAL;
}

int capi_audioadc_hw_domain_disable(const struct capi_audioadc_handle *handle,
				    enum capi_audioadc_hw_domain domain)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->hw_domain_disable != NULL) {
		return handle->ops->hw_domain_disable(handle, domain);
	}

	return -EINVAL;
}

int capi_audioadc_set_bypass_mode(const struct capi_audioadc_handle *handle,
				  enum capi_audioadc_bypass_mode bypass_mode)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->set_bypass_mode != NULL) {
		return handle->ops->set_bypass_mode(handle, bypass_mode);
	}

	return -EINVAL;
}

int capi_audioadc_get_power_on_ack(const struct capi_audioadc_handle *handle, bool *pon_ack)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->get_power_on_ack != NULL) {
		return handle->ops->get_power_on_ack(handle, pon_ack);
	}

	return -EINVAL;
}

int capi_audioadc_resync(const struct capi_audioadc_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->resync != NULL) {
		return handle->ops->resync(handle);
	}

	return -EINVAL;
}

int capi_audioadc_chan_pair_config(const struct capi_audioadc_handle *handle,
				   enum capi_audioadc_chan_pair chan_pair_id,
				   const struct capi_audioadc_chan_pair_cfg *chan_pair_cfg)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->chan_pair_config != NULL) {
		return handle->ops->chan_pair_config(handle, chan_pair_id, chan_pair_cfg);
	}

	return -EINVAL;
}

int capi_audioadc_chan_pair_activate(const struct capi_audioadc_handle *handle,
				     enum capi_audioadc_chan_pair chan_pair_id)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->chan_pair_activate != NULL) {
		return handle->ops->chan_pair_activate(handle, chan_pair_id);
	}

	return -EINVAL;
}

int capi_audioadc_chan_pair_deactivate(const struct capi_audioadc_handle *handle,
				       enum capi_audioadc_chan_pair chan_pair_id)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->chan_pair_deactivate != NULL) {
		return handle->ops->chan_pair_deactivate(handle, chan_pair_id);
	}

	return -EINVAL;
}

int capi_audioadc_chan_pair_set_operational_power_mode(const struct capi_audioadc_handle *handle,
						       enum capi_audioadc_chan_pair chan_pair_id,
						       enum capi_audioadc_op_power_mode power_mode)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->chan_pair_set_operational_power_mode != NULL) {
		return handle->ops->chan_pair_set_operational_power_mode(handle, chan_pair_id,
									 power_mode);
	}

	return -EINVAL;
}

int capi_audioadc_chan_pair_get_operational_power_mode(const struct capi_audioadc_handle *handle,
						       enum capi_audioadc_chan_pair chan_pair_id,
						       enum capi_audioadc_op_power_mode *power_mode)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->chan_pair_get_operational_power_mode != NULL) {
		return handle->ops->chan_pair_get_operational_power_mode(handle, chan_pair_id,
									 power_mode);
	}

	return -EINVAL;
}

int capi_audioadc_chan_activate(const struct capi_audioadc_handle *handle,
				enum capi_audioadc_chan chan_id)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->chan_activate != NULL) {
		return handle->ops->chan_activate(handle, chan_id);
	}

	return -EINVAL;
}

int capi_audioadc_chan_deactivate(const struct capi_audioadc_handle *handle,
				  enum capi_audioadc_chan chan_id)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->chan_deactivate != NULL) {
		return handle->ops->chan_deactivate(handle, chan_id);
	}

	return -EINVAL;
}

int capi_audioadc_chan_deactivate_all(const struct capi_audioadc_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->chan_deactivate_all != NULL) {
		return handle->ops->chan_deactivate_all(handle);
	}

	return -EINVAL;
}

int capi_audioadc_chan_activate_mic_bias(const struct capi_audioadc_handle *handle,
					 enum capi_audioadc_chan chan_id)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->chan_activate_mic_bias != NULL) {
		return handle->ops->chan_activate_mic_bias(handle, chan_id);
	}

	return -EINVAL;
}

int capi_audioadc_chan_deactivate_mic_bias(const struct capi_audioadc_handle *handle,
					   enum capi_audioadc_chan chan_id)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->chan_deactivate_mic_bias != NULL) {
		return handle->ops->chan_deactivate_mic_bias(handle, chan_id);
	}

	return -EINVAL;
}

int capi_audioadc_chan_soft_mute(const struct capi_audioadc_handle *handle,
				 enum capi_audioadc_chan chan_id)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->chan_soft_mute != NULL) {
		return handle->ops->chan_soft_mute(handle, chan_id);
	}

	return -EINVAL;
}

int capi_audioadc_chan_soft_unmute(const struct capi_audioadc_handle *handle,
				   enum capi_audioadc_chan chan_id)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->chan_soft_unmute != NULL) {
		return handle->ops->chan_soft_unmute(handle, chan_id);
	}

	return -EINVAL;
}

int capi_audioadc_chan_hard_mute(const struct capi_audioadc_handle *handle,
				 enum capi_audioadc_chan chan_id)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->chan_hard_mute != NULL) {
		return handle->ops->chan_hard_mute(handle, chan_id);
	}

	return -EINVAL;
}

int capi_audioadc_chan_hard_unmute(const struct capi_audioadc_handle *handle,
				   enum capi_audioadc_chan chan_id)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->chan_hard_unmute != NULL) {
		return handle->ops->chan_hard_unmute(handle, chan_id);
	}

	return -EINVAL;
}

int capi_audioadc_chan_set_digital_gain(const struct capi_audioadc_handle *handle,
					enum capi_audioadc_chan chan_id,
					enum capi_audioadc_digital_filters_gain gain_value)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->chan_set_digital_gain != NULL) {
		return handle->ops->chan_set_digital_gain(handle, chan_id, gain_value);
	}

	return -EINVAL;
}

int capi_audioadc_chan_enable_hpf(const struct capi_audioadc_handle *handle,
				  enum capi_audioadc_chan chan_id)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->chan_enable_hpf != NULL) {
		return handle->ops->chan_enable_hpf(handle, chan_id);
	}

	return -EINVAL;
}

int capi_audioadc_chan_disable_hpf(const struct capi_audioadc_handle *handle,
				   enum capi_audioadc_chan chan_id)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->chan_disable_hpf != NULL) {
		return handle->ops->chan_disable_hpf(handle, chan_id);
	}

	return -EINVAL;
}