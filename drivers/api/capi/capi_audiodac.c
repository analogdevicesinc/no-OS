/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_audiodac.c
 * @brief This file contains a set of small wrapper functions that can be used to access
 *        the Audio DAC driver via its ops pointer structure. Note, it does not
 *        implement any thread safety such as mutually excluding calls to the Audio DAC functions.
 *        If this is needed, it is suggested that this file and all of the other driver
 *        wrappers be copied and enhanced in your project.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>
#include <capi_audiodac.h>

int capi_audiodac_init(struct capi_audiodac_handle **handle,
		       const struct capi_audiodac_config *config)
{
	if (config != NULL && config->ops != NULL && config->ops->init != NULL) {
		return config->ops->init(handle, config);
	}

	return -EINVAL;
}

int capi_audiodac_deinit(const struct capi_audiodac_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->deinit != NULL) {
		return handle->ops->deinit(handle);
	}

	return -EINVAL;
}

int capi_audiodac_clk_config(const struct capi_audiodac_handle *handle,
			     enum capi_audiodac_mclk_freq mclk_freq)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->clk_config != NULL) {
		return handle->ops->clk_config(handle, mclk_freq);
	}

	return -EINVAL;
}

int capi_audiodac_hw_domain_enable(const struct capi_audiodac_handle *handle,
				   enum capi_audiodac_hw_domain domain)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->hw_domain_enable != NULL) {
		return handle->ops->hw_domain_enable(handle, domain);
	}

	return -EINVAL;
}

int capi_audiodac_hw_domain_disable(const struct capi_audiodac_handle *handle,
				    enum capi_audiodac_hw_domain domain)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->hw_domain_disable != NULL) {
		return handle->ops->hw_domain_disable(handle, domain);
	}

	return -EINVAL;
}

int capi_audiodac_chan_pair_config(const struct capi_audiodac_handle *handle,
				   enum capi_audiodac_chan_pair chan_pair_id,
				   const struct capi_audiodac_chan_pair_cfg *chan_pair_cfg)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->chan_pair_config != NULL) {
		return handle->ops->chan_pair_config(handle, chan_pair_id, chan_pair_cfg);
	}

	return -EINVAL;
}

int capi_audiodac_chan_pair_data_interface_activate(const struct capi_audiodac_handle *handle,
						    enum capi_audiodac_chan_pair chan_pair_id)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->chan_pair_data_interface_activate != NULL) {
		return handle->ops->chan_pair_data_interface_activate(handle, chan_pair_id);
	}

	return -EINVAL;
}

int capi_audiodac_chan_pair_data_interface_deactivate(const struct capi_audiodac_handle *handle,
						      enum capi_audiodac_chan_pair chan_pair_id)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->chan_pair_data_interface_deactivate != NULL) {
		return handle->ops->chan_pair_data_interface_deactivate(handle, chan_pair_id);
	}

	return -EINVAL;
}

int capi_audiodac_chan_pair_set_operational_power_mode(const struct capi_audiodac_handle *handle,
						       enum capi_audiodac_chan_pair chan_pair_id,
						       enum capi_audiodac_op_power_mode power_mode)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->chan_pair_set_operational_power_mode != NULL) {
		return handle->ops->chan_pair_set_operational_power_mode(handle, chan_pair_id,
									 power_mode);
	}

	return -EINVAL;
}

int capi_audiodac_chan_pair_get_operational_power_mode(const struct capi_audiodac_handle *handle,
						       enum capi_audiodac_chan_pair chan_pair_id,
						       enum capi_audiodac_op_power_mode *power_mode)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->chan_pair_get_operational_power_mode != NULL) {
		return handle->ops->chan_pair_get_operational_power_mode(handle, chan_pair_id,
									 power_mode);
	}

	return -EINVAL;
}

int capi_audiodac_chan_conversion_activate(const struct capi_audiodac_handle *handle,
					   enum capi_audiodac_chan chan_id)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->chan_conversion_activate != NULL) {
		return handle->ops->chan_conversion_activate(handle, chan_id);
	}

	return -EINVAL;
}

int capi_audiodac_chan_conversion_deactivate(const struct capi_audiodac_handle *handle,
					     enum capi_audiodac_chan chan_id)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->chan_conversion_deactivate != NULL) {
		return handle->ops->chan_conversion_deactivate(handle, chan_id);
	}

	return -EINVAL;
}

int capi_audiodac_chan_soft_mute(const struct capi_audiodac_handle *handle,
				 enum capi_audiodac_chan chan_id)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->chan_soft_mute != NULL) {
		return handle->ops->chan_soft_mute(handle, chan_id);
	}

	return -EINVAL;
}

int capi_audiodac_chan_soft_unmute(const struct capi_audiodac_handle *handle,
				   enum capi_audiodac_chan chan_id)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->chan_soft_unmute != NULL) {
		return handle->ops->chan_soft_unmute(handle, chan_id);
	}

	return -EINVAL;
}

int capi_audiodac_chan_activate_amplifier(const struct capi_audiodac_handle *handle,
					  enum capi_audiodac_chan chan_id)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->chan_activate_amplifier != NULL) {
		return handle->ops->chan_activate_amplifier(handle, chan_id);
	}

	return -EINVAL;
}

int capi_audiodac_chan_deactivate_amplifier(const struct capi_audiodac_handle *handle,
					    enum capi_audiodac_chan chan_id)
{
	if (handle != NULL && handle->ops != NULL &&
	    handle->ops->chan_deactivate_amplifier != NULL) {
		return handle->ops->chan_deactivate_amplifier(handle, chan_id);
	}

	return -EINVAL;
}