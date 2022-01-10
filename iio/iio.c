/***************************************************************************//**
 *   @file   iio.c
 *   @brief  Implementation of iio.
 *   This module implements read/write ops, required by libtinyiiod and further
 *   calls show/store functions, corresponding to device/channel/attribute.
 *   @author Cristian Pop (cristian.pop@analog.com)
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#include "iio.h"
#include "iio_types.h"
#include "iiod.h"
#include "ctype.h"
#include "no-os/util.h"
#include "no-os/list.h"
#include "no-os/error.h"
#include "no-os/uart.h"
#include "no-os/error.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#ifdef ENABLE_IIO_NETWORK
#include "no-os/delay.h"
#include "tcp_socket.h"
#include "no-os/circular_buffer.h"
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define IIOD_PORT		30431
#define MAX_SOCKET_TO_HANDLE	10
#define REG_ACCESS_ATTRIBUTE	"direct_reg_access"
#define IIOD_CONN_BUFFER_SIZE	0x1000

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

static char header[] =
	"<?xml version=\"1.0\" encoding=\"utf-8\"?>"
	"<!DOCTYPE context ["
	"<!ELEMENT context (device | context-attribute)*>"
	"<!ELEMENT context-attribute EMPTY>"
	"<!ELEMENT device (channel | attribute | debug-attribute | buffer-attribute)*>"
	"<!ELEMENT channel (scan-element?, attribute*)>"
	"<!ELEMENT attribute EMPTY>"
	"<!ELEMENT scan-element EMPTY>"
	"<!ELEMENT debug-attribute EMPTY>"
	"<!ELEMENT buffer-attribute EMPTY>"
	"<!ATTLIST context name CDATA #REQUIRED description CDATA #IMPLIED>"
	"<!ATTLIST context-attribute name CDATA #REQUIRED value CDATA #REQUIRED>"
	"<!ATTLIST device id CDATA #REQUIRED name CDATA #IMPLIED>"
	"<!ATTLIST channel id CDATA #REQUIRED type (input|output) #REQUIRED name CDATA #IMPLIED>"
	"<!ATTLIST scan-element index CDATA #REQUIRED format CDATA #REQUIRED scale CDATA #IMPLIED>"
	"<!ATTLIST attribute name CDATA #REQUIRED filename CDATA #IMPLIED>"
	"<!ATTLIST debug-attribute name CDATA #REQUIRED>"
	"<!ATTLIST buffer-attribute name CDATA #REQUIRED>"
	"]>"
	"<context name=\"xml\" description=\"no-OS analog 1.1.0-g0000000 #1 Tue Nov 26 09:52:32 IST 2019 armv7l\" >"
	"<context-attribute name=\"no-OS\" value=\"1.1.0-g0000000\" />";
static char header_end[] = "</context>";

static const char * const iio_chan_type_string[] = {
	[IIO_VOLTAGE] = "voltage",
	[IIO_CURRENT] = "current",
	[IIO_ALTVOLTAGE] = "altvoltage",
	[IIO_ANGL_VEL] = "anglvel",
	[IIO_TEMP] = "temp",
	[IIO_CAPACITANCE] = "capacitance",
};

static const char * const iio_modifier_names[] = {
	[IIO_MOD_X] = "x",
	[IIO_MOD_Y] = "y",
};

/* Parameters used in show and store functions */
struct attr_fun_params {
	void			*dev_instance;
	char			*buf;
	uint32_t			len;
	struct iio_ch_info	*ch_info;
};

/**
 * @struct iio_dev_priv
 * @brief Links a physical device instance "void *dev_instance"
 * with a "iio_device *iio" that describes capabilities of the device.
 */
struct iio_dev_priv {
	/** Will be: iio:device[0...n] n beeing the count of registerd devices*/
	char			dev_id[21];
	/** Device name */
	const char		*name;
	/** Opened channels */
	uint32_t		ch_mask;
	/** Physical instance of a device */
	void			*dev_instance;
	/** Used to read debug attributes */
	uint32_t		active_reg_addr;
	/** Buffer to read or write data */
	struct circular_buffer	*buf;
	uint32_t		buf_size;
	/** Device descriptor(describes channels and attributes) */
	struct iio_device	*dev_descriptor;
	struct iio_data_buffer	*write_buffer;
	struct iio_data_buffer	*read_buffer;
};

