/***************************************************************************//**
 *   @file   iiod.c
 *   @brief  Nonblocking implementation of iiod.
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

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "no_os_lf256fifo.h"
#include "no_os_list.h"
#include "no_os_alloc.h"

#include "iiod.h"
#include "iiod_private.h"

#include "no_os_error.h"
#include "no_os_util.h"

#define SET_DUMMY_IF_NULL(func, dummy) ((func) ? (func) : (dummy))

#define IIOD_SET_RESPONSE(res, res_client, res_dev, res_code) { \
	res.client_id = res_client; \
	res.op = 0; \
	res.dev = res_dev; \
	res.code = res_code; }
#define IIOD_SET_DEFAULT_RESPONSE(res)  	IIOD_SET_RESPONSE(res, 0, 0, 0)
#define IIOD_SET_ERROR_RESPONSE(res, ret) 	IIOD_SET_RESPONSE(res, 0, 0, ret)

static char delim[] = " \r\n";

static const char *attr_types_strs[] = {
	[IIO_ATTR_TYPE_DEBUG] = "DEBUG",
	[IIO_ATTR_TYPE_BUFFER] = "BUFFER",
	[IIO_ATTR_TYPE_CH_OUT] = "OUTPUT",
	[IIO_ATTR_TYPE_CH_IN] = "INPUT"
};

static struct iiod_str cmds[] = {
	[IIOD_CMD_HELP]		= IIOD_STR("HELP"), //I'm here
	[IIOD_CMD_EXIT]		= IIOD_STR("EXIT"),
	[IIOD_CMD_PRINT]	= IIOD_STR("PRINT"),
	[IIOD_CMD_BINARY]   = IIOD_STR("BINARY"),
	[IIOD_CMD_VERSION]	= IIOD_STR("VERSION"),
	[IIOD_CMD_TIMEOUT]	= IIOD_STR("TIMEOUT"),
	[IIOD_CMD_OPEN]		= IIOD_STR("OPEN"),
	[IIOD_CMD_CLOSE]	= IIOD_STR("CLOSE"),
	[IIOD_CMD_READ]		= IIOD_STR("READ"),
	[IIOD_CMD_WRITE]	= IIOD_STR("WRITE"),
	[IIOD_CMD_READBUF]	= IIOD_STR("READBUF"),
	[IIOD_CMD_WRITEBUF]	= IIOD_STR("WRITEBUF"),
	[IIOD_CMD_GETTRIG]	= IIOD_STR("GETTRIG"),
	[IIOD_CMD_SETTRIG]	= IIOD_STR("SETTRIG"),
	[IIOD_CMD_SET]		= IIOD_STR("SET")
};
static const uint32_t priority_array[] = {
	/* Order not tested, just personal expectation. Function can
	 * be improved, this improvement is chosen for simplicity */
	IIOD_CMD_READBUF,
	IIOD_CMD_WRITEBUF,
	IIOD_CMD_READ,
	IIOD_CMD_WRITE,
	IIOD_CMD_OPEN,
	IIOD_CMD_CLOSE,
	IIOD_CMD_BINARY,
	IIOD_CMD_PRINT,
	IIOD_CMD_EXIT,
	IIOD_CMD_TIMEOUT,
	IIOD_CMD_VERSION,
	IIOD_CMD_GETTRIG,
	IIOD_CMD_SETTRIG,
	IIOD_CMD_HELP,
	IIOD_CMD_SET
};

static_assert(NO_OS_ARRAY_SIZE(cmds) == NO_OS_ARRAY_SIZE(priority_array),
	      "Arrays must have the same size");

static int32_t iio_event_cmp(void *data1, void *data2)
{
	return ((struct iiod_event_client *)data1)->client_id -
	       ((struct iiod_event_client *)data2)->client_id;
}

/* Set res->cmd to corresponding cmd and return the processed length of buf */
static int32_t parse_cmd(const char *token, struct comand_desc *res)
{
	uint32_t i;
	struct iiod_str *cmd;

	if (!token)
		return -EINVAL;

	for (i = 0; i < NO_OS_ARRAY_SIZE(cmds); ++i) {
		cmd = &cmds[priority_array[i]];
		if (strcmp(token, cmd->str) == 0) {
			((struct command_data_ascii *)res->command_data)->cmd = priority_array[i];

			return 0;
		}
	}

	return -EINVAL;
}

static int32_t parse_num(const char *token, uint32_t *res, uint32_t base)
{
	char *end_ptr;

	*res = strtoul(token, &end_ptr, base);
	if (*res == 0 && *end_ptr != '\0')
		return -EINVAL;

	return 0;
}

static int32_t iiod_parse_open(const char *token, struct comand_desc *res,
			       char **ctx)
{
	int32_t ret;

	if (!token)
		return -EINVAL;

	ret = parse_num(token, &res->sample_count, 10);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	token = strtok_r(NULL, delim, ctx);
	if (!token)
		return -EINVAL;

	ret = parse_num(token, &res->mask, 16);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	res->cyclic = 0;
	token = strtok_r(NULL, delim, ctx);
	if (token) {
		if (strcmp(token, "CYCLIC") == 0)
			res->cyclic = 1;
		else
			return -EINVAL;
	}

	return 0;
}

static int32_t iiod_parse_set(const char *token, struct comand_desc *res,
			      char **ctx)
{
	if (!token)
		return -EINVAL;

	if (strcmp(token, "BUFFERS_COUNT"))
		return -EINVAL;

	token = strtok_r(NULL, delim, ctx);
	if (!token)
		return -EINVAL;

	return parse_num(token, &res->count, 10);
}

static int32_t iiod_parse_rw_attr(const char *token, struct comand_desc *res,
				  char **ctx)
{
	int32_t i;
	struct command_data_ascii *data = (struct command_data_ascii *)res->command_data;

	res->type = IIO_ATTR_TYPE_DEVICE;
	if (token) {
		for (i = 0; i < IIO_ATTR_TYPE_DEVICE; ++i) {
			if (strcmp(token, attr_types_strs[i]) == 0) {
				token = strtok_r(NULL, delim, ctx);
				res->type = i;
				break;
			}
		}
	}

	if (res->type == IIO_ATTR_TYPE_CH_IN ||
	    res->type == IIO_ATTR_TYPE_CH_OUT) {
		if (!token)
			return -EINVAL;
		strncpy(data->channel, token, sizeof(data->channel));
		token = strtok_r(NULL, delim, ctx);
	}

	if (data->cmd == IIOD_CMD_WRITE) {
		if (!token)
			return -EINVAL;

		if (*token >= '0' && *token <= '9') {
			memset(data->attr, 0, sizeof(data->attr));

			return parse_num(token, &res->bytes_count, 10);
		}

		strncpy(data->attr, token, sizeof(data->attr));
		token = strtok_r(NULL, delim, ctx);
		if (!token)
			return -EINVAL;

		return parse_num(token, &res->bytes_count, 10);
	}

	if (token)
		strncpy(data->attr, token, sizeof(data->attr));
	else
		memset(data->attr, 0, sizeof(data->attr));

	return 0;
}

