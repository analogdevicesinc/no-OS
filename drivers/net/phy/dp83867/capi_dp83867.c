/***************************************************************************//**
 *   @file   capi_dp83867.c
 *   @brief  Implementation of the capi driver for the TI DP83867 PHY.
 *
 *   @author Nicolae-Daniel Deaconescu (Nicolae-daniel.Deaconescu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
#include "capi_dp83867.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"

#define DP83867_RESET_TIMEOUT_MS  500
#define DP83867_RESET_POLL_MS     10

/* ---------- internal helpers ---------- */

static int dp83867_read(struct dp83867_handle *dev, uint8_t reg, uint16_t *val)
{
	if (!dev->fn_read)
		return -EINVAL;
	return dev->fn_read(dev->phy_addr, reg, val);
}

static int dp83867_write(struct dp83867_handle *dev, uint8_t reg, uint16_t val)
{
	if (!dev->fn_write)
		return -EINVAL;
	return dev->fn_write(dev->phy_addr, reg, val);
}

static int dp83867_modify(struct dp83867_handle *dev, uint8_t reg,
			  uint16_t mask, uint16_t val)
{
	uint16_t tmp;
	int ret;

	ret = dp83867_read(dev, reg, &tmp);
	if (ret)
		return ret;

	tmp = (tmp & ~mask) | (val & mask);
	return dp83867_write(dev, reg, tmp);
}

static int dp83867_ext_read(struct dp83867_handle *dev, uint16_t reg,
			    uint16_t *val)
{
	int ret;

	ret = dp83867_write(dev, DP83867_REG_REGCR, DP83867_REGCR_ADDR);
	if (ret)
		return ret;
	ret = dp83867_write(dev, DP83867_REG_ADDAR, reg);
	if (ret)
		return ret;
	ret = dp83867_write(dev, DP83867_REG_REGCR, DP83867_REGCR_DATA_NO_INC);
	if (ret)
		return ret;
	return dp83867_read(dev, DP83867_REG_ADDAR, val);
}

static int dp83867_ext_write(struct dp83867_handle *dev, uint16_t reg,
			     uint16_t val)
{
	int ret;

	ret = dp83867_write(dev, DP83867_REG_REGCR, DP83867_REGCR_ADDR);
	if (ret)
		return ret;
	ret = dp83867_write(dev, DP83867_REG_ADDAR, reg);
	if (ret)
		return ret;
	ret = dp83867_write(dev, DP83867_REG_REGCR, DP83867_REGCR_DATA_NO_INC);
	if (ret)
		return ret;
	return dp83867_write(dev, DP83867_REG_ADDAR, val);
}

static int dp83867_ext_modify(struct dp83867_handle *dev, uint16_t reg,
			      uint16_t mask, uint16_t val)
{
	uint16_t tmp;
	int ret;

	ret = dp83867_ext_read(dev, reg, &tmp);
	if (ret)
		return ret;

	tmp = (tmp & ~mask) | (val & mask);
	return dp83867_ext_write(dev, reg, tmp);
}

static int dp83867_soft_reset(struct dp83867_handle *dev)
{
	uint16_t val;
	int ret, timeout;

	ret = dp83867_modify(dev, DP83867_REG_CONTROL, DP83867_CTRL_RESET,
			     DP83867_CTRL_RESET);
	if (ret)
		return ret;

	for (timeout = DP83867_RESET_TIMEOUT_MS; timeout > 0;
	     timeout -= DP83867_RESET_POLL_MS) {
		no_os_mdelay(DP83867_RESET_POLL_MS);
		ret = dp83867_read(dev, DP83867_REG_CONTROL, &val);
		if (ret)
			return ret;
		if (!(val & DP83867_CTRL_RESET))
			return 0;
	}

	return -ETIMEDOUT;
}

static int dp83867_config_rgmii_delays(struct dp83867_handle *dev)
{
	uint16_t ctl = 0;
	uint16_t dctl;
	int ret;

	if (dev->rgmii.rx_delay_en)
		ctl |= DP83867_RGMII_RX_CLK_DELAY_EN;
	if (dev->rgmii.tx_delay_en)
		ctl |= DP83867_RGMII_TX_CLK_DELAY_EN;

	ret = dp83867_ext_modify(dev, DP83867_REG_RGMIICTL,
				 DP83867_RGMII_RX_CLK_DELAY_EN |
				 DP83867_RGMII_TX_CLK_DELAY_EN, ctl);
	if (ret)
		return ret;

	dctl = no_os_field_prep(DP83867_RGMIIDCTL_TX_DELAY_MASK,
				dev->rgmii.tx_delay_code & 0xF) |
	       no_os_field_prep(DP83867_RGMIIDCTL_RX_DELAY_MASK,
				dev->rgmii.rx_delay_code & 0xF);

	return dp83867_ext_write(dev, DP83867_REG_RGMIIDCTL, dctl);
}

