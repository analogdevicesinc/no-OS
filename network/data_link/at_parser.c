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

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "at_parser.h"
#include "error.h"
#include "circular_buffer.h"
#include "delay.h"
#include "uart.h"
#include "irq.h"
#include "util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* Should be sizeof(async_msgs)/sizeof(*async_msgs) */
#define NB_ASYNC_MESSAGES	1
/* Should be sizeof(responses)/sizeof(*responses) */
#define NB_RESPONSE_MESSAGES	3
/* Max command length: at+cwsap=max_ssid_32,max_pass_64,0,0 -> 110 characters */
#define CMD_BUFF_LEN		120u
/* Maybe this could be smaller. Here must one response at a time */
#define RESULT_BUFF_LEN		500u
/* Used to remove warnings on strings */
#define PUI8(X)			((uint8_t *)(X))
/* Timeout waiting for module response. (20 seconds) */
#define MODULE_TIMEOUT		20000
/* Delay needed when executing reset command */
#define RESET_DELAY		6000

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
	{{(uint8_t *)"", 0}, AT_EXECUTE_OP},
	{{(uint8_t *)"+RST", 4}, AT_EXECUTE_OP},
	{{(uint8_t *)"+GSLP", 5}, AT_SET_OP},
	{{(uint8_t *)"+GMR", 4}, AT_EXECUTE_OP},
	{{(uint8_t *)"+CWMODE", 7}, AT_QUERY_OP | AT_SET_OP | AT_TEST_OP},
	{{(uint8_t *)"+CWJAP", 6}, AT_QUERY_OP | AT_SET_OP},
	{{(uint8_t *)"+CWLAP", 6}, AT_EXECUTE_OP},
	{{(uint8_t *)"+CWQAP", 6}, AT_EXECUTE_OP},
	{{(uint8_t *)"+CWSAP", 6}, AT_QUERY_OP | AT_SET_OP},
	{{(uint8_t *)"+CWLIF", 6}, AT_EXECUTE_OP},
	{{(uint8_t *)"+CIPSTATUS", 10}, AT_EXECUTE_OP},
	{{(uint8_t *)"+CIPSTART", 9}, AT_TEST_OP | AT_SET_OP},
	{{(uint8_t *)"+CIPSEND", 8}, AT_SET_OP},
	{{(uint8_t *)"+CIPCLOSE", 9}, AT_EXECUTE_OP | AT_SET_OP},
	{{(uint8_t *)"+CIFSR", 6}, AT_EXECUTE_OP},
	{{(uint8_t *)"+CIPMUX", 7}, AT_QUERY_OP | AT_SET_OP},
	{{(uint8_t *)"+CIPSERVER", 10}, AT_SET_OP},
	{{(uint8_t *)"+CIPMODE", 8}, AT_QUERY_OP | AT_SET_OP},
	{{(uint8_t *)"+CIPSTO", 7}, AT_QUERY_OP | AT_SET_OP},
	{{(uint8_t *)"+PING", 5}, AT_SET_OP}
};

/* Structure storing a circular buffer for a connection */
struct at_circular_buff {
	/* Size of the buffer */
	uint32_t	size;
	/* Address to the buffer */
	uint8_t		*buff;
	/* Read index */
	uint32_t	ri;
	/* Write index */
	uint32_t	wi;
	/* Number of current elements in buffer */
	uint32_t	count;
	/* Last read len */
	uint32_t	last_len;
	/* Set if overflow */
	bool		is_overflow;
	/* Used for polling in blocking read */
	bool		new_data;
};

/* Structure storing a connection status */
struct connection_desc {
	/* Connection buffer */
	volatile struct at_circular_buff	cbuff;
	/* Pending bytes for reading */
	uint32_t				to_read;
	/* True if connection established */
	bool					active;
	/* Type of connection */
	enum socket_type			type;
};

/* Structure storing the status of the parser */
struct at_desc {
	/* - Uart related fields */
	/* Uart descriptor */
	struct uart_desc	*uart_desc;
	/* Irq descriptor */
	struct irq_ctrl_desc	*irq_desc;
	/* Uart irq id */
	uint32_t		uart_irq_id;