int32_t iiod_parse_line(char *buf, struct comand_desc *res, char **ctx)
{
	int32_t ret;
	char *token;
	struct command_data_ascii *data = (struct command_data_ascii *)res->command_data;

	token = strtok_r(buf, delim, ctx);
	ret = parse_cmd(token, res);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	token = strtok_r(NULL, delim, ctx);
	/* Commands without device */
	switch (data->cmd) {
	case IIOD_CMD_HELP:
	case IIOD_CMD_EXIT:
	case IIOD_CMD_PRINT:
	case IIOD_CMD_VERSION:
	case IIOD_CMD_BINARY:
		return 0;
	case IIOD_CMD_TIMEOUT:
		return parse_num(token, &res->timeout, 10);
	default:
		break;
	}

	strncpy(data->device, token, sizeof(data->device));
	token = strtok_r(NULL, delim, ctx);
	switch (data->cmd) {
	case IIOD_CMD_CLOSE:
	case IIOD_CMD_GETTRIG:
		return 0;
	case IIOD_CMD_OPEN:
		return iiod_parse_open(token, res, ctx);
	case IIOD_CMD_READ:
	case IIOD_CMD_WRITE:
		return iiod_parse_rw_attr(token, res, ctx);
	case IIOD_CMD_READBUF:
	case IIOD_CMD_WRITEBUF:
		return parse_num(token, &res->bytes_count, 10);
	case IIOD_CMD_SETTRIG:
		if (token)
			strncpy(data->trigger, token, sizeof(data->trigger));
		else
			memset(data->trigger, 0, sizeof(data->trigger));

		return 0;
	case IIOD_CMD_SET:
		return iiod_parse_set(token, res, ctx);
	default:
		break;
	}

	return -EINVAL;
}

static uint8_t curr, curr_1;
int32_t iiod_parse_command(char *buf, struct comand_desc *res)
{
	struct iiod_command *cmd = (struct iiod_command *)buf;
	uint8_t *payload = (uint8_t *) buf + sizeof(*cmd);
	struct command_data_binary *data = (struct command_data_binary *)res->command_data;

	data->op_code = cmd->op;
	data->client_id = cmd->client_id;
	data->device = (uint16_t) cmd->dev;
	
	/* Commands without payload */
	switch (data->op_code) {
		case IIOD_OP_PRINT:
		case IIOD_OP_ENABLE_BUFFER:
			return 0;
		case IIOD_OP_TIMEOUT:
			res->timeout = cmd->code;
			return 0;
		case IIOD_OP_CREATE_EVSTREAM:
		case IIOD_OP_FREE_EVSTREAM:
		case IIOD_OP_READ_EVENT:
			return 0;
		case IIOD_OP_READ_ATTR:
			res->type = IIO_ATTR_TYPE_DEVICE;
			data->attr = (uint16_t) (cmd->code >> 16);
			return 0;
		case IIOD_OP_READ_DBG_ATTR:
			res->type = IIO_ATTR_TYPE_DEBUG;
			data->attr = (uint16_t) (cmd->code >> 16);
			return 0;
		case IIOD_OP_READ_BUF_ATTR:
			res->type = IIO_ATTR_TYPE_BUFFER;
			data->attr = (uint16_t) (cmd->code >> 16);
			data->buffer = (uint16_t) cmd->code;
			return 0;
		case IIOD_OP_READ_CHN_ATTR:
			res->type = IIO_ATTR_TYPE_CH_OUT;
			data->attr = (uint16_t) (cmd->code >> 16);
			data->channel = (uint16_t) cmd->code;
			return 0;
		case IIOD_OP_GETTRIG:
		case IIOD_OP_SETTRIG:
			return 0; // TODO: Check what to do here
	default:
		break;
	}
	/* Commands with payload */
	switch (data->op_code) {
		case IIOD_OP_WRITE_ATTR:
			res->type = IIO_ATTR_TYPE_DEVICE;
			data->attr = (uint16_t) (cmd->code >> 16);
			// TODO: iiod_client sends a 8byte length. Same for all write attribute Op.
			res->bytes_count = *(uint32_t *)payload;
			return 0;
		case IIOD_OP_WRITE_DBG_ATTR:
			res->type = IIO_ATTR_TYPE_DEBUG;
			data->attr = (uint16_t) (cmd->code >> 16);
			res->bytes_count = *(uint32_t *)payload;
			return 0;
		case IIOD_OP_WRITE_BUF_ATTR:
			res->type = IIO_ATTR_TYPE_BUFFER;
			data->attr = (uint16_t) (cmd->code >> 16);
			data->buffer = (uint16_t) cmd->code;
			res->bytes_count = *(uint32_t *)payload;
			return 0;
		case IIOD_OP_WRITE_CHN_ATTR:
			res->type = IIO_ATTR_TYPE_CH_OUT;
			data->attr = (uint16_t) (cmd->code >> 16);
			data->channel = (uint16_t) cmd->code;
			res->bytes_count = *(uint32_t *)payload;
			return 0;
		case IIOD_OP_CREATE_BUFFER:
			res->mask = *(uint32_t *)payload;
			return 0;
		case IIOD_OP_DISABLE_BUFFER:
		case IIOD_OP_CREATE_BLOCK:
			// data->block_id[curr] = (int16_t)(cmd->code >> 16);
			data->block_size[curr] = *(uint32_t *)payload; //TODO: Is this correct. We read 8 bytes payload but only use 4?
			return 0;
		case IIOD_OP_FREE_BLOCK:
		case IIOD_OP_FREE_BUFFER:
		case IIOD_OP_TRANSFER_BLOCK:
			data->block_id[curr_1] = (int16_t)(cmd->code >> 16);
			data->code = cmd->code;
//			res->bytes_size[curr_1] = *(uint32_t *)payload;
			//TODO: need to see what to do here
			return 0;
		case IIOD_OP_ENQUEUE_BLOCK_CYCLIC:
		case IIOD_OP_RETRY_DEQUEUE_BLOCK:
		default:
			break;
	}
	
	return -EINVAL;
}

static int dummy_open(struct iiod_ctx *ctx, const void *device,
		      uint32_t samples, uint32_t mask, bool cyclic)
{
	return -EINVAL;
}

static int dummy_close(struct iiod_ctx *ctx, const void *device)
{
	return -EINVAL;
}

static int dummy_rw_attr(struct iiod_ctx *ctx, const void *device,
			 struct iiod_attr *attr, char *buf, uint32_t len)
{
	return -EINVAL;
}

static int dummy_rd_data(struct iiod_ctx *ctx, const void *device, char *buf,
			 uint32_t bytes)
{
	return -EINVAL;
}

static int dummy_wr_data(struct iiod_ctx *ctx, const void *device,
			 const char *trig, uint32_t bytes)
{
	return -EINVAL;
}

static int dummy_set_timeout(struct iiod_ctx *ctx, uint32_t timeout)
{
	return -EINVAL;
}

static int dummy_set_buffers_count(struct iiod_ctx *ctx, const void *device,
				   uint32_t buffers_count)
{
	return -EINVAL;
}

static int dummy_create_block(struct iiod_ctx *ctx, const void *device, struct iio_block *block, uint32_t block_size_bytes){
	return -EINVAL;
}

static int dummy_pre_enable(struct iiod_ctx *ctx, const void *device, uint32_t mask){
	return -EINVAL;
}

static int dummy_create_event_stream (struct iiod_ctx *ctx, const void *device,
		const uint32_t priv) {
	return -EINVAL;
}

static int dummy_read_event (struct iiod_ctx *ctx, const void *device,
		const uint32_t priv, uint8_t *buf) {
	return -EINVAL;
}