struct iio_desc {
	struct iiod_desc	*iiod;
	struct iiod_ops		iiod_ops;
	void			*phy_desc;
	char			*xml_desc;
	uint32_t		xml_size;
	struct iio_dev_priv	*devs;
	uint32_t		nb_devs;
	struct uart_desc	*uart_desc;
	int (*recv)(void *conn, uint8_t *buf, uint32_t len);
	int (*send)(void *conn, uint8_t *buf, uint32_t len);
	/* FIFO for socket descriptors */
	struct circular_buffer	*conns;
#ifdef ENABLE_IIO_NETWORK
	struct tcp_socket_desc	*current_sock;
	/* Instance of server socket */
	struct tcp_socket_desc	*server;
#endif
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

static inline int32_t _pop_conn(struct iio_desc *desc, uint32_t *conn_id)
{
	uint32_t size;

	cb_size(desc->conns, &size);
	if (size < sizeof(uint32_t))
		return -EAGAIN;

	return cb_read(desc->conns, conn_id, sizeof(*conn_id));
}

static inline int32_t _push_conn(struct iio_desc *desc, uint32_t conn_id)
{
	return cb_write(desc->conns, &conn_id, sizeof(conn_id));
}

static inline int32_t _nb_active_conns(struct iio_desc *desc)
{
	uint32_t size;

	cb_size(desc->conns, &size);

	return size / sizeof(uint32_t);
}

#if 0 //TO remove
/* Blocking until new socket available.
 * Will iterate through a list of sockets */
static int32_t _get_next_socket(struct iio_desc *desc)
{
	struct tcp_socket_desc	*sock;
	int32_t			ret;
	uint32_t		nb_active_sockets;

	/* Get all new waiting sockets.
	 * If none is available, wait until first connection */
	do {
		ret = socket_accept(desc->server, &sock);
		if (ret == -EAGAIN) {
			nb_active_sockets = _nb_active_sockets(desc);
			if (nb_active_sockets == 0) {
				/* Wait until a connection exists */
				mdelay(1);
				continue;
			} else {
				break;
			}
		} else if (IS_ERR_VALUE(ret)) {
			return ret;
		}
		/* Add socket to queue */
		ret = _push_sock(desc, sock);
		if (IS_ERR_VALUE(ret))
			return ret;
	} while (true);

	ret = _pop_sock(desc, &desc->current_sock);
	if (IS_ERR_VALUE(ret)) {
		desc->current_sock = NULL;
		return ret;
	}

	return SUCCESS;
}

static int32_t network_read(const void *data, uint32_t len)
{
	uint32_t	i;
	int32_t		ret;

	if ((int32_t)g_desc->current_sock == -1)
		return -1;

	if (g_desc->current_sock == NULL) {
		ret = _get_next_socket(g_desc);
		if (IS_ERR_VALUE(ret))
			return ret;
	}

	i = 0;
	do {
		ret = socket_recv(g_desc->current_sock,
				  (void *)((uint8_t *)data + i), len - i);
		if (IS_ERR_VALUE(ret)) {
			*(int8_t *)data = '*';
			break;
		}

		i += ret;
	} while (i < len);

	if (ret == -ENOTCONN) {
		/* A socket connection is disconnected, so we release
		 * the resources and don't add it again in the list */
		socket_remove(g_desc->current_sock);
		g_desc->current_sock = (void *)-1;
	}

	return i;
}
#endif //REMOVE

static int iio_recv(struct iiod_ctx *ctx, uint8_t *buf, uint32_t len)
{
	struct iio_desc *desc = ctx->instance;

	return desc->recv(ctx->conn, buf, len);
}

static int iio_send(struct iiod_ctx *ctx, uint8_t *buf, uint32_t len)
{
	struct iio_desc *desc = ctx->instance;

	return desc->send(ctx->conn, buf, len);
}

static inline void _print_ch_id(char *buff, struct iio_channel *ch)
{
	if(ch->modified) {
		sprintf(buff, "%s_%s", iio_chan_type_string[ch->ch_type],
			iio_modifier_names[ch->channel2]);
	} else {
		if(ch->indexed) {
			if (ch->diferential)
				sprintf(buff, "%s%d-%s%d", iio_chan_type_string[ch->ch_type],
					(int)ch->channel, iio_chan_type_string[ch->ch_type],
					(int)ch->channel2);
			else
				sprintf(buff, "%s%d", iio_chan_type_string[ch->ch_type],
					(int)ch->channel);
		} else {
			sprintf(buff, "%s", iio_chan_type_string[ch->ch_type]);
		}
	}
}

/**
 * @brief Get channel ID from a list of channels.
 * @param channel - Channel name.
 * @param desc - Device descriptor
 * @param ch_out - If "true" is output channel, if "false" is input channel.
 * @return Channel ID, or negative value if attribute is not found.
 */
static inline struct iio_channel *iio_get_channel(const char *channel,
		struct iio_device *desc, bool ch_out)
{
	int16_t i = 0;
	char	ch_id[64];

	while (i < desc->num_ch) {
		_print_ch_id(ch_id, &desc->channels[i]);
		if (!strcmp(channel, ch_id) &&
		    (desc->channels[i].ch_out == ch_out))
			return &desc->channels[i];
		i++;
	}

	return NULL;
}

/**
 * @brief Find interface with "device_name".
 * @param device_name - Device name.
 * @param iio_dev_privs - List of interfaces.
 * @return Interface pointer if interface is found, NULL otherwise.
 */
static struct iio_dev_priv *get_iio_device(struct iio_desc *desc,
		const char *device_name)
{
	uint32_t i;

	for (i = 0; i < desc->nb_devs; i++) {
		if (strcmp(desc->devs[i].dev_id, device_name) == 0)
			return &desc->devs[i];
	}

	return NULL;
}

/**
 * @brief Read all attributes from an attribute list.
 * @param device - Physical instance of a device.
 * @param buf - Buffer where values are read.
 * @param len - Maximum length of value to be stored in buf.
 * @param channel - Channel properties.
 * @param attributes - List of attributes to be read.
 * @return Number of bytes read or negative value in case of error.
 */
static int iio_read_all_attr(struct attr_fun_params *params,
			     struct iio_attribute *attributes)
{
	int16_t i = 0, j = 0;
	char local_buf[256];
	int attr_length;
	uint32_t *pattr_length;

	while (attributes[i].name) {
		attr_length = attributes[i].show(params->dev_instance,
						 local_buf, params->len,
						 params->ch_info,
						 attributes[i].priv);
		if (IS_ERR_VALUE(attr_length))
			attr_length = snprintf(local_buf, params->len, "%d",
					       attr_length);

		attr_length += 1;//Add '\0' to the count
		pattr_length = (uint32_t *)(params->buf + j);
		if (j + 4 > params->len)
			return -EINVAL;
		*pattr_length = bswap_constant_32(attr_length);
		j += 4;
		if (attr_length >= 0) {
			if (attr_length + j > params->len)
				return -EINVAL;
			sprintf(params->buf + j, "%s", local_buf);
			if (attr_length & 0x3) /* multiple of 4 */
				attr_length = ((attr_length >> 2) + 1) << 2;
			j += attr_length;
		}
		i++;
	}
	if (j == 0)
		return -ENOENT;

	return j;
}

/**
 * @brief Write all attributes from an attribute list.
 * @param device - Physical instance of a device.
 * @param buf - Values to be written.
 * @param len - Length of buf.
 * @param channel - Channel properties.
 * @param attributes - List of attributes to be written.
 * @return Number of written bytes or negative value in case of error.
 */
static int iio_write_all_attr(struct attr_fun_params *params,
			      struct iio_attribute *attributes)
{
	int16_t i = 0, j = 0;
	int16_t attr_length;

	while (attributes[i].name) {
		attr_length = bswap_constant_32((uint32_t)(params->buf + j));
		j += 4;
		attributes[i].store(params->dev_instance, (params->buf + j),
				    attr_length, params->ch_info,
				    attributes[i].priv);
		j += attr_length;
		if (j & 0x3)
			j = ((j >> 2) + 1) << 2;
		i++;
	}

	if (params->len == 0)
		return -ENOENT;

