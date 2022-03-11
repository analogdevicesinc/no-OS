/***************************************************************************//**
 *   @file   network_interface.h
 *   @brief  Interface used for network communication
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 *   @copyright
 * Copyright 2020(c) Analog Devices, Inc.
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

#ifndef NETWORK_INTERFACE_H
#define NETWORK_INTERFACE_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum socket_protocol
 * @brief Type of protocol used by a socket
 */
enum socket_protocol {
	/** TCP Protocol */
	PROTOCOL_TCP,
	/** UDP Protocol */
	PROTOCOL_UDP
};

/**
 * @struct socket_address
 * @brief Represent an endpoint of a connection.
 */
struct socket_address {
	/** Address */
	char		*addr;
	/** Port */
	uint16_t	port;
};

/**
 * @struct network_interface
 * @brief Interface that connect the data layer with the transport layer
 */
struct network_interface {
	/** Network instance */
	void *net;
	/**
	 * @brief Open a socket.
	 * @param net - Network interface
	 * @param proto - Protocol used by the socket
	 * @param sock_id - Address where to store the id for referencing the
	 * socket.
	 * @return
	 *  - 0 : On success
	 *  - -1 : Otherwise
	 */
	int32_t (*socket_open)(void *net, uint32_t *sock_id,
			       enum socket_protocol proto, uint32_t buff_size);
	/**
	 * @brief Close the socket.
	 * @param net - Network interface
	 * @param sock_id - Socket id
	 * @return
	 *  - 0 : On success
	 *  - -1 : Otherwise
	 */
	int32_t (*socket_close)(void *net, uint32_t sock_id);
	/**
	 * @brief Connects TCP socket to a remote host.
	 * @param net - Network interface
	 * @param sock_id - Socket id
	 * @param addr - Remote host
	 * @return
	 *  - 0 : On success
	 *  - -1 : Otherwise
	 */
	int32_t (*socket_connect)(void *net, uint32_t sock_id,
				  struct socket_address *addr);
	/**
	 * @brief Disconnect form a socket
	 * @param net - Network interface
	 * @param sock_id - Socket id
	 * @return
	 *  - 0 : On success
	 *  - -1 : Otherwise
	 */
	int32_t (*socket_disconnect)(void *net, uint32_t sock_id);
	/**
	 * @brief Send data over a TCP socket.
	 * @param net - Network interface
	 * @param sock_id - Socket id
	 * @param data - Buffer of data to send to the host
	 * @param size - Size of the buffer in bytes
	 * @return
	 *  - Number of sent bytes : On success
	 *  - -1 : Otherwise
	 */
	int32_t (*socket_send)(void *net, uint32_t sock_id,
			       const void *data, uint32_t size);
	/**
	 * @brief Receive data over a TCP socket.
	 *
	 * The socket must be connected to a remote host.
	 * The call is non blocking
	 * @param net - Network interface
	 * @param sock_id - Socket id
	 * @param data - Destination buffer for received data
	 * @param size - Maximum data to read
	 * @return
	 *  - Number of bytes received into the buffer.
	 *  - -1 is something went wrong
	 */
	int32_t (*socket_recv)(void *net, uint32_t sock_id,
			       void *data, uint32_t size);
	/**
	 * @brief Send a packet over a UDP socket.
	 * @param net - Network interface
	 * @param sock_id - Socket id
	 * @param data - Buffer of data to send to the host
	 * @param size - Size of the buffer in bytes
	 * @param to - Address of the remote host
	 * @return
	 *  - Number of received bytes : On success
	 *  - -1 : Otherwise
	 */
	int32_t (*socket_sendto)(void *net, uint32_t sock_id,
				 const void *data, uint32_t size,
				 const struct socket_address *to);
	/**
	 * @brief Receive a packet over a UDP socket.
	 * @param net - Network interface
	 * @param sock_id - Socket id
	 * @param data - Destination buffer for received data
	 * @param size - Maximum data to read
	 * @param from - Destination for the source address or NULL
	 * @return
	 *  - Number of sent bytes : On success
	 *  - -1 : Otherwise
	 */
	int32_t (*socket_recvfrom)(void *net, uint32_t sock_id,
				   void *data, uint32_t size,
				   struct socket_address *from);
	/**
	 * @brief Bind a specific port to a socket.
	 *
	 * Only one tcp_socket can be used to bind a port.
	 * @param net - Network interface
	 * @param sock_id - Socket id
	 * @param port - Port to bind
	 * @return
	 *  - 0 : On success
	 *  - \ref Negative error code on failure
	 */
	int32_t (*socket_bind)(void *net, uint32_t sock_id, uint16_t port);

	/**
	 * @brief Start listening for incoming connections.
	 *
	 * Should be call after bind is done.
	 * @param net - Network interface
	 * @param sock_id - Socket id
	 * @param back_log - Number of connections in the socket's listen queue
	 * @return
	 *  - 0 : On success
	 *  - \ref Negative error code on failure
	 */
	int32_t (*socket_listen)(void *net, uint32_t sock_id,
				 uint32_t back_log);

	/**
	 * @brief Accepts a connection on a socket.
	 *
	 * The server socket must be bound and set to listen for connections.
	 * Will block until a new connection arrives
	 * @param net - Network interface
	 * @param sock_id - Socket id
	 * @param client_socket_id - Address where to store the id of the new
	 * connection.
	 * @return
	 *  - 0 : On success
	 *  - \ref Negative error code on failure
	 */
	int32_t (*socket_accept)(void *net, uint32_t sock_id,
				 uint32_t *client_socket_id);
};

#endif
