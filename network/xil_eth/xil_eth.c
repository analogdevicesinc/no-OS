/***************************************************************************//**
 * @file xil_eth.c
 * @brief Implementation of ethernet functions for network.
 * @author Mihail Chindris (mihail.chindris@analog.com)
 ********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * - Neither the name of Analog Devices, Inc. nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * - The use of this software may or may not infringe the patent rights
 * of one or more patent holders. This license does not release you
 * from the requirement that you obtain separate licenses from these
 * patent holders to use this software.
 * - Use of the software either in source or binary form, must be run
 * on or directly connected to an Analog Devices Inc. component.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#ifdef XILINX_PLATFORM

#include <string.h>

#include "lwip/dhcp.h"
#include "lwip/init.h"
#include "lwip/tcp.h"
#include "netif/xadapter.h"
#include "platform.h"
#include "platform_config.h"
#include "xil_cache.h"
#include "xil_eth.h"

#include "no-os/error.h"
#include "no-os/print_log.h"
#include "no-os/util.h"

/*
 * TODO: I would move this to xil_eth_desc but this implies modify the default
 * eth files. I think this would be a better way. But for faster development
 * are kept as globals.
 */
extern volatile int TcpFastTmrFlag;
extern volatile int TcpSlowTmrFlag;
extern volatile int dhcp_timoutcntr;
struct netif *echo_netif;

static void xil_eth_config_noos_if(struct xil_eth_desc *desc);

/* Internal structure describing a connection */
struct socket_desc {
	enum {
		/* Available to be used */
		SOCKET_UNUSED,
		/* Connection set as server bound to a port */
		SOCKET_LISTENING,
		/* Connection set as server and accepting incomming conns  */
		SOCKET_ACCEPTING,
		/* Accept callback received, waiting call to accept function */
		SOCKET_WAITING_ACCEPT,
		/* Socket is connected to remote */
		SOCKET_CONNECTED,
		/* Socked has been disconnected. */
		SOCKET_DISCONNECTED,
	} state;
	/* Xilinx structure of connection state and data */
	struct tcp_pcb *pcb;
	/* List of buffers to be read */
	struct pbuf *p;
	/* Inedex for p->payload. To keep track of read bytes */
	uint32_t p_idx;
	/* Reference to ethernet structure */
	struct xil_eth_desc *desc;
	/* Current socket ID. Same as index in sockets */
	uint32_t id;
};

struct xil_eth_desc {
	/* Structure with network operations */
	struct network_interface noos_net;
	/* Internal xilinx network structure */
	struct netif netif;
	/* Array of all connections. Used or not used */
	struct socket_desc sockets[MAX_SOCKETS];
};

/* Get socket pointer from socket id */
static struct socket_desc *_get_sock(struct xil_eth_desc *desc, uint32_t id)
{
	if (id >= MAX_SOCKETS)
		return NULL;

	return &desc->sockets[id];
}

/* Get first socket with state SOCKET_UNUSED. Then, set id with index */
static int32_t _get_unused_socket(struct xil_eth_desc *desc, uint32_t *id)
{
	uint32_t i;

	for (i = 0; i < MAX_SOCKETS; i++)
		if (desc->sockets[i].state == SOCKET_UNUSED) {
			*id = i;
			desc->sockets[i].state = SOCKET_DISCONNECTED;

			return SUCCESS;
		}

	/* All the available connections are used */
	return -ENOMEM;
}

/* Mark socket as SOCKET_UNUSED. */
static void _release_socket(struct xil_eth_desc *desc, uint32_t id)
{
	struct socket_desc *sock = _get_sock(desc, id);

	sock->state = SOCKET_UNUSED;
}

/*
 * Implement with IPV4 and DHCP enabled by default.
 * For more configs see lwip echo example
 */
int32_t xil_eth_init(struct xil_eth_desc **desc,
		     struct xil_eth_init_param *param)
{
	struct xil_eth_desc *ldesc;
	struct netif *netif;
	ip_addr_t ipaddr, netmask, gw;
	int32_t ret, i;

	ldesc = calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return -ENOMEM;

#if defined (__arm__) && !defined (ARMR5)
#if XPAR_GIGE_PCS_PMA_SGMII_CORE_PRESENT == 1 || \
    XPAR_GIGE_PCS_PMA_1000BASEX_CORE_PRESENT == 1
	ProgramSi5324();
	ProgramSfpPhy();
#endif
#endif

	/* Define this board specific macro in order perform PHY reset on ZCU102 */
#ifdef XPS_BOARD_ZCU102
	if(IicPhyReset()) {
		pr_err("Error performing PHY reset \n\r");
		return -1;
	}
#endif

	init_platform();

