#ifndef _MAX_ETH_H_
#define _MAX_ETH_H_

#include "lwip/netif.h"
#include "no_os_irq.h"
#include "no_os_timer.h"
#include "adin1110.h"
#include "network_interface.h"
#include "tcp_socket.h"

#define MXC_ETH_INTERNAL_BUFF_SIZE			2048
#define MXC_NETIF_MTU_SIZE				1500
#define MXC_ETH_MAX_DATA_SIZE				(MXC_NETIF_MTU_SIZE + 14)

#define MAX_SOCKETS	10

struct max_eth_desc;

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
	struct tcp_pcb *pcb;
	/* List of buffers to be read */
	struct pbuf *p;
	/* Inedex for p->payload. To keep track of read bytes */
	uint32_t p_idx;
	/* Reference to ethernet structure */
	struct max_eth_desc *desc;
	/* Current socket ID. Same as index in sockets */
	uint32_t id;
};

struct max_eth_desc {
	char name[2];
	struct adin1110_desc *mac_desc;
	struct no_os_irq_desc *nvic;
	struct no_os_irq_ctrl_desc *rx_int;
	struct no_os_timer_desc *lwip_tick;
	struct no_os_callback_desc *tick_callback;
	struct no_os_timer_desc *tcp_timer;
	struct no_os_callback_desc *tcp_timer_callback;

	struct network_interface noos_net;
	struct socket_desc sockets[MAX_SOCKETS];
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

extern struct network_interface maxim_net;

#endif /* _MAX_ETH_H_ */