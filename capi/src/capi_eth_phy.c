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

/* IEEE 802.3 standard PHY ID registers used by the bus scan. */
#define CAPI_ETH_PHY_REG_ID1	0x02
#define CAPI_ETH_PHY_REG_ID2	0x03

/**
 * @brief Scan the MDIO bus for a responding PHY.
 *
 */
static int capi_eth_phy_scan(capi_eth_phy_read_fn fn_read, uint8_t *out_addr)
{
	uint16_t id1, id2;
	uint8_t addr;
	int ret;

	if (fn_read == NULL || out_addr == NULL)
		return -EINVAL;

	for (addr = 0; addr <= 31; addr++) {
		ret  = fn_read(addr, CAPI_ETH_PHY_REG_ID1, &id1);
		ret |= fn_read(addr, CAPI_ETH_PHY_REG_ID2, &id2);

		if (ret == 0 &&
		    id1 > 0x0000 && id1 < 0xFFFF &&
		    id2 > 0x0000 && id2 < 0xFFFF) {
			*out_addr = addr;
			return 0;
		}
	}

	return -ENODEV;
}

int capi_eth_phy_init(struct capi_eth_phy_handle **handle,
		      const struct capi_eth_phy_init_config *config)
{
	struct capi_eth_phy_init_config patched;
	int ret;

	if (config == NULL || config->ops == NULL || config->ops->init == NULL)
		return -EINVAL;

	if (config->phy_addr == 0) {
		uint8_t found;

		ret = capi_eth_phy_scan(config->fn_read, &found);
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