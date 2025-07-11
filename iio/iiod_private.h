/***************************************************************************//**
 *   @file   iiod_private.h
 *   @brief  Private header file of iiod
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef IIOD_PRIVATE_H
#define IIOD_PRIVATE_H

#include "iio.h"

#define IIOD_WR				0x1
#define IIOD_ENDL			0x2
#define IIOD_RD				0x4
#define IIOD_PARSER_MAX_BUF_SIZE	128

#define IIOD_STR(cmd) {(cmd), sizeof(cmd) - 1}

#define IIOD_CTX(desc, conn) {.instance = (desc)->app_instance,\
			      .conn = (conn)->conn,\
				  .binary = (conn)->protocol}

/* Used to store a string and its size */
struct iiod_str {
	char *str;
	uint32_t len;
};

/*
 * Commads are the ones documented int the link:
 * https://wiki.analog.com/resources/tools-software/linux-software/libiio_internals#the_network_backend_and_iio_daemon
 */
enum iiod_cmd {
	IIOD_CMD_HELP,
	IIOD_CMD_EXIT,
	IIOD_CMD_PRINT,
	IIOD_CMD_BINARY,
	IIOD_CMD_VERSION,
	IIOD_CMD_TIMEOUT,
	IIOD_CMD_OPEN,
	IIOD_CMD_CLOSE,
	IIOD_CMD_READ,
	IIOD_CMD_WRITE,
	IIOD_CMD_READBUF,
	IIOD_CMD_WRITEBUF,
	IIOD_CMD_GETTRIG,
	IIOD_CMD_SETTRIG,
	IIOD_CMD_SET
};

enum iiod_opcode {
	IIOD_OP_RESPONSE,
	IIOD_OP_PRINT,
	IIOD_OP_TIMEOUT,
	IIOD_OP_READ_ATTR,
	IIOD_OP_READ_DBG_ATTR,
	IIOD_OP_READ_BUF_ATTR,
	IIOD_OP_READ_CHN_ATTR,
	IIOD_OP_WRITE_ATTR,
	IIOD_OP_WRITE_DBG_ATTR,
	IIOD_OP_WRITE_BUF_ATTR,
	IIOD_OP_WRITE_CHN_ATTR,
	IIOD_OP_GETTRIG,
	IIOD_OP_SETTRIG,

	IIOD_OP_CREATE_BUFFER,
	IIOD_OP_FREE_BUFFER,
	IIOD_OP_ENABLE_BUFFER,
	IIOD_OP_DISABLE_BUFFER,

	IIOD_OP_CREATE_BLOCK,
	IIOD_OP_FREE_BLOCK,
	IIOD_OP_TRANSFER_BLOCK,
	IIOD_OP_ENQUEUE_BLOCK_CYCLIC,
	IIOD_OP_RETRY_DEQUEUE_BLOCK,

	IIOD_OP_CREATE_EVSTREAM,
	IIOD_OP_FREE_EVSTREAM,
	IIOD_OP_READ_EVENT,

	IIOD_NB_OPCODES,
};

enum iiod_protocol {
	IIOD_ASCII_COMMAND,
	IIOD_BINARY_COMMAND,
};

struct command_data_ascii {
	/* IIOD Command */
	enum iiod_cmd cmd;
	/* Device Name */
	char device[MAX_DEV_ID];
	/* Channel Name */
	char channel[MAX_CHN_ID];
	/* Attribute Name */
	char attr[MAX_ATTR_NAME];
	/* Trigger Name */
	char trigger[MAX_TRIG_ID];
};

struct command_data_binary {
	/* Client ID */
	uint16_t client_id;
	/* OP Code */
	enum iiod_opcode op_code;
	/* Code Value */
	 int32_t code; // TODO: Remove this variable. Parse the value and keep in different variables
	/* Device ID */
	uint16_t device;
	/* Channel ID */
	uint16_t channel;
	/* Attribute ID */
	uint16_t attr;
	/* Trigger ID */
	uint16_t trigger;
	/* Buffer ID */
	uint16_t buffer;
	uint16_t block_id[MAX_NUM_BLOCKS];
	uint32_t block_size[MAX_NUM_BLOCKS];
	uint32_t bytes_size[MAX_NUM_BLOCKS];
	uint8_t curr;
};