	if (param->use_static_ip) {
		ipaddr = param->ip_addr;
		gw = param->gw;
		netmask = param->netmask;
	} else {
		ipaddr.addr = 0;
		gw.addr = 0;
		netmask.addr = 0;
	}

	lwip_init();

	netif = &ldesc->netif;
	/* Needed by platform files */
	echo_netif = netif;

	/* Add network interface to the netif_list, and set it as default */
	if (!xemac_add(netif, &ipaddr, &netmask, &gw,
		       param->mac_ethernet_address, param->emac_baseaddr)) {
		pr_err("Error adding N/W interface\n\r");
		ret = FAILURE;
		goto cleanup;
	}

	netif_set_default(netif);

	/* now enable interrupts */
	platform_enable_interrupts();

	if (!param->use_static_ip) {
		/* specify that the network if is up */
		netif_set_up(netif);

		/* DHCP Config */
		dhcp_release(netif);

		/* Create a new DHCP client for this interface.
		* Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at
		* the predefined regular intervals after starting the client.
		*/
		dhcp_start(netif);
		dhcp_timoutcntr = 24;

		while(((netif->ip_addr.addr) == 0) && (dhcp_timoutcntr > 0))
			xemacif_input(netif);

		if (dhcp_timoutcntr <= 0) {
			if ((netif->ip_addr.addr) == 0) {
				pr_err("DHCP Timeout\r\n");
				ret = FAILURE;
				goto cleanup;
			}
		}
	}

	xil_eth_config_noos_if(ldesc);

	*desc = ldesc;
	for (i = 0; i < MAX_SOCKETS; ++i) {
		ldesc->sockets[i].id = i;
		ldesc->sockets[i].desc = ldesc;
	}

	return SUCCESS;
cleanup:
	free(ldesc);
	return ret;
}

int32_t xil_eth_remove(struct xil_eth_desc *desc)
{
	free(desc);

	return SUCCESS;
}

/* They seem to be non static in tcp.c but they aren't in any .h file */
void tcp_fasttmr(void);
void tcp_slowtmr(void);

/* It is simpler to call tcp_tmr every 250 seconds than using xil_eth_step */
int32_t xil_eth_step(struct xil_eth_desc *desc)
{
	if (TcpFastTmrFlag) {
		tcp_fasttmr();
		TcpFastTmrFlag = 0;
	}
	if (TcpSlowTmrFlag) {
		tcp_slowtmr();
		TcpSlowTmrFlag = 0;
	}
	xemacif_input(&desc->netif);

	return SUCCESS;
}

int32_t xil_eth_get_network_interface(struct xil_eth_desc *desc,
				      struct network_interface **net)
{
	*net = &desc->noos_net;

	return SUCCESS;
}

int32_t xil_eth_get_ip(struct xil_eth_desc *desc, char *ip_buff,
		       uint32_t buff_size)
{
	ip_addr_t *ip;

	ip = &desc->netif.ip_addr;

	return snprintf(ip_buff, buff_size, "%d.%d.%d.%d", ip4_addr1(ip),
			ip4_addr2(ip), ip4_addr3(ip), ip4_addr4(ip));
}


/******************************************************************************/
/*************************** Functions Declarations *******************************/
/******************************************************************************/

/* Called when new data arrives */
static err_t xil_eth_recv_callback(void *arg, struct tcp_pcb *tpcb,
				   struct pbuf *p, err_t err)
{
	struct socket_desc *sock = arg;

	if (!p) {
		/* Remote has closed connection */
		tcp_recv(sock->pcb, NULL);
		sock->state = SOCKET_DISCONNECTED;

		return ERR_OK;
	} else if (err != ERR_OK) {
		/* Cleanup for unknown reasons */
		pbuf_free(p);
		return err;
	}

	if (!sock->p) {
		/* First buffer. Save it in p */
		sock->p = p;
		sock->p_idx = 0;
	} else {
		/*
		 * TODO: Can this cause error while reading?
		 * Should some locking be use here?
		 */
		/* Concatenate new buffer to p */
		pbuf_cat(sock->p, p);
	}

	return ERR_OK;
}

static void xil_eth_err_callback(void *arg, err_t err)
{
	struct socket_desc *sock = arg;

	sock->state = SOCKET_DISCONNECTED;
}

/* Set default callbacks for a new socket */
static void xil_eth_config_sock(struct socket_desc *sock)
{
	/* Set function argument for callback */
	tcp_arg(sock->pcb, sock);
	tcp_recv(sock->pcb, xil_eth_recv_callback);
	tcp_err(sock->pcb, xil_eth_err_callback);
}

/** @brief See \ref network_interface.socket_open */
static int32_t xil_socket_open(struct xil_eth_desc *desc, uint32_t *sock_id,
			       enum socket_protocol prot, uint32_t buff_size)
{
	int32_t err;
	struct tcp_pcb *pcb;

