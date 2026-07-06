/***************************************************************************//**
 *   @file   lwip_capi.c
 *   @brief  Generic CAPI MAC + CAPI PHY lwIP glue.
 *
 *   Drives a MAC and PHY pair through their CAPI ops tables only.  No
 *   knowledge of any specific MAC or PHY implementation lives here.
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

#ifdef NO_OS_LWIP_NETWORKING

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>

#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "netif/ethernet.h"

#include "lwip_socket.h"
#include "lwip_capi.h"
#include "no_os_alloc.h"


#define LWIP_CAPI_FRAME_BUF_SIZE	1536
static uint8_t lwip_buf[LWIP_CAPI_FRAME_BUF_SIZE];

/**
 * @struct lwip_capi_priv
 * @brief  Per-instance state.
 */
struct lwip_capi_priv {
	struct capi_eth_mac_handle *mac;
	struct capi_eth_phy_handle *phy;

	enum capi_eth_link_state  last_link;
	enum capi_eth_phy_speed   last_speed;
	enum capi_eth_phy_duplex  last_duplex;
};

/* ------------------------------------------------------------------------- */
/* Conversions                                                               */
/* ------------------------------------------------------------------------- */

static enum capi_eth_mac_speed phy_speed_to_mac_speed(enum capi_eth_phy_speed s)
{
	switch (s) {
	case CAPI_ETH_PHY_SPEED_1G:
				return CAPI_ETH_MAC_SPEED_1G;
	case CAPI_ETH_PHY_SPEED_100M:
		return CAPI_ETH_MAC_SPEED_100M;
	case CAPI_ETH_PHY_SPEED_10M:
		return CAPI_ETH_MAC_SPEED_10M;
	default:
		return CAPI_ETH_MAC_SPEED_UNKNOWN;
	}
}

static enum capi_eth_mac_duplex phy_duplex_to_mac_duplex(
	enum capi_eth_phy_duplex d)
{
	switch (d) {
	case CAPI_ETH_PHY_DUPLEX_FULL:
				return CAPI_ETH_MAC_DUPLEX_FULL;
	case CAPI_ETH_PHY_DUPLEX_HALF:
		return CAPI_ETH_MAC_DUPLEX_HALF;
	default:
		return CAPI_ETH_MAC_DUPLEX_UNKNOWN;
	}
}

/* ------------------------------------------------------------------------- */
/* lwIP ops                                                                  */
/* ------------------------------------------------------------------------- */

static int32_t lwip_capi_init(void **desc, void *param)
{
	struct lwip_capi_param *cfg = param;
	struct lwip_capi_priv *priv;
	struct capi_eth_phy_init_config phy_cfg;
	uint8_t mdns_mcast[6] = {0x01, 0x00, 0x5E, 0x00, 0x00, 0xFB};
	int ret;

	if (!desc || !cfg || !cfg->mac || !cfg->phy_ops || !cfg->mdio_bus)
		return -EINVAL;

	priv = no_os_calloc(1, sizeof(*priv));
	if (!priv)
		return -ENOMEM;

	priv->mac         = cfg->mac;
	priv->last_link   = CAPI_ETH_LINK_DOWN;
	priv->last_speed  = CAPI_ETH_PHY_SPEED_UNKNOWN;
	priv->last_duplex = CAPI_ETH_PHY_DUPLEX_UNKNOWN;

	memset(&phy_cfg, 0, sizeof(phy_cfg));
	phy_cfg.phy_addr = cfg->phy_addr;
	phy_cfg.mdio_bus = cfg->mdio_bus;
	phy_cfg.extra    = cfg->phy_extra;
	phy_cfg.ops      = cfg->phy_ops;

	ret = capi_eth_phy_init(&priv->phy, &phy_cfg);
	if (ret) {
		printf("lwip_capi: PHY init failed (%d)\n", ret);
		goto err_priv;
	}

	ret = capi_eth_phy_set_interface(priv->phy, cfg->phy_interface);
	if (ret) {
		printf("lwip_capi: PHY set_interface failed (%d)\n", ret);
		goto err_phy;
	}

	ret = capi_eth_phy_set_mode(priv->phy, &cfg->phy_mode);
	if (ret) {
		printf("lwip_capi: PHY set_mode failed (%d)\n", ret);
		goto err_phy;
	}

	ret = capi_eth_phy_power_control(priv->phy, CAPI_ETH_PHY_POWER_FULL);
	if (ret) {
		printf("lwip_capi: PHY power_control failed (%d)\n", ret);
		goto err_phy;
	}

	/* ---- mDNS multicast filter + MAC start ---- */
	{
		const capi_eth_mac_addr filter[] = {
			{
				mdns_mcast[0], mdns_mcast[1], mdns_mcast[2],
				mdns_mcast[3], mdns_mcast[4], mdns_mcast[5]
			},
		};
		ret = capi_eth_mac_set_address_filter(priv->mac, filter, 1);
		if (ret) {
			printf("lwip_capi: set_address_filter failed (%d)\n", ret);
			goto err_phy;
		}
	}

	ret = capi_eth_mac_power_control(priv->mac, CAPI_ETH_MAC_POWER_FULL);
	if (ret) {
		printf("lwip_capi: MAC power_control failed (%d)\n", ret);
		goto err_phy;
	}

	*desc = priv;
	return 0;

err_phy:
	capi_eth_phy_deinit(priv->phy);
err_priv:
	no_os_free(priv);
	return ret;
}

