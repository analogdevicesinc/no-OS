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

#include "lwipcfg.h"
#include "arch/cc.h"
#include "max_eth.h"

#include "mxc_device.h"
#include "maxim_irq.h"
#include "maxim_gpio_irq.h"

#include "adin1110.h"

#define ETHARP_SUPPORT_STATIC_ENTRIES	1

uint32_t test_cnt = 0;
static struct netif			mxc_eth_netif = {0};
static int				prev_link_status = -1;
static volatile unsigned int		eth_packet_is_in_que = 0;
static unsigned char 			mxc_lwip_internal_buff[MXC_ETH_INTERNAL_BUFF_SIZE];
static uint8_t tmp_payload[MXC_ETH_INTERNAL_BUFF_SIZE];

static void max_eth_config_noos_if(struct max_eth_desc *desc);

/* Get socket pointer from socket id */
static struct socket_desc *_get_sock(struct max_eth_desc *desc, uint32_t id)
{
	if (id >= MAX_SOCKETS)
		return NULL;

	return &desc->sockets[id];
}

/* Get first socket with state SOCKET_UNUSED. Then, set id with index */
static int32_t _get_unused_socket(struct max_eth_desc *desc, uint32_t *id)
{
	uint32_t i;

	for (i = 0; i < MAX_SOCKETS; i++)
		if (desc->sockets[i].state == SOCKET_UNUSED) {
			*id = i;
			desc->sockets[i].state = SOCKET_DISCONNECTED;

			return 0;
		}

	/* All the available connections are used */
	return -ENOMEM;
}

/* Mark socket as SOCKET_UNUSED. */
static void _release_socket(struct max_eth_desc *desc, uint32_t id)
{
	struct socket_desc *sock = _get_sock(desc, id);

	sock->state = SOCKET_UNUSED;
}

static err_t mxc_eth_netif_output(struct netif *netif, struct pbuf *p)
{
	struct max_eth_desc *eth_desc = netif->state;
	struct adin1110_desc *mac_desc = eth_desc->mac_desc;
        struct adin1110_eth_buff buff;
	(void)(netif);
	int ret;

	LINK_STATS_INC(link.xmit);
	pbuf_copy_partial(p, mxc_lwip_internal_buff, p->tot_len, 0);

	memcpy(&buff.mac_dest, mxc_lwip_internal_buff, 14);
	memcpy(&buff.mac_source, &mxc_lwip_internal_buff[6], 6);
	buff.ethertype = no_os_get_unaligned_be16(&mxc_lwip_internal_buff[12]);

	buff.payload_len = p->tot_len - 14;
	buff.payload = &mxc_lwip_internal_buff[14];

	ret = adin1110_write_fifo(mac_desc, 0, &buff);
	if (ret)
		return ret;

	// result = MXC_EMAC_SendSync(mxc_lwip_internal_buff, p->tot_len);
	// if (result)
	// 	return ERR_TIMEOUT;

	return ERR_OK;
}

static err_t max_eth_netif_init(struct netif *netif)
{
	unsigned char hwaddr[MAC_LEN] = {MAC_BYTE1, MAC_BYTE2, MAC_BYTE3,
					 MAC_BYTE4, MAC_BYTE5, MAC_BYTE6};

	netif->linkoutput = mxc_eth_netif_output;
	netif->output = etharp_output;
	netif->mtu = MXC_NETIF_MTU_SIZE;
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET;

	memcpy(netif->hwaddr, hwaddr, MAC_LEN);
	netif->hwaddr_len = MAC_LEN;

	return ERR_OK;
}

static struct pbuf *get_recvd_frames(struct adin1110_desc *mac_desc)
{
	int eth_data_len;
	size_t offset = 0;
	uint8_t buff_dbg[100];
	struct adin1110_eth_buff mac_buff;
	struct pbuf *p = NULL;
	int ret;

	mac_buff.payload = tmp_payload;
	ret = adin1110_reg_read(mac_desc, ADIN1110_RX_FSIZE_REG, &eth_data_len);
	if (ret)
		return NULL;

	if (!eth_data_len)
		return NULL;