	err = _get_unused_socket(desc, sock_id);
	if (IS_ERR_VALUE(err))
		return err;

	pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	if (!pcb) {
		_release_socket(desc, *sock_id);
		return -ENOMEM;
	}
	desc->sockets[*sock_id].pcb = pcb;

	xil_eth_config_sock(&desc->sockets[*sock_id]);

	return SUCCESS;
}

/** @brief See \ref network_interface.socket_close */
static int32_t xil_socket_close(struct xil_eth_desc *desc, uint32_t sock_id)
{
	struct socket_desc *sock;

	sock = _get_sock(desc, sock_id);
	if (!sock)
		return -EINVAL;

	if (sock->state == SOCKET_UNUSED)
		return -ENOENT;

	tcp_recv(sock->pcb, NULL);
	tcp_err(sock->pcb, NULL);
	if (sock->p) {
		tcp_recved(sock->pcb, sock->p->tot_len);
		pbuf_free(sock->p);
	}

	tcp_close(sock->pcb);
	sock->p_idx = 0;
	_release_socket(desc, sock_id);

	return SUCCESS;
}

/** @brief See \ref network_interface.socket_send */
static int32_t xil_socket_send(struct xil_eth_desc *desc, uint32_t sock_id,
			       const void *data, uint32_t size)
{
	struct socket_desc *sock;
	err_t err;
	uint32_t aval;
	uint32_t flags;
	int8_t _err;

	sock = _get_sock(desc, sock_id);
	if (!sock)
		return -EINVAL;

	if (sock->state != SOCKET_CONNECTED)
		return -ENOTCONN;

	aval = tcp_sndbuf(sock->pcb);
	flags = TCP_WRITE_FLAG_COPY;
	if (aval < size)
		/* Partial write */
		flags |= TCP_WRITE_FLAG_MORE;
	size = min(aval, size);
	err = tcp_write(sock->pcb, data, size, flags);
	if (err != ERR_OK) {
		_err = err;
		pr_err("TCP write err: %"PRIi8"\n", _err);
		return _err;
	}

	if (!(flags & TCP_WRITE_FLAG_MORE)) {
		/* Mark data as ready to be sent */
		err = tcp_output(sock->pcb);
		if (err != ERR_OK) {
			_err = err;
			pr_err("TCP output err: %"PRIi8"\n", _err);
			return _err;
		}
	}

	return size;
}

/** @brief See \ref network_interface.socket_recv */
static int32_t xil_socket_recv(struct xil_eth_desc *desc, uint32_t sock_id,
			       void *data, uint32_t size)
{
	struct socket_desc *sock;
	struct pbuf *p, *old_p;
	uint8_t *buf, *pdata;
	uint32_t i, len;

	sock = _get_sock(desc, sock_id);
	if (!sock)
		return -EINVAL;

	if (sock->state != SOCKET_CONNECTED)
		return -ENOTCONN;

	i = 0;
	p = sock->p;
	pdata = data;
	/* Iterate over payloads until requested data has been read */
	while (p && i < size) {
		len = min(size - i, p->len - sock->p_idx);
		buf = p->payload;
		buf += sock->p_idx;
		memcpy(pdata + i, buf, len);
		i += len;
		sock->p_idx += len;
		if (sock->p_idx == p->len) {
			/* Done with current p. Cleanup and mark as read */
			old_p = p;
			p = p->next;
			if (p)
				pbuf_ref(p);
			pbuf_free(old_p);
			tcp_recved(sock->pcb, sock->p_idx);
			sock->p_idx = 0;
		}
	}
	sock->p = p;

	return i;
}

/** @brief See \ref network_interface.socket_bind */
static int32_t xil_socket_bind(struct xil_eth_desc *desc, uint32_t sock_id,
			       uint16_t port)
{
	err_t err;
	struct socket_desc *sock;

	sock = _get_sock(desc, sock_id);
	if (!sock)
		return -EINVAL;

	err = tcp_bind(sock->pcb, IP_ANY_TYPE, port);
	if (err != ERR_OK) {
		pr_err("Unable to bind port %"PRIu16"\n", port);
		return -EINVAL;
	}

	return SUCCESS;
}

/** @brief See \ref network_interface.socket_listen */
static int32_t xil_socket_listen(struct xil_eth_desc *desc, uint32_t sock_id,
				 uint32_t back_log)
{
	struct socket_desc *sock;
	struct tcp_pcb *pcb;

	sock = _get_sock(desc, sock_id);
	if (!sock)
		return -EINVAL;

	pcb = tcp_listen_with_backlog(sock->pcb, back_log);
	if (!pcb) {
		pr_err("Unable to listen on socket\n");
		return -ENOMEM;
	}
	sock->pcb = pcb;
	sock->state = SOCKET_LISTENING;

	xil_eth_config_sock(sock);

	return SUCCESS;
}

