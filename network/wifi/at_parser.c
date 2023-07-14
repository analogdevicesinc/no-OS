/***************************************************************************//**
 *   @file   at_parser.c
 *   @brief  Interface of AT commands for ESP8266 Module (Implementation)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <inttypes.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "at_parser.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_uart.h"
#include "no_os_irq.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* Should be sizeof(async_msgs)/sizeof(*async_msgs) */
#define NB_ASYNC_MESSAGES	3
/* Should be sizeof(responses)/sizeof(*responses) */
#define NB_RESPONSE_MESSAGES	4
/* Max command length: at+cwsap=max_ssid_32,max_pass_64,0,0 -> 110 characters */
#define CMD_BUFF_LEN		120u
/* Maybe this could be smaller. Here must one response at a time */
#define RESULT_BUFF_LEN		500u
/* Used to remove warnings on strings */
#define PUI8(X)			((uint8_t *)(X))
/* Timeout waiting for module response. (20 seconds) */
#define MODULE_TIMEOUT		20000

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/* Command descriptor */
struct cmd_desc {
	struct at_buff	cmd;
	uint32_t	type;
};

/* Structure that links the command string with the command macro by index */
static const struct cmd_desc g_map[] = {
	{{PUI8(""), 0}, AT_EXECUTE_OP},
	{{PUI8("+RST"), 4}, AT_EXECUTE_OP},
	{{PUI8("+GSLP"), 5}, AT_SET_OP},
	{{PUI8("+GMR"), 4}, AT_EXECUTE_OP},
	{{PUI8("+CWMODE"), 7}, AT_QUERY_OP | AT_SET_OP | AT_TEST_OP},
	{{PUI8("+CWJAP"), 6}, AT_QUERY_OP | AT_SET_OP},
	{{PUI8("+CWLAP"), 6}, AT_EXECUTE_OP},
	{{PUI8("+CWQAP"), 6}, AT_EXECUTE_OP},
	{{PUI8("+CWSAP"), 6}, AT_QUERY_OP | AT_SET_OP},
	{{PUI8("+CWLIF"), 6}, AT_EXECUTE_OP},
	{{PUI8("+CIPSTATUS"), 10}, AT_EXECUTE_OP},
	{{PUI8("+CIPSTART"), 9}, AT_TEST_OP | AT_SET_OP},
	{{PUI8("+CIPSEND"), 8}, AT_SET_OP},
	{{PUI8("+CIPCLOSE"), 9}, AT_EXECUTE_OP | AT_SET_OP},
	{{PUI8("+CIFSR"), 6}, AT_EXECUTE_OP},
	{{PUI8("+CIPMUX"), 7}, AT_QUERY_OP | AT_SET_OP},
	{{PUI8("+CIPSERVER"), 10}, AT_SET_OP},
	{{PUI8("+CIPMODE"), 8}, AT_QUERY_OP | AT_SET_OP},
	{{PUI8("+CIPSTO"), 7}, AT_QUERY_OP | AT_SET_OP},
	{{PUI8("+PING"), 5}, AT_SET_OP}
};

/* Structure storing a connection status */
struct connection_desc {
	/* Connection buffer */
	struct no_os_circular_buffer	*cbuff;
	/* Pending bytes for reading */
	uint32_t		to_read;
	/* True if connection established */
	bool			active;
	/* Type of connection */
	enum socket_type	type;
};

/* Structure storing the status of the parser */
struct at_desc {
	/* - Uart related fields */
	/* Uart descriptor */
	struct no_os_uart_desc	*uart_desc;
	/* Irq descriptor */
	struct no_os_irq_ctrl_desc	*irq_desc;
	/* Uart irq id */
	uint32_t		uart_irq_id;

	/* - Connection related fields */
	/* Structures storing connections status */
	struct connection_desc	conn[MAX_CONNECTIONS];
	/* Id of active connection or -1 if non is active */
	int32_t			current_conn;
	/* Stores if running as single or multiple connection mode */
	bool			multiple_conections;