	ret = adin1110_read_fifo(mac_desc, 0, &mac_buff);
	if (ret)
		return NULL;

	memcpy(mxc_lwip_internal_buff, mac_buff.mac_dest, 6);
	offset += 6;
	memcpy(mxc_lwip_internal_buff + offset, mac_buff.mac_source, 6);
	offset += 6;
	memcpy(mxc_lwip_internal_buff + offset, &mac_buff.ethertype, 2);
	offset += 2;
	memcpy(mxc_lwip_internal_buff + offset, mac_buff.payload, mac_buff.payload_len);
	p = pbuf_alloc(PBUF_RAW, eth_data_len, PBUF_POOL);
	if (p != NULL)
		pbuf_take(p, mxc_lwip_internal_buff, eth_data_len);

	return p;
}

static int max_lwip_tick(void *data)
{
	struct netif *netif_desc = data;
	struct adin1110_desc *mac_desc;
	struct max_eth_desc *eth_desc;
	uint32_t link_status;
	struct pbuf *p;
 	int result;
	int ret;

	eth_desc = netif_desc->state;
	mac_desc = eth_desc->mac_desc;

 	/** Check Link State **/
	ret = adin1110_link_state(mac_desc, &link_status);
	if (ret)
		return ret;

	//if (link_status)
	netif_set_link_up(netif_desc);

// 	if (link_status != prev_link_status) {
// 		if (link_status) {
// 			/* Link Down */
// 			netif_set_link_down(netif_desc);

// 			// result = MXC_EMAC_Stop();
// 			// if (result)
// 			// 	return result;
// 		} else {
// 			/* Link Up */
// 			// result = MXC_EMAC_Start();
// 			// if (result)
// 			// 	return result;

// 			netif_set_link_up(netif_desc);

// #if USE_DHCP
// 			result = dhcp_start(netif_desc);
// 			if (result)
// 				return result;
// #endif
// 		}
// 	}
	prev_link_status = link_status;

	p = get_recvd_frames(mac_desc);
	// /** Check Received Frames **/
	// if (eth_packet_is_in_que > 0) {
	// 	__disable_irq();
	// 	p = get_recvd_frames(mac_desc);
	// 	eth_packet_is_in_que--;
	// 	__enable_irq();
	// } else {
	// 	p = NULL;
	// }

	if (p != NULL) {
		LINK_STATS_INC(link.recv);
		ret = netif_desc->input(p, netif_desc);
		if (ret) {
			pbuf_free(p);
			return ret;
		}
	}

	tcp_tmr();
	/** Cyclic Timers Check **/
	sys_check_timeouts();

	return E_NO_ERROR;
}

int max_eth_init(struct netif **netif_desc, struct max_eth_param *param)
{
	struct network_interface *network_descriptor;
	struct no_os_callback_desc *tick_callback;
	struct no_os_irq_init_param nvic_param;
	struct max_eth_desc *descriptor;
	struct netif *netif_descriptor;
	ip4_addr_t ipaddr, netmask, gw;
	int ret;

	char *addr;

	nvic_param = (struct no_os_irq_init_param){
		.irq_ctrl_id = 0,
		.platform_ops = &max_irq_ops,
		.extra = NULL
	};

	if (!param)
		return E_NULL_PTR;

// #if LWIP_NETIF_LINK_CALLBACK
// 	if (!config->link_callback)
// 		return E_NULL_PTR;
// #endif

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
		goto free_netif;
	}

	tick_callback = calloc(1, sizeof(*tick_callback));
	if (!tick_callback) {
		ret = -ENOMEM;
		goto free_desc;
	}

	ret = adin1110_init(&descriptor->mac_desc, &param->adin1110_ip);
	if (ret)
		goto free_tick;

	/* Just for testing */
	ret = adin1110_set_promisc(descriptor->mac_desc, 0, true);
	if (ret)
		return ret;

	// if (!config->sys_get_ms)
	// 	return E_NULL_PTR;

	//memcpy(&mxc_eth_config, config, sizeof(mxc_eth_config_t));

	lwip_init();

