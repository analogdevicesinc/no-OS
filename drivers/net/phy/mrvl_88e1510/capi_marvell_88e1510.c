/***************************************************************************//**
 *   @file   capi_marvell_88e1510.c
 *   @brief  Implementation of capi driver for the Marvell 88E1510 PHY models.
 *
 *   @author Nicolae-Daniel Deaconescu (Nicolae-daniel.Deaconescu@analog.com)
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

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <capi_eth_phy.h>
#include "capi_marvell_88e1510.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"

#define MRVL_RESET_TIMEOUT_MS  500
#define MRVL_RESET_POLL_MS     10

/* ---------- internal helpers ---------- */

static int mrvl_read(struct mrvl_88e1510_handle *dev,
		     uint8_t reg, uint16_t *val)
{
	if (!dev->fn_read)
		return -EINVAL;
	return dev->fn_read(dev->phy_addr, reg, val);
}

static int mrvl_write(struct mrvl_88e1510_handle *dev,
		      uint8_t reg, uint16_t val)
{
	if (!dev->fn_write)
		return -EINVAL;
	return dev->fn_write(dev->phy_addr, reg, val);
}

static int mrvl_modify(struct mrvl_88e1510_handle *dev,
		       uint8_t reg, uint16_t mask, uint16_t val)
{
	uint16_t tmp;
	int ret;

	ret = mrvl_read(dev, reg, &tmp);
	if (ret)
		return ret;

	tmp = (tmp & ~mask) | (val & mask);
	return mrvl_write(dev, reg, tmp);
}

static int mrvl_page_select(struct mrvl_88e1510_handle *dev, uint8_t page)
{
	return mrvl_write(dev, MRVL_REG_PAGE_SELECT, (uint16_t)page);
}

static int mrvl_soft_reset(struct mrvl_88e1510_handle *dev)
{
	uint16_t val;
	int ret, timeout;

	ret = mrvl_modify(dev, MRVL_REG_CONTROL, MRVL_CTRL_RESET,
			  MRVL_CTRL_RESET);
	if (ret)
		return ret;

	for (timeout = MRVL_RESET_TIMEOUT_MS; timeout > 0;
	     timeout -= MRVL_RESET_POLL_MS) {
		no_os_mdelay(MRVL_RESET_POLL_MS);
		ret = mrvl_read(dev, MRVL_REG_CONTROL, &val);
		if (ret)
			return ret;
		if (!(val & MRVL_CTRL_RESET))
			return 0;
	}

	return -ETIMEDOUT;
}

static int mrvl_config_rgmii_delays(struct mrvl_88e1510_handle *dev)
{
	uint16_t val = 0;
	int ret;

	ret = mrvl_page_select(dev, MRVL_PAGE_RGMII_DELAY);
	if (ret)
		return ret;

	if (dev->rgmii.rx_delay_en)
		val |= MRVL_MAC_SPEC_RGMII_RX_DELAY;
	if (dev->rgmii.tx_delay_en)
		val |= MRVL_MAC_SPEC_RGMII_TX_DELAY;

	ret = mrvl_modify(dev, MRVL_REG_MAC_SPEC_CTRL,
			  MRVL_MAC_SPEC_RGMII_RX_DELAY |
			  MRVL_MAC_SPEC_RGMII_TX_DELAY, val);
	if (ret)
		return ret;

	return mrvl_page_select(dev, MRVL_PAGE_COPPER);
}

static int mrvl_config_downshift(struct mrvl_88e1510_handle *dev)
{
	uint16_t val = 0;

	if (dev->downshift_en) {
		uint8_t cnt = dev->downshift_retries;
		if (cnt < 1)
			cnt = 1;
		if (cnt > 8)
			cnt = 8;

		val = MRVL_PSC_DOWNSHIFT_EN |
		      (uint16_t)((cnt - 1) << MRVL_PSC_DOWNSHIFT_CNT_SHIFT);
	}

	return mrvl_modify(dev, MRVL_REG_PHY_SPEC_CTRL,
			   MRVL_PSC_DOWNSHIFT_EN | MRVL_PSC_DOWNSHIFT_CNT_MASK,
			   val);
}

static int mrvl_set_autoneg_adv(struct mrvl_88e1510_handle *dev,
				enum capi_eth_phy_speed speed,
				enum capi_eth_phy_duplex duplex)
{
	uint16_t adv = MRVL_ADV_SELECTOR_IEEE;
	uint16_t adv_1000 = 0;
	int ret;