	//Connection related fields
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
		uint8_t	cmd_buff[CMD_BUFF_LEN];
	} 			buffers;
	/* Stores the output for the application */
	struct circular_buffer	*cb_response;
	/* Stores data received from the module */
	volatile struct at_buff	result;
	/* Buffer to build the command */
	struct at_buff		cmd;
	/* Buffer to read one char */
	uint8_t			read_ch;

	/* - Control fields */
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
	/* Indexes in the async response given by the driver */
	uint8_t			async_idx[NB_ASYNC_MESSAGES];
	/* Indexes in the response given by the driver */
	uint8_t			resp_idx[NB_RESPONSE_MESSAGES];
	/* Ipd idx */
	uint8_t			ipd_idx;
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

static void at_sprintf(struct at_buff *dest, uint8_t *fmt, ...);
static inline void at_sscanf(struct at_buff *src, uint8_t *fmt, uint32_t *p1,
			     uint32_t *p2);
static inline void at_strcpy(struct at_buff *dest, const struct at_buff *src);
static inline void at_strcat(struct at_buff *dest, const struct at_buff *src);
static inline void at_strcatch(struct at_buff *dest, uint8_t ch);
static inline void at_set(struct at_buff *dest, uint8_t *buff, uint32_t len);

/*
 * How this function works it represented by this pseoudocode
 * For each message and its index {
 * 	if (ch == message[index]) {
 * 		index++;
 * 		if (index == strlen(message))
 * 			return message_by_id
 * 		}
 * 	else
 * 		index = 0;
 *
 * 	return -1;
 * }
 */
static inline int32_t match_messages(const struct at_buff *messages,
				     uint8_t	*indexes,
				     uint8_t	nb_messages,
				     uint8_t	ch)
{
	int32_t		count;
	struct at_buff	*msg;
	uint8_t		*idx;

	for (count = 0; count < nb_messages; count++) {
		idx = &indexes[count];
		msg = (struct at_buff *)&messages[count];
		if (ch == msg->buff[*idx]) {
			/* Increment matching index if there is a match */
			(*idx)++;
			if (*idx == msg->len)
				/* Return the index of the match message */
				return count;
		} else if (ch == msg->buff[0]) {
			*idx = 1;
		} else {
			*idx = 0;
		}
	}

	return -1;
}

/* Check if a +IPD command have been received and updates the desc */
static bool is_payload_message(struct at_desc *desc, uint8_t ch)
{
	static const struct at_buff	at_ipd = {PUI8("\r\n+IPD,"), 7};
	/* max_ch_search = at_ipd.len + sizeof("0,1024") */
	static const uint32_t		max_ch_search = 7 + 6;
	struct	at_buff	aux;
	uint32_t	id;
	uint32_t	len;
	uint32_t	idx_in_result;

	/* Update ipd_idx until at_ipd message is matched */
	if (desc->ipd_idx < at_ipd.len) {
		match_messages(&at_ipd, &desc->ipd_idx, 1, ch);
		return false;
	}
	/* Update ipd_idx until ':' is found. Similar to match_messages but
	 * the message is not fix so we could use that function */
	if (ch == ':') {
		/* A well formated +IPD command arrived */
		id = 0;
		idx_in_result = desc->result.len - desc->ipd_idx;
		at_set(&aux, desc->result.buff + idx_in_result, desc->ipd_idx);
		if (desc->multiple_conections == 1)
			at_sscanf(&aux, PUI8("\r\n+IPD,%u,%u"), &id, &len);
		else
			at_sscanf(&aux, PUI8("\r\n+IPD,%u"), &len, NULL);
		desc->result.len -= desc->ipd_idx;
		desc->ipd_idx = 0;

		/* Update the descriptor */
		desc->current_conn = (int32_t)id;
		desc->conn[id].to_read = len;

		return true;
	} else if ((!(ch >= '0' && ch <= '9') && ch != ',') ||
		   desc->ipd_idx > max_ch_search) {
		desc->ipd_idx = 0;
	} else {
		desc->ipd_idx++;
	}

	return false;
}

