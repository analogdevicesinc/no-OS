/***************************************************************************//**
 *   @file   at_parser.h
 *   @brief  Interface of AT commands for ESP8266 Module
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
********************************************************************************
 *  @section at_details AT Parser description
 *
 *  This parser make easy use of the AT commands by defined macros at
 *  \ref at_cmd with an easy interface for command parameters ( \ref in_param )
 *  and their output ( \ref out_param ).
 *
 *  A command can be executed with \ref at_run_cmd and data from a connection
 *  can be read with \ref at_read_buffer .
 *
 *  How AT command work can be found at:\n
 *  https://cdn.sparkfun.com/datasheets/Wireless/WiFi/Command%20Doc.pdf\n
 *  https://github.com/espressif/ESP8266_AT/wiki/basic_at_0019000902
 *
 * @subsection at_example Sample code
 *   @code{.c}
 *	void example() {
 *	   struct at_init_param at_param;
 *	   struct at_desc       *desc;
 *	   union in_out_param   param;
 *	   char                 *ssid = "ssid";
 *	   char                 *pwd = "pwd";
 *	   uint32_t             len;
 *	   char                 buff[100];
 *
 *	   platform_specific_initialization(&at_param);
 *	   at_init(&desc, &at_param);
 *
 *	   //Get Firmware version
 *	   at_run_cmd(desc->at, AT_GET_VERSION, AT_EXECUTE_OP, &param);
 *	   no_os_cb_size(g_param.out.result, &len);
 *	   no_os_cb_read(g_param.out.result, buff, len);
 *	   buff[len] = '\0';
 *	   printf("Result: %s\n", buff);
 *
 *	   //Connect to wifi network
 *	   str_to_at(&param.in.network.ssid, (uint8_t *)ssid);
 *	   str_to_at(&param.in.network.pwd, (uint8_t *)pass);
 *	   at_run_cmd(desc->at, AT_CONNECT_NETWORK, AT_SET_OP, &param);
 *
 *	   at_close(desc);
 *	}
 *    @endcode
*******************************************************************************/

#ifndef AT_DRIVER_H
# define AT_DRIVER_H

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "at_params.h"
#include "no_os_circular_buffer.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/** @brief Maximum number of connection the ESP8266 module can handle */
#define MAX_CONNECTIONS				4
/** @brief Maximum data to send on a chipsend command */
#define MAX_CIPSEND_DATA			2048

/* Remove comment when implementing parsing result */
//#define PARSE_RESULT
/** @brief An overrun occurred in the current buffer connection */
#define AT_ERROR_CONN_BUFFER_OVERRUN		0x2
/** @brief A hardware error occurred in the UART device */
#define AT_ERROR_UART				0x4
/** @brief A hardware error occurred in the UART device */
#define AT_ERROR_CONNECTION_LOST		0x8
/** @brief An overflow occurred in the internal buffer. This error should be
 * reported to developers */
#define AT_ERROR_INTERNAL_BUFFER_OVERFLOW	0x10

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum cmd_operation
 * @brief Command type.
 *
 *  Each \ref at_cmd can be executed as one of this types.
 *  See https://cdn.sparkfun.com/datasheets/Wireless/WiFi/Command%20Doc.pdf
 *  to see for which command which type can be used.
 */
enum cmd_operation {
	/** Test command */
	AT_TEST_OP	= 0x1u,
	/** Query current values of the associated command */
	AT_QUERY_OP	= 0x2u,
	/** Set command. Must use \ref in_param as param in \ref at_run_cmd*/
	AT_SET_OP	= 0x4u,
	/** Execute command */
	AT_EXECUTE_OP	= 0x8u
};

/**
 * @enum at_cmd
 * @brief Available commands for esp8266 module
 */
enum at_cmd {
	/** Attention */
	AT_ATTENTION,			// ""
	/** Reset module */
	AT_RESET,			// "+RST"
	/** TODO . Enter in deep sleep mode.
	 * Use \ref in_param.deep_sleep_time_ms as set parameter
	 */
	AT_DEEP_SLEEP,			// "+GSLP"
	/** View version info*/
	AT_GET_VERSION,			// "+GMR"
	/**
	 * Setting operation mode.
	 * Use \ref in_param.wifi_mode as set parameter
	 */
	AT_SET_OPERATION_MODE,		// "+CWMODE"
	/**
	 * Connect to network.
	 * Use \ref in_param.network as set parameter
	 */
	AT_CONNECT_NETWORK,		// "+CWJAP"
	/** List available networks */
	AT_GET_AVAILABLE_NETWORKS,	// "+CWLAP"
	/** Disconnect from network */
	AT_DISCONNECT_NETWORK,		// "+CWQAP"
	/**
	 * Set access point when running as Hotspot
	 * Use \ref in_param.ap as set parameter
	 */
	AT_SET_ACCESS_POINT,		// "+CWSAP"
	/** Get ips of connected clients to hotspot */
	AT_GET_CONNECTED_IPS,		// "+CWLIF"
	/** Get module status */
	AT_GET_STATUS,			// "+CIPSTATUS"
	/**
	 * Start new connection
	 * Use \ref in_param.connection as set parameter
	 */
	AT_START_CONNECTION,		// "+CIPSTART"
	/**
	 * Send data over connection
	 * Use \ref in_param.send_data as set parameter
	 */
	AT_SEND,			// "+CIPSEND"
	/**
	 * Stop connection.
	 * Use \ref in_param.conn_id as set parameter
	 */
	AT_STOP_CONNECTION,		// "+CIPCLOSE"
	/** Get ip */
	AT_GET_IP,			// "+CIFSR"
	/**
	 * Set multiple or single connection type
	 * Use \ref in_param.conn_type as set parameter
	 */
	AT_SET_CONNECTION_TYPE,		// "+CIPMUX"
	/**
	 * Configure a TCP server
	 * Use \ref in_param.server as set parameter
	 */
	AT_SET_SERVER,			// "+CIPSERVER"
	/**
	 * Set transport mode: Not supported command.
	 * Use \ref in_param.transport_mode as set parameter
	 */
	AT_SET_TRANSPORT_MODE,		// "+CIPMODE"
	/**
	 * Set client timeout
	 * Use \ref in_param.timeout as set parameter
	 */
	AT_SET_CLIENT_TIMEOUT,		// "+CIPSTO"
	/**
	 *  Ping
	 *  Use \ref in_param.ping_ip as set parameter
	 */
	AT_PING				// "+PING"
};