	/* - Data fields */
	/* Static allocated buffers */
	struct {
		uint8_t	result_buff[RESULT_BUFF_LEN];
		uint8_t	app_result_buff[RESULT_BUFF_LEN];
		uint8_t	cmd_buff[CMD_BUFF_LEN];
	} 			buffers;
	/* Stores data received from the module */
	volatile struct at_buff	result;
	/* Buffer to build the command */
	struct at_buff		cmd;
	/* Buffer to read one char */
	uint8_t			read_ch;

	/* - Control fields */
	/* Variable to store errors */
	uint32_t errors;
	/* Store the wifi status */
	bool is_wifi_connected;
	/* State of the callback */
	volatile enum {
		/* Normal mode. Read each char and interpret the result */
		READING_RESPONSES,
		/* When an +IPD is received, callback enter in this mode */
		READING_PAYLOAD,
		/* Used when a reset command have been sent */
		RESETTING_MODULE,
		/* Used when using AT_SEND to wait for the character '>' */
		WAITING_SEND

	}			callback_operation;
	/* Indexes in the ready message */
	uint8_t			ready_idx;
	/* Indexes in the async response given by the driver */
	uint8_t			async_idx[NB_ASYNC_MESSAGES];
	/* Indexes in the response given by the driver */
	uint8_t			resp_idx[NB_RESPONSE_MESSAGES];
	/* Ipd idx */
	uint8_t			ipd_idx;
	/* State of ipd command message */
	enum {
		NOT_MATCH,
		RAEDING_CONN,
		WAITING_COMMA,
		READING_LEN
	}			ipd_stat;
	/* Variable where current message length is built */
	uint32_t		ipd_len;
	/* Will be called when a new connection is created or closed */
	void			(*connection_callback)(void *ctx, enum at_event,
			uint32_t conn_id, struct no_os_circular_buffer **cb);
	/* Context that will be passed to the callback */
	void			*callback_ctx;
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/* Increment idx if current ch match in msg[idx] and return true if idx == len*/
static inline bool match_message(const struct at_buff *msg, uint8_t *idx,
				 uint8_t ch)
{
	if (ch == msg->buff[*idx]) {
		/* Increment matching index if there is a match */
		(*idx)++;
		if (*idx == msg->len)
			/* Return the index of the match message */
			return true;
	} else if (ch == msg->buff[0]) {
		*idx = 1;
	} else {
		*idx = 0;
	}

	return false;
}

/* Check if new payload message have been received and set the payload size in
 * to_read if so.
 */
static inline bool is_payload_message(struct at_desc *desc, uint8_t ch)
{
	static const struct at_buff	at_ipd = {PUI8("\r\n+IPD,"), 7};
	/* max_ch_search = at_ipd.len + sizeof("0,1024") */
	bool		ret;

	ret = false;
	/* Update ipd_idx until at_ipd message is matched */
	if (desc->ipd_idx < at_ipd.len) {
		if (match_message(&at_ipd, &desc->ipd_idx, ch)) {
			if (desc->multiple_conections)
				desc->ipd_stat = RAEDING_CONN;
			else
				desc->ipd_stat = READING_LEN;
		}
		return false;
	}
	/* Update ipd_idx until ':' is found. Similar to match_messages but
	 * the message is not fix so we could use that function
	 */
	switch (desc->ipd_stat) {
	case RAEDING_CONN:
		if (ch < '0' || ch > '4')
			goto reset;
		desc->current_conn = ch - '0';
		desc->ipd_stat = WAITING_COMMA;
		desc->ipd_idx++;
		break;
	case WAITING_COMMA:
		if (ch != ',')
			goto reset;
		desc->ipd_stat = READING_LEN;
		desc->ipd_idx++;
		break;
	case READING_LEN:
		if (ch < '0' || ch > '9') {
			if (ch == ':' && desc->ipd_len > 0) {
				desc->conn[desc->current_conn].to_read =
					desc->ipd_len;
				desc->result.len -= desc->ipd_idx;
				ret = true;
			}
			goto reset;
		}
		desc->ipd_len = desc->ipd_len * 10 + (ch - '0');
		desc->ipd_idx++;
		break;
	default:
		goto reset;
	}

	return false;
reset:
	desc->ipd_idx = 0;
	desc->ipd_len = 0;
	desc->ipd_stat = NOT_MATCH;

	return ret;
}

static inline int32_t check_conn_id(struct at_desc *desc, struct at_buff *msg,
				    int32_t msg_idx)
{
	int32_t	id;
	int32_t	j;

	/* Get the connection number of the closed connection */
	desc->result.len -= (msg->len - 1);
	/* Update connection state */
	id = 0;
	if (desc->multiple_conections) {
		//Response: 2,CLOSED -> id = 0
		j = desc->result.len - 2;
		if (j < 0) {
			id = -1;
		} else {
			if (desc->result.buff[j] >= '0' &&
			    desc->result.buff[j] <= '3') {
				id = desc->result.buff[j] - '0';
				desc->result.len -= 2;
			} else {
				id = -1;
			}
		}
	}
	if (id >= 0 && id < 4) {//Maximum of 4 connections
		/* Close connection */
		desc->current_conn = id;
		desc->conn[id].active = false;
		desc->conn[id].cbuff = NULL;
		/* Notify that a connection was closed */
		desc->connection_callback(desc->callback_ctx,
					  AT_CLOSED_CONNECTION, id, NULL);
	} else {//Not id
		desc->async_idx[msg_idx] = 0;
		return false;
	}

	return true;
}

/* Check if an asynchronous messages was sent by the module and update desc */
static bool is_async_messages(struct at_desc *desc, uint8_t ch)
{
	const static struct at_buff async_msgs[NB_ASYNC_MESSAGES] = {
		{PUI8("CLOSED\r\n"), 8},
		{PUI8("WIFI DISCONNECT\r\n"), 17},
		{PUI8("WIFI GOT IP\r\n"), 13}
	};

	int32_t	i;

	for (i = 0; i < NB_ASYNC_MESSAGES; i++) {
		if (match_message(&async_msgs[i], &desc->async_idx[i], ch))
			break;
	}

	switch (i) {
	case 0: //Match CLOSED\r\n
		if (check_conn_id(desc, (struct at_buff *)&async_msgs[i], i))
			return false;
		break;
	case 1: //WIFI DISCONNECT\r\n
		desc->is_wifi_connected = false;
		break;
	case 2:
		desc->is_wifi_connected = true;
		break;
	default:
		return false;
	}

	/* Clear response indexes */
	memset(desc->async_idx, 0, sizeof(desc->async_idx));

	return true;
}

/* Mark the circular buffer transaction as ended */
static inline void end_conn_read(struct at_desc *desc)
{
	struct connection_desc	*conn;

	conn = &desc->conn[desc->current_conn];

	no_os_cb_end_async_write(conn->cbuff);
}

/* Start new read operation */
static inline void start_conn_read(struct at_desc *desc, bool is_new_message)
{
	struct connection_desc	*conn;
	uint8_t			*buff;
	uint32_t		available_len;
	int32_t			ret;

	conn = &desc->conn[desc->current_conn];

	if (is_new_message && !conn->active) {
		/*
		 * Notify that a new connection has started. Application needs
		 * to set a cbuff for the connection where data will be written.
		 */
		desc->connection_callback(desc->callback_ctx,
					  AT_NEW_CONNECTION,
					  desc->current_conn,
					  &conn->cbuff);
		if (conn->cbuff)
			conn->active = true;
		/*
		 * Else, a AT_STOP_CONNECTION command should be sent to the
		 * esp8266 module. (Application rejects the connection)
		 * This could be done only if implement at_run_cmd with
		 * no_os_uart_write_nonblocking
		 */
	}

	if (!conn->cbuff)
		/* There is no buffer set for this connection */
		goto dummy_read;

	/* Get buffer where data from uart can be written using DMA */
	ret = no_os_cb_prepare_async_write(conn->cbuff, conn->to_read,
					   (void **)&buff, &available_len);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto dummy_read;

	no_os_uart_read_nonblocking(desc->uart_desc, buff, available_len);
	conn->to_read -= available_len;

	return ;
dummy_read:
	/* Data from uart is discarded because an error occured or
	 * there is no buffer available
	 */
	no_os_uart_read_nonblocking(desc->uart_desc, &desc->read_ch, 1);
	conn->to_read -= 1;
}

/* Handle the uart read done */
static void at_callback_rd_done(struct at_desc *desc)
{
	static const struct at_buff ready_msg = {PUI8("ready\r\n"), 7};

	switch (desc->callback_operation) {
	case RESETTING_MODULE:
		if (match_message(&ready_msg, &desc->ready_idx,
				  desc->read_ch))
			desc->callback_operation = READING_RESPONSES;
		break;
	case WAITING_SEND:
	case READING_RESPONSES:
		if (is_payload_message(desc, desc->read_ch)) {
			/* New payload received */
			desc->callback_operation = READING_PAYLOAD;
			start_conn_read(desc, true);
			return ;
		}

		if (desc->read_ch == '>' && desc->callback_operation ==
		    WAITING_SEND) {
			desc->callback_operation = READING_RESPONSES;
		} else if (desc->result.len >= RESULT_BUFF_LEN) {
			desc->errors |=
				AT_ERROR_INTERNAL_BUFFER_OVERFLOW;
			desc->result.len = 0;
		} else if (!is_async_messages(desc, desc->read_ch))
			/* Add received character to result buffer */
			desc->result.buff[desc->result.len++] =
				desc->read_ch;
		break;
	case READING_PAYLOAD:
		/* Receiving payload from connection */
		end_conn_read(desc);
		if (!desc->conn[desc->current_conn].to_read) {
			desc->callback_operation = READING_RESPONSES;
			desc->current_conn = -1;
			break;
		} else {
			start_conn_read(desc, false);
			return ;
		}
		break;
	}

	/* Submit buffer to read the next char */
	no_os_uart_read_nonblocking(desc->uart_desc, &desc->read_ch, 1);
}

/* Handle the uart error */
static void at_callback_error(struct at_desc *desc)
{
	if (desc->callback_operation != RESETTING_MODULE)
		desc->errors |= AT_ERROR_UART;

	/* Submit buffer to read the next char */
	no_os_uart_read_nonblocking(desc->uart_desc, &desc->read_ch, 1);
}

/* Wait the response for the last command for MODULE_TIMEOUT milliseconds */
static int32_t wait_for_response(struct at_desc *desc)
{
	const static struct at_buff responses[NB_RESPONSE_MESSAGES] = {
		{PUI8("\r\nERROR\r\n"), 9},
		{PUI8("\r\nFAIL\r\n"), 8},
		{PUI8("\r\nOK\r\n"), 6},
		{PUI8("\r\nSEND OK\r\n"), 11}
	};
	uint32_t	timeout;
	uint32_t	result;
	uint32_t	i;
	uint32_t	j;

	i = 0;
	timeout = MODULE_TIMEOUT;
	result = -1;
	do {
		if (i < desc->result.len) {
			for (j = 0; j < NB_RESPONSE_MESSAGES; j++)
				if (match_message(&responses[j],
						  &desc->resp_idx[j],
						  desc->result.buff[i]))
					break;

			i++;
			switch (j) {
			case 0: // \r\nERROR\r\n
			case 1: // \r\nFAIL\r\n
				result = -1;
				goto end;
			case 2: // \r\nOK\r\n
			case 3: // \r\nSEND OK\r\n
				result = 0;
				goto end;
			default:
				break;
			}
		}
		no_os_mdelay(1);
	} while (--timeout);

end:
	if (timeout) //If a response arrived clean the result
		desc->result.len -= desc->resp_idx[j];

	memset(desc->resp_idx, 0, sizeof(desc->resp_idx));

	return result;
}

/* Send what is in desc->cmd over the UART and handle special case of AT_SEND */
static int32_t send_cmd(struct at_desc *desc, enum at_cmd cmd,
			union in_param *in_param)
{
	uint32_t timeout = MODULE_TIMEOUT;

