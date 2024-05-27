/***************************************************************************//**
 *   @file   lwip_socket.c
 *   @brief  Source file for the LWIP socket layer
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
#include "lwip/dhcp.h"
#include "netif/ethernet.h"
#include "lwip/inet.h"
#include "lwip/apps/mdns.h"
#include "lwip/apps/mdns.h"
#include "lwip/apps/mdns_priv.h"
#include "lwip/apps/mdns_domain.h"
#include "lwip/apps/mdns_out.h"

#include "lwipcfg.h"
#include "lwipopts.h"
#include "arch/cc.h"
#include "lwip_socket.h"

#include "no_os_delay.h"
#include "tcp_socket.h"

#include "adin1110.h"

static bool mdns_result;
static bool mdns_is_conflict;
static uint32_t mdns_conflict_id;

static void lwip_config_if(struct lwip_network_desc *desc);

/**
 * @brief Get a socket structure based on id.
 * @param desc - lwip sockets layer specific descriptor.
 * @param id - index in the sockets array
 * @return a reference to the socket structure, NULL in case of an error.
 */
static struct lwip_socket_desc *_get_sock(struct lwip_network_desc *desc,
		uint32_t id)
{
	if (id >= NO_OS_MAX_SOCKETS)
		return NULL;

	return &desc->sockets[id];
}

/**
 * @brief Get the id of a socket that is in the SOCKET_CLOSED state (ready to be used).
 * @param desc - lwip sockets layer specific descriptor.
 * @param id - index in the sockets array.
 * @return 0 in the case of success, -ECONNREFUSED if all the connections are in use.
 */
static int32_t _get_closed_socket(struct lwip_network_desc *desc, uint32_t *id)
{
	uint32_t i;

	for (i = 0; i < NO_OS_MAX_SOCKETS; i++)
		if (desc->sockets[i].state == SOCKET_CLOSED) {
			*id = i;

			return 0;
		}

	/* All the available connections are used */
	return -ECONNREFUSED;
}

/**
 * @brief Set a socket in SOCKET_CLOSED (unused) state.
 * @param desc - lwip sockets layer specific descriptor.
 * @param id - index in the sockets array.
 */
static void _release_socket(struct lwip_network_desc *desc, uint32_t id)
{
	struct lwip_socket_desc *sock = _get_sock(desc, id);

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
	struct lwip_network_desc *desc = netif->state;

	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->netif_output)
		return -ENOSYS;

	return desc->platform_ops->netif_output(netif, p);
}

/**
 * @brief Setup a network interface with a set of predefined options.
 * @param netif - the interface be setup.
 * @return 0 in the case of success, negative error code otherwise
 */
static err_t lwip_netif_init(struct netif *netif)
{
	struct lwip_network_desc *desc;

	if (!netif)
		return -EINVAL;

	desc = netif->state;

	netif->linkoutput = lwip_netif_output;
	netif->output = etharp_output;
	netif->mtu = NO_OS_MTU_SIZE;
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP |
		       NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP;

	memcpy(netif->hwaddr, desc->hwaddr, NETIF_MAX_HWADDR_LEN);
	netif->hwaddr_len = NETIF_MAX_HWADDR_LEN;

	return ERR_OK;
}

/**
 * @brief Cycle the lwip specific timers. May call a user defined function. Needs to be called
 * in a loop as fast as possible.
 * @param desc - lwip sockets layer specific descriptor.
 * @param data - parameter to be passed to the user defined function.
 * @return 0 in the case of success, negative error code otherwise
 */