static int dp83867_config_leds(struct dp83867_handle *dev)
{
	return dp83867_modify(dev, DP83867_REG_LEDCR1, 0xFFFF,
			      DP83867_LEDCR1_DEFAULT);
}

static int dp83867_set_autoneg_adv(struct dp83867_handle *dev,
				   enum capi_eth_phy_speed speed,
				   enum capi_eth_phy_duplex duplex)
{
	uint16_t adv = DP83867_ADV_SELECTOR_IEEE;
	uint16_t adv_1000 = 0;
	int ret;

	switch (speed) {
	case CAPI_ETH_PHY_SPEED_1G:
		if (duplex == CAPI_ETH_PHY_DUPLEX_FULL)
			adv_1000 |= DP83867_1000BT_ADV_1000FD;
		else
			adv_1000 |= DP83867_1000BT_ADV_1000HD;
		adv |= DP83867_ADV_100FD | DP83867_ADV_100HD;
		adv |= DP83867_ADV_10FD | DP83867_ADV_10HD;
		break;
	case CAPI_ETH_PHY_SPEED_100M:
		if (duplex == CAPI_ETH_PHY_DUPLEX_FULL)
			adv |= DP83867_ADV_100FD;
		else
			adv |= DP83867_ADV_100HD;
		adv |= DP83867_ADV_10FD | DP83867_ADV_10HD;
		break;
	case CAPI_ETH_PHY_SPEED_10M:
		if (duplex == CAPI_ETH_PHY_DUPLEX_FULL)
			adv |= DP83867_ADV_10FD;
		else
			adv |= DP83867_ADV_10HD;
		break;
	default:
		adv |= DP83867_ADV_100FD | DP83867_ADV_100HD |
		       DP83867_ADV_10FD | DP83867_ADV_10HD;
		adv_1000 |= DP83867_1000BT_ADV_1000FD;
		break;
	}

	ret = dp83867_modify(dev, DP83867_REG_AUTONEG_ADV,
			     DP83867_ADV_100FD | DP83867_ADV_100HD |
			     DP83867_ADV_10FD | DP83867_ADV_10HD, adv);
	if (ret)
		return ret;

	return dp83867_modify(dev, DP83867_REG_1000BT_CONTROL,
			      DP83867_1000BT_ADV_1000FD |
			      DP83867_1000BT_ADV_1000HD, adv_1000);
}

/* ---------- ops implementations ---------- */