static int32_t lwip_capi_remove(void *desc)
{
	struct lwip_capi_priv *priv = desc;

	if (!priv)
		return -EINVAL;

	capi_eth_mac_power_control(priv->mac, CAPI_ETH_MAC_POWER_OFF);
	capi_eth_phy_deinit(priv->phy);
	/* MAC handle is caller-owned; do not deinit it here. */
	no_os_free(priv);
	return 0;
}

static int32_t lwip_capi_netif_output(struct netif *netif, struct pbuf *p)
{
	struct lwip_network_desc *lwip_desc = netif->state;
	struct lwip_capi_priv *priv = lwip_desc->mac_desc;
	struct capi_eth_mac_tx_frame_config cfg = {0};
	uint32_t len;

	LINK_STATS_INC(link.xmit);

	len = pbuf_copy_partial(p, lwip_buf, p->tot_len, 0);

	return capi_eth_mac_send_frame(priv->mac, lwip_buf, len, &cfg);
}

/**
 * @brief Push a PHY link-info delta into the MAC.
 */
static void sync_phy_to_mac(struct lwip_capi_priv *priv,
			    const struct capi_eth_link_info *info)
{
	struct capi_eth_mac_config cfg;

	if (info->state == priv->last_link &&
	    info->speed == priv->last_speed &&
	    info->duplex == priv->last_duplex)
		return;

	if (info->state == CAPI_ETH_LINK_UP) {
		memset(&cfg, 0, sizeof(cfg));
		cfg.speed             = phy_speed_to_mac_speed(info->speed);
		cfg.duplex            = phy_duplex_to_mac_duplex(info->duplex);
		cfg.address_broadcast = true;
		cfg.address_multicast = true;
		cfg.checksum_offload_rx = true;
		cfg.checksum_offload_tx = true;
		capi_eth_mac_configure(priv->mac, &cfg);
		printf("lwip_capi: link up — speed=%d duplex=%d\n",
		       (int)info->speed, (int)info->duplex);
	} else {
		printf("lwip_capi: link down\n");
	}

	priv->last_link   = info->state;
	priv->last_speed  = info->speed;
	priv->last_duplex = info->duplex;
}

static int32_t lwip_capi_step(struct lwip_network_desc *desc, void *data)
{
	struct lwip_capi_priv *priv = desc->mac_desc;
	struct netif *netif = desc->lwip_netif;
	struct capi_eth_link_info info;
	struct pbuf *p;
	uint32_t size;
	int ret;

	(void)data;

	/* PHY -> MAC + lwIP link state */
	if (capi_eth_phy_get_link_info(priv->phy, &info) == 0) {
		sync_phy_to_mac(priv, &info);

		if (info.state == CAPI_ETH_LINK_UP) {
			if (!netif_is_link_up(netif))
				netif_set_link_up(netif);
		} else {
			if (netif_is_link_up(netif))
				netif_set_link_down(netif);
		}
	}

	if (priv->last_link != CAPI_ETH_LINK_UP)
		return 0;

	/* Drain RX */
	for (;;) {
		ret = capi_eth_mac_get_rx_frame_size(priv->mac, &size);
		if (ret || !size)
			break;

		ret = capi_eth_mac_read_frame(priv->mac, lwip_buf,
					      sizeof(lwip_buf), NULL);
		if (ret <= 0)
			break;

		LINK_STATS_INC(link.recv);

		p = pbuf_alloc(PBUF_RAW, (uint16_t)ret, PBUF_POOL);
		if (p) {
			pbuf_take(p, lwip_buf, ret);
			if (netif->input(p, netif) != ERR_OK) {
				LINK_STATS_INC(link.drop);
				pbuf_free(p);
			}
		} else {
			LINK_STATS_INC(link.memerr);
			LINK_STATS_INC(link.drop);
		}
	}

	return 0;
}

const struct no_os_lwip_ops lwip_capi_ops = {
	.init          = lwip_capi_init,
	.remove        = lwip_capi_remove,
	.netif_output  = lwip_capi_netif_output,
	.step          = lwip_capi_step,
};

#endif /* NO_OS_LWIP_NETWORKING */
