/***************************************************************************//**
 *   @file   lwip_net.c
 *   @brief  lwIP adapter for the no_os_net / no_os_socket interfaces.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
 *   @author Alisa-Dariana Roman (alisa.roman@analog.com)
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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
#ifdef NO_OS_NET

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>

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
#include "lwip/dhcp.h"
#include "netif/ethernet.h"
#include "lwip/inet.h"
#include "lwip/apps/mdns.h"
#include "lwip/apps/mdns_priv.h"
#include "lwip/apps/mdns_domain.h"
#include "lwip/apps/mdns_out.h"

#include "lwipcfg.h"
#include "lwipopts.h"
#include "arch/cc.h"
#include "lwip_net.h"

#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_util.h"

static bool mdns_result;
static bool mdns_is_conflict;
static uint32_t mdns_conflict_id;

static const struct no_os_socket_ops lwip_socket_ops;

/**
 * @brief Get a socket structure based on id.
 * @param ld - lwIP backend descriptor.
 * @param id - index in the sockets array
 * @return a reference to the socket structure, NULL in case of an error.
 */
static struct lwip_socket_desc *_get_sock(struct lwip_net_desc *ld, uint32_t id)
{
	if (id >= NO_OS_MAX_SOCKETS)
		return NULL;

	return &ld->sockets[id];
}

/**
 * @brief Get the id of a socket in the SOCKET_CLOSED state (ready to be used).
 * @param ld - lwIP backend descriptor.
 * @param id - index in the sockets array.
 * @return 0 on success, -ECONNREFUSED if all the connections are in use.
 */
static int32_t _get_closed_socket(struct lwip_net_desc *ld, uint32_t *id)
{
	uint32_t i;

	for (i = 0; i < NO_OS_MAX_SOCKETS; i++)
		if (ld->sockets[i].state == SOCKET_CLOSED) {
			*id = i;

			return 0;
		}

	/* All the available connections are used */
	return -ECONNREFUSED;
}

/**
 * @brief Tear down a socket's lwIP resources and mark the pool slot free.
 *
 * Idempotent: safe to call on an already-closed slot (pcb == NULL). Does NOT
 * free the owning no_os_socket_desc; that is done by lwip_sock_close(). This
 * split lets the lwIP receive callback release a slot on a remote-side close
 * without touching a no_os_socket_desc it does not own.
 * @param sock - the socket pool slot.
 */
static void _lwip_socket_teardown(struct lwip_socket_desc *sock)
{
	if (!sock->pcb) {
		sock->state = SOCKET_CLOSED;
		return;
	}

	if (sock->p) {
		tcp_recved(sock->pcb, sock->p->tot_len);
		pbuf_free(sock->p);
	}

	tcp_close(sock->pcb);
	tcp_recv(sock->pcb, NULL);
	tcp_err(sock->pcb, NULL);

	sock->p_idx = 0;
	sock->pcb = NULL;
	sock->p = NULL;
	sock->state = SOCKET_CLOSED;
}

/**
 * @brief Low level pbuf output function. Lwip will call this to send data
 * on the wire.
 * @param netif - the interface to send the pbuf on.
 * @param p - pbuf containing the data to be sent.
 * @return 0 in the case of success, negative error code otherwise
 */
static err_t lwip_netif_output(struct netif *netif, struct pbuf *p)
{
	struct lwip_net_desc *ld = netif->state;

	if (!ld || !ld->mac_ops)
		return -EINVAL;

	if (!ld->mac_ops->netif_output)
		return -ENOSYS;

	return ld->mac_ops->netif_output(netif, p);
}

/**
 * @brief Setup a network interface with a set of predefined options.
 * @param netif - the interface be setup.
 * @return 0 in the case of success, negative error code otherwise
 */
