#ifndef _MAX_ETH_H_
#define _MAX_ETH_H_

#include "lwip/netif.h"
#include "no_os_irq.h"
#include "no_os_timer.h"
#include "adin1110.h"

#define MXC_ETH_INTERNAL_BUFF_SIZE			2048
#define MXC_NETIF_MTU_SIZE				1500
#define MXC_ETH_MAX_DATA_SIZE				(MXC_NETIF_MTU_SIZE + 14)

struct max_eth_desc {
	char name[2];
	struct adin1110_desc *mac_desc;
	struct no_os_irq_desc *nvic;
	struct no_os_timer_desc *lwip_tick;
	struct no_os_callback_desc *tick_callback;
};

struct max_eth_param {
	char name[2];
	netif_status_callback_fn link_callback;

	struct adin1110_init_param adin1110_ip;
	struct no_os_timer_init_param tick_param;
};

/**
 * @brief      Initialize lwIP stack
 *
 * @param      config             configuration parameters
 *
 * @return     #E_NO_ERROR        if successful
 * @return     #E_NULL_PTR        if pointer is null
 */
int max_eth_init(struct netif **, struct max_eth_param *);

/**
 * @brief      Application must call this function when an Ethernet packet is received
 *
 */
void MXC_ETH_RecvIrq(void);

/**
 * @brief      Application must call this function periodically in order to run lwIP stack
 *
 * @return     #E_NO_ERROR        no issue
 * @return     #MXC_ERROR_CODES   phy issue
 */
int MXC_ETH_Tick(void);

#endif /* _MAX_ETH_H_ */