/* Check if an asynchronous messages was sent by the module and update desc */
static bool is_async_messages(struct at_desc *desc, uint8_t ch)
{
	const static struct at_buff async_msgs[NB_ASYNC_MESSAGES] = {
		{PUI8("CLOSED\r\n"), 8}
	};//, //Not used
	//{USTR("WIFI DISCONNECT\r\n"), 8},
	//{USTR("WIFI CONNECTED\r\n"), 8},
	//{USTR("WIFI GOT IP\r\n"), 8},
	//{USTR("ready\r\n"), 8}};
	int32_t	match_idx;
	int32_t	id;
	int32_t	i;

	match_idx = match_messages(async_msgs, desc->async_idx,
				   NB_ASYNC_MESSAGES, ch);
	if (match_idx == 0) { //Match CLOSED\r\n
		/* One response match */
		desc->result.len -= (async_msgs[0].len - 1);
		/* Update connection state */
		id = 0;
		if (desc->multiple_conections) {
			//Response: 2,CLOSED -> id = 0
			i = desc->result.len - 2;
			if (i < 0) {
				id = -1;
			} else {
				if (desc->result.buff[i] >= '0' &&
				    desc->result.buff[i] <= '3') {
					id = desc->result.buff[i] - '0';
					desc->result.len -= 2;
				} else {
					id = -1;
				}
			}
		}
		if (id >= 0 && id < 4) {//Maximum of 4 connections
			desc->conn[id].active = false;
		} else {//Not id
			desc->async_idx[0] = 0;
			return false;
		}

		return true;
	}
	if (match_idx >= 0) /* Clear response indexes */
		memset(desc->async_idx, 0, sizeof(desc->async_idx));

	return false;
}

/* Update the buffer descriptor when a read transaction ended */
static inline void end_conn_read(volatile struct at_circular_buff *cbuff)
{
	cbuff->wi += cbuff->last_len;
	if (cbuff->wi == cbuff->size)
		cbuff->wi = 0; //Move wi at the begining of buff
	if (cbuff->count + cbuff->last_len > cbuff->size) {
		cbuff->is_overflow = true; //Set overflow
		cbuff->count = cbuff->size;
		cbuff->ri = cbuff->wi;
	} else {
		cbuff->count += cbuff->last_len;
	}
	cbuff->new_data = true;
}

/* Start new read operation. If the read buffer is nul, aux_buff will be used */
static inline void start_conn_read(struct connection_desc *conn,
				   struct uart_desc *uart_desc,
				   uint8_t *aux_buff)
{
	uint32_t				len;
	volatile struct at_circular_buff	*cbuff = &(conn->cbuff);

	if (!cbuff->buff || !cbuff->size) {
		/* Read new data in aux_buff */
		uart_read_nonblocking(uart_desc, aux_buff, 1);
		conn->to_read -= 1;
		return ;
	}

	len = min(cbuff->size - cbuff->wi, conn->to_read);
	uart_read_nonblocking(uart_desc, cbuff->buff + cbuff->wi, len);
	conn->to_read -= len;
	cbuff->last_len = len;
}

/* Handle the uart events */
static void at_callback(struct at_desc *desc, uint32_t event, uint8_t *data)
{
	struct connection_desc	*conn;

	switch (event) {
	case READ_DONE:
		switch (desc->callback_operation) {
		case RESETTING_MODULE:
			/* Do nothing */
			break;
		case WAITING_SEND:
		case READING_RESPONSES:
			if (is_payload_message(desc, desc->read_ch)) {
				desc->callback_operation = READING_PAYLOAD;
				conn = &desc->conn[desc->current_conn];
				start_conn_read(conn, desc->uart_desc,
						&desc->read_ch);
				return ;
			}

			if (desc->read_ch == '>' && desc->callback_operation ==
			    WAITING_SEND)
				desc->callback_operation = READING_RESPONSES;
			else if (desc->result.len >= RESULT_BUFF_LEN)
				/* Overflow */
				desc->result.len = 0;
			else if (!is_async_messages(desc, desc->read_ch))
				/* Add received character to result buffer */
				desc->result.buff[desc->result.len++] =
					desc->read_ch;
			break;
		case READING_PAYLOAD:
			/* Receiving payload from connection */
			conn = &desc->conn[desc->current_conn];
			end_conn_read(&conn->cbuff);
			if (!conn->to_read) {
				desc->callback_operation = READING_RESPONSES;
				desc->current_conn = -1;
				break;
			} else {
				start_conn_read(conn, desc->uart_desc,
						&desc->read_ch);
				return ;
			}
			break;
		}
		break;
	case WRITE_DONE:
		return ;
	case ERROR:
		/* TODO reflect errors to user */
		break;
	default:
		/* We never have to get here */
		break;
	}
	/* Submit buffer to read the next char */
	uart_read_nonblocking(desc->uart_desc, &desc->read_ch, 1);
}

