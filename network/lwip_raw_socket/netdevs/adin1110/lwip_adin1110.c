
/***************************************************************************//**
 *   @file   lwip_adin1110.c
 *   @brief  Source file for the ADIN1110 LWIP implementation.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifdef NO_OS_LWIP_NETWORKING

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/timeouts.h"
#include "lwip/stats.h"
#include "lwip/init.h"
#include "lwip/tcpbase.h"
#include "lwip/tcpip.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"
#include "lwip/api.h"
#include "lwip/etharp.h"
#include "netif/ethernet.h"
#include "lwip/inet.h"
#include "lwip_socket.h"
#include "no_os_delay.h"
#include "tcp_socket.h"

#include "adin1110.h"
#include "lwip_adin1110.h"

static uint8_t lwip_buff[ADIN1110_LWIP_BUFF_SIZE];

/**
 * @brief Read a frame from the RX FIFO.
 * @param desc - ADIN1110 descriptor.
 * @param p - the received pbuf.
 * @param len - length of the frame.
 * @return 0 in case of success, negative error otherwise.
 */
static int adin1110_read_frames(struct adin1110_desc *desc, struct pbuf **p,
				uint32_t *len)
{
	struct adin1110_eth_buff mac_buff = {0};
	int ret;

	mac_buff.payload = &lwip_buff[ADIN1110_ETH_HDR_LEN];

	ret = adin1110_read_fifo(desc, 0, &mac_buff);
	if (ret)
		return ret;

	*len = mac_buff.len;
	if (!mac_buff.len)
		return 0;

	memcpy(lwip_buff, mac_buff.mac_dest, ADIN1110_ETH_HDR_LEN);
	*p = pbuf_alloc(PBUF_RAW, mac_buff.len, PBUF_POOL);
	if (!*p)
		return -ENOMEM;

	pbuf_take(*p, lwip_buff, mac_buff.len);

	return 0;
}

/**
 * @brief Read all the frames from the RX FIFO.
 * @param desc - lwip sockets layer specific descriptor.
 * @param data - netif to RX data.
 * @return 0 in case of success, negative error otherwise.
 */
static int32_t adin1110_step(struct lwip_network_desc *desc, void *data)
{
	struct adin1110_desc *mac_desc;
	struct netif *netif_desc;
	struct pbuf *p;
	uint32_t len;
	int ret;

	netif_desc = desc->lwip_netif;
	mac_desc = desc->mac_desc;

	do {
		ret = adin1110_read_frames(mac_desc, &p, &len);
		if (ret)
			return ret;

		if (len) {
			LINK_STATS_INC(link.recv);
			ret = netif_desc->input(p, netif_desc);
			if (ret) {
				if (p->ref)
					pbuf_free(p);
			}
		}
	} while(len);

	return 0;
}

/**
 * @brief Write the data inside a pbuf on the wire.
 * @param net - lwip network descriptor to send data to.
 * @param p - pbuf to be sent.
 * @return 0 in case of success, negative error otherwise.
 */
static int32_t adin1110_netif_output(struct netif *net, struct pbuf *p)
{
	struct lwip_network_desc *lwip_desc;
	struct adin1110_desc *mac_desc;
	struct adin1110_eth_buff buff;
	uint32_t frame_len;

	lwip_desc = net->state;
	mac_desc = lwip_desc->mac_desc;

	LINK_STATS_INC(link.xmit);
	frame_len = pbuf_copy_partial(p, lwip_buff, p->tot_len, 0);

	memcpy(&buff.mac_dest, lwip_buff, ADIN1110_ETH_HDR_LEN);

	buff.len = frame_len;
	buff.payload = &lwip_buff[ADIN1110_ETH_HDR_LEN];

	return adin1110_write_fifo(mac_desc, 0, &buff);
}

/**
 * @brief Intialize and configure filters for the ADIN1110.
 * @param desc - descriptor for the ADIN1110.
 * @param param - parameter for the ADIN1110.
 * @return 0 in case of success, negative error otherwise.
 */
static int32_t adin1110_lwip_init(void **desc, void *param)
{
	struct adin1110_desc *adin1110;
	struct adin1110_init_param *adin1110_ip = param;
	uint8_t mdns_mcast_addr[NETIF_MAX_HWADDR_LEN] = {0x01, 0x00, 0x5E, 0x00, 0x00, 0xFB};
	int32_t ret;

	ret = adin1110_init(&adin1110, adin1110_ip);
	if (ret)
		return ret;

	ret = adin1110_set_mac_addr(adin1110, adin1110->mac_address);
	if (ret)
		goto free_descriptor;

	ret = adin1110_broadcast_filter(adin1110, true);
	if (ret)
		goto free_descriptor;

	ret = adin1110_set_mac_addr(adin1110, mdns_mcast_addr);
	if (ret)
		goto free_descriptor;

	*desc = adin1110;

	return 0;

free_descriptor:
	adin1110_remove(adin1110);

	return ret;
}

/**
 * @brief Remove the ADIN1110 descriptor.
 * @param desc - descriptor for the ADIN1110.
 * @return 0 in case of success, negative error otherwise.
 */
static int32_t adin1110_lwip_remove(void *desc)
{
	struct adin1110_desc *adin1110 = desc;

	return adin1110_remove(adin1110);
}

const struct no_os_lwip_ops adin1110_lwip_ops = {
	.init = adin1110_lwip_init,
	.remove = adin1110_lwip_remove,
	.netif_output = adin1110_netif_output,
	.step = adin1110_step,
};

#endif /* NO_OS_LWIP_NETWORKING */