static int dummy_free_event_stream (struct iiod_ctx *ctx, const void *device,
		const uint32_t priv) {
	return -EINVAL;
}
int32_t iiod_copy_ops(struct iiod_ops *ops, struct iiod_ops *new_ops)
{
	if (!new_ops->recv || !new_ops->send)
		return -EINVAL;

	ops->recv = new_ops->recv;
	ops->send = new_ops->send;

	ops->open = SET_DUMMY_IF_NULL(new_ops->open, dummy_open);
	ops->close = SET_DUMMY_IF_NULL(new_ops->close, dummy_close);
	ops->read_buffer = SET_DUMMY_IF_NULL(new_ops->read_buffer, dummy_rd_data);
	ops->write_buffer = SET_DUMMY_IF_NULL(new_ops->write_buffer, dummy_wr_data);
	ops->read_attr = SET_DUMMY_IF_NULL(new_ops->read_attr, dummy_rw_attr);
	ops->write_attr = SET_DUMMY_IF_NULL(new_ops->write_attr, dummy_rw_attr);
	ops->get_trigger = SET_DUMMY_IF_NULL(new_ops->get_trigger, dummy_rd_data);
	ops->set_trigger = SET_DUMMY_IF_NULL(new_ops->set_trigger, dummy_wr_data);
	ops->set_timeout = SET_DUMMY_IF_NULL(new_ops->set_timeout, dummy_set_timeout);
	ops->set_buffers_count = SET_DUMMY_IF_NULL(new_ops->set_buffers_count,
				 dummy_set_buffers_count);
	ops->refill_buffer = SET_DUMMY_IF_NULL(new_ops->refill_buffer,
					       dummy_close);
	ops->push_buffer = SET_DUMMY_IF_NULL(new_ops->push_buffer,
					     dummy_close);
	ops->create_block = SET_DUMMY_IF_NULL(new_ops->create_block, dummy_create_block);
	ops->pre_enable = SET_DUMMY_IF_NULL(new_ops->pre_enable, dummy_pre_enable);
	ops->create_event_stream = SET_DUMMY_IF_NULL(new_ops->create_event_stream, dummy_create_event_stream);
	ops->read_event = SET_DUMMY_IF_NULL(new_ops->read_event, dummy_read_event);
	ops->free_event_stream = SET_DUMMY_IF_NULL(new_ops->free_event_stream, dummy_free_event_stream);

	return 0;
}

static int32_t iiod_set_protocol(struct iiod_conn_priv *conn,
				enum iiod_protocol protocol)
{
	switch (protocol) {
	case IIOD_ASCII_COMMAND:
		no_os_free(conn->cmd_data.command_data);
		conn->cmd_data.command_data = no_os_calloc(1, sizeof (struct command_data_ascii));
		if (!conn->cmd_data.command_data)
			return -ENOMEM;
		break;
	case IIOD_BINARY_COMMAND:
		no_os_free(conn->cmd_data.command_data);
		conn->cmd_data.command_data = no_os_calloc(1, sizeof (struct command_data_binary));
		if (!conn->cmd_data.command_data)
			return -ENOMEM;
		break;
	default:
		return -EINVAL;
	}

	conn->protocol = protocol;

	return 0;
}

int32_t iiod_init(struct iiod_desc **desc, struct iiod_init_param *param)
{
	struct iiod_desc *ldesc;
	int32_t ret;

	if (!desc || !param || !param->ops)
		return -EINVAL;

	ldesc = (struct iiod_desc *)calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return -ENOMEM;
	ret = iiod_copy_ops(&ldesc->ops, param->ops);
	if (NO_OS_IS_ERR_VALUE(ret)) {
		free(ldesc);

		return ret;
	}

	//ldesc->ops = param->ops;
	ldesc->xml = param->xml;
	ldesc->xml_len = param->xml_len;
	ldesc->app_instance = param->instance;
	ldesc->phy_type = param->phy_type;

	*desc = ldesc;

	return 0;
}

void iiod_remove(struct iiod_desc *desc)
{
	free(desc);
}

static void conn_clean_state(struct iiod_conn_priv *conn)
{
	void *command_data = conn->cmd_data.command_data;
	size_t size = conn->protocol == IIOD_ASCII_COMMAND?
				  sizeof(struct command_data_ascii) :
				  sizeof(struct command_data_binary);

	// TODO: Fix this after block details are moved out of the structure
	if (conn->protocol == IIOD_ASCII_COMMAND)
		memset(command_data, 0, size);
	memset(&conn->cmd_data, 0, sizeof(conn->cmd_data));
	memset(&conn->res, 0, sizeof(conn->res));
	memset(&conn->nb_buf, 0, sizeof(conn->nb_buf));

	conn->cmd_data.command_data = command_data;
	conn->res.buf.buf = NULL;
	conn->res.buf.idx = 0;
	conn->parser_idx = 0;
	conn->state = IIOD_WRITING_EVENT_DATA;
}

int32_t iiod_conn_add(struct iiod_desc *desc, struct iiod_conn_data *data,
		      uint32_t *new_conn_id)
{
	uint32_t i;
	int32_t ret;
	struct iiod_conn_priv *conn;

	if (!desc || !new_conn_id)
		return -EINVAL;

	for (i = 0; i < IIOD_MAX_CONNECTIONS; ++i)
		if (!desc->conns[i].used) {
			conn = &desc->conns[i];
			memset(conn, 0, sizeof(*conn));
			conn->used = 1;
			conn->conn = data->conn;
			/*
			 * TODO in future:
			 * think of using other buffer (e.g. ciruclar_buffer)
			 * to somehow implement zero copy
			 */
			conn->payload_buf = data->buf;
			conn->payload_buf_len = data->len;
			*new_conn_id = i;

			ret = lf256fifo_init(&conn->fifo_stream);

			conn->stream = calloc(1, sizeof(struct iio_stream));
			 if (!conn->stream)
			 	return -ENOMEM;

			conn->stream->blocks = calloc(MAX_NUM_BLOCKS, sizeof(conn->stream->blocks));
			 if (!conn->stream->blocks)
			 	return -ENOMEM;

			ret = iiod_set_protocol(conn, IIOD_ASCII_COMMAND);
			if (NO_OS_IS_ERR_VALUE(ret)) {
				return ret;
			}

			return 0;
		}

	return -EBUSY;
}

int32_t iiod_conn_remove(struct iiod_desc *desc, uint32_t conn_id,
			 struct iiod_conn_data *data)
{
	if (!desc || conn_id > IIOD_MAX_CONNECTIONS ||
	    !desc->conns[conn_id].used)
		return -EINVAL;
	struct iiod_conn_priv *conn;
	conn = &desc->conns[conn_id];
	data->conn = conn->conn;
	data->len = conn->payload_buf_len;
	data->buf = conn->payload_buf;
	conn->used = 0;

	return 0;
}

static int32_t call_op(struct iiod_ops *ops, struct comand_desc *desc,
		       struct iiod_ctx *ctx)
{
	struct command_data_ascii *data = (struct command_data_ascii *)desc->command_data;
	switch (data->cmd) {
	case IIOD_CMD_HELP:
		return -EINVAL;
	case IIOD_CMD_TIMEOUT:
		return ops->set_timeout(ctx, desc->timeout);
	case IIOD_CMD_OPEN:
		return ops->open(ctx, data->device, desc->sample_count,
				 desc->mask, desc->cyclic);
	case IIOD_CMD_CLOSE:
		return ops->close(ctx, data->device);
	case IIOD_CMD_SETTRIG:
		return ops->set_trigger(ctx, data->device, data->trigger,
					strlen(data->trigger));
	case IIOD_CMD_SET:
		return ops->set_buffers_count(ctx, data->device, desc->count);
	default:
		break;
	}

	return -EINVAL;
}

/*
 * Unload data from buf without blocking.
 * When done will return 0, if there is still data to be sent it will return
 * -EAGIAN. On error, an negative error code is returned
 */
static int32_t rw_iiod_buff(struct iiod_desc *desc, struct iiod_conn_priv *conn,
			    struct iiod_buff *buf, uint8_t flags)
{
	struct iiod_ctx ctx = IIOD_CTX(desc, conn);
	uint8_t *tmp_buf;
	int32_t ret;
	int32_t len;