	no_os_uart_write(desc->uart_desc, desc->cmd.buff, desc->cmd.len);
	if (cmd == AT_SEND) {
		desc->callback_operation = WAITING_SEND;
		/* Waiting for ok */
		if (0 != wait_for_response(desc))
			return -1;
		/* Wait until '>' is received */
		while (timeout--) {
			if (WAITING_SEND != desc->callback_operation)
				break;
			no_os_mdelay(1);
		}
		if (timeout == 0)
			return -1;
		/* Write payload */
		no_os_uart_write(desc->uart_desc, in_param->send_data.data.buff,
				 in_param->send_data.data.len);
	} else if (cmd == AT_DISCONNECT_NETWORK) {
		if (desc->is_wifi_connected) {
			/* Wait for WIFI_DISCONNECT */
			do {
				if (desc->is_wifi_connected == 0)
					break;
				no_os_mdelay(1);
			} while (timeout--);

			if (timeout == 0)
				return -1;

			return 0;
		}
	}

	/* Wait for OK, SEND OK or ERROR */
	return wait_for_response(desc);
}

/*
 * @brief Create formated string in dest according with fmt
 * The formated string is concatenated to dest.
 * The parameters are separated by , and the strings are rounded by '\"' and
 * escaped.
 * @param dest - Where the buffer is written.
 * @param fmt - Format: Null terminated string containing the type of the
 * arguments. d for int32_t or s for at_buff. Ex: "dds" meand: 2 integers and a
 * int32_t.
 */
static void set_params(struct at_buff *dest, uint8_t *fmt, ...)
{
	va_list		args;
	char		buff[12]; //A MAX int32_t fits
	struct at_buff	*str;
	uint32_t		i;
	int32_t		nb = 0;

	va_start (args, fmt);
	while (*fmt) {
		switch (*fmt) {
		case 'd':
			nb = va_arg(args, int32_t);
			itoa(nb, buff, 10);
			/* Concatenate buff to dest */
			memcpy(dest->buff + dest->len, buff, strlen(buff));
			dest->len += strlen(buff);
			break;
		case 's':
			str = va_arg(args, struct at_buff *);
			dest->buff[dest->len++] = '\"';
			/* Escape character */
			for (i = 0; i < str->len; i++) {
				uint8_t ch = str->buff[i];
				if (ch == '\"' || ch == ',' || ch == '\\')
					dest->buff[dest->len++] = '\\';
				dest->buff[dest->len++] = ch;
			}
			dest->buff[dest->len++] = '\"';
			break;
		default:
			break;
		}
		fmt++;
		if (*fmt)
			dest->buff[dest->len++] = ',';
	}

	va_end (args);
}

/* Concatenate command parameters to desc->cmd */
static void concat_cmd_param(struct at_desc *desc, enum at_cmd cmd,
			     union in_param *param)
{
	struct at_buff	aux[1];
	uint32_t	conn_id;

