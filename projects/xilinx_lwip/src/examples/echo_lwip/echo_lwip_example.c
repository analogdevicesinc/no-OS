/***************************************************************************//**
 *   @file   echo_lwip_example.c
 *   @brief  TCP echo server over Ethernet (lwIP / CAPI) for Xilinx boards.
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

#include <string.h>
#include <stdio.h>
#include "no_os_error.h"
#include "lwip_socket.h"
#include "lwip_capi.h"
#include "capi_xemacps.h"
#include <capi_mdio.h>
#include "tcpecho_raw.h"
#include "parameters.h"
#include "echo_lwip_example.h"

#if PHY_TYPE == PHY_TYPE_TI_DP83867
#include "capi_dp83867.h"
#else
#include "capi_marvell_88e1510.h"
#endif

/* MAC address - must be unique on the local network */
static uint8_t mac_addr[6] = {0x00, 0x0A, 0x35, 0x00, 0x01, 0x02};

/***************************************************************************//**
 * @brief Run the TCP echo server on the board.
 *
 * Brings up the XEmacPs MAC + PHY pair through the generic CAPI lwIP glue,
 * starts the tcpecho_raw server on port 7, and polls indefinitely.
 *
 * @return 0 on success, negative error code on failure.
*******************************************************************************/
int echo_lwip_example_main(void)
{
	struct lwip_network_desc *lwip_desc;
	struct capi_eth_mac_handle *mac = NULL;
	struct capi_mdio_handle *mdio_bus = NULL;
#if PHY_TYPE == PHY_TYPE_TI_DP83867
	struct dp83867_extra_config phy_cfg = {
		.rgmii = {
			.rx_delay_en   = true,
			.tx_delay_en   = true,
			.rx_delay_code = DP83867_RGMII_DELAY_2_00_NS,
			.tx_delay_code = DP83867_RGMII_DELAY_2_00_NS,
		},
	};
	const struct capi_eth_phy_ops *phy_ops = &dp83867_ops;
#else
	struct mrvl_88e1510_extra_config phy_cfg = {
		.rgmii = { .rx_delay_en = true, .tx_delay_en = true },
		.downshift_en = true,
		.downshift_retries = 3,
	};
	const struct capi_eth_phy_ops *phy_ops = &mrvl_88e1510_ops;
#endif
	struct capi_eth_mac_init_config mac_cfg = {
		.identifier  = GEM_DEVICE_ID,
		.mac_address = (capi_eth_mac_addr *)mac_addr,
		.ops         = &xemacps_capi_mac_ops,
	};
	struct xemacps_mdio_init_config xmdio_extra;
	struct capi_mdio_init_config mdio_cfg = {
		.ops   = &xemacps_capi_mdio_ops,
		.extra = &xmdio_extra,
	};
	struct lwip_capi_param netdev_param = {
		.phy_ops       = phy_ops,
		.phy_extra     = &phy_cfg,
		.phy_addr      = CAPI_ETH_PHY_ADDR_ANY,
		.phy_interface = CAPI_ETH_INTERFACE_RGMII,
		.phy_mode      = {
			.speed          = CAPI_ETH_PHY_SPEED_1G,
			.duplex         = CAPI_ETH_PHY_DUPLEX_FULL,
			.auto_negotiate = true,
			.mdix           = CAPI_ETH_MDIX_AUTO,
		},
	};
	struct lwip_network_param lwip_param = {
		.platform_ops = &lwip_capi_ops,
		.mac_param    = &netdev_param,
	};
	int ret;

	memcpy(netdev_param.hwaddr, mac_addr, sizeof(mac_addr));
	memcpy(lwip_param.hwaddr, mac_addr, sizeof(mac_addr));

	ret = capi_eth_mac_init(&mac, &mac_cfg);
	if (ret)
		return ret;

	xmdio_extra.mac = mac;
	ret = capi_mdio_init(&mdio_bus, &mdio_cfg);
	if (ret) {
		capi_eth_mac_deinit(mac);
		return ret;
	}

	netdev_param.mac      = mac;
	netdev_param.mdio_bus = mdio_bus;

	ret = no_os_lwip_init(&lwip_desc, &lwip_param);
	if (ret) {
		capi_mdio_deinit(mdio_bus);
		capi_eth_mac_deinit(mac);
		return ret;
	}

	tcpecho_raw_init();

	printf("echo: listening on port 7\n");

	while (1)
		no_os_lwip_step(lwip_desc, lwip_desc);

	no_os_lwip_remove(lwip_desc);
	capi_mdio_deinit(mdio_bus);
	capi_eth_mac_deinit(mac);

	return 0;
}