	return params->len;
}

/**
 * @brief Read/write attribute.
 * @param params - Structure describing parameters for store and show functions
 * @param attributes - Array of attributes.
 * @param attr_name - Attribute name to be modified
 * @param is_write -If it has value "1", writes attribute, otherwise reads
 * 		attribute.
 * @return Length of chars written/read or negative value in case of error.
 */
static int iio_rd_wr_attribute(struct attr_fun_params *params,
			       struct iio_attribute *attributes,
			       const char *attr_name,
			       bool is_write)
{
	int16_t i = 0;

	/* Search attribute */
	while (attributes[i].name) {
		if (!strcmp(attr_name, attributes[i].name))
			break;
		i++;
	}

	if (!attributes[i].name)
		return -ENOENT;

	if (is_write) {
		if (!attributes[i].store)
			return -ENOENT;

		return attributes[i].store(params->dev_instance, params->buf,
					   params->len, params->ch_info,
					   attributes[i].priv);
	} else {
		if (!attributes[i].show)
			return -ENOENT;
		return attributes[i].show(params->dev_instance, params->buf,
					  params->len, params->ch_info,
					  attributes[i].priv);
	}
}

/* Read a device register. The register address to read is set on
 * in desc->active_reg_addr in the function set_demo_reg_attr
 */
static int32_t debug_reg_read(struct iio_dev_priv *dev, char *buf, uint32_t len)
{
	uint32_t		value;
	int32_t			ret;

	value = 0;
	ret = dev->dev_descriptor->debug_reg_read(dev->dev_instance,
			dev->active_reg_addr,
			&value);
	if (IS_ERR_VALUE(ret))
		return ret;

	return snprintf(buf, len, "%"PRIu32"", value);
}

/* Flow of reading and writing registers. This is how iio works for
 * direct_reg_access attribute:
 * Read register:
 * 	   //Reg_addr in decimal
 * 	   reg_addr = "10";
 * 	1. debug_reg_write(dev, reg_addr, len);
 * 	2. debug_reg_read(dev, out_buf, out_len);
 * Write register:
 * 	   sprintf(write_buf, "0x%x 0x%x", reg_addr, value);
 * 	1. debug_reg_write(dev, write_buf,len);
 */
static int32_t debug_reg_write(struct iio_dev_priv *dev, const char *buf,
			       uint32_t len)
{
	uint32_t		nb_filled;
	uint32_t		addr;
	uint32_t		value;
	int32_t			ret;

	nb_filled = sscanf(buf, "0x%"PRIx32" 0x%"PRIx32"", &addr, &value);
	if (nb_filled == 2) {
		/* Write register */
		ret = dev->dev_descriptor->debug_reg_write(dev->dev_instance,
				addr, value);
		if (IS_ERR_VALUE(ret))
			return ret;
	} else {
		nb_filled = sscanf(buf, "%"PRIu32, &addr);
		if (nb_filled == 1) {
			dev->active_reg_addr = addr;
			return len;
		} else {
			return -EINVAL;
		}
	}

	return len;
}

static int32_t __iio_str_parse(char *buf, int32_t *integer, int32_t *_fract,
			       bool scale_db)
{
	char *p;

	p = strtok(buf, ".");
	if (p == NULL)
		return -EINVAL;

	*integer = strtol(p, NULL, 0);

	if (scale_db) {
		p = strtok(NULL, "db");
		if (p == NULL)
			p = strtok(NULL, " db");
	} else
		p = strtok(NULL, "\n");

	if (p == NULL)
		return -EINVAL;

	*_fract = strtol(p, NULL, 0);

	return 0;
}

int32_t iio_parse_value(char *buf, enum iio_val fmt, int32_t *val,
			int32_t *val2)
{
	int32_t ret = 0;
	int32_t integer, _fract = 0;
	char ch;

	switch (fmt) {
	case IIO_VAL_INT:
		integer = strtol(buf, NULL, 0);
		break;
	case IIO_VAL_INT_PLUS_MICRO_DB:
		ret = __iio_str_parse(buf, &integer, &_fract, true);
		if (ret < 0)
			return ret;
		_fract *= 100000;
		break;
	case IIO_VAL_INT_PLUS_MICRO:
		ret = __iio_str_parse(buf, &integer, &_fract, false);
		if (ret < 0)
			return ret;
		_fract *= 100000;
		break;
	case IIO_VAL_INT_PLUS_NANO:
		ret = __iio_str_parse(buf, &integer, &_fract, false);
		if (ret < 0)
			return ret;
		_fract *= 100000000;
		break;
	case IIO_VAL_CHAR:
		if (sscanf(buf, "%c", &ch) != 1)
			return -EINVAL;
		integer = ch;
		break;
	default:
		return -EINVAL;
	}

	if (val)
		*val = integer;
	if (val2)
		*val2 = _fract;

	return ret;
}

int iio_format_value(char *buf, uint32_t len, enum iio_val fmt,
		     int32_t size, int32_t *vals)
{
	uint64_t tmp;
	int32_t integer, fractional;
	bool dB = false;
	int32_t i = 0;
	uint32_t l = 0;

	switch (fmt) {
	case IIO_VAL_INT:
		return snprintf(buf, len, "%"PRIi32"", vals[0]);
	case IIO_VAL_INT_PLUS_MICRO_DB:
		dB = true;
	/* intentional fall through */
	case IIO_VAL_INT_PLUS_MICRO:
		return snprintf(buf, len, "%"PRIi32".%06"PRIu32"%s", vals[0],
				(uint32_t)vals[1], dB ? " dB" : "");
	case IIO_VAL_INT_PLUS_NANO:
		return snprintf(buf, len, "%"PRIi32".%09"PRIu32"", vals[0],
				(uint32_t)vals[1]);
	case IIO_VAL_FRACTIONAL:
		tmp = div_s64((int64_t)vals[0] * 1000000000LL, vals[1]);
		fractional = vals[1];
		integer = (int32_t)div_s64_rem(tmp, 1000000000, &fractional);
		return snprintf(buf, len, "%"PRIi32".%09u", integer,
				abs(fractional));
	case IIO_VAL_FRACTIONAL_LOG2:
		tmp = shift_right((int64_t)vals[0] * 1000000000LL, vals[1]);
		integer = (int32_t)div_s64_rem(tmp, 1000000000LL, &fractional);
		return snprintf(buf, len, "%"PRIi32".%09u", integer,
				abs(fractional));
	case IIO_VAL_INT_MULTIPLE: {
		while (i < size) {
			l += snprintf(&buf[l], len - l, "%"PRIi32" ", vals[i]);
			if (l >= len)
				break;
			i++;
		}
		return l;
	}
	case IIO_VAL_CHAR:
		return snprintf(buf, len, "%c", (char)vals[0]);
	default:
		return 0;
	}
}

static struct iio_attribute *get_attributes(enum iio_attr_type type,
		struct iio_dev_priv *dev,
		struct iio_channel *ch)
{
	switch (type) {
	case IIO_ATTR_TYPE_DEBUG:
		return dev->dev_descriptor->debug_attributes;
		break;
	case IIO_ATTR_TYPE_DEVICE:
		return dev->dev_descriptor->attributes;
		break;
	case IIO_ATTR_TYPE_BUFFER:
		return dev->dev_descriptor->buffer_attributes;
		break;
	case IIO_ATTR_TYPE_CH_IN:
	case IIO_ATTR_TYPE_CH_OUT:
		return ch->attributes;
	}