#if USE_DHCP
	ip4_addr_set_zero(&ipaddr);
	ip4_addr_set_zero(&netmask);
	ip4_addr_set_zero(&gw);
#else
	LWIP_PORT_INIT_IPADDR(&ipaddr);
	LWIP_PORT_INIT_NETMASK(&netmask);
	LWIP_PORT_INIT_GW(&gw);
#endif

	addr = ip4addr_ntoa(&netmask);
	addr = ip4addr_ntoa(&gw);
	netif_add(netif_descriptor, &ipaddr, &netmask, &gw, NULL, max_eth_netif_init, ethernet_input);
	netif_descriptor->state = descriptor;

	/* Just for testing, add an ARP table entry */
	ret = update_arp_entry();

	descriptor->name[0] = param->name[0];
	descriptor->name[1] = param->name[1];

	//netif_set_link_callback(&mxc_eth_netif, config->link_callback);

	tick_callback->event = NO_OS_EVT_TIM_ELAPSED;
	tick_callback->peripheral = NO_OS_TIM_IRQ;
	tick_callback->callback = max_lwip_tick;
	tick_callback->ctx = netif_descriptor;

	/* TODO: This error handling is horrible, fix it */
	ret = no_os_timer_init(&descriptor->lwip_tick, &param->tick_param);
	if (ret)
		goto free_tick;

	ret = no_os_irq_ctrl_init(&descriptor->nvic, &nvic_param);
	if (ret)
		goto free_tick;

	ret = no_os_irq_register_callback(descriptor->nvic, MXC_TMR_GET_IRQ(param->tick_param.id),
					  tick_callback);
	if (ret)
		goto free_tick;

	netif_set_default(netif_descriptor);
	netif_set_up(netif_descriptor);

	ret = no_os_irq_enable(descriptor->nvic, MXC_TMR_GET_IRQ(param->tick_param.id));
	if (ret)
		goto free_tick;

	ret = no_os_timer_start(descriptor->lwip_tick);
	if (ret)
		return ret;

	max_eth_config_noos_if(descriptor);

	*netif_desc = netif_descriptor;

	return E_NO_ERROR;

free_tick:
	free(tick_callback);
free_desc:
	free(descriptor);
free_netif:
	free(netif_descriptor);
free_network_descriptor:
	free(network_descriptor);

	return ret;
}

void max_eth_err_callback(struct socket_desc *socket)
{
	eth_packet_is_in_que++;
}

void max_eth_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	eth_packet_is_in_que++;
}

static void max_eth_config_socket(struct socket_desc *socket)
{
	tcp_arg(socket->pcb, socket);
	tcp_recv(socket->pcb, max_eth_recv_callback);
	tcp_err(socket->pcb, max_eth_err_callback);
}

static int32_t max_socket_open(void *net, uint32_t sock_id, enum socket_protocol proto,
			       uint32_t buff_size)
{
	struct max_eth_desc *desc = net;
	struct tcp_pcb *pcb;
	int32_t ret;

	ret = _get_unused_socket(desc, &sock_id);
	if (ret)
		return ret;

	pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
	if (!pcb) {
		_release_socket(desc, sock_id);
		return -ENOMEM;
	}
	desc->sockets[sock_id].pcb = pcb;

	max_eth_config_socket(&desc->sockets[sock_id]);

	return 0;
}

static int32_t max_socket_close(void *net, uint32_t sock_id)
{
	struct max_eth_desc *desc = net;
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

	return 0;
}

static int32_t max_socket_send(void *net, uint32_t sock_id, const void *data,
			       uint32_t size)
{
	struct max_eth_desc *desc = net;
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
	size = no_os_min(aval, size);
	err = tcp_write(sock->pcb, data, size, flags);
	if (err != ERR_OK) {
		_err = err;
		printf("TCP write err: %"PRIi8"\n", _err);
		return _err;
	}

	if (!(flags & TCP_WRITE_FLAG_MORE)) {
		/* Mark data as ready to be sent */
		err = tcp_output(sock->pcb);
		if (err != ERR_OK) {
			_err = err;
			printf("TCP output err: %"PRIi8"\n", _err);
			return _err;
		}
	}

	return size;
}