/*
 * Structure to be filled after a command is parsed.
 * Depending of cmd some fields are set or not
 */
struct comand_desc {
	uint32_t mask;
	uint32_t timeout;
	uint32_t sample_count;
	uint32_t bytes_count;
	uint32_t count;
	bool cyclic;
	void *command_data;
	enum iio_attr_type type;
};

/* Used to store buffer indexes for non blocking transfers */
struct iiod_buff {
	char *buf;
	uint32_t idx;
	uint32_t len;
};

struct iiod_command {
	uint16_t client_id;
	uint8_t op;
	uint8_t dev;
	int32_t code;
};

/* Result after executing a command. */
struct iiod_run_cmd_result {
	uint32_t val;
	/* If set. Val needs to be sent */
	bool write_val;
	/* If buf.len != 0 buf has to be sent */
	struct iiod_buff buf;
};

/* Internal structure to handle a connection state */
struct iiod_conn_priv {
	/* User instance of the connection to be sent in iiod_ctx */
	void *conn;
	/* Unset when can be used from the connection pool */
	bool used;

	/* Command data after parsed */
	struct comand_desc cmd_data;

	/* Result of an executed cmd */
	struct iiod_run_cmd_result res;
	/* IIOD States */
	enum {
		/* Reading line until \n */
		IIOD_READING_LINE,
		/* Execut cmd without I/O operations */
		IIOD_RUNNING_CMD,
		/* Write result of executed cmd */
		IIOD_WRITING_CMD_RESULT,
		IIOD_WRITING_BIN_RESPONSE,
		/* I/O operations for READBUF and WRITEBUF cmds */
		IIOD_RW_BUF,
		/* I/O operations for WRITE cmd */
		IIOD_READING_WRITE_DATA,
		/* Set when a operation is finalized */
		IIOD_LINE_DONE,
		/* Pushing  cyclic buffer until IIO device is closed  */
		IIOD_PUSH_CYCLIC_BUFFER,
	} state;

	/* Buffer to store received line */
	char parser_buf[IIOD_PARSER_MAX_BUF_SIZE];
	/* Index in parser_buf. For nonblocking operation */
	uint32_t parser_idx;
	/* Buffer to store raw data (attributes or buffer data).*/
	char *payload_buf;
	/* Length of payload_buf_len */
	uint32_t payload_buf_len;
	/* Used in nonbloking transfers to save indexes */
	struct iiod_buff nb_buf;

	/* Mask of current opened buffer */
	uint32_t mask;
	/* Buffer to store mask as a string */
	char buf_mask[10];
	/* Context for strtok_r function */
	char *strtok_ctx;
	/* True if the device was open with cyclic buffer flag */
	bool is_cyclic_buffer;
	/* IIOD Command Format */
	enum iiod_protocol protocol;
	struct iiod_command res_header;
	/* Event List */
	struct no_os_list_desc *event;
	/* Buffer to store the event data for transfer */
	uint8_t event_data[16];
};

/* Private iiod information */
struct iiod_desc {
	/* Pool of iiod connections */
	struct iiod_conn_priv conns[IIOD_MAX_CONNECTIONS];
	/* Application operations */
	struct iiod_ops ops;
	/* Application instance */
	void *app_instance;
	/* Address of xml */
	char *xml;
	/* XML length in bytes */
	uint32_t xml_len;
	/* Backend used by IIOD */
	enum physical_link_type phy_type;
	/* State Machine */
	int32_t (*run_state)(struct iiod_desc *,
			      struct iiod_conn_priv *);
};

struct iiod_event_desc {
	uint16_t client_id;
	uint16_t event_read_count;
	struct lf256fifo *event_data;
};

struct __attribute__((packed)) iiod_event_data {
	uint64_t channel_id: 16;
	uint64_t diff_channel_id: 16;
	uint64_t channel_type: 8;
	uint64_t modifier: 8;
	uint64_t event_dir: 7;
	uint64_t is_differential: 1;
	uint64_t event_type: 8;
	int64_t timestamp: 64;
};


#endif //IIOD_PRIVATE_H