	switch (speed) {
	case CAPI_ETH_PHY_SPEED_1G:
		if (duplex == CAPI_ETH_PHY_DUPLEX_FULL)
			adv_1000 |= MRVL_1000BT_ADV_1000FD;
		else
			adv_1000 |= MRVL_1000BT_ADV_1000HD;
		adv |= MRVL_ADV_100FD | MRVL_ADV_100HD;
		adv |= MRVL_ADV_10FD | MRVL_ADV_10HD;
		break;
	case CAPI_ETH_PHY_SPEED_100M:
		if (duplex == CAPI_ETH_PHY_DUPLEX_FULL)
			adv |= MRVL_ADV_100FD;
		else
			adv |= MRVL_ADV_100HD;
		adv |= MRVL_ADV_10FD | MRVL_ADV_10HD;
		break;
	case CAPI_ETH_PHY_SPEED_10M:
		if (duplex == CAPI_ETH_PHY_DUPLEX_FULL)
			adv |= MRVL_ADV_10FD;
		else
			adv |= MRVL_ADV_10HD;
		break;
	default:
		adv |= MRVL_ADV_100FD | MRVL_ADV_100HD |
		       MRVL_ADV_10FD | MRVL_ADV_10HD;
		adv_1000 |= MRVL_1000BT_ADV_1000FD;
		break;
	}

	ret = mrvl_modify(dev, MRVL_REG_AUTONEG_ADV,
			  MRVL_ADV_100FD | MRVL_ADV_100HD |
			  MRVL_ADV_10FD | MRVL_ADV_10HD, adv);
	if (ret)
		return ret;

	return mrvl_modify(dev, MRVL_REG_1000BT_CONTROL,
			   MRVL_1000BT_ADV_1000FD | MRVL_1000BT_ADV_1000HD,
			   adv_1000);
}

/* ---------- ops implementations ---------- */

static int mrvl_init(struct capi_eth_phy_handle **handle,
		     const struct capi_eth_phy_init_config *config)
{
	struct mrvl_88e1510_handle *dev;
	const struct mrvl_88e1510_extra_config *extra;
	uint16_t phyid1, phyid2;
	uint32_t oui;
	int ret;

	if (!handle || !config || !config->fn_read || !config->fn_write)
		return -EINVAL;

	if (*handle) {
		dev = (struct mrvl_88e1510_handle *)*handle;
		dev->base.init_allocated = false;
	} else {
		dev = no_os_calloc(1, sizeof(*dev));
		if (!dev)
			return -ENOMEM;
		dev->base.init_allocated = true;
	}

	dev->base.ops = config->ops;
	dev->phy_addr = config->phy_addr;
	dev->fn_read = config->fn_read;
	dev->fn_write = config->fn_write;

	extra = (const struct mrvl_88e1510_extra_config *)config->extra;
	if (extra) {
		dev->rgmii = extra->rgmii;
		dev->downshift_en = extra->downshift_en;
		dev->downshift_retries = extra->downshift_retries;
	}

	ret = mrvl_soft_reset(dev);
	if (ret)
		goto err_free;

	ret = mrvl_read(dev, MRVL_REG_PHY_ID1, &phyid1);
	if (ret)
		goto err_free;
	ret = mrvl_read(dev, MRVL_REG_PHY_ID2, &phyid2);
	if (ret)
		goto err_free;

	oui = ((uint32_t)phyid1 << 6) | ((phyid2 >> 10) & 0x3F);
	if (oui != MARVELL_PHY_ID) {
		ret = -ENODEV;
		goto err_free;
	}

	ret = mrvl_config_downshift(dev);
	if (ret)
		goto err_free;

	*handle = &dev->base;
	return 0;

err_free:
	if (dev->base.init_allocated)
		no_os_free(dev);
	return ret;
}

static int mrvl_deinit(struct capi_eth_phy_handle *handle)
{
	struct mrvl_88e1510_handle *dev = (struct mrvl_88e1510_handle *)handle;

	if (!dev)
		return -EINVAL;

	mrvl_modify(dev, MRVL_REG_CONTROL, MRVL_CTRL_POWER_DOWN,
		    MRVL_CTRL_POWER_DOWN);

	if (dev->base.init_allocated)
		no_os_free(dev);

	return 0;
}