/* Called when at a new connection request. Prepare structure for new socket */
static err_t xil_eth_accept_callback(void *arg, struct tcp_pcb *new_pcb,
				     err_t err)
{
	int32_t noos_err;
	int8_t _err;
	uint32_t id;
	struct socket_desc *sock;
	struct socket_desc *serv_sock = arg;
	struct xil_eth_desc *desc = serv_sock->desc;

	if (err != ERR_OK) {
		_err = err;
		pr_err("Accept callback err %"PRIi8"\n", _err);
		return ERR_OK;
	}

	noos_err = _get_unused_socket(desc, &id);
	if (IS_ERR_VALUE(noos_err))
		return ERR_MEM;

	sock = _get_sock(desc, id);
	sock->pcb = new_pcb;
	sock->state = SOCKET_WAITING_ACCEPT;

	xil_eth_config_sock(sock);

	return ERR_OK;
}

/** @brief See \ref network_interface.socket_accept */
static int32_t xil_socket_accept(struct xil_eth_desc *desc, uint32_t sock_id,
				 uint32_t *client_socket_id)
{
	struct socket_desc *serv_sock, *cli_sock;
	uint32_t i;

	serv_sock = _get_sock(desc, sock_id);
	if (!serv_sock)
		return -EINVAL;

	if (serv_sock->state != SOCKET_ACCEPTING) {
		if (serv_sock->state != SOCKET_LISTENING)
			return -EINVAL;
		tcp_accept(serv_sock->pcb, xil_eth_accept_callback);
		serv_sock->state = SOCKET_ACCEPTING;
	}

	for (i = 0; i < MAX_SOCKETS; ++i) {
		cli_sock = &desc->sockets[i];
		if (cli_sock->state == SOCKET_WAITING_ACCEPT) {
			if (cli_sock->pcb->local_port ==
			    serv_sock->pcb->local_port) {
				/* New client connection for server */
				*client_socket_id = i;
				cli_sock->state = SOCKET_CONNECTED;
				return SUCCESS;
			}
		}
	}

	return -EAGAIN;
}

/** @brief See \ref network_interface.socket_sendto */
static int32_t xil_socket_sendto(struct xil_eth_desc *desc, uint32_t sock_id,
				 const void *data, uint32_t size,
				 const struct socket_address* to)
{
	return -ENOENT;
}

/** @brief See \ref network_interface.socket_recvfrom */
static int32_t xil_socket_recvfrom(struct xil_eth_desc *desc, uint32_t sock_id,
				   void *data, uint32_t size,
				   struct socket_address *from)
{
	return -ENOENT;
}

/** @brief See \ref network_interface.socket_connect */
static int32_t xil_socket_connect(struct xil_eth_desc *desc, uint32_t sock_id,
				  struct socket_address *addr)
{
	return -ENOENT;

}

/** @brief See \ref network_interface.socket_disconnect */
static int32_t xil_socket_disconnect(struct xil_eth_desc *desc,
				     uint32_t sock_id)
{
	return -ENOENT;
}

static void xil_eth_config_noos_if(struct xil_eth_desc *desc)
{
	struct network_interface *net = &desc->noos_net;

	net->socket_open = (int32_t (*)(void *, uint32_t *,
					enum socket_protocol, uint32_t))
			   xil_socket_open;
	net->socket_close = (int32_t (*)(void *, uint32_t)) xil_socket_close;
	net->socket_connect = (int32_t (*)(void *, uint32_t,
					   struct socket_address *))
			      xil_socket_connect;
	net->socket_disconnect = (int32_t (*)(void *, uint32_t))
				 xil_socket_disconnect;
	net->socket_send = (int32_t (*)(void *, uint32_t, const void *,
					uint32_t))xil_socket_send;
	net->socket_recv = (int32_t (*)(void *, uint32_t, void *, uint32_t))
			   xil_socket_recv;
	net->socket_sendto = (int32_t (*)(void *, uint32_t, const void *,
					  uint32_t,
					  const struct socket_address* to))
			     xil_socket_sendto;
	net->socket_recvfrom = (int32_t (*)(void *, uint32_t, void *, uint32_t,
					    struct socket_address* from))
			       xil_socket_recvfrom;
	net->socket_bind = (int32_t (*)(void *, uint32_t, uint16_t))
			   xil_socket_bind;
	net->socket_listen = (int32_t (*)(void *, uint32_t, uint32_t))
			     xil_socket_listen;
	net->socket_accept = (int32_t (*)(void *, uint32_t, uint32_t*))
			     xil_socket_accept;

	net->net = desc;
}
#endif