int32_t no_os_lwip_step(struct lwip_network_desc *desc, void *data)
{
	int ret;

	sys_check_timeouts();

	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (desc->platform_ops->step) {
		ret = desc->platform_ops->step(desc, data);
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
 * @return 0 in the case of success, negative error code otherwise
 */
static void mdns_name_result(struct netif* netif, u8_t result, s8_t slot)
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
 * @param desc - lwip sockets layer specific descriptor.
 * @param netif - interface on which to add the MDNS responder.
 * @return 0 in the case of success, negative error code otherwise
 */
static int _lwip_start_mdns(struct lwip_network_desc *desc, struct netif *netif)
{
	char mdns_name_buff[256];
	uint32_t len;
	int ret;

	mdns_resp_init();
	mdns_resp_register_name_result_cb(mdns_name_result);

	/*
	 * DNS conflict resolution. If domain_name is already taken, try domain_name-X
	 * (X = 1, 2, ... ), until one is available. This might take a long time
	 * (up to a few seconds) depending on the number of conflicts.
	 */
	do {
		mdns_is_conflict = false;
		len = sprintf(mdns_name_buff, "%s", NO_OS_DOMAIN_NAME);
		if (mdns_conflict_id)
			sprintf(mdns_name_buff + len, "%c%ld", '-', mdns_conflict_id);

		ret = mdns_resp_add_netif(netif, mdns_name_buff);
		if (ret)
			return ret;

		while (!mdns_result)
			no_os_lwip_step(desc, desc);

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
 * @brief Configure lwip and the lower layer network device.
 * @param desc - lwip sockets layer specific descriptor.
 * @param param - initialization parameter.
 * @return 0 in the case of success, negative error code otherwise
 */
int32_t no_os_lwip_init(struct lwip_network_desc **desc,
			struct lwip_network_param *param)
{
	struct network_interface *network_descriptor;
	struct lwip_network_desc *descriptor;
	struct netif *netif_descriptor;
	ip4_addr_t ipaddr, netmask, gw;
	uint32_t raw_netmask[4] = {0};
	uint32_t raw_gateway[4] = {0};
	uint32_t raw_ip[4] = {0};
	int ret;
	int i;

	network_descriptor = calloc(1, sizeof(*network_descriptor));
	if (!network_descriptor)
		return -ENOMEM;

	netif_descriptor = calloc(1, sizeof(*netif_descriptor));
	if (!netif_descriptor) {
		ret = -ENOMEM;
		goto free_network_descriptor;
	}

	descriptor = calloc(1, sizeof(*descriptor));
	if (!descriptor) {
		ret = -ENOMEM;
		goto free_netif_descriptor;
	}

	memcpy(descriptor->hwaddr, param->hwaddr, NETIF_MAX_HWADDR_LEN);

	lwip_init();

#ifdef NO_OS_IP
#ifndef NO_OS_NETMASK
#error NO_OS_NETMASK not defined
#endif
#ifndef NO_OS_GATEWAY
#error NO_OS_GATEWAY not defined
#endif
	sscanf(NO_OS_IP, "%d.%d.%d.%d", &raw_ip[0], &raw_ip[1], &raw_ip[2],
	       &raw_ip[3]);
	sscanf(NO_OS_NETMASK, "%d.%d.%d.%d", &raw_netmask[0], &raw_netmask[1],
	       &raw_netmask[2], &raw_netmask[3]);
	sscanf(NO_OS_GATEWAY, "%d.%d.%d.%d", &raw_gateway[0], &raw_gateway[1],
	       &raw_gateway[2], &raw_gateway[3]);

	IP4_ADDR(&ipaddr, raw_ip[0], raw_ip[1], raw_ip[2], raw_ip[3]);
	IP4_ADDR(&netmask, raw_netmask[0], raw_netmask[1], raw_netmask[2],
		 raw_netmask[3]);
	IP4_ADDR(&gw, raw_gateway[0], raw_gateway[1], raw_gateway[2], raw_gateway[3]);
#else
	ip4_addr_set_zero(&ipaddr);
	ip4_addr_set_zero(&netmask);
	ip4_addr_set_zero(&gw);
#endif
	netif_add(netif_descriptor, &ipaddr, &netmask, &gw, descriptor, lwip_netif_init,
		  ethernet_input);
	descriptor->lwip_netif = netif_descriptor;

	ret = param->platform_ops->init(&descriptor->mac_desc, param->mac_param);
	if (ret)
		goto free_netif;

	descriptor->platform_ops = param->platform_ops;

	netif_set_default(netif_descriptor);
	netif_set_up(netif_descriptor);

	netif_set_link_up(netif_descriptor);

#ifndef NO_OS_IP
	uint32_t dhcp_timeout = 20000;
	ret = dhcp_start(netif_descriptor);
	if (ret)
		goto platform_remove;

	/*
	 * By default, AutoIP is enabled, so it will fall back on getting a link local
	 * address in the 169.254.0.0/16 range after 2 unsuccessful DHCPREQUESTs. Timeout after
	 * ~20s.
	 */
	while (!netif_descriptor->ip_addr.addr && dhcp_timeout > 0) {
		no_os_lwip_step(descriptor, descriptor);
		dhcp_timeout--;
		no_os_mdelay(1);
	}

	if (!dhcp_timeout && !netif_descriptor->ip_addr.addr) {
		ret = -ETIMEDOUT;
		printf("LWIP configuration timed out\n");
		goto platform_remove;
	}
#endif

	ret = _lwip_start_mdns(descriptor, netif_descriptor);
	if (ret)
		goto platform_remove;

	lwip_config_if(descriptor);

	printf("IP address: %s\n", ip4addr_ntoa(&netif_descriptor->ip_addr));
	printf("Network mask: %s\n", ip4addr_ntoa(&netif_descriptor->netmask));
	printf("Gateway's IP address: %s\n", ip4addr_ntoa(&netif_descriptor->gw));
	for (i = 0; i < NO_OS_MAX_SOCKETS; i++) {
		descriptor->sockets[i].state = SOCKET_CLOSED;
		descriptor->sockets[i].desc = descriptor;
		descriptor->sockets[i].id = i;
	}

	*desc = descriptor;

	return 0;

platform_remove:
	param->platform_ops->remove(descriptor->mac_desc);
free_netif:
	netif_remove(netif_descriptor);
free_descriptor:
	free(descriptor);
free_netif_descriptor:
	free(netif_descriptor);
free_network_descriptor:
	free(network_descriptor);

	return ret;
}

/**
 * @brief Configure lwip and the lower layer network device.
 * @param desc - lwip sockets layer specific descriptor.
 * @return 0
 */
int32_t no_os_lwip_remove(struct lwip_network_desc *desc)
{
	if (!desc || !desc->platform_ops)
		return -EINVAL;

	if (!desc->platform_ops->remove)
		return -ENOSYS;

	desc->platform_ops->remove(desc->mac_desc);
	netif_remove(desc->lwip_netif);
	no_os_free(desc->lwip_netif);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Called in case of a lwip error. The pcb may have already been freed.
 * @param arg - lwip sockets layer specific descriptor.
 * @param err - error code.
 */
static void lwip_err_callback(void *arg, err_t err)
{
	struct lwip_socket_desc *socket = arg;

	socket->state = SOCKET_CLOSED;
}

/**
 * @brief Close a socket connection.
 * @param desc - lwip sockets layer specific descriptor.
 * @param param - the index of the socket to be closed.
 * @return 0 in the case of success, negative error code otherwise
 */
static int32_t lwip_socket_close(void *net, uint32_t sock_id)
{
	struct lwip_network_desc *desc = net;
	struct lwip_socket_desc *sock;

	sock = _get_sock(desc, sock_id);
	if (!sock)
		return -EINVAL;

	if (!sock->pcb)
		return 0;

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
	_release_socket(desc, sock_id);

	return 0;
}

/**
 * @brief Called when a pbuf is received.
 * @param desc - lwip sockets layer specific descriptor.
 * @param tcp_pcb - lwip TCP descriptor of the socket from which the pbuf was received.
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
		sock->state = SOCKET_CLOSED;

		lwip_socket_close(sock->desc, sock->id);

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
 * @param desc - lwip sockets layer specific descriptor.
 * @param err - error code.
 */
static void lwip_config_socket(struct lwip_socket_desc *desc)
{
	tcp_arg(desc->pcb, desc);
	tcp_recv(desc->pcb, lwip_recv_callback);
	tcp_err(desc->pcb, lwip_err_callback);
}

/**
 * @brief Create a TCP socket.
 * @param net - lwip sockets layer specific descriptor.
 * @param sock_id - index of the socket that was created.
 * @param proto - Layer 4 protocol (only TCP is supported).
 * @param buff_size - unused.
 * @return 0 in the case of success, negative error code otherwise
 */
static int32_t lwip_socket_open(void *net, uint32_t *sock_id,
				enum socket_protocol proto,
				uint32_t buff_size)
{
	struct lwip_network_desc *desc = net;
	struct tcp_pcb *pcb;
	uint32_t socket_id;
	int32_t ret;

	NO_OS_UNUSED_PARAM(buff_size);
	if (proto != PROTOCOL_TCP)
		return -EPROTONOSUPPORT;

	ret = _get_closed_socket(desc, &socket_id);
	if (ret)
		return ret;

	pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	if (!pcb) {
		_release_socket(desc, socket_id);
		return -ENOMEM;
	}

	ip_set_option(pcb, SOF_REUSEADDR);

	desc->sockets[socket_id].pcb = pcb;
	desc->sockets[socket_id].desc = desc;
	desc->sockets[socket_id].id = socket_id;
	desc->sockets[socket_id].p = NULL;

	lwip_config_socket(&desc->sockets[socket_id]);

	mdns_conflict_id = 0;
	*sock_id = socket_id;

	return 0;
}

/**
 * @brief Send a TCP packet.
 * @param net - lwip sockets layer specific descriptor.
 * @param sock_id - index of the socket to send data to.
 * @param data - pointer to the data array.
 * @param size - size of data array.
 * @return 0 in the case of success, negative error code otherwise
 */
static int32_t lwip_socket_send(void *net, uint32_t sock_id, const void *data,
				uint32_t size)
{
	struct lwip_network_desc *desc = net;
	struct lwip_socket_desc *sock;
	uint32_t avail;
	uint32_t flags;
	err_t err;

	sock = _get_sock(desc, sock_id);
	if (!sock)
		return -EINVAL;

	if (sock->state != SOCKET_CONNECTED)
		return -ENOTCONN;

	avail = tcp_sndbuf(sock->pcb);
	flags = TCP_WRITE_FLAG_COPY;
	if (avail < size)
		/* Partial write */
		flags |= TCP_WRITE_FLAG_MORE;

	size = no_os_min(avail, size);
	err = tcp_write(sock->pcb, data, size, flags);
	if (err != ERR_OK)
		return err;

	if (!(flags & TCP_WRITE_FLAG_MORE)) {
		/* Mark data as ready to be sent */
		err = tcp_output(sock->pcb);
		if (err != ERR_OK)
			return err;
	}

	return size;
}

/**
 * @brief Receive a TCP packet.
 * @param net - lwip sockets layer specific descriptor.
 * @param sock_id - index of the socket to receive data from.
 * @param data - pointer to the data array.
 * @param size - size of data to be read.
 * @return 0 in the case of success, negative error code otherwise
 */
static int32_t lwip_socket_recv(void *net, uint32_t sock_id, void *data,
				uint32_t size)
{
	struct lwip_network_desc *desc = net;
	struct lwip_socket_desc *socket;
	struct pbuf *p, *old_p;
	uint8_t *buf, *pdata;
	uint32_t i, len;

	socket = _get_sock(desc, sock_id);
	if (!socket)
		return -EINVAL;

	if (socket->state != SOCKET_CONNECTED)
		return -ENOTCONN;

	i = 0;
	p = socket->p;
	pdata = data;

	/* Iterate over payloads until requested data has been read */
	while (p && i < size) {
		len = no_os_min(size - i, p->len - socket->p_idx);
		buf = p->payload;
		buf += socket->p_idx;
		memcpy(pdata + i, buf, len);
		i += len;
		socket->p_idx += len;
		if (socket->p_idx == p->len) {
			/* Done with current p. Cleanup and mark as read */
			old_p = p;
			p = p->next;
			if (p)
				pbuf_ref(p);

			if (old_p->ref > 0)
				pbuf_free(old_p);

			tcp_recved(socket->pcb, socket->p_idx);
			socket->p_idx = 0;
		}
	}
	socket->p = p;

	return i;
}

/**
 * @brief Bind a socket to a port.
 * @param net - lwip sockets layer specific descriptor.
 * @param sock_id - index of the socket to receive data from.
 * @param port - port number to bind a socket to.
 * @return 0 in the case of success, negative error code otherwise
 */
static int32_t lwip_socket_bind(void *net, uint32_t sock_id, uint16_t port)
{
	struct lwip_network_desc *desc = net;
	struct lwip_socket_desc *socket;
	err_t err;

	socket = _get_sock(desc, sock_id);
	if (!socket)
		return -EINVAL;

	err = tcp_bind(socket->pcb, IP_ANY_TYPE, port);
	if (err != ERR_OK) {
		printf("Unable to bind port %"PRIu16"\n", port);
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Configure a socket to listen for connections.
 * @param net - lwip sockets layer specific descriptor.
 * @param sock_id - index of the socket to listen (server socket id).
 * @param back_log - connections queue length.
 * @return 0 in the case of success, negative error code otherwise
 */
static int32_t lwip_socket_listen(void *net, uint32_t sock_id,
				  uint32_t back_log)
{
	struct lwip_network_desc *desc = net;
	struct lwip_socket_desc *socket;
	struct tcp_pcb *pcb;

	socket = _get_sock(desc, sock_id);
	if (!socket)
		return -EINVAL;

	pcb = tcp_listen_with_backlog(socket->pcb, back_log);
	if (!pcb) {
		printf("Unable to listen on socket\n");
		return -ENOMEM;
	}
	socket->pcb = pcb;
	socket->state = SOCKET_LISTENING;

	return 0;
}

/**
 * @brief Called when a connecton is accepted.
 * @param net - lwip sockets layer specific descriptor.
 * @param new_pcb - lwip TCP descriptor of the new connection.
 * @param err - error code.
 * @return 0 in the case of success, negative error code otherwise
 */
static err_t lwip_accept_callback(void *arg, struct tcp_pcb *new_pcb, err_t err)
{
	int32_t ret;
	uint32_t id;
	struct lwip_socket_desc *socket;
	struct lwip_socket_desc *serv_sock = arg;
	struct lwip_network_desc *desc = serv_sock->desc;

	if (err != ERR_OK) {
		printf("Accept callback err %d\n", err);
		return err;
	}

	ret = _get_closed_socket(desc, &id);
	if (ret)
		return ret;

	socket = _get_sock(desc, id);
	socket->pcb = new_pcb;
	socket->state = SOCKET_WAITING_ACCEPT;

	tcp_setprio(socket->pcb, 0);
	lwip_config_socket(socket);
	tcp_nagle_disable(socket->pcb);

	return 0;
}

/**
 * @brief Check for incoming connections, and accept if there are any.
 * @param net - lwip sockets layer specific descriptor.
 * @param client_socket_id - index of the assigned client socket.
 * @return 0 in the case of success, negative error code otherwise
 */
static int32_t lwip_socket_accept(void *net, uint32_t sock_id,
				  uint32_t *client_socket_id)
{
	struct lwip_network_desc *desc = net;
	struct lwip_socket_desc *serv_sock;
	struct lwip_socket_desc *cli_sock;
	uint32_t i;

	serv_sock = _get_sock(desc, sock_id);
	if (!serv_sock)
		return -EINVAL;

	if (serv_sock->state != SOCKET_ACCEPTING) {
		if (serv_sock->state != SOCKET_LISTENING)
			return -EINVAL;
		tcp_accept(serv_sock->pcb, lwip_accept_callback);
		serv_sock->state = SOCKET_ACCEPTING;
	}

	for (i = 0; i < NO_OS_MAX_SOCKETS; ++i) {
		cli_sock = &desc->sockets[i];
		if (cli_sock->state == SOCKET_WAITING_ACCEPT) {
			/* New client connection for server */
			*client_socket_id = i;
			cli_sock->state = SOCKET_CONNECTED;
			return 0;
		}
	}

	return -EAGAIN;
}

/**
 * @brief Not implemented.
 * @param net - Not used.
 * @param sock_id - Not used.
 * @param data - Not used.
 * @param size - Not used.
 * @param to - Not used.
 * @return -ENOSYS
 */
static int32_t lwip_socket_sendto(void *net, uint32_t sock_id, const void *data,
				  uint32_t size, const struct socket_address *to)
{
	return -ENOSYS;
}

/**
 * @brief Not implemented.
 * @param net - Not used.
 * @param sock_id - Not used.
 * @param data - Not used.
 * @param size - Not used.
 * @param from - Not used.
 * @return -ENOSYS
 */
static int32_t lwip_socket_recvfrom(void *net, uint32_t sock_id, void *data,
				    uint32_t size, struct socket_address *from)
{
	return -ENOSYS;
}

/**
 * @brief Called once a connect() completes.
 * @param net - lwip sockets layer specific descriptor.
 * @param pcb - unused.
 * @param err - connection result.
 * @return -ENOSYS
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
 * @brief Connect to a remote listening socket.
 * @param net - lwip sockets layer specific descriptor.
 * @param sock_id - index of the local socket.
 * @param addr - IP/port of the remote socket.
 * @return -ENOSYS
 */
static int32_t lwip_socket_connect(void *net, uint32_t sock_id,
				   struct socket_address *addr)
{
	struct lwip_network_desc *desc = net;
	struct lwip_socket_desc *socket;
	struct tcp_pcb *pcb;
	uint8_t ip_addr[4];
	err_t ret;

	sscanf(addr->addr, "%d.%d.%d.%d", &ip_addr[0], &ip_addr[1],
	       &ip_addr[2], &ip_addr[3]);

	ip4_addr_t ip4;
	IP_ADDR4(&ip4, ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3]);
	const ip_addr_t ipaddr = IPADDR4_INIT(ip4.addr);

	socket = _get_sock(desc, sock_id);
	if (!socket)
		return -ENOMEM;

	pcb = socket->pcb;

	return tcp_connect(pcb, &ipaddr, addr->port, lwip_connect_callback);
}

/**
 * @brief Close a connection from the client side.
 * @param net - lwip sockets layer specific descriptor.
 * @param sock_id - index of the remote socket to be closed.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t lwip_socket_disconnect(void *net, uint32_t sock_id)
{
	return lwip_socket_close(net, sock_id);
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

struct network_interface lwip_socket_ops = {
	.socket_open = lwip_socket_open,
	.socket_close = lwip_socket_close,
	.socket_connect = lwip_socket_connect,
	.socket_disconnect = lwip_socket_disconnect,
	.socket_send = lwip_socket_send,
	.socket_recv = lwip_socket_recv,
	.socket_sendto = lwip_socket_sendto,
	.socket_recvfrom = lwip_socket_recvfrom,
	.socket_bind = lwip_socket_bind,
	.socket_listen = lwip_socket_listen,
	.socket_accept = lwip_socket_accept,
};

/**
 * @brief Configure the TCP socket operations for the lwip sockets layer descriptor.
 * @param desc - lwip sockets layer specific descriptor.
 */
static void lwip_config_if(struct lwip_network_desc *desc)
{
	struct network_interface *net = &desc->no_os_net;

	net->socket_open = lwip_socket_open;
	net->socket_close = lwip_socket_close;
	net->socket_connect = lwip_socket_connect;
	net->socket_disconnect = lwip_socket_disconnect;
	net->socket_send = lwip_socket_send;
	net->socket_recv = lwip_socket_recv;
	net->socket_sendto = lwip_socket_sendto;
	net->socket_recvfrom = lwip_socket_recvfrom;
	net->socket_bind = lwip_socket_bind;
	net->socket_listen = lwip_socket_listen;
	net->socket_accept = lwip_socket_accept;

	net->net = desc;
}
#endif
