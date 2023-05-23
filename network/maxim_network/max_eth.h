#ifndef _MAX_ETH_H_
#define _MAX_ETH_H_

#include "lwip/netif.h"
#include "no_os_irq.h"
#include "no_os_timer.h"
#include "adin1110.h"
#include "network_interface.h"
#include "tcp_socket.h"

#define ADIN1110_LWIP_BUFF_SIZE				1530
#define MXC_NETIF_MTU_SIZE				1500
#define NO_OS_DOMAIN_NAME				"swiot"

#define MAX_SOCKETS	50

struct max_eth_desc;

struct socket_desc {
	enum {
		/* Available to be used */
		SOCKET_CLOSED,
		/* Connection set as server bound to a port */
		SOCKET_LISTENING,
		/* Connection set as server and accepting incomming conns  */
		SOCKET_ACCEPTING,
		/* Accept callback received, waiting call to accept function */
		SOCKET_WAITING_ACCEPT,
		/* Socket is connected to remote */
		SOCKET_CONNECTED,
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
	struct netif *lwip_netif;
	struct network_interface noos_net;
	struct socket_desc sockets[MAX_SOCKETS];
	void *extra;
};

struct max_eth_param {
	char name[2];
	struct adin1110_init_param adin1110_ip;
	void *extra;
};

int max_lwip_tick(void *);

/**
 * @brief      Initialize lwIP stack
 *
 * @param      config             configuration parameters
 *
 * @return     #E_NO_ERROR        if successful
 * @return     #E_NULL_PTR        if pointer is null
 */
int max_eth_init(struct netif **, struct max_eth_param *);

extern struct network_interface maxim_net;

#endif /* _MAX_ETH_H_ */