/* Wait the response for the last command for MODULE_TIMEOUT milliseconds */
static int32_t wait_for_response(struct at_desc *desc)
{
	const static struct at_buff responses[NB_RESPONSE_MESSAGES] = {
		{PUI8("\r\nERROR\r\n"), 9},
		{PUI8("\r\nOK\r\n"), 6},
		{PUI8("\r\nSEND OK\r\n"), 11}
	};
	int32_t		match_idx;
	uint32_t	timeout;
	uint32_t	result;
	uint32_t	i;

	i = 0;
	timeout = MODULE_TIMEOUT;
	result = FAILURE;
	do {
		if (i < desc->result.len) {
			match_idx = match_messages(responses, desc->resp_idx,
						   NB_RESPONSE_MESSAGES,
						   desc->result.buff[i]);
			i++;
			switch (match_idx) {
			case 0: // \r\nERROR\r\n
				result = FAILURE;
				goto end;
			case 1: // \r\nOK\r\n
			case 2: // \r\nSEND OK\r\n
				result = SUCCESS;
				goto end;
			default:
				break;
			}
		}
		mdelay(1);
	} while (timeout--);

end:
	if (timeout) //If a response arrived clean the result
		desc->result.len -= desc->resp_idx[match_idx];

	memset(desc->resp_idx, 0, sizeof(desc->resp_idx));

	return result;
}

/* Wait until '>' is received */
static inline int32_t wait_for_send(struct at_desc *desc)
{




	return FAILURE;
}

/* Send what is in desc->cmd over the UART and handle special case of AT_SEND */
static int32_t send_cmd(struct at_desc *desc, enum at_cmd cmd,
			union in_param *in_param)
{
	uint32_t timeout = MODULE_TIMEOUT;

	uart_write(desc->uart_desc, desc->cmd.buff, desc->cmd.len);
	if (cmd == AT_SEND) {
		desc->callback_operation = WAITING_SEND;
		/* Waiting for ok */
		if (SUCCESS != wait_for_response(desc))
			return FAILURE;
		/* Wait until '>' is received */
		while (timeout--) {
			if (WAITING_SEND != desc->callback_operation)
				break;
			mdelay(1);
		}
		if (timeout == 0)
			return FAILURE;
		/* Write payload */
		uart_write(desc->uart_desc, in_param->send_data.data.buff,
			   in_param->send_data.data.len);
	}

	/* Wait for OK, SEND OK or ERROR */
	return wait_for_response(desc);
}

/* Concatenate command parameters to desc->cmd */
static void concat_cmd_param(struct at_desc *desc, enum at_cmd cmd,
			     union in_param *param)
{
	struct at_buff	aux[1];
	uint32_t	conn_id;

