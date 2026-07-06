/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file capi_eth_phy.c
 * @brief Thin wrapper functions that dispatch Ethernet PHY driver calls
 *        through the ops pointer structure.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <capi_eth_phy.h>
#include <capi_mdio.h>

int capi_eth_phy_init(struct capi_eth_phy_handle **handle,
		      const struct capi_eth_phy_init_config *config)
{
	struct capi_eth_phy_init_config patched;
	int ret;

	if (config == NULL || config->ops == NULL || config->ops->init == NULL)
		return -EINVAL;

	if (config->phy_addr == CAPI_ETH_PHY_ADDR_ANY) {
		uint8_t found;

		ret = capi_mdio_scan(config->mdio_bus, &found);
		if (ret)
			return ret;

		patched = *config;
		patched.phy_addr = found;
		return patched.ops->init(handle, &patched);
	}

	return config->ops->init(handle, config);
}

int capi_eth_phy_deinit(struct capi_eth_phy_handle *handle)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->deinit != NULL) {
		return handle->ops->deinit(handle);
	}

	return -EINVAL;
}

int capi_eth_phy_power_control(struct capi_eth_phy_handle *handle,
			       enum capi_eth_phy_power_state state)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->power_control != NULL) {
		return handle->ops->power_control(handle, state);
	}

	return -EINVAL;
}

int capi_eth_phy_set_interface(struct capi_eth_phy_handle *handle,
			       enum capi_eth_interface interface)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->set_interface != NULL) {
		return handle->ops->set_interface(handle, interface);
	}

	return -EINVAL;
}

int capi_eth_phy_set_mode(struct capi_eth_phy_handle *handle,
			  const struct capi_eth_phy_mode_config *config)
{
	if (handle != NULL && handle->ops != NULL && handle->ops->set_mode != NULL) {
		return handle->ops->set_mode(handle, config);
	}

	return -EINVAL;
}

int capi_eth_phy_get_link_state(struct capi_eth_phy_handle *handle,
				enum capi_eth_link_state *state)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->get_link_state != NULL) {
		return handle->ops->get_link_state(handle, state);
	}

	return -EINVAL;
}

int capi_eth_phy_get_link_info(struct capi_eth_phy_handle *handle,
			       struct capi_eth_link_info *info)
{
	if (handle != NULL && handle->ops != NULL
	    && handle->ops->get_link_info != NULL) {
		return handle->ops->get_link_info(handle, info);
	}

	return -EINVAL;
}