	return NULL;
}

/**
 * @brief Read global attribute of a device.
 * @param ctx - IIO instance and conn instance
 * @param device - String containing device name.
 * @param attr - String containing attribute name.
 * @param buf - Buffer where value is read.
 * @param len - Maximum length of value to be stored in buf.
 * @return Number of bytes read.
 */
static int iio_read_attr(struct iiod_ctx *ctx, const char *device,
			 struct iiod_attr *attr, char *buf, uint32_t len)
{
	struct iio_dev_priv *dev;
	struct iio_ch_info ch_info;
	struct iio_channel *ch = NULL;
	struct attr_fun_params params;
	struct iio_attribute *attributes;
	int8_t ch_out;

	dev = get_iio_device(ctx->instance, device);
	if (!dev)
		return FAILURE;

	if (attr->type == IIO_ATTR_TYPE_DEBUG &&
	    strcmp(attr->name, REG_ACCESS_ATTRIBUTE) == 0) {
		if (dev->dev_descriptor->debug_reg_read)
			return debug_reg_read(dev, buf, len);
		else
			return -ENOENT;
	}

	if (attr->channel) {
		ch_out = attr->type == IIO_ATTR_TYPE_CH_OUT ? 1 : 0;
		ch = iio_get_channel(attr->channel, dev->dev_descriptor,
				     ch_out);
		if (!ch)
			return -ENOENT;
		ch_info.ch_out = ch_out;
		ch_info.ch_num = ch->channel;
		ch_info.type = ch->ch_type;
		ch_info.differential = ch->diferential;
		ch_info.address = ch->address;
		params.ch_info = &ch_info;
	} else {
		params.ch_info = NULL;
	}

	params.buf = buf;
	params.len = len;
	params.dev_instance = dev->dev_instance;
	attributes = get_attributes(attr->type, dev, ch);
	if (!strcmp(attr->name, ""))
		return iio_read_all_attr(&params, attributes);
	else
		return iio_rd_wr_attribute(&params, attributes, attr->name, 0);
}

/**
 * @brief Write global attribute of a device.
 * @param device - String containing device name.
 * @param ctx - IIO instance and conn instance
 * @param attr - String containing attribute name.
 * @param buf - Value to be written.
 * @param len - Length of data.
 * @return Number of written bytes.
 */
static int iio_write_attr(struct iiod_ctx *ctx, const char *device,
			  struct iiod_attr *attr, char *buf, uint32_t len)
{
	struct iio_dev_priv	*dev;
	struct attr_fun_params	params;
	struct iio_attribute	*attributes;
	struct iio_ch_info ch_info;
	struct iio_channel *ch = NULL;
	int8_t ch_out;

	dev = get_iio_device(ctx->instance, device);
	if (!dev)
		return -ENODEV;

	if (attr->type == IIO_ATTR_TYPE_DEBUG &&
	    strcmp(attr->name, REG_ACCESS_ATTRIBUTE) == 0) {
		if (dev->dev_descriptor->debug_reg_write)
			return debug_reg_write(dev, buf, len);
		else
			return -ENOENT;
	}

	if (attr->channel) {
		ch_out = attr->type == IIO_ATTR_TYPE_CH_OUT ? 1 : 0;
		ch = iio_get_channel(attr->channel, dev->dev_descriptor,
				     ch_out);
		if (!ch)
			return -ENOENT;

		ch_info.ch_out = ch_out;
		ch_info.ch_num = ch->channel;
		ch_info.type = ch->ch_type;
		ch_info.differential = ch->diferential;
		ch_info.address = ch->address;
		params.ch_info = &ch_info;
	} else {
		params.ch_info = NULL;
	}

