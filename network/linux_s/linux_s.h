/*
 * linux_s.h
 *
 *  Created on: Feb 15, 2021
 *      Author: nramona
 */

#ifndef NOOS_NETWORK_LINUX_S_LINUX_S_H_
#define NOOS_NETWORK_LINUX_S_LINUX_S_H_

#include "network_interface.h"

#define SOCKET_NO 30
#define SOCKETS (SOCKET_NO + 1)

/* Structure storing data used by a socket */
// struct lsocket_desc {
// 	/* file descriptor */
// 	int32_t fd;
// 	/* Circular buffer size */
// 	uint32_t		cb_size;
// 	/* Socket type */
// 	enum socket_protocol	type;
// 	/* States of a socket structure */
// 	enum {
// 		/* The socket structure is unused */
// 		LINUX_SOCKET_UNUSED,
// 		/* Socket is waiting for accept */
// 		LINUX_SOCKET_WAITING_ACCEPT,
// 		/* Socket structure have been initialized */
// 		LINUX_SOCKET_DISCONNECTED,
// 		/* Socket connected to a server or to a client */
// 		LINUX_SOCKET_CONNECTED,
// 		/* State of server socket when it listen to new connections */
// 		LINUX_SOCKET_LISTENING
// 	}			state;
// };

struct lserver_desc {
	/* Server fd */
	int32_t	fd;
	/* Server port */
	uint16_t	port;
	/* Ids of clientes socket that are available*/
	uint32_t	client_ids[SOCKET_NO];
	/* Number of cliente socket available*/
	uint32_t	back_log_clients;
};


struct linux_desc {
	/*Id of the open socket*/
	uint32_t id;

	/*Sockets*/
	int32_t sockets[SOCKETS];

	/*Server descriptor*/
	struct lserver_desc server;

	/*Reference to the network*/
	struct network_interface net;
};

struct linux_init_param {
/*	Maximum size for incoming data
	uint32_t max_buff_size;*/

	/* Reference to the network*/
	struct network_interface* net;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

int32_t linux_init(struct linux_desc** desc, struct linux_init_param* param);

int32_t linux_remove(struct linux_desc* desc);

int32_t linux_connect();

int32_t linux_get_network_interface(struct linux_desc *desc, struct network_interface** net);

#endif /* NOOS_NETWORK_LINUX_S_LINUX_S_H_ */
