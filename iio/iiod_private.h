/***************************************************************************//**
 *   @file   iiod_private.h
 *   @brief  Private header file of iiod
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

#ifndef IIOD_PRIVATE_H
#define IIOD_PRIVATE_H

#include "iio.h"

#define IIOD_WR				0x1
#define IIOD_ENDL			0x2
#define IIOD_RD				0x4
#define IIOD_PARSER_MAX_BUF_SIZE	128

#define IIOD_STR(cmd) {(cmd), sizeof(cmd) - 1}

#define IIOD_CTX(desc, conn) {.instance = (desc)->app_instance,\
			      .conn = (conn)->conn}


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

/*
 * Structure to be filled after a command is parsed.
 * Depending of cmd some fields are set or not
 */
struct comand_desc {
	enum iiod_cmd cmd;
	uint32_t mask;
	uint32_t timeout;
	uint32_t sample_count;
	uint32_t bytes_count;
	uint32_t count;
	bool cyclic;
	char device[MAX_DEV_ID];
	char channel[MAX_CHN_ID];
	char attr[MAX_ATTR_NAME];
	char trigger[MAX_TRIG_ID];
	enum iio_attr_type type;
};

/* Used to store buffer indexes for non blocking transfers */
struct iiod_buff {
	char *buf;
	uint32_t idx;
	uint32_t len;
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
};

#endif //IIOD_PRIVATE_H