static int32_t max_socket_recv(void *net, uint32_t sock_id, void *data, uint32_t size)
{
	struct max_eth_desc *desc = net;
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
		len = no_os_min(size - i, p->len - sock->p_idx);
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

static int32_t max_socket_bind(void *net, uint32_t sock_id, uint16_t port)
{
	struct max_eth_desc *desc = net;
	struct socket_desc *sock;
	err_t err;

	sock = _get_sock(desc, sock_id);
	if (!sock)
		return -EINVAL;

	err = tcp_bind(sock->pcb, IP_ANY_TYPE, port);
	if (err != ERR_OK) {
		printf("Unable to bind port %"PRIu16"\n", port);
		return -EINVAL;
	}

	return 0;
}

static int32_t max_socket_listen(void *net, uint32_t sock_id, uint32_t back_log)
{
	struct max_eth_desc *desc = net;
	struct socket_desc *sock;
	struct tcp_pcb *pcb;

	sock = _get_sock(desc, sock_id);
	if (!sock)
		return -EINVAL;

	pcb = tcp_listen_with_backlog(sock->pcb, back_log);
	if (!pcb) {
		printf("Unable to listen on socket\n");
		return -ENOMEM;
	}
	sock->pcb = pcb;
	sock->state = LISTEN;

	max_eth_config_socket(sock);

	return 0;
}

static err_t max_eth_accept_callback(void *arg, struct tcp_pcb *new_pcb, err_t err)
{
	int32_t ret;
	int8_t _err;
	uint32_t id;
	struct socket_desc *sock;
	struct socket_desc *serv_sock = arg;
	struct max_eth_desc *desc = serv_sock->desc;

	if (err != ERR_OK) {
		_err = err;
		printf("Accept callback err %"PRIi8"\n", _err);
		return ERR_OK;
	}

	ret = _get_unused_socket(desc, &id);
	if (ret)
		return ret;

	sock = _get_sock(desc, id);
	sock->pcb = new_pcb;
	sock->state = SOCKET_WAITING_ACCEPT;

	max_eth_config_socket(sock);

	return 0;
}

static int32_t max_socket_accept(void *net, uint32_t sock_id, uint32_t *client_socket_id)
{
	struct max_eth_desc *desc = net;
	struct socket_desc *serv_sock;
	struct socket_desc *cli_sock;
	uint32_t i;

	serv_sock = _get_sock(desc, sock_id);
	if (!serv_sock)
		return -EINVAL;

	if (serv_sock->state != SOCKET_ACCEPTING) {
		if (serv_sock->state != SOCKET_LISTENING)
			return -EINVAL;
		tcp_accept(serv_sock->pcb, max_eth_accept_callback);
		serv_sock->state = SOCKET_ACCEPTING;
	}

	for (i = 0; i < MAX_SOCKETS; ++i) {
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

static int32_t max_socket_sendto(void *net, uint32_t sock_id, const void *data,
				 uint32_t size, const struct socket_address *to)
{
	return -ENOENT;
}

static int32_t max_socket_recvfrom(void *net, uint32_t sock_id, void *data, uint32_t size,
				   struct socket_address *from)
{
	return -ENOENT;
}

static int32_t max_socket_connect(void *net, uint32_t sock_id,
				  struct socket_address *addr)
{
	return -ENOENT;

}

static int32_t max_socket_disconnect(void *net, uint32_t sock_id)
{
	return -ENOENT;
}

u32_t sys_now(void)
{
	return 0;
}

static void max_eth_config_noos_if(struct max_eth_desc *desc)
{
	struct network_interface *net = &desc->noos_net;

	net->socket_open = max_socket_open;
	net->socket_close = max_socket_close;
	net->socket_connect = max_socket_connect;
	net->socket_disconnect = max_socket_disconnect;
	net->socket_send = max_socket_send;
	net->socket_recv = max_socket_recv;
	net->socket_sendto = max_socket_sendto;
	net->socket_recvfrom = max_socket_recvfrom;
	net->socket_bind = max_socket_bind;
	net->socket_listen = max_socket_listen;
	net->socket_accept = max_socket_accept;

	net->net = desc;
}