	params.buf = (char *)buf;
	params.len = len;
	params.dev_instance = dev->dev_instance;
	attributes = get_attributes(attr->type, dev, ch);
	if (!strcmp(attr->name, ""))
		return iio_write_all_attr(&params, attributes);
	else
		return iio_rd_wr_attribute(&params, attributes, attr->name, 1);
}

#if 0 /* TODO remove */
/**
 * @brief Read channel attribute.
 * @param device_name - String containing device name.
 * @param channel - String containing channel name.
 * @param ch_out -Channel type input/output.
 * @param attr - String containing attribute name.
 * @param buf - Buffer where value is stored.
 * @param len - Maximum length of value to be stored in buf.
 * @return - Number of bytes read.
 */
static int iio_ch_read_attr(const char *device_id, const char *channel,
			    bool ch_out, const char *attr, char *buf, uint32_t len)
{
	struct iio_dev_priv	*dev;
	struct iio_ch_info	ch_info;
	struct iio_channel	*ch;
	struct attr_fun_params	params;

	dev = get_iio_device(device_id);
	if (!dev)
		return FAILURE;

	ch = iio_get_channel(channel, dev->dev_descriptor, ch_out);
	if (!ch)
		return -ENOENT;

	ch_info.ch_out = ch_out;
	ch_info.ch_num = ch->channel;
	ch_info.type = ch->ch_type;
	ch_info.differential = ch->diferential;
	ch_info.address = ch->address;
	params.buf = buf;
	params.len = len;
	params.dev_instance = dev->dev_instance;
	params.ch_info = &ch_info;
	if (!strcmp(attr, ""))
		return iio_read_all_attr(&params, ch->attributes);
	else
		return iio_rd_wr_attribute(&params, ch->attributes, (char *)attr, 0);
}

/**
 * @brief Write channel attribute.
 * @param device - String containing device name.
 * @param channel - String containing channel name.
 * @param ch_out - Channel type input/output.
 * @param attr - String containing attribute name.
 * @param buf - Value to be written.
 * @param len - Length of data in "buf" parameter.
 * @return Number of written bytes.
 */
static int iio_ch_write_attr(const char *device_id, const char *channel,
			     bool ch_out, const char *attr, const char *buf, uint32_t len)
{
	struct iio_dev_priv	*dev;
	struct iio_ch_info	ch_info;
	struct iio_channel	*ch;
	struct attr_fun_params	params;

	dev = get_iio_device(device_id);
	if (!dev)
		return -ENOENT;

	ch = iio_get_channel(channel, dev->dev_descriptor, ch_out);
	if (!ch)
		return -ENOENT;

	ch_info.ch_out = ch_out;
	ch_info.ch_num = ch->channel;
	ch_info.type = ch->ch_type;
	ch_info.differential = ch->diferential;
	ch_info.address = ch->address;
	params.buf = (char *)buf;
	params.len = len;
	params.dev_instance = dev->dev_instance;
	params.ch_info = &ch_info;
	if (!strcmp(attr, ""))
		return iio_write_all_attr(&params, ch->attributes);
	else
		return iio_rd_wr_attribute(&params, ch->attributes, (char *)attr, 1);
}
#endif /* TODO remvoe */

static uint32_t bytes_to_samples(struct iio_dev_priv *dev, uint32_t bytes)
{
	uint32_t bytes_per_sample;
	uint32_t first_ch;
	bool	 first_ch_found;
	uint32_t nb_active_ch;
	uint32_t mask;

	mask = dev->ch_mask;
	first_ch = 0;
	nb_active_ch = 0;
	first_ch_found = false;
	while (mask) {
		if ((mask & 1)) {
			if (!first_ch_found)
				first_ch_found = true;
			else
				first_ch++;
			nb_active_ch++;
		}
		mask >>= 1;
	}
	bytes_per_sample = dev->dev_descriptor->channels[first_ch]
			   .scan_type->storagebits / 8;

	return bytes / bytes_per_sample / nb_active_ch;
}

static uint32_t samples_to_bytes(struct iio_dev_priv *dev, uint32_t samples)
{
	uint32_t bytes_per_sample;
	uint32_t first_ch;
	bool	 first_ch_found;
	uint32_t nb_active_ch;
	uint32_t mask;

	mask = dev->ch_mask;
	first_ch = 0;
	nb_active_ch = 0;
	first_ch_found = false;
	while (mask) {
		if ((mask & 1)) {
			if (!first_ch_found)
				first_ch_found = true;
			else
				first_ch++;
			nb_active_ch++;
		}
		mask >>= 1;
	}
	bytes_per_sample = dev->dev_descriptor->channels[first_ch]
			   .scan_type->storagebits / 8;

	return samples * bytes_per_sample * nb_active_ch;
}

/**
 * @brief  Open device.
 * @param ctx - IIO instance and conn instance
 * @param device - String containing device name.
 * @param sample_size - Sample size.
 * @param mask - Channels to be opened.
 * @return SUCCESS, negative value in case of failure.
 */
static int iio_open_dev(struct iiod_ctx *ctx, const char *device,
			uint32_t samples, uint32_t mask, bool cyclic)
{
	struct iio_dev_priv *dev;
	uint32_t ch_mask;
	int32_t ret;

	dev = get_iio_device(ctx->instance, device);
	if (!dev)
		return -ENODEV;

	ch_mask = 0xFFFFFFFF >> (32 - dev->dev_descriptor->num_ch);

	if (mask & ~ch_mask)
		return -ENOENT;

	dev->ch_mask = mask;
	dev->buf_size = samples_to_bytes(dev, samples);;
	ret = cb_init(&dev->buf, dev->buf_size);
	if (IS_ERR_VALUE(ret))
		return ret;

	if (dev->dev_descriptor->prepare_transfer)
		return dev->dev_descriptor->prepare_transfer(
			       dev->dev_instance, mask);

	return SUCCESS;
}

/**
 * @brief Close device.
 * @param ctx - IIO instance and conn instance
 * @param device - String containing device name.
 * @return SUCCESS, negative value in case of failure.
 */
static int iio_close_dev(struct iiod_ctx *ctx, const char *device)
{
	struct iio_dev_priv *dev;

	dev = get_iio_device(ctx->instance, device);
	if (!dev)
		return FAILURE;

	cb_remove(dev->buf);
	dev->ch_mask = 0;
	if (dev->dev_descriptor->end_transfer)
		return dev->dev_descriptor->end_transfer(dev->dev_instance);

	return SUCCESS;
}

#if 0 /* TODO remove */
/**
 * @brief Get device mask, this specifies the channels that are used.
 * @param device - String containing device name.
 * @param mask - Channels that are opened.
 * @return SUCCESS, negative value in case of failure.
 */
static int32_t iio_get_mask(const char *device, uint32_t *mask)
{
	struct iio_dev_priv *dev;

	dev = get_iio_device(device);
	if (!dev)
		return -ENODEV;

	*mask = dev->ch_mask;

	return SUCCESS;
}

static uint32_t bytes_to_samples(struct iio_dev_priv *intf, uint32_t bytes)
{
	uint32_t bytes_per_sample;
	uint32_t first_ch;
	bool	 first_ch_found;
	uint32_t nb_active_ch;
	uint32_t mask;

	mask = intf->ch_mask;
	first_ch = 0;
	nb_active_ch = 0;
	first_ch_found = false;
	while (mask) {
		if ((mask & 1)) {
			if (!first_ch_found)
				first_ch_found = true;
			else
				first_ch++;
			nb_active_ch++;
		}
		mask >>= 1;
	}
	bytes_per_sample = intf->dev_descriptor->channels[first_ch]
			   .scan_type->storagebits / 8;

	return bytes / bytes_per_sample / nb_active_ch;
}

/**
 * @brief Transfer data from device into RAM.
 * @param device - String containing device name.
 * @param bytes_count - Number of bytes.
 * @return Bytes_count or negative value in case of error.
 */
static int iio_transfer_dev_to_mem(const char *device, uint32_t bytes_count)
{
	struct iio_dev_priv *dev = get_iio_device(device);
	struct iio_data_buffer	*r_buff;
	uint32_t		samples;
	int			ret;

	r_buff = dev->read_buffer;
	if (r_buff && dev->dev_descriptor->read_dev) {
		if (bytes_count > r_buff->size)
			return -ENOMEM;
		samples = bytes_to_samples(dev, bytes_count);
		ret = dev->dev_descriptor->read_dev(
			      dev->dev_instance,
			      r_buff->buff, samples);
		return ret < 0 ? ret : (int)bytes_count;
	}

	return -ENOENT;
}
#endif /* TODO remove */

/**
 * @brief Read chunk of data from RAM to pbuf. Call
 * "iio_transfer_dev_to_mem()" first.
 * This function is probably called multiple times by libtinyiiod after a
 * "iio_transfer_dev_to_mem" call, since we can only read "bytes_count" bytes.
 * @param device - String containing device name.
 * @param pbuf - Buffer where value is stored.
 * @param offset - Offset to the remaining data after reading n chunks.
 * @param bytes_count - Number of bytes to read.
 * @return: Bytes_count or negative value in case of error.
 */
static int iio_read_buffer(struct iiod_ctx *ctx, const char *device, char *buf,
			   uint32_t bytes)
{
	struct iio_dev_priv	*dev;
	uint32_t		samples;
	int32_t			ret;
	uint32_t		size;
	void			*cb_buff;
	uint32_t		available;

	dev = get_iio_device(ctx->instance, device);
	if (!dev)
		return -EINVAL;

	if (!dev->dev_descriptor->read_dev)
		return -EINVAL;

	ret = cb_size(dev->buf, &size);
	if (IS_ERR_VALUE(ret))
		return ret;

	if (size == 0) {
		/* Refill buffer */
		ret = cb_prepare_async_write(dev->buf, dev->buf_size, &cb_buff,
					     &available);
		if (IS_ERR_VALUE(ret))
			return ret;
		if (available < dev->buf_size)
			return -EINVAL;

		samples = bytes_to_samples(dev, dev->buf_size);
		ret = dev->dev_descriptor->read_dev(dev->dev_instance, cb_buff,
						    samples);
		if (ret < 0)
			return ret;

		ret = cb_end_async_write(dev->buf);
		if (IS_ERR_VALUE(ret))
			return ret;
	}

	ret = cb_read(dev->buf, buf, bytes);
	if (IS_ERR_VALUE(ret))
		return ret;

	return bytes;
}

#if 0 /* TODO remove */
/**
 * @brief Transfer memory to device.
 * @param device - String containing device name.
 * @param bytes_count - Number of bytes to transfer.
 * @return Bytes_count or negative value in case of error.
 */
static int iio_transfer_mem_to_dev(const char *device, uint32_t bytes_count)
{
	struct iio_dev_priv *dev = get_iio_device(device);
	struct iio_data_buffer	*w_buff;
	int			ret;
	uint32_t		samples;

	w_buff = dev->write_buffer;
	if (w_buff && dev->dev_descriptor->write_dev) {
		if (bytes_count > w_buff->size)
			return -ENOMEM;
		samples = bytes_to_samples(dev, bytes_count);
		ret = dev->dev_descriptor->write_dev(
			      dev->dev_instance,
			      w_buff->buff, samples);
		return ret < 0 ? ret : (int)bytes_count;
	}

	return -ENOENT;
}
#endif /* TODO remove */

/**
 * @brief Write chunk of data into RAM.
 * This function is probably called multiple times by libtinyiiod before a
 * "iio_transfer_mem_to_dev" call, since we can only write "bytes_count" bytes
 * at a time.
 * @param device - String containing device name.
 * @param buf - Values to write.
 * @param offset - Offset in memory after the nth chunk of data.
 * @param bytes_count - Number of bytes to write.
 * @return Bytes_count or negative value in case of error.
 */
static int iio_write_buffer(struct iiod_ctx *ctx, const char *device, char *buf,
			    uint32_t bytes)
{
	struct iio_dev_priv	*dev;
	uint32_t		samples;
	int32_t			ret;
	void			*cb_buff;
	uint32_t		available;
	uint32_t		size;

	dev = get_iio_device(ctx->instance, device);
	if (!dev)
		return -EINVAL;

	if (!dev->dev_descriptor->write_dev)
		return -EINVAL;

	ret = cb_size(dev->buf, &size);
	if (IS_ERR_VALUE(ret))
		return ret;

	if (size < dev->buf_size) {
		ret = cb_write(dev->buf, buf, bytes);
		if (IS_ERR_VALUE(ret))
			return ret;
		size += bytes;
	}
	if (size == dev->buf_size) {
		/* Refill buffer */
		ret = cb_prepare_async_read(dev->buf, dev->buf_size, &cb_buff,
					    &available);
		if (IS_ERR_VALUE(ret))
			return ret;
		if (available < dev->buf_size)
			return -EINVAL;

		samples = bytes_to_samples(dev, dev->buf_size);
		ret = dev->dev_descriptor->write_dev(dev->dev_instance, cb_buff,
						     samples);
		if (ret < 0)
			return ret;

		ret = cb_end_async_read(dev->buf);
		if (IS_ERR_VALUE(ret))
			return ret;
	}

	return bytes;
}

#if 0 /* TODO remove */
/**
 * @brief Get a merged xml containing all devices.
 * @param outxml - Generated xml.
 * @return SUCCESS in case of success or negative value otherwise.
 */
static int iio_get_xml(char **outxml)
{
	if (!outxml)
		return FAILURE;

	*outxml = g_desc->xml_desc;

	return g_desc->xml_size;
}
#endif

#ifdef ENABLE_IIO_NETWORK

static int32_t accept_network_clients(struct iio_desc *desc)
{
	struct tcp_socket_desc *sock;
	struct iiod_conn_data data;
	int32_t ret;
	do {
		uint32_t id;
		ret = socket_accept(desc->server, &sock);
		if (IS_ERR_VALUE(ret))
			return ret;

		data.conn = sock;
		data.buf = calloc(1, IIOD_CONN_BUFFER_SIZE);
		data.len = IIOD_CONN_BUFFER_SIZE;

		ret = iiod_conn_add(desc->iiod, &data, &id);
		if (IS_ERR_VALUE(ret))
			return ret;

		ret = _push_conn(desc, id);
		if (IS_ERR_VALUE(ret))
			return ret;
	} while (true);

	return SUCCESS;
}
#endif

/**
 * @brief Execute an iio step
 * @param desc - IIo descriptor
 * @return SUCCESS in case of success or negative value otherwise.
 */
int iio_step(struct iio_desc *desc)
{
	struct iiod_conn_data data;
	uint32_t conn_id;
	int32_t ret;

#ifdef ENABLE_IIO_NETWORK
	if (desc->server) {
		ret = accept_network_clients(desc);
		if (IS_ERR_VALUE(ret) && ret != -EAGAIN)
			return ret;
	}
#endif

	ret = _pop_conn(desc, &conn_id);
	if (IS_ERR_VALUE(ret))
		return ret;

	ret = iiod_conn_step(desc->iiod, conn_id);
	if (ret == -ENOTCONN) {
#ifdef ENABLE_IIO_NETWORK
		if (desc->server) {
			iiod_conn_remove(desc->iiod, conn_id, &data);
			socket_remove(data.conn);
			free(data.buf);
		}
#endif
	} else {
		_push_conn(desc, conn_id);
	}

	return ret;
}

/*
 * Generate an xml describing a device and write it to buff.
 * Will return the size of the xml.
 * If buff_size is 0, no data will be written to buff, but size will be returned
 */
static uint32_t iio_generate_device_xml(struct iio_device *device, char *name,
					char *id, char *buff,
					uint32_t buff_size)
{
	struct iio_channel	*ch;
	struct iio_attribute	*attr;
	char			ch_id[50];
	int32_t			i;
	int32_t			j;
	int32_t			k;
	int32_t			n;

	if ((int32_t)buff_size == -1)
		n = 0;
	else
		n = buff_size;

	if (buff == NULL)
		/* Set dummy value for buff. It is used only for counting */
		buff = ch_id;

	i = 0;
	i += snprintf(buff, max(n - i, 0),
		      "<device id=\"%s\" name=\"%s\">", id, name);

	/* Write channels */
	if (device->channels)
		for (j = 0; j < device->num_ch; j++) {
			ch = &device->channels[j];
			_print_ch_id(ch_id, ch);
			i += snprintf(buff + i, max(n - i, 0),
				      "<channel id=\"%s\"",
				      ch_id);
			if(ch->name)
				i += snprintf(buff + i, max(n - i, 0),
					      " name=\"%s\"",
					      ch->name);
			i += snprintf(buff + i, max(n - i, 0),
				      " type=\"%s\" >",
				      ch->ch_out ? "output" : "input");

			if (ch->scan_type)
				i += snprintf(buff + i, max(n - i, 0),
					      "<scan-element index=\"%d\""
					      " format=\"%s:%c%d/%d>>%d\" />",
					      ch->scan_index,
					      ch->scan_type->is_big_endian ? "be" : "le",
					      ch->scan_type->sign,
					      ch->scan_type->realbits,
					      ch->scan_type->storagebits,
					      ch->scan_type->shift);

			/* Write channel attributes */
			if (ch->attributes)
				for (k = 0; ch->attributes[k].name; k++) {
					attr = &ch->attributes[k];
					i += snprintf(buff + i, max(n - i, 0), "<attribute name=\"%s\" ", attr->name);
					if (ch->diferential) {
						switch (attr->shared) {
						case IIO_SHARED_BY_ALL:
							i += snprintf(buff + i, max(n - i, 0),
								      "filename=\"%s\"",
								      attr->name);
							break;
						case IIO_SHARED_BY_DIR:
							i += snprintf(buff + i, max(n - i, 0),
								      "filename=\"%s_%s\"",
								      ch->ch_out ? "out" : "in",
								      attr->name);
							break;
						case IIO_SHARED_BY_TYPE:
							i += snprintf(buff + i, max(n - i, 0),
								      "filename=\"%s_%s-%s_%s\"",
								      ch->ch_out ? "out" : "in",
								      iio_chan_type_string[ch->ch_type],
								      iio_chan_type_string[ch->ch_type],
								      attr->name);
							break;
						case IIO_SEPARATE:
							if (!ch->indexed) {
								// Differential channels must be indexed!
								return -EINVAL;
							}
							i += snprintf(buff + i, max(n - i, 0),
								      "filename=\"%s_%s%d-%s%d_%s\"",
								      ch->ch_out ? "out" : "in",
								      iio_chan_type_string[ch->ch_type],
								      ch->channel,
								      iio_chan_type_string[ch->ch_type],
								      ch->channel2,
								      attr->name);
							break;
						}
					} else {
						switch (attr->shared) {
						case IIO_SHARED_BY_ALL:
							i += snprintf(buff + i, max(n - i, 0),
								      "filename=\"%s\"",
								      attr->name);
							break;
						case IIO_SHARED_BY_DIR:
							i += snprintf(buff + i, max(n - i, 0),
								      "filename=\"%s_%s\"",
								      ch->ch_out ? "out" : "in",
								      attr->name);
							break;
						case IIO_SHARED_BY_TYPE:
							i += snprintf(buff + i, max(n - i, 0),
								      "filename=\"%s_%s_%s\"",
								      ch->ch_out ? "out" : "in",
								      iio_chan_type_string[ch->ch_type],
								      attr->name);
							break;
						case IIO_SEPARATE:
							if (ch->indexed)
								i += snprintf(buff + i, max(n - i, 0),
									      "filename=\"%s_%s%d_%s\"",
									      ch->ch_out ? "out" : "in",
									      iio_chan_type_string[ch->ch_type],
									      ch->channel,
									      attr->name);
							else
								i += snprintf(buff + i, max(n - i, 0),
									      "filename=\"%s_%s_%s\"",
									      ch->ch_out ? "out" : "in",
									      iio_chan_type_string[ch->ch_type],
									      attr->name);
							break;
						}
					}
					i += snprintf(buff + i, max(n - i, 0), " />");
				}

			i += snprintf(buff + i, max(n - i, 0), "</channel>");
		}

	/* Write device attributes */
	if (device->attributes)
		for (j = 0; device->attributes[j].name; j++)
			i += snprintf(buff + i, max(n - i, 0),
				      "<attribute name=\"%s\" />",
				      device->attributes[j].name);

	/* Write debug attributes */
	if (device->debug_attributes)
		for (j = 0; device->debug_attributes[j].name; j++)
			i += snprintf(buff + i, max(n - i, 0),
				      "<debug-attribute name=\"%s\" />",
				      device->debug_attributes[j].name);
	if (device->debug_reg_read || device->debug_reg_write)
		i += snprintf(buff + i, max(n - i, 0),
			      "<debug-attribute name=\""REG_ACCESS_ATTRIBUTE"\" />");

	/* Write buffer attributes */
	if (device->buffer_attributes)
		for (j = 0; device->buffer_attributes[j].name; j++)
			i += snprintf(buff + i, max(n - i, 0),
				      "<buffer-attribute name=\"%s\" />",
				      device->buffer_attributes[j].name);

	i += snprintf(buff + i, max(n - i, 0), "</device>");

	return i;
}

static int32_t iio_init_xml(struct iio_desc *desc)
{
	struct iio_dev_priv *dev;
	uint32_t size, of;
	int32_t i;

	/* -2 because of the 0 character */
	size = sizeof(header) + sizeof(header_end) - 2;
	for (i = 0; i < desc->nb_devs; i++) {
		dev = desc->devs + i;
		size += iio_generate_device_xml(dev->dev_descriptor,
						(char *)dev->name,
						dev->dev_id, NULL, -1);
	}

	desc->xml_desc = (char *)calloc(size + 1, sizeof(*desc->xml_desc));
	if (!desc->xml_desc)
		return -ENOMEM;

	desc->xml_size = size;

	strcpy(desc->xml_desc, header);
	of = sizeof(header) - 1;
	for (i = 0; i < desc->nb_devs; i++) {
		dev = desc->devs + i;
		of += iio_generate_device_xml(dev->dev_descriptor,
					      (char *)dev->name, dev->dev_id,
					      desc->xml_desc + of, size - of);
	}

	strcpy(desc->xml_desc + of, header_end);

	return SUCCESS;
}

static int32_t iio_init_devs(struct iio_desc *desc,
			     struct iio_device_init *devs, int32_t n)
{
	uint32_t i;
	int32_t ret;
	struct iio_dev_priv *ldev;
	struct iio_device_init *ndev;