	switch (cmd) {
	case AT_DEEP_SLEEP:
		set_params(&desc->cmd, PUI8("d"),
			   param->deep_sleep_time_ms);
		break;
	case AT_SET_OPERATION_MODE:
		set_params(&desc->cmd, PUI8("d"), param->wifi_mode);
		break;

	case AT_CONNECT_NETWORK:
		set_params(&desc->cmd, PUI8("ss"),
			   &param->network.ssid,
			   &param->network.pwd);
		break;
	case AT_SET_ACCESS_POINT:
		set_params(&desc->cmd, PUI8("ssdd"),
			   &param->ap.ssid, &param->ap.pwd,
			   param->ap.ch_id, param->ap.encription);
		break;
	case AT_START_CONNECTION:
		if (param->connection.soket_type == SOCKET_TCP) {
			str_to_at(aux, PUI8("TCP"));
			if (desc->multiple_conections) {
				set_params(&desc->cmd, PUI8("dssd"),
					   (int32_t)param->connection.id,
					   aux,
					   &param->connection.addr,
					   (int32_t)param->connection.port);
			} else {
				set_params(&desc->cmd, PUI8("ssd"),
					   aux,
					   &param->connection.addr,
					   (int32_t)param->connection.port);
			}
		} else {
			str_to_at(aux, PUI8("UDP"));
			if (desc->multiple_conections) {
				set_params(
					&desc->cmd, PUI8("dssddd"),
					(int32_t)param->connection.id,
					aux,
					&param->connection.addr,
					(int32_t)param->connection.port,
					(int32_t)param->connection.local_port,
					(int32_t)param->connection.udp_mode);
			} else {
				set_params(
					&desc->cmd,
					PUI8("ssddd"),
					aux,
					&param->connection.addr,
					(int32_t)param->connection.port,
					(int32_t)param->connection.local_port,
					(int32_t)param->connection.udp_mode);
			}
		}
		break;
	case AT_SEND:
		conn_id = desc->multiple_conections ? param->connection.id : 0;
		if (desc->conn[conn_id].type == SOCKET_TCP) {
			if (desc->multiple_conections)
				set_params(&desc->cmd, PUI8("dd"),
					   (int32_t)param->send_data.id,
					   (int32_t)param->send_data.data.len);
			else
				set_params(&desc->cmd, PUI8("d"),
					   (int32_t)param->send_data.data.len);
		} else {
			if (desc->multiple_conections)
				set_params(
					&desc->cmd, PUI8("ddsd"),
					(int32_t)param->send_data.id,
					(int32_t)param->send_data.data.len,
					&param->send_data.remote_ip,
					(int32_t)param->send_data.remote_port);
			else
				set_params(
					&desc->cmd, PUI8("dsd"),
					(int32_t)param->send_data.data.len,
					&param->send_data.remote_ip,
					(int32_t)param->send_data.remote_port);
		}
		break;
	case AT_STOP_CONNECTION:
		set_params(&desc->cmd, PUI8("d"), param->conn_id);
		break;
	case AT_SET_CONNECTION_TYPE:
		set_params(&desc->cmd, PUI8("d"),
			   (int32_t)param->conn_type);
		break;
	case AT_SET_SERVER:
		if (param->server.action == CREATE_SERVER)
			set_params(&desc->cmd, PUI8("dd"),
				   (int32_t)param->server.action,
				   (int32_t)param->server.port);
		else
			set_params(&desc->cmd, PUI8("d"),
				   (int32_t)param->server.action);
		break;
	case AT_SET_TRANSPORT_MODE:
		set_params(&desc->cmd, PUI8("d"),
			   (int32_t)param->transport_mode);
		break;
	case AT_SET_CLIENT_TIMEOUT:
		set_params(&desc->cmd, PUI8("d"),
			   (int32_t)param->timeout);
		break;
	case AT_PING:
		set_params(&desc->cmd, PUI8("s"), &param->ping_ip);
		break;
	default:
		return;
	}
}

/*
 * Create the AT command with the specified parameters in desc->cmd
 * The format of the command will be: AT[CMD][OP]<parmas>\r\n
 */
static void build_cmd(struct at_desc *desc, enum at_cmd cmd,
		      enum cmd_operation op, union in_out_param *param)
{
	desc->cmd.len = 0;
	/* AT */
	desc->cmd.buff[desc->cmd.len++] = 'A';
	desc->cmd.buff[desc->cmd.len++] = 'T';
	/* CMD */
	memcpy(desc->cmd.buff + desc->cmd.len, g_map[cmd].cmd.buff,
	       g_map[cmd].cmd.len);
	desc->cmd.len += g_map[cmd].cmd.len;
	/* OP */
	if (op == AT_QUERY_OP)
		desc->cmd.buff[desc->cmd.len++] = '?';
	if (op == AT_TEST_OP) {
		desc->cmd.buff[desc->cmd.len++] = '=';
		desc->cmd.buff[desc->cmd.len++] = '?';
	}
	if (op == AT_SET_OP && param) {
		desc->cmd.buff[desc->cmd.len++] = '=';
		concat_cmd_param(desc, cmd, &param->in);
	}
	/* \r\n */
	desc->cmd.buff[desc->cmd.len++] = '\r';
	desc->cmd.buff[desc->cmd.len++] = '\n';
}

/* Send ATE0 command to stop echo */
static int32_t stop_echo(struct at_desc *desc)
{
	no_os_uart_write(desc->uart_desc, (uint8_t *)"ATE0\r\n", 6);

	if (0 != wait_for_response(desc))
		return -1;
	desc->result.len = 0;

	return 0;
}

/* Handle special cases */
static int32_t handle_special(struct at_desc *desc, enum at_cmd cmd)
{
	uint32_t timeout;

	switch (cmd) {
	case AT_RESET:
		desc->callback_operation = RESETTING_MODULE;
		no_os_uart_write(desc->uart_desc, desc->cmd.buff, desc->cmd.len);
		timeout = MODULE_TIMEOUT;
		do {
			/* Wait for "ready" message */
			if (desc->callback_operation != RESETTING_MODULE)
				break;
			no_os_mdelay(1);
		} while (--timeout);
		if (!timeout)
			return -1;

		desc->callback_operation = READING_PAYLOAD;
		desc->result.len = 0;
		if (0 != stop_echo(desc))
			return -1;
		at_run_cmd(desc, AT_DISCONNECT_NETWORK, AT_EXECUTE_OP, NULL);

		break;
	case AT_DEEP_SLEEP:
		/* TODO : Implement when needed if possible */
		return -1;
	default:
		return -1;
	}
	return 0;

}

/* TODO - Further development: Parse instead of coping */
static int32_t parse_result(struct at_desc *desc, enum at_cmd cmd,
			    union out_param *result)
{

