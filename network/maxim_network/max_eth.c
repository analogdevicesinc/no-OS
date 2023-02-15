#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/timeouts.h"
#include "lwip/stats.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"
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

uint32_t test_cnt = 0;
static struct netif			mxc_eth_netif = {0};
static int				prev_link_status = -1;
static volatile unsigned int		eth_packet_is_in_que = 0;
static unsigned char 			mxc_lwip_internal_buff[MXC_ETH_INTERNAL_BUFF_SIZE];
static uint8_t tmp_payload[MXC_ETH_INTERNAL_BUFF_SIZE];

static err_t mxc_eth_netif_output(struct netif *netif, struct pbuf *p)
{
	struct adin1110_desc *mac_desc = netif->state;
        struct adin1110_eth_buff buff;
	int result;
	(void)(netif);
	int ret;

	LINK_STATS_INC(link.xmit);
	pbuf_copy_partial(p, mxc_lwip_internal_buff, p->tot_len, 0);

	buff.payload_len = p->tot_len;
	buff.payload = mxc_lwip_internal_buff;

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
		return ret;

	if (!eth_data_len)
		return -EAGAIN;

	ret = adin1110_read_fifo(mac_desc, 0, &mac_buff);
	if (ret)
		return ret;

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

// 	/** Check Link State **/
	ret = adin1110_link_state(mac_desc, &link_status);
	if (ret)
		return ret;

	if (!link_status)
		test_cnt = 0;

	test_cnt++;

	return ret;

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
// 	prev_link_status = link_status;

// 	p = get_recvd_frames(mac_desc);
// 	// /** Check Received Frames **/
// 	// if (eth_packet_is_in_que > 0) {
// 	// 	__disable_irq();
// 	// 	p = get_recvd_frames(mac_desc);
// 	// 	eth_packet_is_in_que--;
// 	// 	__enable_irq();
// 	// } else {
// 	// 	p = NULL;
// 	// }

// 	if (p != NULL) {
// 		LINK_STATS_INC(link.recv);
// 		if (netif_desc->input(p, netif_desc) != ERR_OK) {
// 			pbuf_free(p);
// 		}
// 	}

// 	/** Cyclic Timers Check **/
// 	sys_check_timeouts();

// 	return E_NO_ERROR;
}

int max_eth_init(struct netif **netif_desc, struct max_eth_param *param)
{
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

	netif_descriptor = calloc(1, sizeof(*netif_descriptor));
	if (!netif_descriptor)
		return -ENOMEM;

	descriptor = calloc(1, sizeof(*descriptor));
	if (!descriptor)
		goto free_netif;

	tick_callback = calloc(1, sizeof(*tick_callback));
	if (!tick_callback)
		goto free_desc;

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
	netif_add(netif_descriptor, &ipaddr, &netmask, &gw, NULL, max_eth_netif_init, netif_input);
	netif_descriptor->state = descriptor;

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

	*netif_desc = netif_descriptor;

	return E_NO_ERROR;

free_tick:
	free(tick_callback);
free_desc:
	free(descriptor);
free_netif:
	free(netif_descriptor);

	return ret;
}

void MXC_ETH_RecvIrq(void)
{
	eth_packet_is_in_que++;
}

u32_t sys_now(void)
{
	return 0;
}