	switch (cmd) {
	case AT_DEEP_SLEEP:
		at_sprintf(&desc->cmd, PUI8("d"),
			   param->deep_sleep_time_ms);
		break;
	case AT_SET_OPERATION_MODE:
		at_sprintf(&desc->cmd, PUI8("d"), param->wifi_mode);
		break;

	case AT_CONNECT_NETWORK:
		at_sprintf(&desc->cmd, PUI8("ss"),
			   &param->network.ssid,
			   &param->network.pwd);
		break;
	case AT_SET_ACCESS_POINT:
		at_sprintf(&desc->cmd, PUI8("ssdd"),
			   &param->ap.ssid, &param->ap.pwd,
			   param->ap.ch_id, param->ap.encription);
		break;
	case AT_START_CONNECTION:
		if (param->connection.soket_type == SOCKET_TCP) {
			str_to_at(aux, PUI8("TCP"));
			if (desc->multiple_conections) {
				at_sprintf(&desc->cmd, PUI8("dssd"),
					   (int32_t)param->connection.id,
					   aux,
					   &param->connection.addr,
					   (int32_t)param->connection.port);
			} else {
				at_sprintf(&desc->cmd, PUI8("ssd"),
					   aux,
					   &param->connection.addr,
					   (int32_t)param->connection.port);
			}
		} else {
			str_to_at(aux, PUI8("UDP"));
			if (desc->multiple_conections) {
				at_sprintf(
					&desc->cmd, PUI8("dssddd"),
					(int32_t)param->connection.id,
					aux,
					&param->connection.addr,
					(int32_t)param->connection.port,
					(int32_t)param->connection.local_port,
					(int32_t)param->connection.udp_mode);
			} else {
				at_sprintf(
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
				at_sprintf(&desc->cmd, PUI8("dd"),
					   (int32_t)param->send_data.id,
					   (int32_t)param->send_data.data.len);
			else
				at_sprintf(&desc->cmd, PUI8("d"),
					   (int32_t)param->send_data.data.len);
		} else {
			if (desc->multiple_conections)
				at_sprintf(
					&desc->cmd, PUI8("ddsd"),
					(int32_t)param->send_data.id,
					(int32_t)param->send_data.data.len,
					&param->send_data.remote_ip,
					(int32_t)param->send_data.remote_port);
			else
				at_sprintf(
					&desc->cmd, PUI8("dsd"),
					(int32_t)param->send_data.data.len,
					&param->send_data.remote_ip,
					(int32_t)param->send_data.remote_port);
		}
		break;
	case AT_STOP_CONNECTION:
		at_sprintf(&desc->cmd, PUI8("d"), param->conn_id);
		break;
	case AT_SET_CONNECTION_TYPE:
		at_sprintf(&desc->cmd, PUI8("d"),
			   (int32_t)param->conn_type);
		break;
	case AT_SET_SERVER:
		at_sprintf(&desc->cmd, PUI8("dd"),
			   (int32_t)param->server.action,
			   (int32_t)param->server.port);
		break;
	case AT_SET_TRANSPORT_MODE:
		at_sprintf(&desc->cmd, PUI8("d"),
			   (int32_t)param->transport_mode);
		break;
	case AT_SET_CLIENT_TIMEOUT:
		at_sprintf(&desc->cmd, PUI8("d"),
			   (int32_t)param->timeout);
		break;
	case AT_PING:
		at_sprintf(&desc->cmd, PUI8("s"), &param->ping_ip);
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
	at_strcatch(&desc->cmd, 'A');
	at_strcatch(&desc->cmd, 'T');
	/* CMD */
	at_strcat(&desc->cmd, &g_map[cmd].cmd);
	/* OP */
	if (op == AT_QUERY_OP)
		at_strcatch(&desc->cmd, '?');
	if (op == AT_TEST_OP) {
		at_strcatch(&desc->cmd, '=');
		at_strcatch(&desc->cmd, '?');
	}
	if (op == AT_SET_OP && param) {
		at_strcatch(&desc->cmd, '=');
		concat_cmd_param(desc, cmd, &param->in);
	}
	/* \r\n */
	at_strcatch(&desc->cmd, '\r');
	at_strcatch(&desc->cmd, '\n');
}

/* Send ATE0 command to stop echo */
static int32_t stop_echo(struct at_desc *desc)
{
	uart_write(desc->uart_desc, (uint8_t *)"ATE0\r\n", 6);

	if (SUCCESS != wait_for_response(desc))
		return FAILURE;
	desc->result.len = 0;

	return SUCCESS;
}

/* Handle special cases */
static int32_t handle_special(struct at_desc *desc, enum at_cmd cmd)
{
	switch (cmd) {
	case AT_RESET:
		desc->callback_operation = RESETTING_MODULE;
		uart_write(desc->uart_desc, desc->cmd.buff, desc->cmd.len);
		mdelay(RESET_DELAY); //Maybe is better wait for ready
		desc->callback_operation = READING_PAYLOAD;
		desc->result.len = 0;
		if (SUCCESS != stop_echo(desc))
			return FAILURE;
		break;
	case AT_DEEP_SLEEP:
		/* TODO */
		return FAILURE;
	default:
		return FAILURE;
	}
	return SUCCESS;

}

/*
 * Copy the result buffer to desc->cb_response and copy a referens to result
 * TODO: Parse result to parameters
 */
static int32_t parse_result(struct at_desc *desc, enum at_cmd cmd,
			    union out_param *result)
{
	static struct at_buff	resp = {PUI8("WIFI DISCONNECT\r\n"), 17};
	uint32_t		timeout;
	int32_t			ret;

	ret = SUCCESS;
	/* Copy the result buffer into the result parameter */
	if (cmd == AT_DISCONNECT_NETWORK) {
		//Wait for WIFI_DISCONNECT
		timeout = 10;
		do {
			if (resp.len >= desc->result.len)
				if (0 == memcmp(resp.buff, desc->result.buff,
						resp.len))
					break;
			mdelay(100);
		} while (timeout--);
		if (timeout == 0) {
			ret = FAILURE;
			goto end;
		}
	}

	/* TODO - Parse instead of coping */
	if (desc->result.len > 0)
		if (SUCCESS != cb_write(desc->cb_response, desc->result.buff,
					desc->result.len))
			ret = FAILURE;

end:
	desc->result.len = 0;
	result->result = desc->cb_response;
	return ret;
}

/**
 * @brief Execut an AT command
 * @param desc - AT parser reference
 * @param cmd - Command
 * @param op - Type of operation to be done for the specified command
 * @param param - Input and output param for the command. The data in the output
 * param will be available until a new call is made.
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t at_run_cmd(struct at_desc *desc, enum at_cmd cmd, enum cmd_operation op,
		   union in_out_param *param)
{
	uint32_t	id;

	if (!desc || cmd == AT_SET_TRANSPORT_MODE)
		return FAILURE;

	if (!(g_map[cmd].type & op))
		return FAILURE;

	build_cmd(desc, cmd, op, param);

	if (cmd == AT_DEEP_SLEEP || cmd == AT_RESET)
		return handle_special(desc, cmd);

	if (SUCCESS != send_cmd(desc, cmd, &param->in))
		return FAILURE;

	/* Update driver status according with commands */
	if (cmd == AT_SET_CONNECTION_TYPE)
		desc->multiple_conections = param->in.conn_type;
	if (cmd == AT_START_CONNECTION && op == AT_SET_OP) {
		id = desc->multiple_conections ? param->in.connection.id : 0;
		desc->conn[id].active = true;
		desc->conn[id].type = param->in.connection.soket_type;
	}

	/* Fill the output parameter */
	if (param) {
		if (op == AT_SET_OP)
			cmd = AT_GET_VERSION; //Only copy response
		if (SUCCESS != parse_result(desc, cmd, &param->out))
			return FAILURE;
	} else
		/* Clear the result*/
		desc->result.len = 0;

	return SUCCESS;
}

/**
 * @brief Submit a buffer used to store the data received from a connection.
 *
 * The application mustn't use this buffer and it should read from it using
 * \ref at_read_buffer .\n
 * The buffer must be submitted before a \ref AT_START_CONNECTION is done,
 * otherwise data received from that connection will be lost.\n
 * The buffer is associated with a connection id so the same will be use even
 * if an other connection is started on the same id.
 * @param desc - AT parser reference
 * @param conn_id - Connection id for which the buffer will be submitted
 * @param buff - Buffer to be submitted. NULL to unlink the buffer.
 * @param size - Size of the submitted buffer
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t at_submit_buffer(struct at_desc *desc, uint32_t conn_id,
			 uint8_t *buff, uint32_t size)
{
	volatile struct at_circular_buff *cbuff;

	if (!desc || conn_id >= MAX_CONNECTIONS)
		return FAILURE;

	cbuff = &desc->conn[conn_id].cbuff;
	cbuff->buff = buff;
	cbuff->size = size;
	cbuff->ri = 0;
	cbuff->wi = 0;
	cbuff->is_overflow = false;

	return SUCCESS;
}

/* Read blocking from cb to buff, to_read bytes */
static inline void read_cb(volatile struct at_circular_buff *cb, uint8_t *buff,
			   uint32_t to_read)
{
	uint32_t	len;
	uint32_t	buff_idx;
	bool		wait_new_data;

	if (!to_read)
		return ;

	buff_idx = 0;
	wait_new_data = false;
	do {
		if (wait_new_data) {
			/* This can be done with timeout */
			while (!cb->new_data)
				;
			cb->new_data = false;
			wait_new_data = false;
		}
		len = min(cb->size - cb->ri, to_read);
		if (len > cb->count) {
			wait_new_data = true;
			len = cb->count;
		}
		if (len) {
			memcpy(buff + buff_idx, cb->buff + cb->ri, len);
			if (cb->ri + len == cb->size)
				cb->ri = 0;
			else
				cb->ri += len;
			cb->count -= len;
			buff_idx += len;
		}
	} while (buff_idx < to_read);
}

/**
 * @brief Read data received from a connection
 * @param desc - AT parser reference
 * @param conn_id - Connection id
 * @param buff - Buffer where to read the data
 * @param to_read - Maximum bytes to read.
 * @param actual_read - Number of bytes that have been received at the moment of
 * the call. If NULL, the call will block until to_read bytes are received.
 * @param is_overflow - Will be set if an overflow has occurred
 * @return
 *  - \ref SUCCESS : On success, even if overflow occured
 *  - \ref FAILURE : Otherwise
 */
int32_t at_read_buffer(struct at_desc *desc, uint32_t conn_id,
		       uint8_t *buff, uint32_t to_read,
		       uint32_t *actual_read, bool *is_overflow)
{
	volatile struct at_circular_buff	*cbuff;
	uint32_t				i;

	if (!desc || conn_id >= MAX_CONNECTIONS || !to_read)
		return FAILURE;

	cbuff = &desc->conn[conn_id].cbuff;
	if (is_overflow)
		*is_overflow = cbuff->is_overflow;
	cbuff->is_overflow = false;

	if (!actual_read) {
		i = to_read;
	} else {
		i = min(to_read, cbuff->count);
		*actual_read = i;
	}

	/* Blocking read */
	read_cb(cbuff, buff, i);

	return SUCCESS;
}

/**
 * @brief Initialize the AT parser
 * @param desc - Address where to store the AT parser reference used by the
 * driver functions.
 * @param param - Initializing data
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t at_init(struct at_desc **desc, const struct at_init_param *param)
{
	struct at_desc		*ldesc;
	union in_out_param	result;
	struct callback_desc	callback_desc;
	uint32_t		conn;

	if (!desc || !param)
		return FAILURE;

	ldesc = calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return FAILURE;

	ldesc->uart_desc = param->uart_desc;
	ldesc->irq_desc = param->irq_desc;
	ldesc->uart_irq_id = param->uart_irq_id;
	callback_desc.callback =
		(void (*)(void*, uint32_t, void*))at_callback;
	callback_desc.ctx = ldesc;
	callback_desc.config = param->uart_irq_conf;
	if (SUCCESS != irq_register_callback(ldesc->irq_desc,
					     ldesc->uart_irq_id,
					     &callback_desc))
		goto free_desc;

	if (SUCCESS != irq_enable(ldesc->irq_desc, ldesc->uart_irq_id))
		goto free_irq;

	/* The read will be handled by the callback */
	uart_read_nonblocking(ldesc->uart_desc, &ldesc->read_ch, 1);

	/* Link buffer structure with static buffers */
	ldesc->result.buff = ldesc->buffers.result_buff;
	ldesc->result.len = 0;
	ldesc->cmd.buff = ldesc->buffers.cmd_buff;
	ldesc->cmd.len = CMD_BUFF_LEN;

	if (SUCCESS != cb_init(&ldesc->cb_response, RESULT_BUFF_LEN,
			       sizeof(uint8_t)))
		goto free_irq;

	ldesc->callback_operation = READING_RESPONSES;

	/* Disable echoing response */
	if (SUCCESS != stop_echo(ldesc))
		goto free_cb;

	/* Test AT */
	if (SUCCESS != at_run_cmd(ldesc, AT_ATTENTION, AT_EXECUTE_OP, NULL))
		goto free_cb;

	/* Get the connection type */
	if (SUCCESS == at_run_cmd(ldesc, AT_SET_CONNECTION_TYPE, AT_QUERY_OP,
				  &result)) {
		/* ldesc->cmd used as temporary buffer */
		cb_size(result.out.result, &ldesc->cmd.len);
		cb_read(result.out.result, ldesc->cmd.buff, ldesc->cmd.len);
		at_sscanf(&ldesc->cmd, PUI8("+CIPMUX:%u\r\n"), &conn, NULL);
		ldesc->cmd.len = 0;
		ldesc->multiple_conections = conn ? true : false;
	} else {
		goto free_cb;
	}

	*desc = ldesc;
	return SUCCESS;

free_cb:
	cb_remove(ldesc->cb_response);
free_irq:
	irq_unregister(ldesc->irq_desc, ldesc->uart_irq_id);
free_desc:
	free(ldesc);
	*desc = NULL;
	return FAILURE;
}

/**
 * @brief Free resources allocated at \ref at_init
 * @param desc - AT parser reference
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t at_remove(struct at_desc *desc)
{
	if (!desc)
		return FAILURE;
	cb_remove(desc->cb_response);
	irq_unregister(desc->irq_desc, desc->uart_irq_id);
	free(desc);

	return SUCCESS;
}

/**
 * @brief Convert null terminated string to at_buff
 * @param dest - Destination buffer
 * @param src - Source buffer
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t str_to_at(struct at_buff *dest, const uint8_t *src)
{
	if (!dest || !src)
		return FAILURE;

	dest->buff = (uint8_t *)src;
	dest->len = strlen((char *)src);

	return SUCCESS;
}

/**
 * @brief Convert at_buff to null terminated string
 * @param dest - Destination buffer
 * @param src - Source buffer
 * @return
 *  - \ref SUCCESS : On success
 *  - \ref FAILURE : Otherwise
 */
int32_t at_to_str(uint8_t **dest, const struct at_buff *src)
{
	if (!src || !dest)
		return FAILURE;

	src->buff[src->len] = 0;
	*dest = src->buff;

	return SUCCESS;
}

/* copy src into dest */
static inline void at_strcpy(struct at_buff *dest, const struct at_buff *src)
{
	memcpy(dest->buff, src->buff, src->len);
	dest->len = src->len;
}

/* Set at_buff structure parameters */
static inline void at_set(struct at_buff *dest, uint8_t *buff, uint32_t len)
{
	dest->buff = buff;
	dest->len = len;
}

/* Concatenate src after dest */
static inline void at_strcat(struct at_buff *dest, const struct at_buff *src)
{
	memcpy(dest->buff + dest->len, src->buff, src->len);
	dest->len += src->len;
}

/* Concatenate ch after dest */
static inline void at_strcatch(struct at_buff *dest, uint8_t ch)
{
	dest->buff[dest->len] = ch;
	dest->len++;
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
static void at_sprintf(struct at_buff *dest, uint8_t *fmt, ...)
{
	va_list		args;
	uint8_t		buff[12]; //A MAX int32_t fits
	struct at_buff	aux_str[1];
	struct at_buff	*str;
	uint32_t		i;
	int32_t		nb = 0;

	va_start (args, fmt);
	while (*fmt) {
		switch (*fmt) {
		case 'd':
			nb = va_arg(args, int32_t);
			itoa(nb, (char *)buff, 10);
			str_to_at(aux_str, buff);
			at_strcat(dest, aux_str);
			break;
		case 's':
			str = va_arg(args, struct at_buff *);
			at_strcatch(dest, '\"');
			/* Escape character */
			for (i = 0; i < str->len; i++) {
				uint8_t ch = str->buff[i];
				if (ch == '\"' || ch == ',' || ch == '\\')
					at_strcatch(dest, '\\');
				at_strcatch(dest, ch);
			}
			at_strcatch(dest, '\"');
			break;
		default:
			break;
		}
		fmt++;
		if (*fmt)
			at_strcatch(dest, ',');
	}

	va_end (args);
}


/* Wrap the sscanf function for 1 or 2 integer parameters using at_buff */
static inline void at_sscanf(struct at_buff *src, uint8_t *fmt, uint32_t *p1,
			     uint32_t *p2)
{
	uint8_t		ch;

	ch = src->buff[src->len];
	src->buff[src->len] = 0;
	if (p2)
		sscanf((char *)src->buff, (char *)fmt, p1, p2);
	else
		sscanf((char *)src->buff, (char *)fmt, p1);
	src->buff[src->len] = ch;
}