static err_t lwip_netif_init(struct netif *netif)
{
	struct lwip_net_desc *ld;

	if (!netif)
		return -EINVAL;

	ld = netif->state;

	netif->linkoutput = lwip_netif_output;
	netif->output = etharp_output;
	netif->mtu = NO_OS_MTU_SIZE;
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP |
		       NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP;

	memcpy(netif->hwaddr, ld->hwaddr, NETIF_MAX_HWADDR_LEN);
	netif->hwaddr_len = NETIF_MAX_HWADDR_LEN;

	return ERR_OK;
}

/**
 * @brief Cycle the lwIP specific timers and poll the MAC. Needs to be called
 * in a loop as fast as possible.
 * @param ld - lwIP backend descriptor.
 * @return 0 in the case of success, negative error code otherwise
 */
static int32_t _lwip_step(struct lwip_net_desc *ld)
{
	int ret;

	sys_check_timeouts();

	if (!ld || !ld->mac_ops)
		return -EINVAL;

	if (ld->mac_ops->step) {
		ret = ld->mac_ops->step(ld, NULL);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Callback for mdns_resp_add_netif(). Increases the domain name id if a
 * conflict is detected.
 * @param netif - interface on which the MDNS responder was added.
 * @param result - domain name allocation result (success or conflict)
 * @param slot - mDNS service slot.
 */
static void mdns_name_result(struct netif *netif, u8_t result, s8_t slot)
{
	mdns_result = true;

	if (result == MDNS_PROBING_CONFLICT) {
		mdns_is_conflict = true;
		mdns_conflict_id++;

		return;
	}

	mdns_is_conflict = false;
}

static void srv_txt(struct mdns_service *service, void *txt_userdata)
{
	mdns_resp_add_service_txtitem(service, "path=/", 6);
}

/**
 * @brief Start the domain name allocation process.
 * @param ld - lwIP backend descriptor.
 * @param netif - interface on which to add the MDNS responder.
 * @return 0 in the case of success, negative error code otherwise
 */
static int _lwip_start_mdns(struct lwip_net_desc *ld, struct netif *netif)
{
	char mdns_name_buff[256];
	uint32_t len;
	int ret;

	mdns_resp_init();
	mdns_resp_register_name_result_cb(mdns_name_result);

	/*
	 * DNS conflict resolution. If domain_name is already taken, try
	 * domain_name-X (X = 1, 2, ... ), until one is available. This might
	 * take a long time (up to a few seconds) depending on the number of
	 * conflicts.
	 */
	do {
		mdns_is_conflict = false;
		len = sprintf(mdns_name_buff, "%s", CONFIG_NO_OS_DOMAIN_NAME);
		if (mdns_conflict_id)
			sprintf(mdns_name_buff + len, "%c%ld", '-', mdns_conflict_id);

		ret = mdns_resp_add_netif(netif, mdns_name_buff);
		if (ret)
			return ret;

		while (!mdns_result)
			_lwip_step(ld);

		if (mdns_is_conflict)
			mdns_resp_remove_netif(netif);

		mdns_result = false;
	} while (mdns_is_conflict);

	ret = mdns_resp_add_service(netif, "analog", "_iio", DNSSD_PROTO_TCP,
				    30431, srv_txt, NULL);
	if (ret)
		return ret;

	mdns_resp_announce(netif);

	return 0;
}

/**
 * @brief Configure lwIP and the lower layer network device.
 * @param desc - the network interface descriptor to be returned.
 * @param param - initialization parameter (extra holds a lwip_net_param).
 * @return 0 in the case of success, negative error code otherwise
 */
static int32_t lwip_net_init(struct no_os_net_desc **desc,
			     const struct no_os_net_init_param *param)
{
	struct lwip_net_param *lwip_param;
	struct no_os_net_desc *nd;
	struct lwip_net_desc *ld;
	struct netif *netif_descriptor;
	ip4_addr_t ipaddr, netmask, gw;
#ifdef CONFIG_NO_OS_IP
	uint8_t raw_netmask[4] = {0};
	uint8_t raw_gateway[4] = {0};
	uint8_t raw_ip[4] = {0};
#endif
	int ret;
	int i;

	if (!desc || !param || !param->extra)
		return -EINVAL;

	lwip_param = param->extra;
	if (!lwip_param->mac_ops)
		return -EINVAL;

	nd = no_os_calloc(1, sizeof(*nd));
	if (!nd)
		return -ENOMEM;

	netif_descriptor = no_os_calloc(1, sizeof(*netif_descriptor));
	if (!netif_descriptor) {
		ret = -ENOMEM;
		goto free_nd;
	}

	ld = no_os_calloc(1, sizeof(*ld));
	if (!ld) {
		ret = -ENOMEM;
		goto free_netif_desc;
	}

	memcpy(ld->hwaddr, lwip_param->hwaddr, NETIF_MAX_HWADDR_LEN);
	ld->extra = lwip_param->extra;

	lwip_init();

#ifdef CONFIG_NO_OS_IP
#ifndef CONFIG_NO_OS_NETMASK
#error CONFIG_NO_OS_NETMASK not defined
#endif
#ifndef CONFIG_NO_OS_GATEWAY
#error CONFIG_NO_OS_GATEWAY not defined
#endif
	no_os_net_ipv4_to_bytes(CONFIG_NO_OS_IP, raw_ip);
	no_os_net_ipv4_to_bytes(CONFIG_NO_OS_NETMASK, raw_netmask);
	no_os_net_ipv4_to_bytes(CONFIG_NO_OS_GATEWAY, raw_gateway);

	IP4_ADDR(&ipaddr, raw_ip[0], raw_ip[1], raw_ip[2], raw_ip[3]);
	IP4_ADDR(&netmask, raw_netmask[0], raw_netmask[1], raw_netmask[2],
		 raw_netmask[3]);
	IP4_ADDR(&gw, raw_gateway[0], raw_gateway[1], raw_gateway[2], raw_gateway[3]);
#else
	ip4_addr_set_zero(&ipaddr);
	ip4_addr_set_zero(&netmask);
	ip4_addr_set_zero(&gw);
#endif
	netif_add(netif_descriptor, &ipaddr, &netmask, &gw, ld, lwip_netif_init,
		  ethernet_input);
	ld->lwip_netif = netif_descriptor;

	ret = lwip_param->mac_ops->init(&ld->mac_desc, lwip_param->mac_param);
	if (ret)
		goto free_netif;

	ld->mac_ops = lwip_param->mac_ops;

	netif_set_default(netif_descriptor);
	netif_set_up(netif_descriptor);

	netif_set_link_up(netif_descriptor);

#ifndef CONFIG_NO_OS_IP
	uint32_t dhcp_timeout = 20000;
	ret = dhcp_start(netif_descriptor);
	if (ret)
		goto platform_remove;

	/*
	 * By default, AutoIP is enabled, so it will fall back on getting a link
	 * local address in the 169.254.0.0/16 range after 2 unsuccessful
	 * DHCPREQUESTs. Timeout after ~20s.
	 */
	while (!netif_descriptor->ip_addr.addr && dhcp_timeout > 0) {
		_lwip_step(ld);
		dhcp_timeout--;
		no_os_mdelay(1);
	}

	if (!dhcp_timeout && !netif_descriptor->ip_addr.addr) {
		ret = -ETIMEDOUT;
		printf("LWIP configuration timed out\n");
		goto platform_remove;
	}
#endif

	ret = _lwip_start_mdns(ld, netif_descriptor);
	if (ret)
		goto platform_remove;

	printf("IP address: %s\n", ip4addr_ntoa(&netif_descriptor->ip_addr));
	printf("Network mask: %s\n", ip4addr_ntoa(&netif_descriptor->netmask));
	printf("Gateway's IP address: %s\n", ip4addr_ntoa(&netif_descriptor->gw));

	for (i = 0; i < NO_OS_MAX_SOCKETS; i++) {
		ld->sockets[i].state = SOCKET_CLOSED;
		ld->sockets[i].net = ld;
		ld->sockets[i].id = i;
	}

	nd->extra = ld;
	*desc = nd;

	return 0;

platform_remove:
	lwip_param->mac_ops->remove(ld->mac_desc);
free_netif:
	netif_remove(netif_descriptor);
	no_os_free(ld);
free_netif_desc:
	no_os_free(netif_descriptor);
free_nd:
	no_os_free(nd);

	return ret;
}

/**
 * @brief Free the resources allocated by lwip_net_init().
 * @param desc - the network interface descriptor.
 * @return 0 in the case of success, negative error code otherwise
 */
static int32_t lwip_net_remove(struct no_os_net_desc *desc)
{
	struct lwip_net_desc *ld;

	if (!desc || !desc->extra)
		return -EINVAL;

	ld = desc->extra;

	if (!ld->mac_ops || !ld->mac_ops->remove)
		return -ENOSYS;

	ld->mac_ops->remove(ld->mac_desc);
	netif_remove(ld->lwip_netif);
	no_os_free(ld->lwip_netif);
	no_os_free(ld);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Service the network interface (drive lwIP timers, poll the MAC).
 * @param desc - the network interface descriptor.
 * @return 0 in the case of success, negative error code otherwise
 */
static int32_t lwip_net_step(struct no_os_net_desc *desc)
{
	if (!desc || !desc->extra)
		return -EINVAL;

	return _lwip_step(desc->extra);
}

/**
 * @brief Get the current IP address as a dotted-decimal string.
 * @param desc - the network interface descriptor.
 * @param buf - destination buffer.
 * @param size - size of the destination buffer.
 * @return 0 in the case of success, negative error code otherwise
 */
static int32_t lwip_net_get_ip(struct no_os_net_desc *desc, char *buf,
			       uint32_t size)
{
	struct lwip_net_desc *ld;
	char *ip;

	if (!desc || !desc->extra || !buf)
		return -EINVAL;

	ld = desc->extra;
	if (!ld->lwip_netif)
		return -EINVAL;

	ip = ip4addr_ntoa(&ld->lwip_netif->ip_addr);
	if (!ip)
		return -EINVAL;

	if (strlen(ip) >= size)
		return -EINVAL;

	strcpy(buf, ip);

	return 0;
}

/**
 * @brief Called in case of a lwIP error. The pcb may have already been freed.
 * @param arg - the socket pool slot.
 * @param err - error code.
 */
static void lwip_err_callback(void *arg, err_t err)
{
	struct lwip_socket_desc *sock = arg;

	sock->state = SOCKET_CLOSED;
}

/**
 * @brief Called when a pbuf is received.
 * @param arg - the socket pool slot.
 * @param tpcb - lwIP TCP descriptor of the socket the pbuf was received from.
 * @param p - the received pbuf.
 * @param err - error code.
 * @return 0 in the case of success, negative error code otherwise
 */
static err_t lwip_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p,
				err_t err)
{
	struct lwip_socket_desc *sock = arg;

	/* The remote side has closed the connection. */
	if (!p) {
		tcp_recv(sock->pcb, NULL);
		_lwip_socket_teardown(sock);

		return ERR_OK;
	}

	if (err != ERR_OK) {
		pbuf_free(p);
		return err;
	}

	if (!sock->p) {
		sock->p = p;
		sock->p_idx = 0;
	} else {
		pbuf_cat(sock->p, p);
	}

	return ERR_OK;
}

/**
 * @brief Configure the receive and error callbacks.
 * @param sock - the socket pool slot.
 */
static void lwip_config_socket(struct lwip_socket_desc *sock)
{
	tcp_arg(sock->pcb, sock);
	tcp_recv(sock->pcb, lwip_recv_callback);
	tcp_err(sock->pcb, lwip_err_callback);
}

/**
 * @brief Called when a connection is accepted.
 * @param arg - the listening socket pool slot.
 * @param new_pcb - lwIP TCP descriptor of the new connection.
 * @param err - error code.
 * @return 0 in the case of success, negative error code otherwise
 */
static err_t lwip_accept_callback(void *arg, struct tcp_pcb *new_pcb, err_t err)
{
	int32_t ret;
	uint32_t id;
	struct lwip_socket_desc *sock;
	struct lwip_socket_desc *serv_sock = arg;
	struct lwip_net_desc *ld = serv_sock->net;

	if (err != ERR_OK) {
		printf("Accept callback err %d\n", err);
		return err;
	}

	ret = _get_closed_socket(ld, &id);
	if (ret)
		return ret;

	sock = _get_sock(ld, id);
	sock->pcb = new_pcb;
	sock->state = SOCKET_WAITING_ACCEPT;

	tcp_setprio(sock->pcb, 0);
	lwip_config_socket(sock);
	tcp_nagle_disable(sock->pcb);

	return 0;
}

/**
 * @brief Called once a connect() completes.
 * @param arg - the socket pool slot.
 * @param pcb - unused.
 * @param err - connection result.
 * @return ERR_OK on success, error code otherwise
 */
static err_t lwip_connect_callback(void *arg, struct tcp_pcb *pcb, err_t err)
{
	struct lwip_socket_desc *sock = arg;

	if (err != ERR_OK)
		return err;

	if (!arg)
		return -EINVAL;

	sock->state = SOCKET_CONNECTED;

	return ERR_OK;
}

/**
 * @brief Open (create) a TCP socket on the network interface.
 * @param net - the owning network interface descriptor.
 * @param sock - destination for the newly created socket descriptor.
 * @param proto - Layer 4 protocol (only TCP is supported).
 * @param buff_size - unused.
 * @return 0 in the case of success, negative error code otherwise
 */
static int32_t lwip_net_socket_open(struct no_os_net_desc *net,
				    struct no_os_socket_desc **sock,
				    enum no_os_socket_protocol proto,
				    uint32_t buff_size)
{
	struct lwip_net_desc *ld;
	struct no_os_socket_desc *s;
	struct tcp_pcb *pcb;
	uint32_t id;
	int32_t ret;

	NO_OS_UNUSED_PARAM(buff_size);

	if (!net || !net->extra || !sock)
		return -EINVAL;

	if (proto != NO_OS_SOCKET_TCP)
		return -EPROTONOSUPPORT;

	ld = net->extra;

	ret = _get_closed_socket(ld, &id);
	if (ret)
		return ret;

	pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	if (!pcb)
		/* Slot is already SOCKET_CLOSED (from _get_closed_socket). */
		return -ENOMEM;

	ip_set_option(pcb, SOF_REUSEADDR);

	ld->sockets[id].pcb = pcb;
	ld->sockets[id].net = ld;
	ld->sockets[id].id = id;
	ld->sockets[id].p = NULL;

	lwip_config_socket(&ld->sockets[id]);

	mdns_conflict_id = 0;

	s = no_os_socket_alloc(net, &lwip_socket_ops, id);
	if (!s) {
		tcp_close(pcb);
		ld->sockets[id].pcb = NULL;
		ld->sockets[id].state = SOCKET_CLOSED;
		return -ENOMEM;
	}
	s->extra = &ld->sockets[id];

	*sock = s;

	return 0;
}

/**
 * @brief Close a socket connection and free its descriptor.
 * @param sock - the socket descriptor.
 * @return 0 in the case of success, negative error code otherwise
 */
static int32_t lwip_sock_close(struct no_os_socket_desc *sock)
{
	struct lwip_socket_desc *slot;

	if (!sock || !sock->extra)
		return -EINVAL;

	slot = sock->extra;
	_lwip_socket_teardown(slot);
	no_os_free(sock);

	return 0;
}

/**
 * @brief Connect to a remote listening socket.
 * @param sock - the socket descriptor.
 * @param addr - IP/port of the remote socket.
 * @return 0 in the case of success, negative error code otherwise
 */
static int32_t lwip_sock_connect(struct no_os_socket_desc *sock,
				 struct no_os_sockaddr *addr)
{
	struct lwip_socket_desc *slot;
	uint8_t ip_addr[4];
	ip4_addr_t ip4;

	if (!sock || !sock->extra || !addr || !addr->addr)
		return -EINVAL;

	slot = sock->extra;

	sscanf(addr->addr, "%hhu.%hhu.%hhu.%hhu", &ip_addr[0], &ip_addr[1],
	       &ip_addr[2], &ip_addr[3]);

	IP_ADDR4(&ip4, ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
	const ip_addr_t ipaddr = IPADDR4_INIT(ip4.addr);

	return tcp_connect(slot->pcb, &ipaddr, addr->port, lwip_connect_callback);
}

/**
 * @brief Close a connection from the client side.
 * @param sock - the socket descriptor.
 * @return 0 in the case of success, negative error code otherwise
 */
static int32_t lwip_sock_disconnect(struct no_os_socket_desc *sock)
{
	struct lwip_socket_desc *slot;

	if (!sock || !sock->extra)
		return -EINVAL;

	slot = sock->extra;
	_lwip_socket_teardown(slot);

	return 0;
}

/**
 * @brief Send a TCP packet.
 * @param sock - the socket descriptor.
 * @param data - pointer to the data array.
 * @param size - size of data array.
 * @return Number of bytes sent, negative error code otherwise
 */
static int32_t lwip_sock_send(struct no_os_socket_desc *sock, const void *data,
			      uint32_t size)
{
	struct lwip_socket_desc *slot;
	uint32_t avail;
	uint32_t flags;
	err_t err;

	if (!sock || !sock->extra || !data)
		return -EINVAL;

	slot = sock->extra;

	if (slot->state != SOCKET_CONNECTED)
		return -ENOTCONN;

	avail = tcp_sndbuf(slot->pcb);
	flags = TCP_WRITE_FLAG_COPY;
	if (avail < size)
		/* Partial write */
		flags |= TCP_WRITE_FLAG_MORE;

	size = no_os_min(avail, size);
	err = tcp_write(slot->pcb, data, size, flags);
	if (err != ERR_OK)
		return err;

	if (!(flags & TCP_WRITE_FLAG_MORE)) {
		/* Mark data as ready to be sent */
		err = tcp_output(slot->pcb);
		if (err != ERR_OK)
			return err;
	}

	return size;
}

/**
 * @brief Receive a TCP packet.
 * @param sock - the socket descriptor.
 * @param data - pointer to the data array.
 * @param size - size of data to be read.
 * @return Number of bytes received, negative error code otherwise
 */
static int32_t lwip_sock_recv(struct no_os_socket_desc *sock, void *data,
			      uint32_t size)
{
	struct lwip_socket_desc *slot;
	struct pbuf *p, *old_p;
	uint8_t *buf, *pdata;
	uint32_t i, len;

	if (!sock || !sock->extra || !data)
		return -EINVAL;

	slot = sock->extra;

	if (slot->state != SOCKET_CONNECTED)
		return -ENOTCONN;

	i = 0;
	p = slot->p;
	pdata = data;

	/* Iterate over payloads until requested data has been read */
	while (p && i < size) {
		len = no_os_min(size - i, p->len - slot->p_idx);
		buf = p->payload;
		buf += slot->p_idx;
		memcpy(pdata + i, buf, len);
		i += len;
		slot->p_idx += len;
		if (slot->p_idx == p->len) {
			/* Done with current p. Cleanup and mark as read */
			old_p = p;
			p = p->next;
			if (p)
				pbuf_ref(p);

			if (old_p->ref > 0)
				pbuf_free(old_p);

			tcp_recved(slot->pcb, slot->p_idx);
			slot->p_idx = 0;
		}
	}
	slot->p = p;

	return i;
}

/**
 * @brief Not implemented (TCP only).
 * @return -ENOSYS
 */
static int32_t lwip_sock_sendto(struct no_os_socket_desc *sock,
				const void *data,
				uint32_t size, const struct no_os_sockaddr *to)
{
	return -ENOSYS;
}

/**
 * @brief Not implemented (TCP only).
 * @return -ENOSYS
 */
static int32_t lwip_sock_recvfrom(struct no_os_socket_desc *sock, void *data,
				  uint32_t size, struct no_os_sockaddr *from)
{
	return -ENOSYS;
}

/**
 * @brief Bind a socket to a port.
 * @param sock - the socket descriptor.
 * @param port - port number to bind the socket to.
 * @return 0 in the case of success, negative error code otherwise
 */
static int32_t lwip_sock_bind(struct no_os_socket_desc *sock, uint16_t port)
{
	struct lwip_socket_desc *slot;
	err_t err;

	if (!sock || !sock->extra)
		return -EINVAL;

	slot = sock->extra;

	err = tcp_bind(slot->pcb, IP_ANY_TYPE, port);
	if (err != ERR_OK) {
		printf("Unable to bind port %"PRIu16"\n", port);
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Configure a socket to listen for connections.
 * @param sock - the socket descriptor (server socket).
 * @param backlog - connections queue length.
 * @return 0 in the case of success, negative error code otherwise
 */
static int32_t lwip_sock_listen(struct no_os_socket_desc *sock,
				uint32_t backlog)
{
	struct lwip_socket_desc *slot;
	struct tcp_pcb *pcb;

	if (!sock || !sock->extra)
		return -EINVAL;

	slot = sock->extra;

	pcb = tcp_listen_with_backlog(slot->pcb, backlog);
	if (!pcb) {
		printf("Unable to listen on socket\n");
		return -ENOMEM;
	}
	slot->pcb = pcb;
	slot->state = SOCKET_LISTENING;

	return 0;
}

/**
 * @brief Check for incoming connections, and accept if there are any.
 * @param sock - the listening socket descriptor.
 * @param client - destination for the newly accepted socket descriptor.
 * @return 0 on success, -EAGAIN if no pending connection, error code otherwise
 */
static int32_t lwip_sock_accept(struct no_os_socket_desc *sock,
				struct no_os_socket_desc **client)
{
	struct lwip_socket_desc *serv_slot;
	struct lwip_socket_desc *cli_slot;
	struct no_os_socket_desc *cli;
	struct lwip_net_desc *ld;
	uint32_t i;

	if (!sock || !sock->extra || !client)
		return -EINVAL;

	serv_slot = sock->extra;
	ld = serv_slot->net;

	if (serv_slot->state != SOCKET_ACCEPTING) {
		if (serv_slot->state != SOCKET_LISTENING)
			return -EINVAL;
		tcp_accept(serv_slot->pcb, lwip_accept_callback);
		serv_slot->state = SOCKET_ACCEPTING;
	}

	for (i = 0; i < NO_OS_MAX_SOCKETS; ++i) {
		cli_slot = &ld->sockets[i];
		if (cli_slot->state == SOCKET_WAITING_ACCEPT) {
			/* New client connection for server */
			cli = no_os_socket_alloc(sock->net, &lwip_socket_ops, i);
			if (!cli)
				return -ENOMEM;
			cli->extra = cli_slot;
			cli_slot->state = SOCKET_CONNECTED;
			*client = cli;

			return 0;
		}
	}

	return -EAGAIN;
}

/**
 * @brief Get the time from system power-up (ms resolution).
 * @return Time in ms.
 */
u32_t sys_now(void)
{
	struct no_os_time time = no_os_get_time();

	return time.s * 1000 + time.us / 1000;
}

static const struct no_os_socket_ops lwip_socket_ops = {
	.close = lwip_sock_close,
	.connect = lwip_sock_connect,
	.disconnect = lwip_sock_disconnect,
	.send = lwip_sock_send,
	.recv = lwip_sock_recv,
	.sendto = lwip_sock_sendto,
	.recvfrom = lwip_sock_recvfrom,
	.bind = lwip_sock_bind,
	.listen = lwip_sock_listen,
	.accept = lwip_sock_accept,
};

const struct no_os_net_platform_ops lwip_net_ops = {
	.init = lwip_net_init,
	.remove = lwip_net_remove,
	.step = lwip_net_step,
	.get_ip = lwip_net_get_ip,
	.socket_open = lwip_net_socket_open,
};

#endif /* NO_OS_NET */
