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
	return ((struct iiod_event_desc *)data1)->client_id -
	       ((struct iiod_event_desc *)data2)->client_id;
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
			res->cmd = priority_array[i];

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
		strncpy(res->channel, token, sizeof(res->channel));
		token = strtok_r(NULL, delim, ctx);
	}

	if (res->cmd == IIOD_CMD_WRITE) {
		if (!token)
			return -EINVAL;

		if (*token >= '0' && *token <= '9') {
			memset(res->attr, 0, sizeof(res->attr));

			return parse_num(token, &res->bytes_count, 10);
		}

		strncpy(res->attr, token, sizeof(res->attr));
		token = strtok_r(NULL, delim, ctx);
		if (!token)
			return -EINVAL;

		return parse_num(token, &res->bytes_count, 10);
	}

	if (token)
		strncpy(res->attr, token, sizeof(res->attr));
	else
		memset(res->attr, 0, sizeof(res->attr));

	return 0;
}

int32_t iiod_parse_line(char *buf, struct comand_desc *res, char **ctx)
{
	int32_t ret;
	char *token;

	token = strtok_r(buf, delim, ctx);
	ret = parse_cmd(token, res);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	token = strtok_r(NULL, delim, ctx);
	/* Commands without device */
	switch (res->cmd) {
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

	strncpy(res->device, token, sizeof(res->device));
	token = strtok_r(NULL, delim, ctx);
	switch (res->cmd) {
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
			strncpy(res->trigger, token, sizeof(res->trigger));
		else
			memset(res->trigger, 0, sizeof(res->trigger));

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
	int32_t ret;
	struct iiod_binary_cmd *cmd = (struct iiod_binary_cmd *)buf;
	uint8_t *payload = buf + sizeof(*cmd);
	res->op_code = cmd->op;
	
	/* Commands without payload */
	switch (res->op_code) {
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
			res->binary_data.device = (uint16_t) cmd->dev;
			res->binary_data.attr = (uint16_t) (cmd->code >> 16);
			return 0;
		case IIOD_OP_READ_DBG_ATTR:
			res->type = IIO_ATTR_TYPE_BUFFER;
			res->binary_data.device = (uint16_t) cmd->dev;
			res->binary_data.attr = (uint16_t) (cmd->code >> 16);
			return 0;
		case IIOD_OP_READ_BUF_ATTR:
			res->type = IIO_ATTR_TYPE_BUFFER;
			res->binary_data.device = (uint16_t) cmd->dev;
			res->binary_data.attr = (uint16_t) (cmd->code >> 16);
			res->binary_data.buffer = (uint16_t) cmd->code;
			return 0;
		case IIOD_OP_READ_CHN_ATTR:
			res->type = IIO_ATTR_TYPE_CH_OUT;
			res->binary_data.device = (uint16_t) cmd->dev;
			res->binary_data.attr = (uint16_t) (cmd->code >> 16);
			res->binary_data.channel = (uint16_t) cmd->code;
			return 0;
		case IIOD_OP_GETTRIG:
		case IIOD_OP_SETTRIG:
			return 0; // TODO: Check what to do here
	default:
		break;
	}
	/* Commands with payload */
	switch (res->op_code) {
		case IIOD_OP_WRITE_ATTR:
		case IIOD_OP_WRITE_DBG_ATTR:
		case IIOD_OP_WRITE_BUF_ATTR:
		case IIOD_OP_WRITE_CHN_ATTR:
			break; //TODO: See what to do.
		case IIOD_OP_CREATE_BUFFER:
			res->mask = *(uint32_t *)payload;
			return 0;
		case IIOD_OP_DISABLE_BUFFER:
		case IIOD_OP_CREATE_BLOCK:
			res->block_id[curr] = (int16_t)(cmd->code >> 16);
			res->block_size[curr] = *(uint32_t *)payload; //TODO: Is this correct. We read 8 bytes payload but only use 4?
			return 0;
		case IIOD_OP_FREE_BLOCK:
		case IIOD_OP_FREE_BUFFER:
		case IIOD_OP_TRANSFER_BLOCK:
//			res->bytes_size[curr_1] = *(uint32_t *)payload;
			//SWADO: need to see what to do here
			return 0;
		case IIOD_OP_ENQUEUE_BLOCK_CYCLIC:
		case IIOD_OP_RETRY_DEQUEUE_BLOCK:
		default:
			break;
	}
	
	switch (res->cmd) {
	case IIOD_CMD_CLOSE:
	case IIOD_CMD_GETTRIG:
		return 0;
	case IIOD_CMD_OPEN:
//		return iiod_parse_open(token, res, ctx);
	case IIOD_CMD_READ:
	case IIOD_CMD_WRITE:
//		return iiod_parse_rw_attr(token, res, ctx);
	case IIOD_CMD_READBUF:
	case IIOD_CMD_WRITEBUF:
//		return parse_num(token, &res->bytes_count, 10);
	case IIOD_CMD_SETTRIG:
//		if (token)
//			strncpy(res->trigger, token, sizeof(res->trigger));
//		else
//			memset(res->trigger, 0, sizeof(res->trigger));

		return 0;
	case IIOD_CMD_SET:
//		return iiod_parse_set(token, res, ctx);
	default:
		break;
	}

	return -EINVAL;
}

// static int32_t iiod_parse_binary_cmd(struct iiod_conn_priv *conn, struct iiod_binary_cmd *cmd){
// 	conn->cmd_data.cmd = cmd->cmd_id;
// 	conn->cmd_data.bytes_count = cmd->length;
// 	memcpy(conn->cmd_data.device, cmd->payload, cmd->length);
// 	return 0;
// }



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

static int32_t detect_protocol(struct iiod_conn_priv *conn,
			       struct iiod_desc *desc)
{
	uint8_t header[4];
	int32_t ret = desc->ops.recv(conn, header, sizeof(header));
	if (ret < 0)
		return ret;

	if (header[0] == 0)
		conn->is_binary_protocol = true;
	else
		conn->is_binary_protocol = false;

	return 0;
}

void iiod_remove(struct iiod_desc *desc)
{
	free(desc);
}

static void conn_clean_state(struct iiod_conn_priv *conn)
{
	memset(&conn->cmd_data, 0, sizeof(conn->cmd_data));
	memset(&conn->res, 0, sizeof(conn->res));
	memset(&conn->nb_buf, 0, sizeof(conn->nb_buf));

	conn->res.buf.buf = NULL;
	conn->res.buf.idx = 0;
	conn->parser_idx = 0;
	conn->state = IIOD_READING_LINE;
}

int32_t iiod_conn_add(struct iiod_desc *desc, struct iiod_conn_data *data,
		      uint32_t *new_conn_id)
{
	uint32_t i;
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

static int32_t call_op(struct iiod_ops *ops, struct comand_desc *data,
		       struct iiod_ctx *ctx)
{
	switch (data->cmd) {
	case IIOD_CMD_HELP:
		return -EINVAL;
	case IIOD_CMD_TIMEOUT:
		return ops->set_timeout(ctx, data->timeout);
	case IIOD_CMD_OPEN:
		return ops->open(ctx, data->device, data->sample_count,
				 data->mask, data->cyclic);
	case IIOD_CMD_CLOSE:
		return ops->close(ctx, data->device);
	case IIOD_CMD_SETTRIG:
		return ops->set_trigger(ctx, data->device, data->trigger,
					strlen(data->trigger));
	case IIOD_CMD_SET:
		return ops->set_buffers_count(ctx, data->device, data->count);
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

	conn->nb_buf.buf = conn->payload_buf;
	len = no_os_min(conn->payload_buf_len, conn->cmd_data.bytes_count);
	max_to_read = len - conn->nb_buf.len;
	ret = desc->ops.read_buffer(&ctx, conn->cmd_data.device,
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

	/*
	 * When using the network backend wait for a whole buffer to be filled
	 * before sending in order to reduce the ammount of network traffic.
	 */
	if (desc->phy_type == USE_NETWORK)
		return do_read_buff_delayed(desc, conn);

	ctx = (struct iiod_ctx)IIOD_CTX(desc, conn);
	if (conn->nb_buf.len == 0) {
		conn->nb_buf.buf = conn->payload_buf;
		len = no_os_min(conn->payload_buf_len,
				conn->cmd_data.bytes_count);
		/* Read from dev */
		ret = desc->ops.read_buffer(&ctx, conn->cmd_data.device,
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
	ret = desc->ops.write_buffer(&ctx, conn->cmd_data.device,
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
	struct iiod_attr attr = {
		.type = data->type,
		.name = data->attr,
		.channel = data->channel
	};
	int32_t ret;

	switch (data->cmd) {
	case IIOD_CMD_HELP:
	case IIOD_CMD_TIMEOUT:
	case IIOD_CMD_OPEN:
	case IIOD_CMD_CLOSE:
	case IIOD_CMD_SETTRIG:
	case IIOD_CMD_SET:
		if (data->cmd == IIOD_CMD_OPEN) {
			conn->mask = data->mask;
			if (data->cyclic)
				conn->is_cyclic_buffer = true;
		}
		if (data->cmd == IIOD_CMD_CLOSE)
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
		conn->is_binary_protocol = true;

		conn->res.write_val = 1;
		conn->res.val = 0;
		break;
	case IIOD_CMD_VERSION:
		conn->res.buf.buf = IIOD_VERSION;
		conn->res.buf.len = IIOD_VERSION_LEN;
		break;
	case IIOD_CMD_READ:
	case IIOD_CMD_GETTRIG:
		if (data->cmd == IIOD_CMD_READ)
			ret = desc->ops.read_attr(&ctx, data->device, &attr,
						  conn->payload_buf,
						  conn->payload_buf_len);
		else
			ret = desc->ops.get_trigger(&ctx, data->device,
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
		ret = desc->ops.write_attr(&ctx, data->device, &attr,
					   conn->payload_buf,
					   data->bytes_count);
		conn->nb_buf.len = 0;
		conn->res.val = ret;
		conn->res.write_val = 1;
		break;
	case IIOD_CMD_READBUF:
		conn->res.write_val = 1;
		ret = desc->ops.refill_buffer(&ctx, data->device);
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

// static int32_t iiod_run_binary_cmd(struct iiod_desc *desc, struct iiod_conn_priv *conn){
// 	struct iiod_binary_cmd *cmd = (struct iiod_binary_cmd *)conn->payload_buf;
// 	struct iiod_ctx ctx = IIOD_CTX(desc, conn);

// 	switch(cmd->cmd_id) {
// 	case IIOD_CMD_WRITE:
// 		conn->cmd_data.bytes_count = cmd->length;
// 		memcpy(conn->cmd_data.device, cmd->payload, cmd->length);
// 		break;
// 	case IIOD_CMD_READ:
// 		conn->cmd_data.bytes_count = cmd->length;
// 		memcpy(conn->cmd_data.device, cmd->payload, cmd->length);
// 		break;
// 	default:
// 		return -EINVAL;
// 	}

// 	return 0;
// }

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
	uint8_t len = sizeof(struct iiod_binary_cmd) - conn->parser_idx;

	while ((len > 0) && (conn->parser_idx < IIOD_PARSER_MAX_BUF_SIZE - 1)){
		ret = desc->ops.recv(&ctx, ch, len);
		if (ret == -EAGAIN || ret == 0)
//			return -EAGAIN; // TODO: Fix this.
			continue;

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
	struct iiod_binary_cmd *cmd = (struct iiod_binary_cmd *) conn->parser_buf;
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

	if (conn->parser_idx < sizeof(struct iiod_binary_cmd)) {
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

static int32_t iiod_read_binary_cmd(struct iiod_desc *desc,
				    struct iiod_conn_priv *conn)
{
	static int cnt = 0;
	struct iiod_ctx ctx = {
		.instance = desc->app_instance,
		.conn = conn->conn
	};
	int32_t ret;
	struct iiod_binary_cmd cmd;

	memset(&cmd, 0, sizeof(struct iiod_binary_cmd));

	ret = desc->ops.recv(&ctx, (uint8_t *)&cmd, sizeof(cmd));
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	// if (cmd.length > 0){
	// 	ret = desc->ops.recv(&ctx, (uint8_t *)conn->payload_buf,
	// 			     cmd.length);
	// 	if (NO_OS_IS_ERR_VALUE(ret))
	// 		return ret;
	// }

	//Parse binary command
	conn->cmd_data.op_code = cmd.op;

	memset(&conn->cmd_response_data, 0, sizeof(conn->cmd_response_data));
//	conn->cmd_response_data.client_id = 0;
//	conn->cmd_response_data.dev = 0;
	//sprintf(conn->cmd_data.device, "%d", cmd.dev);

	static uint8_t curr;


	switch (cmd.op) {
		uint8_t len;
//		case IIOD_OP_WRITE_CHN_ATTR:
//		case IIOD_OP_WRITE_ATTR:
//
//			sprintf(conn->cmd_data.attr, "%d", (int16_t)cmd.code);
//			sprintf(conn->cmd_data.channel, "%d", (int16_t)(cmd.code>>16));
//			// Handle binary write command
//			ret = desc->ops.recv(&ctx, (uint8_t *)&len,
//				     1);
//			if (NO_OS_IS_ERR_VALUE(ret))
//				return ret;
//
//			ret = desc->ops.recv(&ctx, (uint8_t *)conn->payload_buf,
//				     len);
//			if (NO_OS_IS_ERR_VALUE(ret))
//				return ret;
//
//			break;
//		case IIOD_OP_READ_ATTR:
//		case IIOD_OP_READ_CHN_ATTR:
//			sprintf(conn->cmd_data.attr, "%d", (int16_t)cmd.code);
//			sprintf(conn->cmd_data.channel, "%d", (int16_t)(cmd.code>>16));
//			// Handle binary read command
//			break;
	//case IIOD_OP_TIMEOUT: //2
	case 1:
	case 2:
	case 4:
		if (!cnt) {
			conn->res.val = 0;
			conn->res.write_val = 1;
			conn->res.second_write = false;
			//conn->state = IIOD_WRITING_CMD_RESULT; //todo: need to update this as well i think

			conn->state = IIOD_READING_LINE; //IIOD_WRITING_BIN_RESPONSE;

			//Send response cmd
			ret = desc->ops.send(&ctx, (uint8_t *)&conn->cmd_response_data,
					     sizeof(conn->cmd_response_data));
			if (NO_OS_IS_ERR_VALUE(ret))
				return ret;

			cnt++;
		} else {
			conn->res.val = desc->xml_len;
			conn->res.write_val = 1;
			conn->res.buf.buf = desc->xml;
			conn->res.buf.len = desc->xml_len;
			conn->state = IIOD_READING_LINE; //IIOD_WRITING_BIN_RESPONSE;
			conn->res.buf.idx = 0;

			conn->cmd_response_data.code = desc->xml_len; //0
			conn->res.second_write = true;

			//Send response cmd
			ret = desc->ops.send(&ctx, (uint8_t *)&conn->cmd_response_data,
					     sizeof(conn->cmd_response_data));
			if (NO_OS_IS_ERR_VALUE(ret))
				return ret;

			if (conn->res.second_write) {
				/* Send buf from result. Non blocking */
				if (conn->res.buf.buf &&
				    conn->res.buf.idx < conn->res.buf.len) {
					ret = rw_iiod_buff(desc, conn, &conn->res.buf,
							   IIOD_WR);
					if (NO_OS_IS_ERR_VALUE(ret))
						return ret;
				}
			}
		}

		//conn->cmd_response_data.code = 0; //can be given though to mask value recieved as client expects something
		break;
	//case IIOD_OP_PRINT: //1
//		case 8:
//			conn->res.val= desc->xml_len;
//			conn->res.write_val = 1;
//			conn->res.buf.buf = desc->xml;
//			conn->res.buf.len = desc->xml_len;
//			conn->state = IIOD_READING_LINE; //IIOD_WRITING_BIN_RESPONSE;
//			conn->res.buf.idx = 0;
//
//			conn->cmd_response_data.code = desc->xml_len; //0
//			conn->res.second_write = true;
//
//			//Send response cmd
//			ret = desc->ops.send(&ctx, (uint8_t *)&conn->cmd_response_data,
//					     sizeof(conn->cmd_response_data));
//			if (NO_OS_IS_ERR_VALUE(ret))
//				return ret;
//
//			if(conn->res.second_write){
//			/* Send buf from result. Non blocking */
//				if (conn->res.buf.buf &&
//					conn->res.buf.idx < conn->res.buf.len) {
//					ret = rw_iiod_buff(desc, conn, &conn->res.buf,
//							IIOD_WR);
//					if (NO_OS_IS_ERR_VALUE(ret))
//						return ret;
//				}
//			}
//
//			break;
//		case IIOD_OP_CREATE_BUFFER:
//			ret = desc->ops.recv(&ctx, (uint8_t *)&conn->cmd_data.mask, 4);
//			if (NO_OS_IS_ERR_VALUE(ret))
//				return ret;
//			conn->res.val = 0;
//			conn->res.write_val = 1;
//			conn->state = IIOD_WRITING_BIN_RESPONSE; //todo: hopefully res.buf is vacated/emptied
//
//			conn->cmd_response_data.code = 0; //can be given though to mask value recieved as client expects something
//			break;
//		case IIOD_OP_CREATE_BLOCK: // 4 times create block
//			//take block index from (int16_t)(cmd.code>>16)
//			sprintf(conn->cmd_data.block_id[curr], "%d", (int16_t)(cmd.code>>16));
//
//			//  receive block size
//			ret = desc->ops.recv(&ctx, (uint8_t *)&conn->cmd_data.block_size[curr], 8);
//			if (NO_OS_IS_ERR_VALUE(ret))
//				return ret;
//
//			curr = (curr + 1) % 4;
//			conn->res.val = 0;
//			conn->res.write_val = 1;
//			conn->state = IIOD_WRITING_BIN_RESPONSE;
//
//			conn->cmd_response_data.code = 0; //for precaution
//			break;
//
//		case IIOD_OP_TRANSFER_BLOCK:
//			//take block index from (int16_t)(cmd.code>>16)
//			//op for enqueue block
//			sprintf(conn->cmd_data.block_id[curr], "%d", (int16_t)(cmd.code>>16)); //todo: see
//
//			//  receive block size
//			ret = desc->ops.recv(&ctx, (uint8_t *)&conn->cmd_data.bytes_size[curr], 8);
//			if (NO_OS_IS_ERR_VALUE(ret))
//				return ret;
//
//
//			conn->state = IIOD_WRITING_BIN_RESPONSE;//IIOD_READING_LINE; //first send cmd then transfer block
//
//			conn->cmd_response_data.code = conn->cmd_data.bytes_size[curr];
//			curr = (curr + 1) % 4;
//			//transfer block to client
//			break;
//
//		case IIOD_OP_ENABLE_BUFFER:
//			// enable/start buffer streaming
//			// call submit buffer here
//			// fill some dummy data to the buffer
//
//			ret = do_read_buff(desc, conn); //todo: not sure of enabling and others
//			if (NO_OS_IS_ERR_VALUE(ret))
//				return ret;
//
//			conn->state = IIOD_LINE_DONE;
//
//			//ret = rw_iiod_buff(desc, conn, &conn->nb_buf, IIOD_WR);
//
//			break;
//
//		case IIOD_OP_RETRY_DEQUEUE_BLOCK:
//			// dequeue block from the buffer
//			// fill some dummy data to the buffer
//			// only when previous deque fails
//			break;

	// Add cases for other binary commands
	default:
		return -EINVAL;
	}

	return 0;
}

//static int32_t iiod_read_binary_cmd_new(struct iiod_desc *desc,
//					struct iiod_conn_priv *conn)
//{
//
//	static uint8_t buffer[1024];
//	static int cnt = 0;
//	struct iiod_ctx ctx = {
//		.instance = desc->app_instance,
//		.conn = conn->conn
//	};
//	int32_t ret;
//	struct iiod_binary_cmd cmd;
//	static uint8_t *blocks[16];
//	static struct iio_stream *stream;
//	static struct no_os_list_desc *event_list;
//	static struct lf256fifo *fifo_stream;
//	struct iiod_event_desc *evt_data;
//	static struct iiod_buff buff;
//
//	struct iiod_event_data data = {
//		.channel_id = 0,
//		.diff_channel_id = 0,
//		.channel_type = IIO_VOLTAGE,
//		.modifier = IIO_NO_MOD,
//		.event_dir = IIO_EV_DIR_NONE,
//		.is_differential = 0,
//		.event_type = IIO_EV_TYPE_THRESH,
//		.timestamp = 1
//	};
//
//	memset(&cmd, 0, sizeof(struct iiod_binary_cmd));
//
//#if 0
//	ret = desc->ops.recv(&ctx, (uint8_t *)&cmd, sizeof(cmd));
//	if (NO_OS_IS_ERR_VALUE(ret))
//		return ret;
//
//	conn->cmd_data.op_code = cmd.op;
//#else
//	ret = iiod_read_cmd_header(desc, conn);
//	if (NO_OS_IS_ERR_VALUE(ret))
//		return ret;
//
//	memcpy(&cmd, &conn->cmd_data_bin, sizeof(struct iiod_binary_cmd));
//#endif
//
//	// if (cmd.length > 0){
//	// 	ret = desc->ops.recv(&ctx, (uint8_t *)conn->payload_buf,
//	// 			     cmd.length);
//	// 	if (NO_OS_IS_ERR_VALUE(ret))
//	// 		return ret;
//	// }
//
//
//
//	memset(&conn->cmd_response_data, 0, sizeof(conn->cmd_response_data));
////	conn->cmd_response_data.client_id = 0;
////	conn->cmd_response_data.dev = 0;
//	//sprintf(conn->cmd_data.device, "%d", cmd.dev);
//
//	static uint8_t cl_id = 1;
//	static uint8_t buf_id;
//	struct iiod_event_desc evt_data_key = {.client_id = cmd.client_id};
//
//	switch (cmd.op) {
//		//switch(conn->cmd_data_bin.op) {
//		uint8_t len;
//	case IIOD_OP_TIMEOUT: //2: //timeout
//		conn->res.val = 0;
//		conn->res.write_val = 1;
//		conn->res.second_write = false;
//		//conn->state = IIOD_WRITING_CMD_RESULT; //todo: need to update this as well i think
//
//		conn->state = IIOD_READING_LINE; //IIOD_WRITING_BIN_RESPONSE;
//
//		//Send response cmd
//		ret = desc->ops.send(&ctx, (uint8_t *)&conn->cmd_response_data,
//				     sizeof(conn->cmd_response_data));
//		if (NO_OS_IS_ERR_VALUE(ret))
//			return ret;
//
//		break;
//
//	case IIOD_OP_PRINT: //print 1
//		conn->res.val = desc->xml_len;
//		conn->res.write_val = 1;
//		conn->res.buf.buf = desc->xml;
//		conn->res.buf.len = desc->xml_len;
//		conn->state = IIOD_READING_LINE; //IIOD_WRITING_BIN_RESPONSE;
//		conn->res.buf.idx = 0;
//
//		conn->cmd_response_data.code = desc->xml_len; //0
//		conn->res.second_write = true;
//
//		//Send response cmd
//		ret = desc->ops.send(&ctx, (uint8_t *)&conn->cmd_response_data,
//				     sizeof(conn->cmd_response_data));
//		if (NO_OS_IS_ERR_VALUE(ret))
//			return ret;
//
//		if (conn->res.second_write) {
//			/* Send buf from result. Non blocking */
//			if (conn->res.buf.buf &&
//			    conn->res.buf.idx < conn->res.buf.len) {
//				do {
//					ret = rw_iiod_buff(desc, conn, &conn->res.buf,
//							   IIOD_WR);
//				} while (ret == -EAGAIN);
//				if (NO_OS_IS_ERR_VALUE(ret))
//					return ret;
//			}
//		}
//
//
//		//conn->cmd_response_data.code = 0; //can be given though to mask value recieved as client expects something
//		break;
//
//	case 8:
//		break;
//
//	case 5:
//		break;
//
//	case IIOD_OP_CREATE_BUFFER: //13: //create buffer
//		// we currently support single buffer applications with multiple blocks
//
//#if 0
//		ret = desc->ops.recv(&ctx, (uint8_t *)&conn->cmd_data.mask,
//				     4); //read mask in next header
//		if (NO_OS_IS_ERR_VALUE(ret))
//			return ret;
//#else
//		//get ch-mask from this cmd payload
//		ret = iiod_read_generic(desc, conn, (uint8_t *)&conn->cmd_data.mask,
//					4);
//		if (NO_OS_IS_ERR_VALUE(ret))
//			return ret;
//#endif
//
//		conn->res.val = 0;
//		conn->res.write_val = 1;
//		conn->state = IIOD_READING_LINE; //IIOD_WRITING_BIN_RESPONSE;
//
//		//Send response cmd
//		ret = desc->ops.send(&ctx, (uint8_t *)&conn->cmd_response_data,
//				     sizeof(conn->cmd_response_data));
//		if (NO_OS_IS_ERR_VALUE(ret))
//			return ret;
//		break;
//
//	case IIOD_OP_CREATE_BLOCK: //17: //create block
//		/* get block index, client id from cmd and block size from cmd payload */
//		//take block index from (cmd.code>>16)
//		sprintf(&conn->cmd_data.block_id[curr], "%d", (int16_t)(cmd.code >> 16));
//
//		//  receive block size
//#if 0
//		ret = desc->ops.recv(&ctx, (uint8_t *)&conn->cmd_data.block_size[curr], 8);
//		if (NO_OS_IS_ERR_VALUE(ret))
//			return ret;
//#else
//		//get corresponding block size
//		ret = iiod_read_generic(desc, conn, (uint8_t *)&conn->cmd_data.block_size[curr],
//					8);
//		if (NO_OS_IS_ERR_VALUE(ret))
//			return ret;
//#endif
//
//		conn->res.val = 0;
//		conn->res.write_val = 1;
//		conn->state = IIOD_READING_LINE; //IIOD_WRITING_BIN_RESPONSE;
//
//		/* Send response with block size as code created and cl id */
//		conn->cmd_response_data.code = conn->cmd_data.block_size[curr];
//
//		//new responder io is created in the client..  so create a corresponding one in here
//		conn->cmd_response_data.client_id = cmd.client_id;
//
//		//malloc buffers of size given
//		if (!curr) {
//			stream = calloc(1, sizeof(*stream));
//			if (!stream)
//				return -ENOMEM;
//
//			stream->blocks = calloc(MAX_NUM_BLOCKS, sizeof(*stream->blocks));
//			if (!stream->blocks)
//				return -ENOMEM;
//
//			ret = lf256fifo_init(&fifo_stream);
//			if (NO_OS_IS_ERR_VALUE(ret))
//				return ret;
//		}
//
//		stream->blocks[curr] = calloc(1, sizeof(*stream->blocks[curr]));
//		if (!stream->blocks[curr])
//			return -ENOMEM;
//		stream->blocks[curr]->cl_id = cmd.client_id;
//		stream->blocks[curr]->size = conn->cmd_data.block_size[curr];
//		stream->blocks[curr]->data = (uint8_t *)calloc(1,
//					     conn->cmd_data.block_size[curr] * sizeof(uint8_t));
//		if (!stream->blocks[curr]->data)
//			return -ENOMEM;
//
//		stream->nb_blocks++;
//
//		memset(stream->blocks[curr]->data, (curr + 1) << 4,
//		       conn->cmd_data.block_size[curr]);
//
//		//Send response cmd
//		ret = desc->ops.send(&ctx, (uint8_t *)&conn->cmd_response_data,
//				     sizeof(conn->cmd_response_data));
//		if (NO_OS_IS_ERR_VALUE(ret))
//			return ret;
//
//		curr = (curr + 1) % MAX_NUM_BLOCKS;
//		break;
//
//	case IIOD_OP_TRANSFER_BLOCK: //19: //transfer op
//		/* get block index, client id from cmd and block size from cmd payload */
//		//take block index from (cmd.code>>16)
//		uint8_t wr = (uint8_t)(int16_t)(cmd.code >> 16);
//		//sprintf(&wr, "%d", (int16_t)(cmd.code >> 16));
//
//		//enqueue buf idx
//		lf256fifo_write(fifo_stream, wr);
//		//  receive block size
//#if 0
//		ret = desc->ops.recv(&ctx, (uint8_t *)&conn->cmd_data.bytes_size[curr_1], 8);
//		if (NO_OS_IS_ERR_VALUE(ret))
//			return ret;
//#else
//		//get corresponding requested bytes size //TODO: see what to do with these
//		ret = iiod_read_generic(desc, conn,
//					(uint8_t *)&conn->cmd_data.bytes_size[curr_1],
//					8);
//		if (NO_OS_IS_ERR_VALUE(ret))
//			return ret;
//#endif
//
//		if (stream->started) {
//			ret = lf256fifo_read(fifo_stream, &buf_id);
//			if (NO_OS_IS_ERR_VALUE(ret))
//				return ret;
//			conn->cmd_response_data.client_id = stream->blocks[buf_id]->cl_id;
//			conn->cmd_response_data.code = stream->blocks[buf_id]->size;
//			//before payload.. header to be sent again with corresponding cl id and code
//			ret = desc->ops.send(&ctx, (uint8_t *)&conn->cmd_response_data,
//					     sizeof(conn->cmd_response_data));
//
//			//Now send the response back buffer payload req bytes
//			// ret = desc->ops.send(&ctx, stream->blocks[buf_id]->data,
//			// 		     stream->blocks[buf_id]->size);
//			// if (NO_OS_IS_ERR_VALUE(ret))
//			// 	return ret;
//
//			buff.buf = stream->blocks[buf_id]->data;
//			buff.len = stream->blocks[buf_id]->size;
//			buff.idx = 0;
//
//			do {
//				ret = rw_iiod_buff(desc, conn,
//						   &buff,
//						   IIOD_WR);
//			} while (ret == -EAGAIN);
//			if (NO_OS_IS_ERR_VALUE(ret))
//				return ret;
//		}
//		//curr_1++;
//		conn->state = IIOD_READING_LINE;
//
//		break;
//
//	case IIOD_OP_ENABLE_BUFFER: //15: //start streaming
//		conn->state = IIOD_READING_LINE; //IIOD_WRITING_BIN_RESPONSE;
//
//		//Send response cmd
//		ret = desc->ops.send(&ctx, (uint8_t *)&conn->cmd_response_data,
//				     sizeof(conn->cmd_response_data));
//		if (NO_OS_IS_ERR_VALUE(ret))
//			return ret;
//
//		conn->res.val = 0; //dummy op
//		stream->started = true;
//
//		//memset(buffer, 0x10, sizeof(buffer));
//
//
//		//transfer here
//		for (int i = 0; i < stream->nb_blocks; i++) {
//
//			ret = lf256fifo_read(fifo_stream, &buf_id);
//			if (NO_OS_IS_ERR_VALUE(ret))
//				return ret;
//			conn->cmd_response_data.client_id = stream->blocks[buf_id]->cl_id;
//			conn->cmd_response_data.code = stream->blocks[buf_id]->size;
//			//before payload.. header to be sent again with corresponding cl id and code
//			ret = desc->ops.send(&ctx, (uint8_t *)&conn->cmd_response_data,
//					     sizeof(conn->cmd_response_data));
//
//			//Now send the response back buffer payload req bytes
//			// ret = desc->ops.send(&ctx, stream->blocks[buf_id]->data,
//			// 		     stream->blocks[buf_id]->size);
//			// if (NO_OS_IS_ERR_VALUE(ret))
//			// 	return ret;
//
//			buff.buf = stream->blocks[buf_id]->data;
//			buff.len = stream->blocks[buf_id]->size;
//			buff.idx = 0;
//
//			do {
//				ret = rw_iiod_buff(desc, conn,
//						   &buff,
//						   IIOD_WR);
//			} while (ret == -EAGAIN);
//			if (NO_OS_IS_ERR_VALUE(ret))
//				return ret;
//
//		}
//
//		break;
//
//	case IIOD_OP_FREE_BLOCK:
//		//Dealloc blocks created
//		if (stream && stream->blocks) {
//			for (int i = 0; i < stream->nb_blocks; i++) {
//				if (stream->blocks[i]) {
//					free(stream->blocks[i]->data);
//					free(stream->blocks[i]);
//				}
//			}
//			free(stream->blocks);
//			stream->blocks = NULL;
//			stream->nb_blocks = 0;
//		}
//
//		//send response.. calls 4 times for all blocks
//
//		//Send response cmd
//		ret = desc->ops.send(&ctx, (uint8_t *)&conn->cmd_response_data,
//				     sizeof(conn->cmd_response_data));
//		if (NO_OS_IS_ERR_VALUE(ret))
//			return ret;
//
//		break;
//
//	case IIOD_OP_FREE_BUFFER:
//		//dealloc buffer
//
//		//Send response cmd
//		ret = desc->ops.send(&ctx, (uint8_t *)&conn->cmd_response_data,
//				     sizeof(conn->cmd_response_data));
//		if (NO_OS_IS_ERR_VALUE(ret))
//			return ret;
//
//		break;
//
//	case IIOD_OP_RETRY_DEQUEUE_BLOCK:
//		// dequeue block from the buffer
//		// fill some dummy data to the buffer
//		// only when previous deque fails
//		break;
//
//	case IIOD_OP_CREATE_EVSTREAM:
//		/* Create a list of events, if not done already */
//		if (!event_list) {
//			ret = no_os_list_init(&event_list, NO_OS_LIST_PRIORITY_LIST,
//					      iio_event_cmp);
//			if (NO_OS_IS_ERR_VALUE(ret))
//				return ret;
//		}
//
//		/* Find the element for the Client ID */
//		ret = no_os_list_read_find(event_list, (void **)&evt_data, &evt_data_key);
//
//		/* If no event data is found insert a new one */
//		if (ret) {
//			evt_data = (struct iiod_event_desc *) no_os_calloc(1, sizeof(*evt_data));
//			if (!evt_data)
//				return -ENOMEM;
//
//			evt_data->client_id = cmd.client_id;
//			evt_data->event_data = NULL;
//
//			ret = no_os_list_add_last(event_list, evt_data);
//			if (ret)
//				goto free_action;
//		} else {
//			return -EINVAL;
//		}
//
//		conn->cmd_response_data.client_id = cmd.client_id;
//		//Send response cmd
//		ret = desc->ops.send(&ctx, (uint8_t *)&conn->cmd_response_data,
//				     sizeof(conn->cmd_response_data));
//		if (NO_OS_IS_ERR_VALUE(ret))
//			return ret;
//		break;
//free_action:
//		no_os_free(evt_data);
//		return -EINVAL;
//	case IIOD_OP_FREE_EVSTREAM:
//		struct iiod_event_desc discard_evt_key = {.client_id = cmd.client_id};
//		struct iiod_event_desc *discard_evt;
//		if (!event_list) {
//			return -EINVAL;
//		}
//
//		/* Find the element for the Client ID */
//		ret = no_os_list_read_find(event_list, (void **)&discard_evt, &discard_evt_key);
//		if (NO_OS_IS_ERR_VALUE(ret))
//			return ret;
//
//		while (discard_evt->event_data) {
//			discard_evt->event_data = no_os_fifo_remove(discard_evt->event_data);
//		}
//
//		no_os_free(discard_evt);
//
//		//Send response cmd
//		ret = desc->ops.send(&ctx, (uint8_t *)&conn->cmd_response_data,
//				     sizeof(conn->cmd_response_data));
//		if (NO_OS_IS_ERR_VALUE(ret))
//			return ret;
//		break;
//	case IIOD_OP_READ_EVENT:
//		if (!event_list) {
//			return -EINVAL;
//		}
//
//		/* Find the element for the Client ID */
//		ret = no_os_list_read_find(event_list, (void **)&evt_data, &evt_data_key);
//		if (NO_OS_IS_ERR_VALUE(ret))
//			return ret;
//
//		no_os_fifo_insert(&evt_data->event_data, &data, sizeof(data));
//
//		if (evt_data->event_data != NULL) {
//			conn->cmd_response_data.client_id = cmd.client_id;
//			conn->cmd_response_data.code = evt_data->event_data->len;
//			//Send response cmd
//			ret = desc->ops.send(&ctx, (uint8_t *)&conn->cmd_response_data,
//					     sizeof(conn->cmd_response_data));
//			if (NO_OS_IS_ERR_VALUE(ret))
//				return ret;
//
//			//Send response payload
//			ret = desc->ops.send(&ctx, (uint8_t *)evt_data->event_data->data,
//					     evt_data->event_data->len);
//			if (NO_OS_IS_ERR_VALUE(ret))
//				return ret;
//
//			evt_data->event_data = no_os_fifo_remove(evt_data->event_data);
//		} else {
//			evt_data->event_read_count++;
//		}
//
//		break;
//	default:
//		return -EINVAL;
//	}
//
//	return 0;
//}


















static int32_t iiod_run_cmd_new(struct iiod_desc *desc,
					struct iiod_conn_priv *conn)
{
	static uint8_t buffer[1024];
	static int cnt = 0;
	struct iiod_ctx ctx = IIOD_CTX(desc, conn);
	struct iiod_binary_cmd *command = (struct iiod_binary_cmd *) &conn->parser_buf;
	struct comand_desc *data = &conn->cmd_data;
	struct iiod_attr attr = {
		.type = data->type,
		.name = NULL,
		.channel = NULL,
		.idx = -1,
		.ch_id = -1
	};

	int32_t ret;
	int32_t i;
	static uint8_t *blocks[16];
	static struct iio_stream *stream;
	static struct lf256fifo *fifo_stream;
	struct iiod_event_desc *event_desc;
	static struct iiod_buff buff;

	struct iiod_event_data evt_data = {
		.channel_id = 0,
		.diff_channel_id = 0,
		.channel_type = IIO_VOLTAGE,
		.modifier = IIO_NO_MOD,
		.event_dir = IIO_EV_DIR_NONE,
		.is_differential = 0,
		.event_type = IIO_EV_TYPE_THRESH,
		.timestamp = 1
	};

	memset(&conn->cmd_response_data, 0, sizeof(conn->cmd_response_data));

	static uint8_t cl_id = 1;
	static uint8_t buf_id;
	struct iiod_event_desc event_desc_key = {.client_id = command->client_id};

	switch (command->op) {
		// uint8_t len;
	case IIOD_OP_PRINT:
		conn->res.write_val = 1;
		conn->res.buf.buf = desc->xml;
		conn->res.buf.len = desc->xml_len;
		conn->res.buf.idx = 0;
		conn->state = IIOD_WRITING_BIN_RESPONSE;

		conn->cmd_response_data.code = desc->xml_len; //0
		conn->res.second_write = true;
		break;
	case IIOD_OP_TIMEOUT:
		// conn->res.val = 0;
		conn->res.write_val = 1;
		conn->res.second_write = false;
		conn->state = IIOD_WRITING_BIN_RESPONSE;
		break;

	case IIOD_OP_READ_ATTR:
	case IIOD_OP_READ_DBG_ATTR:
	case IIOD_OP_READ_BUF_ATTR:
	case IIOD_OP_READ_CHN_ATTR:
		attr.idx = data->binary_data.attr;
		if (command->op == IIOD_OP_READ_CHN_ATTR) {
			attr.ch_id = data->binary_data.channel;
		}
		ret = desc->ops.read_attr(&ctx, &data->binary_data.device, &attr,
					  conn->payload_buf,
					  conn->payload_buf_len);
		conn->res.write_val = 1;
		conn->res.buf.idx = 0;
		conn->res.second_write = false;
		conn->cmd_response_data.code = ret;
		if (ret) {
			conn->res.buf.buf = conn->payload_buf;
			conn->res.buf.len = ret;
			conn->res.second_write = true;
		}
		conn->state = IIOD_WRITING_BIN_RESPONSE;
		break;
	case IIOD_OP_WRITE_ATTR:
	case IIOD_OP_WRITE_DBG_ATTR:
	case IIOD_OP_WRITE_BUF_ATTR:
	case IIOD_OP_WRITE_CHN_ATTR:
		break;

	case IIOD_OP_CREATE_BUFFER: 
		conn->res.val = 0;
		conn->res.write_val = 1;
		conn->res.second_write = false;
		conn->state = IIOD_WRITING_BIN_RESPONSE;
		break;

	case IIOD_OP_CREATE_BLOCK:
		conn->res.val = 0;
		conn->res.write_val = 1;
		conn->res.second_write = false;
		conn->state = IIOD_WRITING_BIN_RESPONSE;

		/* Send response with block size as code created and cl id */
		conn->cmd_response_data.code = conn->cmd_data.block_size[curr];

		//new responder io is created in the client..  so create a corresponding one in here
		conn->cmd_response_data.client_id = command->client_id;

		//malloc buffers of size given
		if (!curr) {
			stream = calloc(1, sizeof(*stream));
			if (!stream)
				return -ENOMEM;

			stream->blocks = calloc(MAX_NUM_BLOCKS, sizeof(*stream->blocks));
			if (!stream->blocks)
				return -ENOMEM;

			ret = lf256fifo_init(&fifo_stream);
			if (NO_OS_IS_ERR_VALUE(ret))
				return ret;
		}

		stream->blocks[curr] = calloc(1, sizeof(*stream->blocks[curr]));
		if (!stream->blocks[curr])
			return -ENOMEM;
		stream->blocks[curr]->cl_id = command->client_id;
		stream->blocks[curr]->size = conn->cmd_data.block_size[curr];
		stream->blocks[curr]->data = (uint8_t *)calloc(1,
					     conn->cmd_data.block_size[curr] * sizeof(uint8_t));
		if (!stream->blocks[curr]->data)
			return -ENOMEM;

		stream->nb_blocks++;

		//Dummy data for transfers
		memset(stream->blocks[curr]->data, (curr + 1) << 4,
		       conn->cmd_data.block_size[curr]);

		curr = (curr + 1) % MAX_NUM_BLOCKS;
		break;

	case IIOD_OP_TRANSFER_BLOCK:
		/* get block index, client id from cmd and block size from cmd payload */
		//take block index from (cmd.code>>16)
		uint8_t wr = (uint8_t)(int16_t)(command->code >> 16);

		//enqueue buf idx
		lf256fifo_write(fifo_stream, wr);

		if (stream->started) {
			ret = lf256fifo_read(fifo_stream, &buf_id);
			if (NO_OS_IS_ERR_VALUE(ret))
				return ret;
			conn->cmd_response_data.client_id = stream->blocks[buf_id]->cl_id;
			conn->cmd_response_data.code = stream->blocks[buf_id]->size;
			//before payload.. header to be sent again with corresponding cl id and code

			//SWADO
			ret = desc->ops.send(&ctx, (uint8_t *)&conn->cmd_response_data,
					     sizeof(conn->cmd_response_data));

			//Now send the response back buffer payload req bytes
			// ret = desc->ops.send(&ctx, stream->blocks[buf_id]->data,
			// 		     stream->blocks[buf_id]->size);
			// if (NO_OS_IS_ERR_VALUE(ret))
			// 	return ret;

			buff.buf = stream->blocks[buf_id]->data;
			buff.len = stream->blocks[buf_id]->size;
			buff.idx = 0;

			do {
				ret = rw_iiod_buff(desc, conn,
						   &buff,
						   IIOD_WR);
			} while (ret == -EAGAIN);
			if (NO_OS_IS_ERR_VALUE(ret))
				return ret;
		}
		//curr_1++;
		conn->state = IIOD_READING_LINE;

		break;

	case IIOD_OP_ENABLE_BUFFER:
		conn->state = IIOD_READING_LINE; //IIOD_WRITING_BIN_RESPONSE;

		//Send response cmd
		ret = desc->ops.send(&ctx, (uint8_t *)&conn->cmd_response_data,
				     sizeof(conn->cmd_response_data));
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;

		conn->res.val = 0; //dummy op
		stream->started = true;

		//memset(buffer, 0x10, sizeof(buffer));

		//transfer here
		for (int i = 0; i < stream->nb_blocks; i++) {

			ret = lf256fifo_read(fifo_stream, &buf_id);
			if (NO_OS_IS_ERR_VALUE(ret))
				return ret;
			conn->cmd_response_data.client_id = stream->blocks[buf_id]->cl_id;
			conn->cmd_response_data.code = stream->blocks[buf_id]->size;
			//before payload.. header to be sent again with corresponding cl id and code
			ret = desc->ops.send(&ctx, (uint8_t *)&conn->cmd_response_data,
					     sizeof(conn->cmd_response_data));

			buff.buf = stream->blocks[buf_id]->data;
			buff.len = stream->blocks[buf_id]->size;
			buff.idx = 0;

			do {
				ret = rw_iiod_buff(desc, conn,
						   &buff,
						   IIOD_WR);
			} while (ret == -EAGAIN);
			if (NO_OS_IS_ERR_VALUE(ret))
				return ret;
		}
		break;

	case IIOD_OP_FREE_BLOCK:
		//Dealloc blocks created
		conn->res.val = 0;
		conn->res.write_val = 1;
		conn->res.second_write = false;
		conn->state = IIOD_WRITING_BIN_RESPONSE;
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
		conn->res.val = 0;
		conn->res.write_val = 1;
		conn->res.second_write = false;
		conn->state = IIOD_WRITING_BIN_RESPONSE;
		//dealloc buffer

		break;

	case IIOD_OP_RETRY_DEQUEUE_BLOCK:
		// dequeue block from the buffer
		// fill some dummy data to the buffer
		// only when previous deque fails
		break;

	case IIOD_OP_CREATE_EVSTREAM:
		/* Create a list of events, if not done already */
		if (!conn->cmd_data.event) {
			ret = no_os_list_init(&conn->cmd_data.event, NO_OS_LIST_PRIORITY_LIST,
					      iio_event_cmp);
			if (NO_OS_IS_ERR_VALUE(ret)) {
				// TODO: Send a error response
				break;
			}
		}

		/* Find the element for the Client ID */
		ret = no_os_list_read_find(conn->cmd_data.event, (void **)&event_desc, &event_desc_key);

		/* If no event data is found insert a new one */
		if (ret) {
			event_desc = (struct iiod_event_desc *) no_os_calloc(1, sizeof(*event_desc));
			if (!event_desc) {
				ret = -ENOMEM;
				goto fail;
			}

			event_desc->client_id = command->client_id;
			event_desc->event_read_count = 0;
			ret = lf256fifo_init(&event_desc->event_data);
			if (!event_desc->event_data) {
				ret = -ENOMEM;
				goto free_event_desc;
			}

			ret = no_os_list_add_last(conn->cmd_data.event, event_desc);
			if (ret)
				goto free_event_data;
		} else {
			return -EINVAL;
		}

		conn->cmd_response_data.client_id = command->client_id;

		conn->res.val = 0;
		conn->res.write_val = 1;
		conn->res.second_write = false;
		conn->state = IIOD_WRITING_BIN_RESPONSE;
		break;
free_event_data:
		lf256fifo_remove(event_desc->event_data);
free_event_desc:
		no_os_free(event_desc);
fail:
		// TODO: Send a error response
		return ret;
	case IIOD_OP_FREE_EVSTREAM:
		if (!conn->cmd_data.event) {
			// TODO: Send a error response
			break;
		}

		/* Find the element for the Client ID */
		ret = no_os_list_read_find(conn->cmd_data.event, (void **)&event_desc, &event_desc_key);
		if (NO_OS_IS_ERR_VALUE(ret)) {
			// TODO: Send a error response
			break;
		}

		lf256fifo_remove(event_desc->event_data);
		no_os_free(event_desc);

		conn->res.val = 0;
		conn->res.write_val = 1;
		conn->res.second_write = false;
		conn->state = IIOD_WRITING_BIN_RESPONSE;
		break;
	case IIOD_OP_READ_EVENT:
		if (!conn->cmd_data.event)  {
			// TODO: Send a error response
			break;
		}

		/* Find the element for the Client ID */
		ret = no_os_list_read_find(conn->cmd_data.event, (void **)&event_desc, &event_desc_key);
		if (NO_OS_IS_ERR_VALUE(ret)) {
			// TODO: Send a error response
			break;
		}

		for (i = 0; (i < sizeof(struct iiod_event_data)) && !ret; i++) {
			ret = lf256fifo_write(event_desc->event_data, ((char *)&evt_data)[i]);
		}

		if (!lf256fifo_is_empty(event_desc->event_data)) {
			conn->cmd_response_data.client_id = command->client_id;
			conn->cmd_response_data.code = sizeof(struct iiod_event_data);

			for (i = 0; (i < sizeof(struct iiod_event_data)) && !ret; i++) {
				ret = lf256fifo_read(event_desc->event_data, conn->event_data + i);
			}

			conn->res.val = 0;
			conn->res.write_val = 1;
			conn->res.second_write = true;

			conn->res.buf.buf = (char *)conn->event_data;
			conn->res.buf.len = sizeof(struct iiod_event_data);
			conn->res.buf.idx = 0;
			conn->state = IIOD_WRITING_BIN_RESPONSE;
		} else {
			event_desc->event_read_count++;
		}

		break;
	default:
		return -EINVAL;
	}

	return 0;
}


static int32_t iiod_run_state_bin(struct iiod_desc *desc,
			      struct iiod_conn_priv *conn)
{

	struct iiod_ctx ctx = IIOD_CTX(desc, conn);
	int32_t ret;

	switch (conn->state) {
		case IIOD_READING_LINE:
			ret = iio_read_command(desc, conn);
			if (NO_OS_IS_ERR_VALUE(ret))
				return ret;
			
			/* Fill struct comand_desc with data from line. No I/O */
			ret = iiod_parse_command(conn->parser_buf, &conn->cmd_data);
			if (NO_OS_IS_ERR_VALUE(ret)) {
				/* Parsing line failed */
				conn->res.write_val = 1;
				conn->res.val = ret;
				conn->state = IIOD_WRITING_CMD_RESULT;
			} else if (conn->cmd_data.cmd == IIOD_CMD_WRITE) {
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

//		conn->state = IIOD_WRITING_BIN_RESPONSE;
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
	struct iiod_ctx ctx = {
		.instance = desc->app_instance,
		.conn = conn->conn
	};
	int32_t ret;

	switch (conn->state) {
	case IIOD_READING_LINE:
		if (!conn->is_binary_protocol) {
			/* Read input data until \n. I/O Calls */
			ret = iiod_read_line(desc, conn);
			if (NO_OS_IS_ERR_VALUE(ret))
				return ret;

			/* Fill struct comand_desc with data from line. No I/O */
			ret = iiod_parse_line(conn->parser_buf, &conn->cmd_data,
					      &conn->strtok_ctx);
		} else {
			ret = iiod_run_state_bin(desc, conn);
			// ret = iio_read_command(desc, conn);
			// if (NO_OS_IS_ERR_VALUE(ret))
			// 	return ret;
			
			// /* Fill struct comand_desc with data from line. No I/O */
			// ret = iiod_parse_command(conn->parser_buf, &conn->cmd_data);
//			ret = iiod_read_binary_cmd_new(desc, conn);
//			if (NO_OS_IS_ERR_VALUE(ret))
//				return ret;
			//conn->state = IIOD_READING_LINE; //IIOD_RUNNING_CMD;
			//}
		}
		if (NO_OS_IS_ERR_VALUE(ret)) {
			/* Parsing line failed */
			conn->res.write_val = 1;
			conn->res.val = ret;
			conn->state = IIOD_WRITING_CMD_RESULT;
		} else if (conn->cmd_data.cmd == IIOD_CMD_WRITE) {
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

		if (conn->is_binary_protocol) {
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

		if (conn->cmd_data.cmd != IIOD_CMD_READBUF &&
		    conn->cmd_data.cmd != IIOD_CMD_WRITEBUF) {
			if (conn->is_cyclic_buffer && conn->cmd_data.cmd != IIOD_CMD_OPEN)
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
			ret = desc->ops.send(&ctx, (uint8_t *)&conn->cmd_response_data,
					     sizeof(conn->cmd_response_data));
			if (NO_OS_IS_ERR_VALUE(ret))
				return ret;

			if (conn->res.second_write) {
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

		conn->state =  IIOD_READING_LINE; //IIOD_LINE_DONE;
		return 0;

	case IIOD_RW_BUF:
		/* IIOD_CMD_READBUF and IIOD_CMD_WRITEBUF special case */
		/* Non blocking read/write until all data is processed */
		if (conn->cmd_data.cmd == IIOD_CMD_READBUF)
			ret = do_read_buff(desc, conn);
		else {
			ret = do_write_buff(desc, conn);
			if (ret == 0) {
				conn->res.write_val = 1;
				ret = desc->ops.push_buffer(&ctx,
							    conn->cmd_data.device);
				if (NO_OS_IS_ERR_VALUE(ret)) {
					conn->res.val = ret;
					conn->state = IIOD_LINE_DONE;

					return 0;
				}
				memset(&conn->res.buf, 0, sizeof(conn->res.buf));
				conn->res.val = conn->cmd_data.bytes_count;
				conn->cmd_data.cmd = IIOD_CMD_PRINT;
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
					    conn->cmd_data.device);
		/* If an error was encountered, close connection */
		if (NO_OS_IS_ERR_VALUE(ret)) {
			conn->res.val = ret;
			desc->ops.close(&ctx, conn->cmd_data.device);
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
		if (!NO_OS_IS_ERR_VALUE(ret) && conn->cmd_data.cmd == IIOD_CMD_CLOSE) {
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