static int mrvl_power_control(struct capi_eth_phy_handle *handle,
			      enum capi_eth_phy_power_state state)
{
	struct mrvl_88e1510_handle *dev = (struct mrvl_88e1510_handle *)handle;

	if (!dev)
		return -EINVAL;

	switch (state) {
	case CAPI_ETH_PHY_POWER_OFF:
	case CAPI_ETH_PHY_POWER_LOW:
		return mrvl_modify(dev, MRVL_REG_CONTROL,
				   MRVL_CTRL_POWER_DOWN,
				   MRVL_CTRL_POWER_DOWN);
	case CAPI_ETH_PHY_POWER_FULL:
		return mrvl_modify(dev, MRVL_REG_CONTROL,
				   MRVL_CTRL_POWER_DOWN, 0);
	default:
		return -EINVAL;
	}
}

static int mrvl_set_interface(struct capi_eth_phy_handle *handle,
			      enum capi_eth_interface interface)
{
	struct mrvl_88e1510_handle *dev = (struct mrvl_88e1510_handle *)handle;
	int ret;

	if (!dev)
		return -EINVAL;

	dev->interface = interface;

	if (interface == CAPI_ETH_INTERFACE_RGMII) {
		ret = mrvl_config_rgmii_delays(dev);
		if (ret)
			return ret;
		return mrvl_soft_reset(dev);
	}

	return 0;
}

static int mrvl_set_mode(struct capi_eth_phy_handle *handle,
			 const struct capi_eth_phy_mode_config *config)
{
	struct mrvl_88e1510_handle *dev = (struct mrvl_88e1510_handle *)handle;
	uint16_t ctrl = 0;
	uint16_t ctrl_mask;
	uint16_t mdix_val;
	int ret;

	if (!dev || !config)
		return -EINVAL;

	switch (config->mdix) {
	case CAPI_ETH_MDIX_MDI:
		mdix_val = MRVL_PSC_MDIX_FORCE_MDI;
		break;
	case CAPI_ETH_MDIX_MDIX:
		mdix_val = MRVL_PSC_MDIX_FORCE_MDIX;
		break;
	case CAPI_ETH_MDIX_AUTO:
	default:
		mdix_val = MRVL_PSC_MDIX_AUTO;
		break;
	}

	ret = mrvl_modify(dev, MRVL_REG_PHY_SPEC_CTRL,
			  MRVL_PSC_MDIX_MASK, mdix_val);
	if (ret)
		return ret;

	if (config->auto_negotiate) {
		ret = mrvl_set_autoneg_adv(dev, config->speed, config->duplex);
		if (ret)
			return ret;
		ctrl |= MRVL_CTRL_AUTONEG_EN | MRVL_CTRL_RESTART_AUTONEG;
	} else {
		switch (config->speed) {
		case CAPI_ETH_PHY_SPEED_1G:
			ctrl |= MRVL_CTRL_SPEED_MSB;
			break;
		case CAPI_ETH_PHY_SPEED_100M:
			ctrl |= MRVL_CTRL_SPEED_LSB;
			break;
		case CAPI_ETH_PHY_SPEED_10M:
		default:
			break;
		}

		if (config->duplex == CAPI_ETH_PHY_DUPLEX_FULL)
			ctrl |= MRVL_CTRL_DUPLEX;
	}

	if (config->loopback)
		ctrl |= MRVL_CTRL_LOOPBACK;
	if (config->isolate)
		ctrl |= MRVL_CTRL_ISOLATE;

	ctrl_mask = MRVL_CTRL_AUTONEG_EN | MRVL_CTRL_RESTART_AUTONEG |
		    MRVL_CTRL_SPEED_MSB | MRVL_CTRL_SPEED_LSB |
		    MRVL_CTRL_DUPLEX | MRVL_CTRL_LOOPBACK |
		    MRVL_CTRL_ISOLATE;

	return mrvl_modify(dev, MRVL_REG_CONTROL, ctrl_mask, ctrl);
}

static int mrvl_get_link_state(struct capi_eth_phy_handle *handle,
			       enum capi_eth_link_state *state)
{
	struct mrvl_88e1510_handle *dev = (struct mrvl_88e1510_handle *)handle;
	uint16_t pss;
	int ret;

	if (!dev || !state)
		return -EINVAL;

	ret = mrvl_read(dev, MRVL_REG_PHY_SPEC_STATUS, &pss);
	if (ret)
		return ret;