	len = buf->len - buf->idx;
	if (len) {
		tmp_buf = (uint8_t *)buf->buf + buf->idx;
		if (flags & IIOD_WR)
			ret = desc->ops.send(&ctx, tmp_buf, len);
		else
			ret = desc->ops.recv(&ctx, tmp_buf, len);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;

		buf->idx += ret;
		if (ret < len)
			return -EAGAIN;
	}

	if (flags & IIOD_ENDL) {
		ret = desc->ops.send(&ctx, (uint8_t *)"\n", 1);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;

		if (ret != 1)
			return -EAGAIN;
	}

	return 0;
}

static int32_t rw_iiod_binary_buff(struct iiod_desc *desc,
				   struct iiod_conn_priv *conn,
				   struct iiod_buff *buf, uint8_t flags)
{
	struct iiod_ctx ctx = IIOD_CTX(desc, conn);
	int32_t ret;
	int32_t len;

	len = buf->len - buf->idx;
	if (len) {
		if (flags & IIOD_WR)
			ret = desc->ops.send(&ctx, (uint8_t *)buf->buf + buf->idx,
					     len);
		else
			ret = desc->ops.recv(&ctx, (uint8_t *)buf->buf + buf->idx,
					     len);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;

		buf->idx += ret;
		if (ret < len)
			return -EAGAIN;
	}

	return 0;
}

static int32_t do_read_buff_delayed(struct iiod_desc *desc,
				    struct iiod_conn_priv *conn)
{
	struct iiod_ctx ctx = IIOD_CTX(desc, conn);
	uint32_t max_to_read;
	int32_t ret, len;
	struct command_data_ascii *data;

	conn->nb_buf.buf = conn->payload_buf;
	data = (struct command_data_ascii *)conn->cmd_data.command_data;
	len = no_os_min(conn->payload_buf_len, conn->cmd_data.bytes_count);
	max_to_read = len - conn->nb_buf.len;
	ret = desc->ops.read_buffer(&ctx, data->device,
				    conn->nb_buf.buf + conn->nb_buf.len, max_to_read);
	if (ret < 0)
		return ret;

	conn->nb_buf.len += ret;

	if (conn->nb_buf.len < conn->cmd_data.bytes_count)
		return -EAGAIN;

	ret = rw_iiod_buff(desc, conn, &conn->nb_buf, IIOD_WR);
	if (ret < 0)
		return ret;

	conn->nb_buf.len = 0;
	conn->nb_buf.idx = 0;

	return 0;
}

static int32_t do_read_buff(struct iiod_desc *desc, struct iiod_conn_priv *conn)
{
	struct iiod_ctx ctx;
	int32_t ret, len;
	struct command_data_ascii *data;

	/*
	 * When using the network backend wait for a whole buffer to be filled
	 * before sending in order to reduce the ammount of network traffic.
	 */
	if (desc->phy_type == USE_NETWORK)
		return do_read_buff_delayed(desc, conn);

	ctx = (struct iiod_ctx)IIOD_CTX(desc, conn);
	data = (struct command_data_ascii *)conn->cmd_data.command_data;

	if (conn->nb_buf.len == 0) {
		conn->nb_buf.buf = conn->payload_buf;
		len = no_os_min(conn->payload_buf_len,
				conn->cmd_data.bytes_count);
		/* Read from dev */
		ret = desc->ops.read_buffer(&ctx, data->device,
					    conn->nb_buf.buf, len);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;
		len = ret;
		conn->nb_buf.len = len;
		conn->nb_buf.idx = 0;
	}
	if (conn->nb_buf.idx < conn->nb_buf.len) {
		/* Write on conn */
		ret = rw_iiod_buff(desc, conn, &conn->nb_buf, IIOD_WR);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;

		conn->cmd_data.bytes_count -= conn->nb_buf.len;
		conn->nb_buf.len = 0;
		if (conn->cmd_data.bytes_count)
			return -EAGAIN;
	}

	return 0;
}

static int32_t do_write_buff(struct iiod_desc *desc,
			     struct iiod_conn_priv *conn)
{
	struct iiod_ctx ctx = IIOD_CTX(desc, conn);
	int32_t ret, len;
	struct command_data_ascii *data = (struct command_data_ascii *)conn->cmd_data.command_data;

	if (conn->nb_buf.len == 0) {
		conn->nb_buf.buf = conn->payload_buf;
		len = no_os_min(conn->payload_buf_len,
				conn->cmd_data.bytes_count);
		conn->nb_buf.len = len;
		conn->nb_buf.idx = 0;
	}
	if (conn->nb_buf.idx < conn->nb_buf.len) {
		/* Read from conn */
		ret = rw_iiod_buff(desc, conn, &conn->nb_buf, IIOD_RD);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;
	}
	/* Write to dev */
	ret = desc->ops.write_buffer(&ctx, data->device,
				     conn->nb_buf.buf, conn->nb_buf.len);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	conn->cmd_data.bytes_count -= conn->nb_buf.len;
	conn->nb_buf.len = 0;
	if (conn->cmd_data.bytes_count)
		return -EAGAIN;

	return 0;
}

static int32_t iiod_run_cmd(struct iiod_desc *desc,
			    struct iiod_conn_priv *conn)
{
	struct iiod_ctx ctx = IIOD_CTX(desc, conn);
	struct comand_desc *data = &conn->cmd_data;
	struct command_data_ascii *cmd_det = (struct command_data_ascii *)data->command_data;

	struct iiod_attr attr = {
		.type = data->type,
		.name = cmd_det->attr,
		.channel = cmd_det->channel
	};
	int32_t ret;

