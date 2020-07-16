/***************************************************************************//**
 *   @file   at_params.h
 *   @brief  Structures used by at_parser
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
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

#ifndef AT_PARAMS_H
#define AT_PARAMS_H

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct at_buff
 * @brief Structure used in the at_parser to work with buffers
 *
 * It can be initialized from a null terminated string using \ref str_to_at
 * and vice versa with \ref at_to_str .
 */
struct at_buff {
	/** Address of the buffer */
	uint8_t		*buff;
	/** Size of data written in buffer */
	uint32_t	len;
};

/**
 * @enum encryption_type
 * @brief Type of wifi security
 */
enum encryption_type {
	/** Open */
	OPEN		= 0u,
	/** WEP */
	WEP		= 1u,
	/** WPA_PSK */
	WPA_PSK		= 2u,
	/** WPA2_PSK */
	WPA2_PSK	= 3u,
	/** WPA_WPA2_PSK */
	WPA_WPA2_PSK	= 4u
};

/**
 * @enum socket_type
 * @brief Type of a connection
 */
enum socket_type {
	/** TCP Connection */
	SOCKET_TCP,
	/** UDP Connection */
	SOCKET_UDP
};

/**
 * @enum cwmod_param
 * @brief Type of operation for module
 */
enum cwmod_param {
	/** Operate as client */
	CLIENT = 1,
	/** Operate as access point */
	ACCESS_POINT,
	/** Operate as both */
	CLIENT_AND_ACCESS_POINT
};

/**
 * @struct cwjap_param
 * @brief Parameter used to connect to a wifi network
 */
struct cwjap_param {
	/** Ssid */
	struct at_buff	ssid;
	/** Password */
	struct at_buff	pwd;
};

/**
 * @struct cwsap_param
 * @brief Parameter used to set an access point
 */
struct cwsap_param {
	/** AP ssid */
	struct at_buff		ssid;
	/** AP password  */
	struct at_buff		pwd;
	/** AP channel ID */
	uint32_t		ch_id;
	/** Encription type */
	enum encryption_type	encription;
};

/**
 * @struct cipstart_param
 * @brief Parameter used to start a new connection
 */
struct cipstart_param {
	/** Connection id. Used only if \ref MULTIPLE_CONNECTION is set */
	uint32_t		id;
	/** Connection type */
	enum socket_type	soket_type;
	/** Connection address. Example "mqtt.brocker.com" */
	struct at_buff		addr;
	/** Connection port */
	uint16_t		port;
	/* Parameters used only for UDP connection */
	/** Local port. Used only if \ref SOCKET_UDP is used */
	uint16_t		local_port;
	/** Udp mode. Used only if \ref SOCKET_UDP is used */
	enum {
		UDP_ENTITY_NO_CHANGE,
		UDP_ENTITY_CHANGE_ONE,
		UDP_ENTITY_CHANGE_ALLOWED
	}			udp_mode;
};

/**
 * @struct cipsend_param
 * @brief Structure used to send data over a connection
 */
struct cipsend_param {
	/** Connection id. Used only if \ref MULTIPLE_CONNECTION is set */
	uint32_t	id;
	/**
	 * Data to be sent. data.buff should be the address of the buffer and
	 * data.len its size.
	 */
	struct at_buff	data;
	/* Parameters used only for UDP connection */
	/** Remote address.  Used only if \ref SOCKET_UDP is used */
	struct at_buff	remote_ip;
	/** Remote port. Used only if \ref SOCKET_UDP is used */
	uint16_t	remote_port;
};

/**
 * @enum connection_type
 * @brief Parameter used to set the connection type
 */
enum connection_type {
	/** Only one connection available */
	SINGLE_CONNECTION,
	/** Multiple connection available */
	MULTIPLE_CONNECTION
};

/**
 * @struct cipserver_param
 * @brief Parameter used to configure a TCP server
 */
struct cipserver_param {
	/** Command for open or close the server*/
	enum {
		DELETE_SERVER,
		CREATE_SERVER
	}		action;
	/** Port */
	uint16_t	port;
};

/**
 * @enum cipmode_param
 * @brief Not implemented
 */
enum cipmode_param {
	/** Normal mode */
	NORMAL_MODE,
	/** Unvernished mode */
	UNVARNISHED_MODE
};

/**
 * @struct cwlap_set_param
 * @brief Not used. Find an wifi with specific conditions
 */
struct cwlap_set_param {
	/** ssid*/
	struct at_buff		ssid;
	/** mac */
	struct at_buff		mac;
	/** channel */
	uint8_t			channel;
};

#if defined(PARSE_RESULT)
/* Search networks. Not used in driver*/
struct cwlap_exec_param {
	struct at_buff		ssid;
	struct at_buff		mac;
	enum encryption_type	encryption;
	/* I think in dB */
	int32_t			strength;
	uint8_t			channel;
};

/* Search networks. Not used in driver*/
struct cipstatus_param {
	enum {
		GOT_IP		= 1u,
		CONNECTED	= 3u,
		DISCONNECTED	= 4u
	}			stat;
	struct {
		uint8_t			connection_id;
		enum socket_type	socket_type;
		struct at_buff		remote_ip;
		uint16_t		*remote_port;
		uint16_t		*local_port;
		enum {
			RUNNING_AS_CLIENT	= 0u,
			RUNNING_AS_SERVER	= 1u
		}			tetype;
	}			connections[4]; //Max 4 connections. To test it.
};

struct cifsr_param {
	struct at_buff		soft_ap_ip;
	struct at_buff		station_ip;
};

struct cwlap_querry_param {
	struct at_buff		ssid;
	struct at_buff		mac;
	uint8_t			channel;
	/* I think in dB */
	int32_t			strength;
};
#endif

#endif /* AT_PARAMS_H */