	desc->nb_devs = n;
	desc->devs = (struct iio_dev_priv *)calloc(desc->nb_devs,
			sizeof(*desc->devs));
	if (!desc->devs)
		return -ENOMEM;

	for (i = 0; i < n; i++) {
		ndev = devs + i;
		ldev = desc->devs + i;
		ldev->dev_descriptor = ndev->dev_descriptor;
		ldev->read_buffer = devs[i].read_buff;
		ldev->write_buffer = devs[i].write_buff;
		sprintf(ldev->dev_id, "iio:device%"PRIu32"", i);
		ldev->dev_instance = ndev->dev;
		ldev->name = ndev->name;
	}

	ret = iio_init_xml(desc);
	if (IS_ERR_VALUE(ret))
		free(desc->devs);

	return ret;
}

/**
 * @brief Set communication ops and read/write ops that will be called
 * from "libtinyiiod".
 * @param desc - iio descriptor.
 * @param init_param - appropriate init param.
 * @return SUCCESS in case of success or negative value otherwise.
 */
int iio_init(struct iio_desc **desc, struct iio_init_param *init_param)
{
	int32_t			ret;
	struct iio_desc		*ldesc;
	struct iiod_ops		*ops;
	struct iiod_init_param	iiod_param;
	uint32_t		conn_id;

	if (!desc || !init_param)
		return -EINVAL;

	ldesc = (struct iio_desc *)calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return -ENOMEM;

	ret = iio_init_devs(ldesc, init_param->devs, init_param->nb_devs);
	if (IS_ERR_VALUE(ret))
		goto free_desc;

	/* device operations */
	ops = &ldesc->iiod_ops;
	ops->read_attr = iio_read_attr;
	ops->write_attr = iio_write_attr;
	ops->read_buffer = iio_read_buffer;
	ops->write_buffer = iio_write_buffer;
	ops->open = iio_open_dev;
	ops->close = iio_close_dev;
	ops->send = iio_send;
	ops->recv = iio_recv;

	iiod_param.instance = ldesc;
	iiod_param.ops = ops;
	iiod_param.xml = ldesc->xml_desc;
	iiod_param.xml_len = ldesc->xml_size;

	ret = iiod_init(&ldesc->iiod, &iiod_param);
	if (IS_ERR_VALUE(ret))
		goto free_devs;

	ret = cb_init(&ldesc->conns,
		      sizeof(uint32_t) * (IIOD_MAX_CONNECTIONS + 1));
	if (IS_ERR_VALUE(ret))
		goto free_iiod;

	if (init_param->phy_type == USE_UART) {
		ldesc->send = (int (*)())uart_write;
		ldesc->recv = (int (*)())uart_read;
		ldesc->uart_desc = init_param->uart_desc;

		struct iiod_conn_data data = {
			.conn = ldesc->uart_desc,
			.buf = calloc(1, IIOD_CONN_BUFFER_SIZE),
			.len = IIOD_CONN_BUFFER_SIZE
		};
		ret = iiod_conn_add(ldesc->iiod, &data, &conn_id);
		if (IS_ERR_VALUE(ret))
			goto free_conns;
		_push_conn(ldesc, conn_id);
	}
#ifdef ENABLE_IIO_NETWORK
	else if (init_param->phy_type == USE_NETWORK) {
		ldesc->send = (int (*)())socket_send;
		ldesc->recv = (int (*)())socket_recv;
		ret = socket_init(&ldesc->server,
				  init_param->tcp_socket_init_param);
		if (IS_ERR_VALUE(ret))
			goto free_conns;
		ret = socket_bind(ldesc->server, IIOD_PORT);
		if (IS_ERR_VALUE(ret))
			goto free_pylink;
		ret = socket_listen(ldesc->server, MAX_BACKLOG);
		if (IS_ERR_VALUE(ret))
			goto free_pylink;
	}
#endif
	else {
		ret = -EINVAL;
		goto free_conns;
	}

	*desc = ldesc;

	return SUCCESS;

free_pylink:
#ifdef ENABLE_IIO_NETWORK
	socket_remove(ldesc->server);
#endif
free_conns:
	cb_remove(ldesc->conns);
free_iiod:
	iiod_remove(ldesc->iiod);
free_devs:
	free(ldesc->devs);
	free(ldesc->xml_desc);
free_desc:
	free(ldesc);

	return ret;
}

/**
 * @brief Free the resources allocated by "iio_init()".
 * @param desc: iio descriptor.
 * @return SUCCESS in case of success or negative value otherwise.
 */
int iio_remove(struct iio_desc *desc)
{
	if (!desc)
		return -EINVAL;

#ifdef ENABLE_IIO_NETWORK
	socket_remove(desc->server);
#endif
	cb_remove(desc->conns);
	iiod_remove(desc->iiod);
	free(desc->devs);
	free(desc->xml_desc);
	free(desc);

	return SUCCESS;
}