	result->result.buff = desc->buffers.app_result_buff;
	memcpy(result->result.buff, desc->result.buff, desc->result.len);
	result->result.len = desc->result.len;
	desc->result.len = 0;

	return 0;
}

/**
 * @brief Execut an AT command
 * @param desc - AT parser reference
 * @param cmd - Command
 * @param op - Type of operation to be done for the specified command
 * @param param - Input and output param for the command. The data in the output
 * param will be available until a new call is made.
 * @return
 *  - 0 : On success
 *  - -1 : Otherwise
 */
int32_t at_run_cmd(struct at_desc *desc, enum at_cmd cmd, enum cmd_operation op,
		   union in_out_param *param)
{
	uint32_t	id;
	int32_t		ret;

	if (!desc || cmd == AT_SET_TRANSPORT_MODE)
		return -1;

	if (!(g_map[cmd].type & op))
		return -1;

	build_cmd(desc, cmd, op, param);

	if (cmd == AT_DEEP_SLEEP || cmd == AT_RESET)
		return handle_special(desc, cmd);

	ret = send_cmd(desc, cmd, &param->in);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	/* Update driver status according with commands */
	if (cmd == AT_SET_CONNECTION_TYPE)
		desc->multiple_conections = param->in.conn_type;
	if (cmd == AT_START_CONNECTION && op == AT_SET_OP) {
		id = desc->multiple_conections ? param->in.connection.id : 0;
		desc->conn[id].type = param->in.connection.soket_type;
	}

