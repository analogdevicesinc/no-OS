/***************************************************************************//**
 *   @file   lwip_xemacps.c
 *   @brief  lwIP glue layer for the XEmacPs MAC driver.
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

#ifdef NO_OS_LWIP_NETWORKING

#include <string.h>
#include <errno.h>

#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "netif/ethernet.h"

#include "lwip_socket.h"
#include "lwip_xemacps.h"

static uint8_t lwip_buf[XEMACPS_MAX_FRAME_SIZE];

/**
 * @brief Initialize the XEmacPs MAC and configure multicast filters.
 * @param desc - Set to MAC descriptor on success.
 * @param param - Pointer to xemacps_init_param.
 * @return 0 on success, negative error code otherwise.
 */
static int32_t xemacps_lwip_init(void **desc, void *param)
{
	struct xemacps_init_param *ip = param;
	struct xemacps_desc *d;
	uint8_t mdns_mcast[6] = {0x01, 0x00, 0x5E, 0x00, 0x00, 0xFB};
	int ret;

	ret = xemacps_init(&d, ip);
	if (ret)
		return ret;

	ret = xemacps_set_mcast_hash(d, mdns_mcast);
	if (ret) {
		xemacps_remove(d);
		return ret;
	}

	*desc = d;
	return 0;
}

/**
 * @brief Remove the XEmacPs MAC descriptor.
 * @param desc - MAC descriptor.
 * @return 0 on success, negative error code otherwise.
 */
static int32_t xemacps_lwip_remove(void *desc)
{
	return xemacps_remove(desc);
}

/**
 * @brief Transmit a pbuf chain via the XEmacPs MAC.
 * @param netif - lwIP network interface.
 * @param p     - pbuf chain to transmit.
 * @return 0 on success, negative error code otherwise.
 */
static int32_t xemacps_netif_output(struct netif *netif, struct pbuf *p)
{
	struct lwip_network_desc *lwip_desc = netif->state;
	struct xemacps_desc *d = lwip_desc->mac_desc;
	uint32_t len;

	LINK_STATS_INC(link.xmit);

	len = pbuf_copy_partial(p, lwip_buf, p->tot_len, 0);

	return xemacps_send(d, lwip_buf, len);
}

/**
 * @brief Poll for received frames and deliver them to lwIP.
 * @param desc - lwIP network descriptor.
 * @param data - Unused.
 * @return 0 on success, negative error code otherwise.
 */
static int32_t xemacps_step(struct lwip_network_desc *desc, void *data)
{
	struct xemacps_desc *d = desc->mac_desc;
	struct netif *netif = desc->lwip_netif;
	struct pbuf *p;
	uint32_t len;
	int ret;

	ret = xemacps_poll(d);
	if (ret)
		return ret;

	if (xemacps_get_link_state(d)) {
		if (!netif_is_link_up(netif))
			netif_set_link_up(netif);
	} else {
		if (netif_is_link_up(netif))
			netif_set_link_down(netif);
	}

	do {
		ret = xemacps_recv(d, lwip_buf, &len);
		if (ret)
			return ret;

		if (!len)
			break;

		LINK_STATS_INC(link.recv);

		p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
		if (p) {
			pbuf_take(p, lwip_buf, len);
			if (desc->lwip_netif->input(p, desc->lwip_netif) != ERR_OK) {
				LINK_STATS_INC(link.drop);
				pbuf_free(p);
			}
		} else {
			LINK_STATS_INC(link.memerr);
			LINK_STATS_INC(link.drop);
		}
	} while (1);

	return 0;
}

const struct no_os_lwip_ops xemacps_lwip_ops = {
	.init          = xemacps_lwip_init,
	.remove        = xemacps_lwip_remove,
	.netif_output  = xemacps_netif_output,
	.step          = xemacps_step,
};

#endif /* NO_OS_LWIP_NETWORKING */