	switch (cmd_det->cmd) {
	case IIOD_CMD_HELP:
	case IIOD_CMD_TIMEOUT:
	case IIOD_CMD_OPEN:
	case IIOD_CMD_CLOSE:
	case IIOD_CMD_SETTRIG:
	case IIOD_CMD_SET:
		if (cmd_det->cmd == IIOD_CMD_OPEN) {
			conn->mask = data->mask;
			if (data->cyclic)
				conn->is_cyclic_buffer = true;
		}
		if (cmd_det->cmd == IIOD_CMD_CLOSE)
			/* Set is_cyclic_buffer to false every time the device is closed */
			conn->is_cyclic_buffer = false;
		conn->res.val = call_op(&desc->ops, data, &ctx);
		conn->res.write_val = 1;
		break;
	case IIOD_CMD_EXIT:
		conn->res.val = 0;
		conn->res.write_val = 1;

		return -ENOTCONN;
	case IIOD_CMD_PRINT:
		conn->res.val = desc->xml_len;
		conn->res.write_val = 1;
		conn->res.buf.buf = desc->xml;
		conn->res.buf.len = desc->xml_len;
		break;
	case IIOD_CMD_BINARY:
		ret = iiod_set_protocol(conn, IIOD_BINARY_COMMAND);

		conn->res.write_val = 1;
		conn->res.val = ret;
		break;
	case IIOD_CMD_VERSION:
		conn->res.buf.buf = IIOD_VERSION;
		conn->res.buf.len = IIOD_VERSION_LEN;
		break;
	case IIOD_CMD_READ:
	case IIOD_CMD_GETTRIG:
		if (cmd_det->cmd == IIOD_CMD_READ)
			ret = desc->ops.read_attr(&ctx, cmd_det->device, &attr,
						  conn->payload_buf,
						  conn->payload_buf_len);
		else
			ret = desc->ops.get_trigger(&ctx, cmd_det->device,
						    conn->payload_buf,
						    conn->payload_buf_len);
		conn->res.val = ret;
		conn->res.write_val = 1;
		if (!NO_OS_IS_ERR_VALUE(ret)) {
			conn->res.buf.buf = conn->payload_buf;
			conn->res.buf.len = ret;
		}
		break;
	case IIOD_CMD_WRITE:
		conn->payload_buf[data->bytes_count] = '\0';
		ret = desc->ops.write_attr(&ctx, cmd_det->device, &attr,
					   conn->payload_buf,
					   data->bytes_count);
		conn->nb_buf.len = 0;
		conn->res.val = ret;
		conn->res.write_val = 1;
		break;
	case IIOD_CMD_READBUF:
		conn->res.write_val = 1;
		ret = desc->ops.refill_buffer(&ctx, cmd_det->device, 0);
		if (NO_OS_IS_ERR_VALUE(ret)) {
			conn->res.val = ret;
			break;
		}
		conn->res.val = data->bytes_count;
		ret = snprintf(conn->buf_mask, 10, "%08"PRIx32, conn->mask);
		conn->res.buf.buf = conn->buf_mask;
		conn->res.buf.len = ret;
		break;
	case IIOD_CMD_WRITEBUF:
		conn->res.val = data->bytes_count;
		conn->res.write_val = 1;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int32_t iiod_read_generic(struct iiod_desc *desc,
				 struct iiod_conn_priv *conn, uint8_t* buff, uint8_t len)
{
	struct iiod_ctx ctx = {
		.instance = desc->app_instance,
		.conn = conn->conn
	};
	int32_t ret;
	uint8_t *ch = (uint8_t *)buff;

	while (len > 0) {
		ret = desc->ops.recv(&ctx, ch, 1);
		if (NO_OS_IS_ERR_VALUE(ret)) {
			if (ret == -EAGAIN)
				continue;
			else
				return ret;
		}
		len -= ret;
		ch += ret;
	}

	return 0;
}

static int32_t iiod_read_line(struct iiod_desc *desc,
			      struct iiod_conn_priv *conn)
{
	struct iiod_ctx ctx = {
		.instance = desc->app_instance,
		.conn = conn->conn
	};
	int32_t ret;
	char *ch;

	while (conn->parser_idx < IIOD_PARSER_MAX_BUF_SIZE - 1) {
		ch = conn->parser_buf + conn->parser_idx;
		ret = desc->ops.recv(&ctx, (uint8_t *)ch, 1);
		if (ret == -EAGAIN || ret == 0)
			return -EAGAIN;

		if (NO_OS_IS_ERR_VALUE(ret))
			goto end;

		if (conn->parser_idx == 0 && (*ch == '\n' || *ch == '\r'))
			continue ;

		++conn->parser_idx;
		if (*ch == '\n') {
			conn->parser_buf[conn->parser_idx] = '\0';
			ret = 0;
			goto end;
		}
	}

	ret = -EIO;
end:
	conn->parser_idx = 0;
	return ret;
}

static int32_t iiod_read_cmd_header(struct iiod_desc *desc,
				    struct iiod_conn_priv *conn)
{
	struct iiod_ctx ctx = {
		.instance = desc->app_instance,
		.conn = conn->conn
	};
	int32_t ret;
	uint8_t *ch = (uint8_t *)conn->parser_buf + conn->parser_idx;
	uint8_t len = sizeof(struct iiod_command) - conn->parser_idx;

	while ((len > 0) && (conn->parser_idx < IIOD_PARSER_MAX_BUF_SIZE - 1)){
		ret = desc->ops.recv(&ctx, ch, len);
		if (ret == -EAGAIN || ret == 0)
			return -EAGAIN; // TODO: Fix this.
//			continue;

		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;

		// if ret is less than the size of the command, it means that we have
		// not received the full command yet, so we need to read more data
		len -= ret;
		ch += ret;
		conn->parser_idx += ret;
	}

	if (len > 0) {
		// If we have not received the full command, return an error
		return -EIO;
	}

	return 0;
}

static int32_t iiod_read_cmd_payload(struct iiod_desc *desc,
				    struct iiod_conn_priv *conn)
{
	int32_t ret;
	struct iiod_command *cmd = (struct iiod_command *) conn->parser_buf;
	uint8_t *ch = (uint8_t *)conn->parser_buf + conn->parser_idx;
	uint8_t payload_len;

	switch (cmd->op) {
		case IIOD_OP_PRINT:
		case IIOD_OP_TIMEOUT:
		case IIOD_OP_READ_ATTR:
		case IIOD_OP_READ_DBG_ATTR:
		case IIOD_OP_READ_BUF_ATTR:
		case IIOD_OP_READ_CHN_ATTR:
		case IIOD_OP_ENABLE_BUFFER:
		case IIOD_OP_CREATE_EVSTREAM:
		case IIOD_OP_FREE_EVSTREAM:
		case IIOD_OP_READ_EVENT:
			return 0;
		case IIOD_OP_WRITE_ATTR:
		case IIOD_OP_WRITE_DBG_ATTR:
		case IIOD_OP_WRITE_BUF_ATTR:
		case IIOD_OP_WRITE_CHN_ATTR:
			payload_len = 8;
			break;
		case IIOD_OP_GETTRIG:
		case IIOD_OP_SETTRIG:
			return 0; // TODO: Check what to do here
		case IIOD_OP_CREATE_BUFFER:
			payload_len = 4;
			break;
		case IIOD_OP_DISABLE_BUFFER:
		case IIOD_OP_CREATE_BLOCK:
			payload_len = 8;
			break;
		case IIOD_OP_TRANSFER_BLOCK:
			payload_len = 8;
			break;
		case IIOD_OP_FREE_BLOCK:
		case IIOD_OP_FREE_BUFFER:
			return 0;
		case IIOD_OP_ENQUEUE_BLOCK_CYCLIC:
		case IIOD_OP_RETRY_DEQUEUE_BLOCK:
		default:
			return -EINVAL;
	}

	do {
		ret = iiod_read_generic(desc, conn, ch, payload_len);
	} while (ret == -EAGAIN);

	return ret;
}

static int32_t iio_read_command(struct iiod_desc *desc,
				    struct iiod_conn_priv *conn) {
	int32_t ret;

	if (conn->parser_idx < sizeof(struct iiod_command)) {
		ret = iiod_read_cmd_header(desc, conn);
		if (ret == -EAGAIN)
			return -EAGAIN;
		if (NO_OS_IS_ERR_VALUE(ret))
			goto end;
	}

	ret = iiod_read_cmd_payload(desc, conn);
	if (ret == -EAGAIN)
		return -EAGAIN;
	
end:
	conn->parser_idx = 0;
	return ret;
}

static uint16_t block_ids[16];

static int32_t iiod_run_cmd_new(struct iiod_desc *desc,
					struct iiod_conn_priv *conn)
{
	int32_t ret;
	int32_t i;
	static uint8_t buffer[1024];
	static int cnt = 0;
	struct iiod_ctx ctx = IIOD_CTX(desc, conn);
	struct command_data_binary *data =
			(struct command_data_binary *)conn->cmd_data.command_data;
	struct comand_desc *cmd_desc = &conn->cmd_data;

	struct iiod_attr attr = {
		.type = conn->cmd_data.type,
		.name = NULL,
		.channel = NULL,
		.idx = -1,
		.ch_id = -1
	};

	static uint8_t *blocks[16];
	struct iio_stream *stream = conn->stream;
	struct lf256fifo *fifo_stream = conn->fifo_stream;
	struct iiod_event_client *client_desc;
	static struct iiod_buff buff;

//	struct iiod_event_data evt_data = {
//		.channel_id = 0,
//		.diff_channel_id = 0,
//		.channel_type = IIO_VOLTAGE,
//		.modifier = IIO_NO_MOD,
//		.event_dir = IIO_EV_DIR_NONE,
//		.is_differential = 0,
//		.event_type = IIO_EV_TYPE_THRESH,
//		.timestamp = 1
//	};

	IIOD_SET_DEFAULT_RESPONSE(conn->res_header);

	static uint8_t buf_id;
	struct iiod_event_client event_client_key = {.client_id = data->client_id};

	conn->res.write_val = 0;

	switch (data->op_code) {
	case IIOD_OP_PRINT:
		conn->res.buf.buf = desc->xml;
		conn->res.buf.len = desc->xml_len;
		conn->res.buf.idx = 0;

		conn->res_header.code = desc->xml_len;
		break;
	case IIOD_OP_TIMEOUT:
		conn->res.buf.len = 0;
		break;

	case IIOD_OP_READ_ATTR:
	case IIOD_OP_READ_DBG_ATTR:
	case IIOD_OP_READ_BUF_ATTR:
	case IIOD_OP_READ_CHN_ATTR:
		attr.idx = data->attr;
		if (data->op_code == IIOD_OP_READ_CHN_ATTR) {
			attr.ch_id = data->channel;
		}
		ret = desc->ops.read_attr(&ctx, &data->device, &attr,
					  conn->payload_buf,
					  conn->payload_buf_len);

		conn->res.buf.len = 0;
		if (NO_OS_IS_ERR_VALUE(ret)) {
			goto command_fail;
		} else if (ret){
			conn->res.buf.idx = 0;
			conn->res.buf.buf = conn->payload_buf;
			conn->res.buf.buf[ret] = '\0';
			conn->res.buf.len = ret + 1;
			conn->res_header.code = conn->res.buf.len;
		}
		break;

	case IIOD_OP_WRITE_ATTR:
	case IIOD_OP_WRITE_DBG_ATTR:
	case IIOD_OP_WRITE_BUF_ATTR:
	case IIOD_OP_WRITE_CHN_ATTR:
		attr.idx = data->attr;
		if (data->op_code == IIOD_OP_WRITE_CHN_ATTR) {
			attr.ch_id = data->channel;
		}
		ret = desc->ops.write_attr(&ctx, &data->device, &attr,
					   conn->payload_buf,
					   conn->cmd_data.bytes_count);
		if (NO_OS_IS_ERR_VALUE(ret)) {
			goto command_fail;
		}

		conn->res.buf.len = 0;
		break;

	case IIOD_OP_CREATE_BUFFER:
		conn->res.buf.len = 0;
		break;

	case IIOD_OP_CREATE_BLOCK:
		conn->res.buf.len = 0;

		/* Send response with block size as code created and cl id */
		conn->res_header.code = data->block_size[curr];

		//new responder io is created in the client..  so create a corresponding one in here
		conn->res_header.client_id = data->client_id;

		stream->blocks[curr] = calloc(1, sizeof(*stream->blocks[curr]));
		if (!stream->blocks[curr])
			return -ENOMEM;

		if(desc->ops.create_block)
			ret = desc->ops.create_block(&ctx, &data->device,
							 stream->blocks[curr], data->block_size[curr]);

		stream->blocks[curr]->cl_id = data->client_id;
		stream->blocks[curr]->size = data->block_size[curr];

		stream->nb_blocks++;

		curr = (curr + 1) % MAX_NUM_BLOCKS;
		break;

	case IIOD_OP_TRANSFER_BLOCK:
		/* get block index, client id from cmd and block size from cmd payload */
		//take block index from (cmd.code>>16)
		// TODO: Remove the code dependency and use a different variable.
		uint8_t wr = data->block_id[curr_1];
		block_ids[curr_1] = wr;

		//enqueue buf idx
		lf256fifo_write(fifo_stream, wr);

		if (stream->started) {
			lf256fifo_get(fifo_stream, &wr);
			ret = desc->ops.refill_buffer(&ctx, &data->device, wr);
			if (NO_OS_IS_ERR_VALUE(ret))	
				return ret;
		}

		curr_1 = (curr_1 + 1)%stream->nb_blocks;

		conn->state = IIOD_READING_LINE;

		break;

	case IIOD_OP_ENABLE_BUFFER:
		conn->state = IIOD_WRITING_BIN_RESPONSE;
		// conn->res.write_val = 1;
		uint8_t id;
		lf256fifo_get(fifo_stream, &id);
		ret = desc->ops.pre_enable(&ctx, &data->device, cmd_desc->mask, block_ids);

		ret = desc->ops.refill_buffer(&ctx, &data->device, id);

		conn->res.val = 0; //dummy op
		stream->started = true;

		break;

	case IIOD_OP_FREE_BLOCK:
		//Dealloc blocks created
		stream->started = false;

		ret = desc->ops.close(&ctx, &data->device);

		conn->res.val = 0;
		// conn->res.write_val = 1;
		conn->res.buf.len = 0;
		if (stream && stream->blocks) {
			for (int i = 0; i < stream->nb_blocks; i++) {
				if (stream->blocks[i]) {
					free(stream->blocks[i]->data);
					free(stream->blocks[i]);
				}
			}
			free(stream->blocks);
			stream->blocks = NULL;
			stream->nb_blocks = 0;
		}

		break;

	case IIOD_OP_FREE_BUFFER:
		conn->res.buf.len = 0;
		conn->state = IIOD_WRITING_BIN_RESPONSE;
		curr=0;
		curr_1=0;
		//dealloc buffer

		break;

	case IIOD_OP_RETRY_DEQUEUE_BLOCK:
		// dequeue block from the buffer
		// fill some dummy data to the buffer
		// only when previous deque fails
		break;

	case IIOD_OP_CREATE_EVSTREAM:
		conn->res_header.client_id = data->client_id;
		conn->res.buf.len = 0;

		/* Create a list of events, if not done already */
		if (!conn->events.clients) {
			ret = no_os_list_init(&conn->events.clients, NO_OS_LIST_PRIORITY_LIST,
					      iio_event_cmp);
			if (NO_OS_IS_ERR_VALUE(ret)) {
				goto command_fail;
			}
		}

		/* Find the element for the Client ID */
		ret = no_os_list_read_find(conn->events.clients, (void **)&client_desc, &event_client_key);

		/* If no client data is found insert a new one */
		if (ret) {
			client_desc = (struct iiod_event_client *) no_os_calloc(1, sizeof(*client_desc));
			if (!client_desc) {
				ret = -ENOMEM;
				goto command_fail;
			}

			ret = desc->ops.create_event_stream(&ctx, &data->device, data->client_id);
			if (NO_OS_IS_ERR_VALUE(ret))
				goto free_event_desc;

			client_desc->client_id = data->client_id;
			client_desc->dev_id = data->device;
			client_desc->event_read_count = 0;

			conn->events.num_clients++;
			 ret = no_os_list_add_last(conn->events.clients, client_desc);
			 if (NO_OS_IS_ERR_VALUE(ret))
			 	goto free_event_data;
		} else {
			ret = -EINVAL;
			goto command_fail;
		}

		break;
free_event_data:
		lf256fifo_remove(client_desc->event_data);
free_event_desc:
		no_os_free(client_desc);
		goto command_fail;
		break;
	case IIOD_OP_FREE_EVSTREAM:
		if (!conn->events.clients) {
			ret = -EINVAL;
			goto command_fail;
		}

		/* Find the element for the Client ID */
		ret = no_os_list_read_find(conn->events.clients, (void **)&client_desc, &event_client_key);
		if (NO_OS_IS_ERR_VALUE(ret)) {
			ret = -EINVAL;
			goto command_fail;
		}

		lf256fifo_remove(client_desc->event_data);
		no_os_free(client_desc);

		conn->res.buf.len = 0;
		break;
	case IIOD_OP_READ_EVENT:
		if (!conn->events.clients)  {
			ret = -EINVAL;
			goto command_fail;
		}

		/* Find the element for the Client ID */
		ret = no_os_list_read_find(conn->events.clients, (void **)&client_desc, &event_client_key);
		if (NO_OS_IS_ERR_VALUE(ret)) {
			goto command_fail;
		}

		client_desc->event_read_count++;
		break;
	default:
		return -EINVAL;
	}

	return 0;
command_fail:
	conn->res_header.code = ret;
	conn->res.buf.len = 0;
	return 0;

}


static int32_t iiod_run_state_bin(struct iiod_desc *desc,
			      struct iiod_conn_priv *conn)
{
	int32_t ret;
	struct iiod_ctx ctx = IIOD_CTX(desc, conn);
	struct command_data_binary *data = (struct command_data_binary *)conn->cmd_data.command_data;

	uint8_t c;
	static struct iiod_buff buff;
	switch (conn->state) {
		case IIOD_WRITING_EVENT_DATA:	
			uint32_t i;
			struct iiod_event_client *client;
			for (i = 0; i < conn->events.num_clients; i++) {
				ret = no_os_list_read_idx(conn->events.clients, (void **)&client, i);
				if (NO_OS_IS_ERR_VALUE(ret)) {
					// TODO: Error. Dont know how to manage this.
				}
				if (client->event_read_count) {
					ret = desc->ops.read_event(&ctx, &data->device,
								client->client_id, conn->event_data);
					if (ret > 0) {
						/* Event Found */
						IIOD_SET_RESPONSE(conn->res_header, client->client_id,
								client->dev_id, sizeof(struct iio_event));

						ret = desc->ops.send(&ctx, (uint8_t *)&conn->res_header,
								 sizeof(conn->res_header));

						buff.buf = conn->event_data;
						buff.len = NO_OS_ARRAY_SIZE(conn->event_data);
						buff.idx = 0;

						do {
							ret = rw_iiod_buff(desc, conn,
									&buff,
									IIOD_WR);
						} while (ret == -EAGAIN);

						client->event_read_count--;
					} else if ((ret == -EAGAIN) || (ret == 0)) {
						/* No Event Found */
						continue;
					} else {
						IIOD_SET_RESPONSE(conn->res_header, client->client_id, client->dev_id, ret);
						conn->res.write_val = 0;
						conn->state = IIOD_WRITING_BIN_RESPONSE;
						return 0;
					}
				}
			}
			conn->state = IIOD_WRITING_BUF_DATA;
			return 0;

		case IIOD_WRITING_BUF_DATA:
			if(conn->stream->started){
				if(!lf256fifo_is_empty(conn->fifo_stream)){
					lf256fifo_get(conn->fifo_stream, &c);
					if(conn->stream->blocks[c]->bytes_used == conn->stream->blocks[c]->size) {
						conn->res_header.client_id = conn->stream->blocks[c]->cl_id;
						conn->res_header.code = conn->stream->blocks[c]->size;

						ret = desc->ops.send(&ctx, (uint8_t *)&conn->res_header,
								 sizeof(conn->res_header));

						buff.buf = conn->stream->blocks[c]->data;
						buff.len = conn->stream->blocks[c]->size;
						buff.idx = 0;

						do {
							ret = rw_iiod_buff(desc, conn,
									&buff,
									IIOD_WR);
						} while (ret == -EAGAIN);

						conn->stream->blocks[c]->bytes_used = 0;

						ret = lf256fifo_read(conn->fifo_stream, &c);
						if (NO_OS_IS_ERR_VALUE(ret))
							return ret;
					} else {
						conn->state = IIOD_READING_LINE;
						return 0;
					}
				}
			}

			conn->state = IIOD_READING_LINE;
			return 0;

		case IIOD_READING_LINE:
			ret = iio_read_command(desc, conn);
			if (NO_OS_IS_ERR_VALUE(ret)) {
				if ((ret == -EAGAIN) && (conn->parser_idx)) {
					return ret;
				} else if (ret == -EAGAIN) {
					conn->state = IIOD_LINE_DONE;
					return 0;
				}
				if (!strncmp(conn->parser_buf, "BINARY\r\n", 8)) {
					/* If we receive again the "BINARY\r\n" string, send a
					 * return code of zero and continue as usual.
					 * This can happen with the serial backend when the
					 * client disconnects and a new client appears. */
					conn->res.val = 0;
					conn->res.write_val = 1;
					conn->state = IIOD_WRITING_BIN_RESPONSE;

					return 0;
				}
				IIOD_SET_ERROR_RESPONSE(conn->res_header, ret);
				conn->res.write_val = 0;
				conn->state = IIOD_WRITING_BIN_RESPONSE;
			}

			/* Fill struct comand_desc with data from line. No I/O */
			ret = iiod_parse_command(conn->parser_buf, &conn->cmd_data);
			if (NO_OS_IS_ERR_VALUE(ret)) {
				/* Parsing line failed */
				conn->res.write_val = 1;
				conn->res.val = ret;
				conn->state = IIOD_WRITING_CMD_RESULT;
			} else if ((data->op_code >= IIOD_OP_WRITE_ATTR) &&
					(data->op_code <= IIOD_OP_WRITE_CHN_ATTR)){
				/* Special case. Attribute needs to be read */
				conn->nb_buf.buf = conn->payload_buf;
				conn->nb_buf.len = conn->cmd_data.bytes_count;
				conn->nb_buf.idx = 0;
				conn->state = IIOD_READING_WRITE_DATA;
			} else {
				conn->state = IIOD_RUNNING_CMD;
			}
			return 0;
	case IIOD_RUNNING_CMD:
		/* Execute or call necessary ops depending on cmd. No I/O */
		ret = iiod_run_cmd_new(desc, conn);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;

		if ((data->op_code == IIOD_OP_TRANSFER_BLOCK) ||
			(data->op_code == IIOD_OP_READ_EVENT))
			conn->state = IIOD_LINE_DONE; //READ
		else
			conn->state = IIOD_WRITING_BIN_RESPONSE;
		return 0;

	default:
		/* Should never get here */
		return -EINVAL;
	}
}

/*
 * Function will return SUCCESS when a state was processed.
 * If a state is still in processing state, it will return -EAGAIN.
 * If other error occur. E.g. Connection errors, they are returned and
 * the connection must be cleaned up.
 */
static int32_t iiod_run_state(struct iiod_desc *desc,
			      struct iiod_conn_priv *conn)
{
	int32_t ret;
	struct iiod_ctx ctx = IIOD_CTX(desc, conn);
	struct command_data_ascii *data = (struct command_data_ascii *)conn->cmd_data.command_data;

	uint8_t c;
	static struct iiod_buff buff;

	switch (conn->state) {
	case IIOD_WRITING_BUF_DATA:
	case IIOD_WRITING_EVENT_DATA:
		if (conn->protocol == IIOD_BINARY_COMMAND) {
			return iiod_run_state_bin(desc, conn);
		}
		__attribute__((fallthrough));

	case IIOD_READING_LINE:
		if (conn->protocol == IIOD_ASCII_COMMAND) {
			/* Read input data until \n. I/O Calls */
			ret = iiod_read_line(desc, conn);
			if (NO_OS_IS_ERR_VALUE(ret))
				return ret;

			/* Fill struct comand_desc with data from line. No I/O */
			ret = iiod_parse_line(conn->parser_buf, &conn->cmd_data,
					      &conn->strtok_ctx);
		} else {
			return iiod_run_state_bin(desc, conn);
		}
		if (NO_OS_IS_ERR_VALUE(ret)) {
			/* Parsing line failed */
			conn->res.write_val = 1;
			conn->res.val = ret;
			conn->state = IIOD_WRITING_CMD_RESULT;
		} else if (data->cmd == IIOD_CMD_WRITE) {
			/* Special case. Attribute needs to be read */
			conn->nb_buf.buf = conn->payload_buf;
			conn->nb_buf.len = conn->cmd_data.bytes_count;
			conn->nb_buf.idx = 0;
			conn->state = IIOD_READING_WRITE_DATA;
		} else {
			conn->state = IIOD_RUNNING_CMD;
		}

		return 0;

	case IIOD_RUNNING_CMD:
		/* Execute or call necessary ops depending on cmd. No I/O */

		if (conn->protocol) {
			/* Read input data until \n. I/O Calls */
			return ret = iiod_run_state_bin(desc, conn);
		}

		ret = iiod_run_cmd(desc, conn);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;

		conn->state = IIOD_WRITING_CMD_RESULT;

		return 0;
	case IIOD_WRITING_CMD_RESULT:
		/* Write result or the length of data to be sent*/
		if (conn->res.write_val) {
			if (conn->nb_buf.len == 0) {
				conn->nb_buf.buf = conn->parser_buf;
				ret = sprintf(conn->nb_buf.buf, "%"PRIi32,
					      conn->res.val);
				conn->nb_buf.len = ret;
				conn->nb_buf.idx = 0;
			}
			/* Non-blocking. Will enter here until val is sent */
			if (conn->nb_buf.idx < conn->nb_buf.len) {
				ret = rw_iiod_buff(desc, conn, &conn->nb_buf,
						   IIOD_WR | IIOD_ENDL);
				if (NO_OS_IS_ERR_VALUE(ret))
					return ret;
			}
		}
		/* Send buf from result. Non blocking */
		if (conn->res.buf.buf &&
		    conn->res.buf.idx < conn->res.buf.len) {
			ret = rw_iiod_buff(desc, conn, &conn->res.buf,
					   IIOD_WR | IIOD_ENDL);
			if (NO_OS_IS_ERR_VALUE(ret))
				return ret;
		}

		if (data->cmd != IIOD_CMD_READBUF &&
				data->cmd != IIOD_CMD_WRITEBUF) {
			if (conn->is_cyclic_buffer && data->cmd != IIOD_CMD_OPEN)
				conn->state = IIOD_PUSH_CYCLIC_BUFFER;
			else
				conn->state = IIOD_LINE_DONE;
		} else {
			/* Preapre for IIOD_RW_BUF state */
			memset(&conn->nb_buf, 0, sizeof(conn->nb_buf));
			conn->state = IIOD_RW_BUF;
		}

		return 0;

	case IIOD_WRITING_BIN_RESPONSE:
		if (conn->res.write_val) {
			if (conn->nb_buf.len == 0) {
				conn->nb_buf.buf = conn->parser_buf;
				ret = sprintf(conn->nb_buf.buf, "%"PRIi32,
					      conn->res.val);
				conn->nb_buf.len = ret;
				conn->nb_buf.idx = 0;
			}
			/* Non-blocking. Will enter here until val is sent */
			if (conn->nb_buf.idx < conn->nb_buf.len) {
				ret = rw_iiod_buff(desc, conn, &conn->nb_buf,
						   IIOD_WR | IIOD_ENDL);
				if (NO_OS_IS_ERR_VALUE(ret))
					return ret;
			}
		} else {
			ret = desc->ops.send(&ctx, (uint8_t *)&conn->res_header,
					     sizeof(conn->res_header));
			if (NO_OS_IS_ERR_VALUE(ret))
				return ret;

			if (conn->res.buf.len) {
				/* Send buf from result. Non blocking */
				if (conn->res.buf.buf &&
				    conn->res.buf.idx < conn->res.buf.len) {
					do {
						ret = rw_iiod_buff(desc, conn, &conn->res.buf,
								   IIOD_WR);
					} while (ret == -EAGAIN);
					if (NO_OS_IS_ERR_VALUE(ret))
						return ret;
				}
			}
		}

		conn->state = IIOD_LINE_DONE; //IIOD_READING_LINE;
		return 0;

	case IIOD_RW_BUF:
		/* IIOD_CMD_READBUF and IIOD_CMD_WRITEBUF special case */
		/* Non blocking read/write until all data is processed */
		if (data->cmd == IIOD_CMD_READBUF)
			ret = do_read_buff(desc, conn);
		else {
			ret = do_write_buff(desc, conn);
			if (ret == 0) {
				conn->res.write_val = 1;
				ret = desc->ops.push_buffer(&ctx,
						data->device);
				if (NO_OS_IS_ERR_VALUE(ret)) {
					conn->res.val = ret;
					conn->state = IIOD_LINE_DONE;

					return 0;
				}
				memset(&conn->res.buf, 0, sizeof(conn->res.buf));
				conn->res.val = conn->cmd_data.bytes_count;
				data->cmd = IIOD_CMD_PRINT;
				conn->state = IIOD_WRITING_CMD_RESULT;

				return 0;
			}
		}
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;

		conn->state = IIOD_LINE_DONE;

		return 0;
	case IIOD_READING_WRITE_DATA:
		/* Read attribute */
		ret = rw_iiod_buff(desc, conn, &conn->nb_buf, IIOD_RD);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;

		conn->state = IIOD_RUNNING_CMD;

		return 0;
	case IIOD_PUSH_CYCLIC_BUFFER:
		/* Push puffer to IIO application */
		ret = desc->ops.push_buffer(&ctx,
				data->device);
		/* If an error was encountered, close connection */
		if (NO_OS_IS_ERR_VALUE(ret)) {
			conn->res.val = ret;
			desc->ops.close(&ctx, data->device);
			conn->state = IIOD_LINE_DONE;
			conn->is_cyclic_buffer = false;
			return 0;
		}

		/* Read data from the client to verify whether a close command has been sent */
		ret = iiod_read_line(desc, conn);
		if (NO_OS_IS_ERR_VALUE(ret))
			return 0;

		/* Fill struct comand_desc with data from line */
		ret = iiod_parse_line(conn->parser_buf, &conn->cmd_data,
				      &conn->strtok_ctx);
		if (!NO_OS_IS_ERR_VALUE(ret) && data->cmd == IIOD_CMD_CLOSE) {
			/* Exit this state only if a close command is received
			   All other commands will be ignored.
			*/
			conn->nb_buf.len = 0;
			conn->state = IIOD_RUNNING_CMD;
			conn->is_cyclic_buffer = false;
		}
		return 0;

	default:
		/* Should never get here */
		return -EINVAL;
	}
}

int32_t iiod_conn_step(struct iiod_desc *desc, uint32_t conn_id)
{
	struct iiod_conn_priv *conn;
	int32_t ret;

	if (!desc || conn_id > IIOD_MAX_CONNECTIONS ||
	    !desc->conns[conn_id].used)
		return -EINVAL;

	conn = &desc->conns[conn_id];
	do {
		ret = iiod_run_state(desc, conn);
		if (ret == -EAGAIN)
			return ret;
		if (NO_OS_IS_ERR_VALUE(ret) || conn->state == IIOD_LINE_DONE)
			break;
		//The loop will continue because the state was changed.
	} while (true);

	conn_clean_state(conn);

	return ret;
}