	*state = (pss & MRVL_PSS_LINK) ? CAPI_ETH_LINK_UP : CAPI_ETH_LINK_DOWN;
	return 0;
}

static int mrvl_get_link_info(struct capi_eth_phy_handle *handle,
			      struct capi_eth_link_info *info)
{
	struct mrvl_88e1510_handle *dev = (struct mrvl_88e1510_handle *)handle;
	uint16_t pss;
	int ret;

	if (!dev || !info)
		return -EINVAL;

	ret = mrvl_read(dev, MRVL_REG_PHY_SPEC_STATUS, &pss);
	if (ret)
		return ret;

	info->state = (pss & MRVL_PSS_LINK) ?
		      CAPI_ETH_LINK_UP : CAPI_ETH_LINK_DOWN;

	if (info->state != CAPI_ETH_LINK_UP) {
		info->speed = CAPI_ETH_PHY_SPEED_UNKNOWN;
		info->duplex = CAPI_ETH_PHY_DUPLEX_UNKNOWN;
		info->mdix = CAPI_ETH_MDIX_AUTO;
		return 0;
	}

	switch (pss & MRVL_PSS_SPEED_MASK) {
	case MRVL_PSS_SPEED_1000:
		info->speed = CAPI_ETH_PHY_SPEED_1G;
		break;
	case MRVL_PSS_SPEED_100:
		info->speed = CAPI_ETH_PHY_SPEED_100M;
		break;
	case MRVL_PSS_SPEED_10:
	default:
		info->speed = CAPI_ETH_PHY_SPEED_10M;
		break;
	}

	info->duplex = (pss & MRVL_PSS_DUPLEX) ?
		       CAPI_ETH_PHY_DUPLEX_FULL : CAPI_ETH_PHY_DUPLEX_HALF;

	info->mdix = (pss & MRVL_PSS_MDIX) ?
		     CAPI_ETH_MDIX_MDIX : CAPI_ETH_MDIX_MDI;

	return 0;
}

static int mrvl_register_callback(struct capi_eth_phy_handle *handle,
				  capi_eth_phy_event_callback const callback,
				  void *const callback_arg)
{
	struct mrvl_88e1510_handle *dev = (struct mrvl_88e1510_handle *)handle;
	uint16_t irq_mask, dummy;
	int ret;

	if (!dev)
		return -EINVAL;

	dev->callback = callback;
	dev->callback_arg = callback_arg;

	if (callback) {
		irq_mask = MRVL_IRQ_LINK_STATUS_CHANGE;
		dev->irq_mask = irq_mask;
	} else {
		irq_mask = 0;
		dev->irq_mask = 0;
	}

	ret = mrvl_read(dev, MRVL_REG_IRQ_STATUS, &dummy);
	if (ret)
		return ret;

	return mrvl_write(dev, MRVL_REG_IRQ_ENABLE, irq_mask);
}

static void mrvl_isr(void *handle)
{
	struct mrvl_88e1510_handle *dev = (struct mrvl_88e1510_handle *)handle;
	uint16_t irq_status, pss;
	uint32_t events = 0;

	if (!dev || !dev->callback)
		return;

	if (mrvl_read(dev, MRVL_REG_IRQ_STATUS, &irq_status))
		return;

	if (!(irq_status & dev->irq_mask))
		return;

	if (irq_status & MRVL_IRQ_LINK_STATUS_CHANGE) {
		if (mrvl_read(dev, MRVL_REG_PHY_SPEC_STATUS, &pss))
			return;

		if (pss & MRVL_PSS_LINK)
			events |= CAPI_ETH_PHY_EVENT_LINK_UP;
		else
			events |= CAPI_ETH_PHY_EVENT_LINK_DOWN;
	}

	if (events)
		dev->callback(events, dev->callback_arg);
}

/* ---------- ops table ---------- */

const struct capi_eth_phy_ops mrvl_88e1510_ops = {
	.init              = mrvl_init,
	.deinit            = mrvl_deinit,
	.power_control     = mrvl_power_control,
	.set_interface     = mrvl_set_interface,
	.set_mode          = mrvl_set_mode,
	.get_link_state    = mrvl_get_link_state,
	.get_link_info     = mrvl_get_link_info,
	.register_callback = mrvl_register_callback,
	.isr               = mrvl_isr,
};