	/* Fill the output parameter */
	if (param) {
		if (op == AT_SET_OP)
			cmd = AT_GET_VERSION; //Only copy response
		ret = parse_result(desc, cmd, &param->out);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;
	} else
		/* Clear the result*/
		desc->result.len = 0;

	if (desc->errors) {
		ret = desc->errors;
		desc->errors = 0;
		return -ret;
	}

	return 0;
}

/**
 * @brief Initialize the AT parser
 * @param desc - Address where to store the AT parser reference used by the
 * driver functions.
 * @param param - Initializing data
 * @return
 *  - 0 : On success
 *  - -1 : Otherwise
 */
int32_t at_init(struct at_desc **desc, const struct at_init_param *param)
{
	struct at_desc		*ldesc;
	union in_out_param	result;
	struct no_os_callback_desc	callback_desc_rd;
	struct no_os_callback_desc	callback_desc_err;
	uint32_t		conn;
	uint8_t			*str;
	int			retry = 2, err;

	if (!desc || !param || !param->connection_callback)
		return -1;

	ldesc = no_os_calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return -1;

	ldesc->connection_callback = param->connection_callback;
	ldesc->callback_ctx = param->callback_ctx;

	ldesc->uart_desc = param->uart_desc;
	ldesc->irq_desc = param->irq_desc;
	ldesc->uart_irq_id = param->uart_irq_id;

	callback_desc_rd.ctx = ldesc;
	callback_desc_rd.event = NO_OS_EVT_UART_RX_COMPLETE;
	callback_desc_rd.handle = ldesc->uart_desc->extra;
	callback_desc_rd.peripheral = NO_OS_UART_IRQ;
	callback_desc_rd.callback = (void (*)(void *))at_callback_rd_done;
	if (0 != no_os_irq_register_callback(ldesc->irq_desc,
					     ldesc->uart_irq_id,
					     &callback_desc_rd))
		goto free_desc;
	callback_desc_err.ctx = ldesc;
	callback_desc_err.event = NO_OS_EVT_UART_ERROR;
	callback_desc_err.handle = ldesc->uart_desc->extra;
	callback_desc_err.peripheral = NO_OS_UART_IRQ;
	callback_desc_err.callback = (void (*)(void *))at_callback_error;
	if (0 != no_os_irq_register_callback(ldesc->irq_desc,
					     ldesc->uart_irq_id,
					     &callback_desc_err))
		goto free_desc;

	if (0 != no_os_irq_enable(ldesc->irq_desc, ldesc->uart_irq_id))
		goto free_irq;

	/* Link buffer structure with static buffers */
	ldesc->result.buff = ldesc->buffers.result_buff;
	ldesc->result.len = 0;
	ldesc->cmd.buff = ldesc->buffers.cmd_buff;
	ldesc->cmd.len = CMD_BUFF_LEN;

	ldesc->callback_operation = READING_RESPONSES;

	/* The read will be handled by the callback */
	no_os_uart_read_nonblocking(ldesc->uart_desc, &ldesc->read_ch, 1);

	/** Software reset */
	if (param->sw_reset_en)
		if (at_run_cmd(ldesc, AT_RESET, AT_EXECUTE_OP, NULL))
			goto free_irq;

	/* Disable echoing response (try 2 times)*/
	do {
		err = stop_echo(ldesc);
	} while (--retry && err);

	if (err)
		goto free_irq;

	/* Test AT */
	if (0 != at_run_cmd(ldesc, AT_ATTENTION, AT_EXECUTE_OP, NULL))
		goto free_irq;

	/* Get the connection type */
	if (0 == at_run_cmd(ldesc, AT_SET_CONNECTION_TYPE, AT_QUERY_OP,
			    &result)) {
		/* Convert to null terminated string in order to use sscanf */
		at_to_str(&str, &result.out.result);
		sscanf((char *)str, "+CIPMUX:%"PRIu32"\r\n", &conn);

		ldesc->cmd.len = 0;
		ldesc->multiple_conections = conn ? true : false;
	} else {
		goto free_irq;
	}

	*desc = ldesc;
	return 0;

free_irq:
	no_os_irq_unregister_callback(ldesc->irq_desc, ldesc->uart_irq_id, NULL);
free_desc:
	no_os_free(ldesc);
	*desc = NULL;
	return -1;
}

/**
 * @brief no_os_free resources allocated at \ref at_init
 * @param desc - AT parser reference
 * @return
 *  - 0 : On success
 *  - -1 : Otherwise
 */
int32_t at_remove(struct at_desc *desc)
{
	if (!desc)
		return -1;

	no_os_irq_unregister_callback(desc->irq_desc, desc->uart_irq_id, NULL);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Convert null terminated string to at_buff
 * @param dest - Destination buffer
 * @param src - Source buffer
 * @return
 *  - 0 : On success
 *  - -1 : Otherwise
 */
int32_t str_to_at(struct at_buff *dest, const uint8_t *src)
{
	if (!dest || !src)
		return -1;

	dest->buff = (uint8_t *)src;
	dest->len = strlen((char *)src);

	return 0;
}

/**
 * @brief Convert at_buff to null terminated string
 * @param dest - Destination buffer
 * @param src - Source buffer
 * @return
 *  - 0 : On success
 *  - -1 : Otherwise
 */
int32_t at_to_str(uint8_t **dest, const struct at_buff *src)
{
	if (!src || !dest)
		return -1;

	src->buff[src->len] = 0;
	*dest = src->buff;

	return 0;
}