static int dp83867_init(struct capi_eth_phy_handle **handle,
			const struct capi_eth_phy_init_config *config)
{
	struct dp83867_handle *dev;
	const struct dp83867_extra_config *extra;
	uint16_t phyid1, phyid2;
	uint32_t phy_id;
	int ret;

	if (!handle || !config || !config->fn_read || !config->fn_write)
		return -EINVAL;

	if (*handle) {
		dev = (struct dp83867_handle *)*handle;
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

	extra = (const struct dp83867_extra_config *)config->extra;
	if (extra)
		dev->rgmii = extra->rgmii;

	ret = dp83867_soft_reset(dev);
	if (ret)
		goto err_free;

	ret = dp83867_read(dev, DP83867_REG_PHY_ID1, &phyid1);
	if (ret)
		goto err_free;
	ret = dp83867_read(dev, DP83867_REG_PHY_ID2, &phyid2);
	if (ret)
		goto err_free;

	phy_id = ((uint32_t)phyid1 << 16) | phyid2;
	if ((phy_id & DP83867_PHY_ID_MASK) !=
	    (DP83867_PHY_ID & DP83867_PHY_ID_MASK)) {
		ret = -ENODEV;
		goto err_free;
	}

	ret = dp83867_config_leds(dev);
	if (ret)
		goto err_free;

	*handle = &dev->base;
	return 0;

err_free:
	if (dev->base.init_allocated)
		no_os_free(dev);
	return ret;
}

static int dp83867_deinit(struct capi_eth_phy_handle *handle)
{
	struct dp83867_handle *dev = (struct dp83867_handle *)handle;

	if (!dev)
		return -EINVAL;

	dp83867_modify(dev, DP83867_REG_CONTROL, DP83867_CTRL_POWER_DOWN,
		       DP83867_CTRL_POWER_DOWN);

	if (dev->base.init_allocated)
		no_os_free(dev);

	return 0;
}

static int dp83867_power_control(struct capi_eth_phy_handle *handle,
				 enum capi_eth_phy_power_state state)
{
	struct dp83867_handle *dev = (struct dp83867_handle *)handle;

	if (!dev)
		return -EINVAL;

	switch (state) {
	case CAPI_ETH_PHY_POWER_OFF:
	case CAPI_ETH_PHY_POWER_LOW:
		return dp83867_modify(dev, DP83867_REG_CONTROL,
				      DP83867_CTRL_POWER_DOWN,
				      DP83867_CTRL_POWER_DOWN);
	case CAPI_ETH_PHY_POWER_FULL:
		return dp83867_modify(dev, DP83867_REG_CONTROL,
				      DP83867_CTRL_POWER_DOWN, 0);
	default:
		return -EINVAL;
	}
}

static int dp83867_set_interface(struct capi_eth_phy_handle *handle,
				 enum capi_eth_interface interface)
{
	struct dp83867_handle *dev = (struct dp83867_handle *)handle;
	int ret;

	if (!dev)
		return -EINVAL;

	dev->interface = interface;

	if (interface == CAPI_ETH_INTERFACE_RGMII) {
		ret = dp83867_config_rgmii_delays(dev);
		if (ret)
			return ret;
		return dp83867_soft_reset(dev);
	}

	return 0;
}

static int dp83867_set_mode(struct capi_eth_phy_handle *handle,
			    const struct capi_eth_phy_mode_config *config)
{
	struct dp83867_handle *dev = (struct dp83867_handle *)handle;
	uint16_t ctrl = 0;
	uint16_t ctrl_mask;
	uint16_t mdix_val;
	int ret;

	if (!dev || !config)
		return -EINVAL;

	switch (config->mdix) {
	case CAPI_ETH_MDIX_MDI:
		mdix_val = no_os_field_prep(DP83867_PHYCR_MDI_CROSSOVER_MASK,
					    DP83867_PHYCR_MDI_CROSSOVER_MANUAL_MDI);
		break;
	case CAPI_ETH_MDIX_MDIX:
		mdix_val = no_os_field_prep(DP83867_PHYCR_MDI_CROSSOVER_MASK,
					    DP83867_PHYCR_MDI_CROSSOVER_MANUAL_MDIX);
		break;
	case CAPI_ETH_MDIX_AUTO:
	default:
		mdix_val = no_os_field_prep(DP83867_PHYCR_MDI_CROSSOVER_MASK,
					    DP83867_PHYCR_MDI_CROSSOVER_AUTO);
		break;
	}

	ret = dp83867_modify(dev, DP83867_REG_PHYCR,
			     DP83867_PHYCR_MDI_CROSSOVER_MASK, mdix_val);
	if (ret)
		return ret;

	if (config->auto_negotiate) {
		ret = dp83867_set_autoneg_adv(dev, config->speed, config->duplex);
		if (ret)
			return ret;
		ctrl |= DP83867_CTRL_AUTONEG_EN | DP83867_CTRL_RESTART_AUTONEG;
	} else {
		switch (config->speed) {
		case CAPI_ETH_PHY_SPEED_1G:
			ctrl |= DP83867_CTRL_SPEED_MSB;
			break;
		case CAPI_ETH_PHY_SPEED_100M:
			ctrl |= DP83867_CTRL_SPEED_LSB;
			break;
		case CAPI_ETH_PHY_SPEED_10M:
		default:
			break;
		}

		if (config->duplex == CAPI_ETH_PHY_DUPLEX_FULL)
			ctrl |= DP83867_CTRL_DUPLEX;
	}

	if (config->loopback)
		ctrl |= DP83867_CTRL_LOOPBACK;
	if (config->isolate)
		ctrl |= DP83867_CTRL_ISOLATE;

	ctrl_mask = DP83867_CTRL_AUTONEG_EN | DP83867_CTRL_RESTART_AUTONEG |
		    DP83867_CTRL_SPEED_MSB | DP83867_CTRL_SPEED_LSB |
		    DP83867_CTRL_DUPLEX | DP83867_CTRL_LOOPBACK |
		    DP83867_CTRL_ISOLATE;

	return dp83867_modify(dev, DP83867_REG_CONTROL, ctrl_mask, ctrl);
}

static int dp83867_get_link_state(struct capi_eth_phy_handle *handle,
				  enum capi_eth_link_state *state)
{
	struct dp83867_handle *dev = (struct dp83867_handle *)handle;
	uint16_t sts;
	int ret;

	if (!dev || !state)
		return -EINVAL;

	ret = dp83867_read(dev, DP83867_REG_PHYSTS, &sts);
	if (ret)
		return ret;

	*state = (sts & DP83867_PHYSTS_LINK) ? CAPI_ETH_LINK_UP :
		 CAPI_ETH_LINK_DOWN;
	return 0;
}

static int dp83867_get_link_info(struct capi_eth_phy_handle *handle,
				 struct capi_eth_link_info *info)
{
	struct dp83867_handle *dev = (struct dp83867_handle *)handle;
	uint16_t sts;
	int ret;

	if (!dev || !info)
		return -EINVAL;

	ret = dp83867_read(dev, DP83867_REG_PHYSTS, &sts);
	if (ret)
		return ret;

	info->state = (sts & DP83867_PHYSTS_LINK) ?
		      CAPI_ETH_LINK_UP : CAPI_ETH_LINK_DOWN;

	if (info->state != CAPI_ETH_LINK_UP) {
		info->speed = CAPI_ETH_PHY_SPEED_UNKNOWN;
		info->duplex = CAPI_ETH_PHY_DUPLEX_UNKNOWN;
		info->mdix = CAPI_ETH_MDIX_AUTO;
		return 0;
	}

	if (sts & DP83867_PHYSTS_1000)
		info->speed = CAPI_ETH_PHY_SPEED_1G;
	else if (sts & DP83867_PHYSTS_100)
		info->speed = CAPI_ETH_PHY_SPEED_100M;
	else
		info->speed = CAPI_ETH_PHY_SPEED_10M;

	info->duplex = (sts & DP83867_PHYSTS_DUPLEX) ?
		       CAPI_ETH_PHY_DUPLEX_FULL : CAPI_ETH_PHY_DUPLEX_HALF;

	info->mdix = (sts & DP83867_PHYSTS_MDIX) ?
		     CAPI_ETH_MDIX_MDIX : CAPI_ETH_MDIX_MDI;

	return 0;
}

static int dp83867_register_callback(struct capi_eth_phy_handle *handle,
				     capi_eth_phy_event_callback const callback,
				     void *const callback_arg)
{
	struct dp83867_handle *dev = (struct dp83867_handle *)handle;
	uint16_t irq_mask, dummy;
	int ret;

	if (!dev)
		return -EINVAL;

	dev->callback = callback;
	dev->callback_arg = callback_arg;

	if (callback) {
		irq_mask = DP83867_MICR_LINK_STS_CHNG_INT_EN;
		dev->irq_mask = irq_mask;

		/* Enable INTn open-drain output on LED_2/INT pin */
		ret = dp83867_modify(dev, DP83867_REG_CFG3,
				     DP83867_CFG3_INT_OE, DP83867_CFG3_INT_OE);
		if (ret)
			return ret;
	} else {
		irq_mask = 0;
		dev->irq_mask = 0;
	}

	/* Read-to-clear any pending interrupts before (re-)enabling */
	ret = dp83867_read(dev, DP83867_REG_ISR, &dummy);
	if (ret)
		return ret;

	return dp83867_write(dev, DP83867_REG_MICR, irq_mask);
}

static void dp83867_isr(void *handle)
{
	struct dp83867_handle *dev = (struct dp83867_handle *)handle;
	uint16_t isr, sts;
	uint32_t events = 0;

	if (!dev || !dev->callback)
		return;

	if (dp83867_read(dev, DP83867_REG_ISR, &isr))
		return;

	if (!(isr & dev->irq_mask))
		return;

	if (isr & DP83867_ISR_LINK_STS_CHNG) {
		if (dp83867_read(dev, DP83867_REG_PHYSTS, &sts))
			return;

		if (sts & DP83867_PHYSTS_LINK)
			events |= CAPI_ETH_PHY_EVENT_LINK_UP;
		else
			events |= CAPI_ETH_PHY_EVENT_LINK_DOWN;
	}

	if (events)
		dev->callback(events, dev->callback_arg);
}

/* ---------- ops table ---------- */

const struct capi_eth_phy_ops dp83867_ops = {
	.init              = dp83867_init,
	.deinit            = dp83867_deinit,
	.power_control     = dp83867_power_control,
	.set_interface     = dp83867_set_interface,
	.set_mode          = dp83867_set_mode,
	.get_link_state    = dp83867_get_link_state,
	.get_link_info     = dp83867_get_link_info,
	.register_callback = dp83867_register_callback,
	.isr               = dp83867_isr,
};