/**
 * @union out_param
 * @brief Parameters returned by the command
 */
union out_param {
#if defined(PARSE_RESULT)
	/* TODO : Implement parsing response to parameters */
	/** Result for TEST_OPERATIONS */
	struct at_buff		test_result;
	/** Result for \ref AT_GET_VERSION */
	struct at_buff		version;
	/** Result for \ref AT_GET_IP */
	struct cifsr_param	local_ip;
	/** Result for \ref AT_GET_AVAILABLE_NETWORKS */
	struct cwlap_exec_param	available_networks;
	/** Result for \ref AT_GET_STATUS */
	struct cipstatus_param	status;
	/** Result form AT_SET_OPERATION_MODE */
	enum cwmod_param	wifi_mode;
	/** Result form AT_CONNECT_NETWORK */
	struct cwlap_querry_param wifi_info;
	/** Result form AT_SET_ACCESS_POINT */
	struct cwsap_param	ap_info;
	/** Result form AT_SET_CONNECTION_TYPE */
	struct cipmux_param	con_type;
	/** Result form AT_SET_TRANSPORT_MODE */
	struct cipmode_param	transp_mode;
	/** Result form AT_SET_CLIENT_TIMEOUT */
	struct cipsto_param	timeout;
#else
	/**
	 * The result of the executing command is written in result.
	 * The circular buffer is of type uint8_t. See example of how to read
	 * the result.
	 */
	struct at_buff	result;
#endif
};

/**
 * @union in_param
 * @brief Parameters used to set a command
 */
union in_param {
	/** Param for \ref AT_DEEP_SLEEP */
	uint32_t		deep_sleep_time_ms;
	/** Param for \ref AT_SET_OPERATION_MODE */
	enum cwmod_param	wifi_mode;
	/** Param for \ref AT_CONNECT_NETWORK */
	struct cwjap_param	network;
	/** Param for \ref AT_SET_ACCESS_POINT */
	struct cwsap_param	ap;
	/** Param for \ref AT_START_CONNECTION */
	struct cipstart_param	connection;
	/** Param for \ref AT_SEND */
	struct cipsend_param	send_data;
	/** Param for \ref AT_STOP_CONNECTION */
	uint32_t		conn_id;
	/** Param for \ref AT_SET_CONNECTION_TYPE */
	enum connection_type	conn_type;
	/** Param for \ref AT_SET_SERVER */
	struct cipserver_param	server;
	/** Param for \ref AT_SET_TRANSPORT_MODE */
	enum cipmode_param	transport_mode;
	/** Param for \ref AT_SET_CLIENT_TIMEOUT . Range 0~7200 seconds */
	uint32_t		timeout;
	/** Param for \ref AT_PING */
	struct at_buff		ping_ip;
};

/**
 * @enum at_event
 * @brief Event to be passed to the callback
 */
enum at_event {
	AT_NEW_CONNECTION,
	AT_CLOSED_CONNECTION
};

/**
 * @union in_out_param
 * @brief Parameter used to send and receive data from a command
 */
union in_out_param {
	union out_param out;
	union in_param	in;
};

/**
 * @struct at_init_param
 * @brief Parameter to initialize parser
 */
struct at_init_param {
	/* Should be initialized outside in order to fill uart_irq_conf */
	struct no_os_uart_desc	*uart_desc;
	struct no_os_irq_ctrl_desc	*irq_desc;
	uint32_t		uart_irq_id;
	void			*uart_irq_conf;
	/* Context that will be passed to the callback */
	void			*callback_ctx;
	/*
	 * Will be called when a new connection is created or deleted.
	 * When an AT_NEW_CONNECTION event is received, user can save in cb a
	 * circular buffer where to write data received from the connection.
	 * If *cb is set to NULL data will not be saved
	 */
	void			(*connection_callback)(void *ctx,
			enum at_event event,
			uint32_t conn_id,
			struct no_os_circular_buffer **cb);
	/* Software reset enable */
	bool		sw_reset_en;
};

/**
 * @struct at_desc
 * @brief Parser descriptor.
 */
struct at_desc;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initialize parser*/
int32_t at_init(struct at_desc **desc,const struct at_init_param *param);
/* Free resources used by parser */
int32_t at_remove(struct at_desc *desc);

/* Execute an AT command */
int32_t at_run_cmd(struct at_desc *desc, enum at_cmd cmd, enum cmd_operation op,
		   union in_out_param *param);
/* Convert null terminated string to at_buff */
int32_t str_to_at(struct at_buff *dest, const uint8_t *src);
/* Convert at_buff to null terminated string */
int32_t at_to_str(uint8_t **dest, const struct at_buff *src);

#endif
