/***************************************************************************//**
 *   @file   iio.c
 *   @brief  Implementation of iio.
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
#include "no_os_util.h"
#include "no_os_list.h"
#include "no_os_error.h"
#include "no_os_uart.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_circular_buffer.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#ifdef NO_OS_NETWORKING
#include "no_os_delay.h"
#include "tcp_socket.h"
#endif

#ifdef NO_OS_LWIP_NETWORKING
#include "no_os_delay.h"
#include "tcp_socket.h"
#include "lwip_socket.h"
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define IIOD_PORT		30431
#define MAX_SOCKET_TO_HANDLE	10
#define REG_ACCESS_ATTRIBUTE	"direct_reg_access"
#define IIOD_CONN_BUFFER_SIZE	0x1000
#define NO_TRIGGER				(uint32_t)-1

#define NO_OS_STRINGIFY(x) #x
#define NO_OS_TOSTRING(x) NO_OS_STRINGIFY(x)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

static char uart_buff[IIOD_CONN_BUFFER_SIZE];

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
	"<context name=\"xml\" description=\"no-OS/projects/"
	NO_OS_TOSTRING(NO_OS_PROJECT)" "
	NO_OS_TOSTRING(NO_OS_VERSION)"\" >";
static char header_end[] = "</context>";

static const char * const iio_chan_type_string[] = {
	[IIO_VOLTAGE] = "voltage",
	[IIO_CURRENT] = "current",
	[IIO_ALTVOLTAGE] = "altvoltage",
	[IIO_ANGL_VEL] = "anglvel",
	[IIO_TEMP] = "temp",
	[IIO_CAPACITANCE] = "capacitance",
	[IIO_ACCEL] = "accel",
	[IIO_RESISTANCE] = "resistance",
	[IIO_MAGN] = "magn",
	[IIO_INCLI] = "incli",
	[IIO_VELOCITY] = "velocity",
	[IIO_ANGL] = "angl",
	[IIO_ROT] = "rot",
	[IIO_COUNT] = "count",
	[IIO_DELTA_ANGL] = "deltaangl",
	[IIO_DELTA_VELOCITY] = "deltavelocity",
};

static const char * const iio_modifier_names[] = {
	[IIO_MOD_X] = "x",
	[IIO_MOD_Y] = "y",
	[IIO_MOD_Z] = "z",
	[IIO_MOD_TEMP_AMBIENT] = "ambient",
	[IIO_MOD_PITCH] = "pitch",
	[IIO_MOD_YAW] = "yaw",
	[IIO_MOD_ROLL] = "roll",
};

/* Parameters used in show and store functions */
struct attr_fun_params {
	void			*dev_instance;
	char			*buf;
	uint32_t			len;
	struct iio_ch_info	*ch_info;
};

struct iio_buffer_priv {
	/* Field visible by user */
	struct iio_buffer	public;
	/** Buffer to read or write data. A reference will be found in buffer */
	struct no_os_circular_buffer	cb;
	/* Buffer provide by user. */
	int8_t			*raw_buf;
	/* Length of raw_buf */
	uint32_t		raw_buf_len;
	/* Set when this devices has buffer */
	bool			initalized;
	/* Set when no_os_calloc was used to initalize cb.buf */
	bool			allocated;
};

/**
 * @struct iio_dev_priv
 * @brief Links a physical device instance "void *dev_instance"
 * with a "iio_device *iio" that describes capabilities of the device.
 */
struct iio_dev_priv {
	/** Will be: iio:device[0...n] n beeing the count of registerd devices*/
	char			dev_id[MAX_DEV_ID];
	/** Device name */
	const char		*name;
	/** Physical instance of a device */
	void			*dev_instance;
	/** Structure to be passed to callbacks */
	struct iio_device_data  dev_data;
	/** Used to read debug attributes */
	uint32_t		active_reg_addr;
	/** Device descriptor(describes channels and attributes) */
	struct iio_device	*dev_descriptor;
	/* Structure storing buffer related fields */
	struct iio_buffer_priv buffer;
	/* Set to -1 when no trigger is set*/
	uint32_t		trig_idx;
};

/**
 * @struct iio_trig_priv
 * @brief Links a physical trigger instance "void *instance"
 * with a "iio_trigger *descriptor" that describes capabilities of the trigger.
 */
struct iio_trig_priv {
	/** Will be: iio:trigger[0...n] */
	char	id[MAX_TRIG_ID];
	/** Trigger name */
	char	*name;
	/** Physical instance of a trigger */
	void	*instance;
	/** Trigger descriptor(describes type of trigger and its attributes) */
	struct iio_trigger *descriptor;
	/** Set to true when the triggering condition is met */
	bool	triggered;
};

struct iio_desc {
	struct iiod_desc	*iiod;
	struct iiod_ops		iiod_ops;
	void			*phy_desc;
	char			*xml_desc;
	uint32_t		xml_size;
	struct iio_ctx_attr	*ctx_attrs;
	uint32_t		nb_ctx_attr;
	struct iio_dev_priv	*devs;
	uint32_t		nb_devs;
	struct iio_trig_priv	*trigs;
	uint32_t		nb_trigs;
	struct no_os_uart_desc	*uart_desc;
	int (*recv)(void *conn, uint8_t *buf, uint32_t len);
	int (*send)(void *conn, uint8_t *buf, uint32_t len);
	/* FIFO for socket descriptors */
	struct no_os_circular_buffer	*conns;
#if defined(NO_OS_NETWORKING) || defined(NO_OS_LWIP_NETWORKING)
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

	no_os_cb_size(desc->conns, &size);
	if (size < sizeof(uint32_t))
		return -EAGAIN;

	return no_os_cb_read(desc->conns, conn_id, sizeof(*conn_id));
}

static inline int32_t _push_conn(struct iio_desc *desc, uint32_t conn_id)
{
	return no_os_cb_write(desc->conns, &conn_id, sizeof(conn_id));
}

static inline int32_t _nb_active_conns(struct iio_desc *desc)
{
	uint32_t size;

	no_os_cb_size(desc->conns, &size);

	return size / sizeof(uint32_t);
}


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
	char	ch_id[MAX_CHN_ID];

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
 * @brief Find interface with "trigger_id".
 * @param trigger_id - Trigger id (trigger0, trigger1, etc.).
 * @param iio_trig_privs - List of interfaces.
 * @return Interface pointer if interface is found, NULL otherwise.
 */
static struct iio_trig_priv *get_iio_trig_device(struct iio_desc *desc,
		const char *trigger_id)
{
	uint32_t i;

	for (i = 0; i < desc->nb_trigs; i++) {
		if (strcmp(desc->trigs[i].id, trigger_id) == 0)
			return &desc->trigs[i];
	}

	return NULL;
}

/**
 * @brief Sets buffers count.
 * @param ctx           - IIO instance and conn instance.
 * @param device        - String containing device name.
 * @param buffers_count - Value to be set.
 * @return Positive if index was set, negative if not.
 */
static int iio_set_buffers_count(struct iiod_ctx *ctx, const char *device,
				 uint32_t buffers_count)
{
	struct iio_desc *desc = ctx->instance;

	if(!get_iio_device(desc, device))
		return -ENODEV;

	/* Our implementation uses a circular buffer to send/receive data so
	 * only 1 is a valid value.
	 */
	if (buffers_count != 1)
		return -EINVAL;

	return 0;
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
	/* TODO Not sure if working corectly */
	return -EINVAL;

#if 0
	int16_t i = 0, j = 0;
	char local_buf[256];
	int attr_length;
	uint32_t *pattr_length;

	while (attributes[i].name) {
		attr_length = attributes[i].show(params->dev_instance,
						 local_buf, params->len,
						 params->ch_info,
						 attributes[i].priv);
		if (NO_OS_IS_ERR_VALUE(attr_length))
			attr_length = snprintf(local_buf, params->len, "%d",
					       attr_length);

		attr_length += 1;//Add '\0' to the count
		pattr_length = (uint32_t *)(params->buf + j);
		if (j + 4 > params->len)
			return -EINVAL;
		*pattr_length = no_os_bswap_constant_32(attr_length);
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
#endif
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
	/* TODO Not sure if working corectly */
	return -EINVAL;

#if 0
	int16_t i = 0, j = 0;
	int16_t attr_length;

	while (attributes[i].name) {
		attr_length = no_os_bswap_constant_32((uint32_t)(params->buf + j));
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
#endif
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
	if (NO_OS_IS_ERR_VALUE(ret))
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
		if (NO_OS_IS_ERR_VALUE(ret))
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

	*_fract = strtol(p, NULL, 10);

	return 0;
}

static int32_t _iio_fract_interpret(int32_t fract, int32_t subunits)
{
	int32_t temp = fract;

	while ((subunits != 0) || (temp != 0)) {
		temp /= 10;
		subunits /= 10;
		if (!temp)
			break;
		if (subunits <= 1)
			fract /= 10;
	}

	return fract * subunits;
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
		_fract = _iio_fract_interpret(_fract, 1000000);
		break;
	case IIO_VAL_INT_PLUS_MICRO:
		ret = __iio_str_parse(buf, &integer, &_fract, false);
		if (ret < 0)
			return ret;
		_fract = _iio_fract_interpret(_fract, 1000000);
		break;
	case IIO_VAL_INT_PLUS_NANO:
		ret = __iio_str_parse(buf, &integer, &_fract, false);
		if (ret < 0)
			return ret;
		_fract = _iio_fract_interpret(_fract, 1000000000);
		break;
	case IIO_VAL_FRACTIONAL:
		ret = __iio_str_parse(buf, &integer, &_fract, false);
		if (ret < 0)
			return ret;
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
	int64_t tmp;
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
		tmp = no_os_div_s64((int64_t)vals[0] * 1000000000LL, vals[1]);
		fractional = vals[1];
		integer = (int32_t)no_os_div_s64_rem(tmp, 1000000000, &fractional);

		if (integer == 0 && fractional < 0)
			return snprintf(buf, len, "-0.%09u", abs(fractional));

		return snprintf(buf, len, "%"PRIi32".%09u", integer,
				abs(fractional));
	case IIO_VAL_FRACTIONAL_LOG2:
		tmp = no_os_shift_right((int64_t)vals[0] * 1000000000LL, vals[1]);
		integer = (int32_t)no_os_div_s64_rem(tmp, 1000000000LL, &fractional);

		if (integer == 0 && fractional < 0)
			return snprintf(buf, len, "-0.%09u", abs(fractional));

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
 * @brief Returns trigger attributes.
 * @param type - Attribute type.
 * @param trig - Trigger instance.
 * @return Attributes pointer if attributes exist, NULL otherwise.
 */
static struct iio_attribute *get_trig_attributes(enum iio_attr_type type,
		struct iio_trig_priv *trig)
{
	switch (type) {
	/* Only device type attributes allowed for triggers */
	case IIO_ATTR_TYPE_DEVICE:
		return trig->descriptor->attributes;
		break;
	default:
		break;
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
	struct iio_trig_priv *trig_dev;
	struct iio_ch_info ch_info;
	struct iio_channel *ch = NULL;
	struct attr_fun_params params;
	struct iio_attribute *attributes;
	int8_t ch_out;

	dev = get_iio_device(ctx->instance, device);

	/* If IIO device with given name is found, handle reading of attributes */
	if (dev) {
		if (attr->type == IIO_ATTR_TYPE_DEBUG &&
		    strcmp(attr->name, REG_ACCESS_ATTRIBUTE) == 0) {
			if (dev->dev_descriptor->debug_reg_read)
				return debug_reg_read(dev, buf, len);
			return -ENOENT;
		}

		if (attr->channel[0] != '\0') {
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
		return iio_rd_wr_attribute(&params, attributes, attr->name, 0);
	}

	/* IIO device with given name is not found, verify if it corresponds to a trigger */
	trig_dev = get_iio_trig_device(ctx->instance, device);

	/* If IIO trigger with given name is found, handle reading of attributes */
	if(trig_dev) {
		params.ch_info = NULL; /* Triggers cannot have channels */
		params.buf = buf;
		params.len = len;
		params.dev_instance = trig_dev->instance;
		attributes = get_trig_attributes(attr->type, trig_dev);
		if (!strcmp(attr->name, ""))
			return iio_read_all_attr(&params, attributes);
		return iio_rd_wr_attribute(&params, attributes, attr->name, 0);
	}

	/* No device and no trigger with given name were found */
	return -ENODEV;
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
	struct iio_trig_priv *trig_dev;
	struct attr_fun_params	params;
	struct iio_attribute	*attributes;
	struct iio_ch_info ch_info;
	struct iio_channel *ch = NULL;
	int8_t ch_out;

	dev = get_iio_device(ctx->instance, device);

	/* If IIO device with given name is found, handle writing of attributes */
	if (dev) {

		if (attr->type == IIO_ATTR_TYPE_DEBUG &&
		    strcmp(attr->name, REG_ACCESS_ATTRIBUTE) == 0) {
			if (dev->dev_descriptor->debug_reg_write)
				return debug_reg_write(dev, buf, len);
			return -ENOENT;
		}

		if (attr->channel[0] != '\0') {
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
		return iio_rd_wr_attribute(&params, attributes, attr->name, 1);
	}

	/* IIO device with given name is not found, verify if it corresponds to a trigger */
	trig_dev = get_iio_trig_device(ctx->instance, device);

	/* If IIO trigger with given name is found, handle writing of attributes */
	if(trig_dev) {
		params.ch_info = NULL; /* Triggers cannot have channels */
		params.buf = (char *)buf;
		params.len = len;
		params.dev_instance = trig_dev->instance;
		attributes = get_trig_attributes(attr->type, trig_dev);
		if (!strcmp(attr->name, ""))
			return iio_read_all_attr(&params, attributes);
		return iio_rd_wr_attribute(&params, attributes, attr->name, 1);
	}

	/* No device and no trigger with given name were found */
	return -ENODEV;
}

/**
 * @brief Searches for trigger id and returns trigger index.
 * @param desc - IIO descriptor.
 * @param id   - Trigger id (trigger0, trigger1, etc.).
 * @return Trigger index. NO_TRIGGER in case trigger is not found.
 */
static uint32_t iio_get_trig_idx_by_id(struct iio_desc *desc, const char *id)
{
	uint32_t i;

	if (!id)
		return NO_TRIGGER;

	for (i = 0; i < desc->nb_trigs; i++)
		if (strcmp(desc->trigs[i].id, id) == 0)
			return i;

	return NO_TRIGGER;
}

/**
 * @brief Searches for trigger name and returns trigger index.
 * @param desc - IIO descriptor.
 * @param name - Trigger name.
 * @return Trigger index. NO_TRIGGER in case trigger is not found.
 */
static uint32_t iio_get_trig_idx_by_name(struct iio_desc *desc,
		const char *name)
{
	uint32_t i;

	if (!name)
		return NO_TRIGGER;

	for (i = 0; i < desc->nb_trigs; i++)
		if (strcmp(desc->trigs[i].name, name) == 0)
			return i;

	return NO_TRIGGER;
}

/**
 * @brief Searches for active trigger of the given device and returns trigger name.
 * @param ctx     - IIO instance and conn instance.
 * @param device  - String containing device name.
 * @param trigger - Trigger name to be returned.
 * @param len     - Maximum length of value to be stored in name.
 * @return Number of bytes written in name.
 */
static int iio_get_trigger(struct iiod_ctx *ctx, const char *device,
			   char *trigger, uint32_t len)
{
	struct iio_dev_priv *dev;
	struct iio_trig_priv *trig;
	struct iio_desc *desc = ctx->instance;

	if (!desc->nb_trigs)
		return -ENOENT;

	trig = get_iio_trig_device(desc, device);
	/* Device is a trigger and triggers cannot have other triggers */
	if (trig)
		return -ENOENT;

	dev = get_iio_device(desc, device);
	if (!dev)
		return -ENODEV;

	if (dev->trig_idx == NO_TRIGGER) {
		trigger[0] = '\0';

		return 0;
	}

	return snprintf(trigger, len, "%s", desc->trigs[dev->trig_idx].name);
}

/**
 * @brief Searches for given trigger id for the given device and if found, it
 * sets the trigger.
 * @param ctx     - IIO instance and conn instance.
 * @param device  - String containing device name.
 * @param trigger - Trigger id to be set.
 * @param len     - Maximum length of value to be returned.
 * @return Positive if index was set, negative if not.
 */
static int iio_set_trigger(struct iiod_ctx *ctx, const char *device,
			   const char *trigger, uint32_t len)
{
	struct iio_dev_priv	*dev;
	struct iio_trig_priv	*trig;
	uint32_t i;
	struct iio_desc *desc = ctx->instance;

	if (!desc->nb_trigs)
		return -ENOENT;

	trig = get_iio_trig_device(desc, device);
	/* Device is a trigger and triggers cannot have other triggers */
	if (trig)
		return -ENOENT;

	dev = get_iio_device(desc, device);
	if (!dev)
		return -ENODEV;

	if (trigger[0] == '\0') {
		dev->trig_idx = NO_TRIGGER;
		return 0;
	}

	i = iio_get_trig_idx_by_id(desc, trigger);
	if (i == NO_TRIGGER)
		return -EINVAL;

	dev->trig_idx = i;

	return len;
}

/**
 * @brief Asynchronous trigger processing routine.
 * @param desc - IIO descriptor.
 */
static void iio_process_async_triggers(struct iio_desc *desc)
{
	struct iio_dev_priv *dev;
	uint32_t i;

	for (i = 0; i < desc->nb_devs; i++) {
		dev = desc->devs + i;
		if (dev->trig_idx == NO_TRIGGER)
			continue;

		if (!desc->trigs[dev->trig_idx].triggered)
			continue;

		if (dev->dev_descriptor->trigger_handler) {
			dev->dev_descriptor->trigger_handler(&dev->dev_data);
			desc->trigs[i].triggered = 0;
		}
	}
}

/**
 * @brief Searches for trigger name and processes the trigger based on its
 * type (sync or async with the interrupt).
 * @param desc         - IIO descriptor.
 * @param trigger_name - Trigger name.
 *
 * @return ret - Result of the processing procedure.
 */
int iio_process_trigger_type(struct iio_desc *desc, char *trigger_name)
{
	uint32_t i;
	uint32_t trig_id;
	struct iio_trig_priv *trig;

	trig_id = iio_get_trig_idx_by_name(desc, trigger_name);

	if (trig_id == NO_TRIGGER)
		return -EINVAL;

	struct iio_dev_priv *dev;

	for (i = 0; i < desc->nb_devs; i++) {
		dev = desc->devs + i;
		if (dev->trig_idx == trig_id) {
			trig = &desc->trigs[trig_id];
			if (trig->descriptor->is_synchronous) {
				if (dev->dev_descriptor->trigger_handler)
					dev->dev_descriptor->trigger_handler(&dev->dev_data);
			} else {
				trig->triggered = 1;
			}
		}
	}

	return 0;
}

static uint32_t bytes_per_scan(struct iio_channel *channels, uint32_t mask)
{
	uint32_t cnt, i, length, largest = 1;

	cnt = 0;
	i = 0;
	while (mask) {
		if ((mask & 1)) {
			length = channels[i].scan_type->storagebits / 8;

			if (length > largest)
				largest = length;

			if (cnt % length)
				cnt += 2 * length - (cnt % length);
			else
				cnt += length;
		}

		mask >>= 1;
		++i;
	}

	if (cnt % largest)
		cnt += largest - (cnt % largest);

	return cnt;
}

/**
 * @brief  Open device.
 * @param ctx - IIO instance and conn instance
 * @param device - String containing device name.
 * @param sample_size - Sample size.
 * @param mask - Channels to be opened.
 * @return 0, negative value in case of failure.
 */
static int iio_open_dev(struct iiod_ctx *ctx, const char *device,
			uint32_t samples, uint32_t mask, bool cyclic)
{
	struct iio_desc *desc;
	struct iio_dev_priv *dev;
	struct iio_trig_priv *trig;
	uint32_t ch_mask;
	int32_t ret;
	int8_t *buf;
	uint32_t buf_size;

	dev = get_iio_device(ctx->instance, device);
	if (!dev)
		return -ENODEV;

	if (!dev->buffer.initalized)
		return -EINVAL;

	ch_mask = 0xFFFFFFFF >> (32 - dev->dev_descriptor->num_ch);
	mask &= ch_mask;
	if (!mask)
		return -ENOENT;

	dev->buffer.public.cyclic_info.is_cyclic = cyclic;
	dev->buffer.public.cyclic_info.buff_index = 0;

	dev->buffer.public.active_mask = mask;
	dev->buffer.public.bytes_per_scan =
		bytes_per_scan(dev->dev_descriptor->channels, mask);
	dev->buffer.public.size = dev->buffer.public.bytes_per_scan * samples;
	dev->buffer.public.samples = samples;
	if (dev->buffer.raw_buf && dev->buffer.raw_buf_len) {
		if (dev->buffer.raw_buf_len < dev->buffer.public.size)
			/* Need a bigger buffer or to allocate */
			return -ENOMEM;
		buf_size = dev->buffer.raw_buf_len - (dev->buffer.raw_buf_len %
						      dev->buffer.public.size);
		buf = dev->buffer.raw_buf;
	} else {
		if (dev->buffer.allocated) {
			/* Free in case iio_close_dev wasn't called to free it*/
			no_os_free(dev->buffer.cb.buff);
			dev->buffer.allocated = 0;
		}
		buf_size = dev->buffer.public.size;
		buf = (int8_t *)no_os_calloc(dev->buffer.public.size, sizeof(*buf));
		if (!buf)
			return -ENOMEM;
		dev->buffer.allocated = 1;
	}

	ret = no_os_cb_cfg(&dev->buffer.cb, buf, buf_size);
	if (NO_OS_IS_ERR_VALUE(ret)) {
		if (dev->buffer.allocated) {
			no_os_free(dev->buffer.cb.buff);
			dev->buffer.allocated = 0;
		}

		return ret;
	}

	if (dev->dev_descriptor->pre_enable) {
		ret = dev->dev_descriptor->pre_enable(dev->dev_instance, mask);
		if (NO_OS_IS_ERR_VALUE(ret)) {
			if (dev->buffer.allocated) {
				no_os_free(dev->buffer.cb.buff);
				dev->buffer.allocated = 0;
			}
			return ret;
		}
	}

	desc = ctx->instance;
	if (dev->trig_idx != NO_TRIGGER) {
		trig = &desc->trigs[dev->trig_idx];
		if (trig->descriptor->enable)
			ret = trig->descriptor->enable(trig->instance);
	}

	return ret;
}

/**
 * @brief Close device.
 * @param ctx - IIO instance and conn instance
 * @param device - String containing device name.
 * @return 0, negative value in case of failure.
 */
static int iio_close_dev(struct iiod_ctx *ctx, const char *device)
{
	struct iio_desc *desc;
	struct iio_dev_priv *dev;
	struct iio_trig_priv *trig;
	int ret = 0;

	dev = get_iio_device(ctx->instance, device);
	if (!dev)
		return -1;

	if (!dev->buffer.initalized)
		return -EINVAL;

	if (dev->buffer.allocated) {
		/* Should something else be used to free internal strucutre */
		no_os_free(dev->buffer.cb.buff);
		dev->buffer.allocated = 0;
	}

	desc = ctx->instance;
	if(dev->trig_idx != NO_TRIGGER) {
		trig = &desc->trigs[dev->trig_idx];
		if (trig->descriptor->disable) {
			ret = trig->descriptor->disable(trig->instance);
			if (ret)
				return ret;
		}
	}

	dev->buffer.public.active_mask = 0;
	if (dev->dev_descriptor->post_disable)
		ret = dev->dev_descriptor->post_disable(dev->dev_instance);

	return ret;
}

static int iio_call_submit(struct iiod_ctx *ctx, const char *device,
			   enum iio_buffer_direction dir)
{
	struct iio_dev_priv *dev;

	dev = get_iio_device(ctx->instance, device);
	if (!dev || !dev->buffer.initalized)
		return -EINVAL;

	dev->buffer.public.dir = dir;
	if (dev->dev_descriptor->submit && dev->trig_idx==NO_TRIGGER)
		return dev->dev_descriptor->submit(&dev->dev_data);
	else if ((dir == IIO_DIRECTION_INPUT && dev->dev_descriptor->read_dev
		  && dev->trig_idx==NO_TRIGGER)
		 || (dir == IIO_DIRECTION_OUTPUT &&
		     dev->dev_descriptor->write_dev && dev->trig_idx==NO_TRIGGER)) {
		/* Code used to don't break devices using read_dev */
		int32_t ret;
		void *buff;
		struct iio_buffer *buffer = &dev->buffer.public;

		ret = iio_buffer_get_block(buffer, &buff);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;

		if (dir == IIO_DIRECTION_INPUT)
			ret = dev->dev_descriptor->read_dev(dev->dev_instance,
							    buff, buffer->samples);
		else
			ret = dev->dev_descriptor->write_dev(dev->dev_instance,
							     buff, buffer->samples);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;

		return iio_buffer_block_done(buffer);
	}

	return 0;
}

static int iio_push_buffer(struct iiod_ctx *ctx, const char *device)
{
	return iio_call_submit(ctx, device, IIO_DIRECTION_OUTPUT);
}

static int iio_refill_buffer(struct iiod_ctx *ctx, const char *device)
{
	return iio_call_submit(ctx, device, IIO_DIRECTION_INPUT);
}

/**
 * @brief Read chunk of data from RAM to pbuf. Call
 * "iio_transfer_dev_to_mem()" first.
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
	int32_t			ret;
	uint32_t		size;

	dev = get_iio_device(ctx->instance, device);
	if (!dev || !dev->buffer.initalized)
		return -EINVAL;

	ret = no_os_cb_size(&dev->buffer.cb, &size);
#ifdef IIO_IGNORE_BUFF_OVERRUN_ERR
#warning Buffer overrun error checking is disabled.
	if (ret != -NO_OS_EOVERRUN)
#endif
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;

	bytes = no_os_min(size, bytes);
	if (!bytes)
		return -EAGAIN;


	ret = no_os_cb_read(&dev->buffer.cb, buf, bytes);
#ifdef IIO_IGNORE_BUFF_OVERRUN_ERR
	if (ret != -NO_OS_EOVERRUN)
#endif
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;

	return bytes;
}


/**
 * @brief Write chunk of data into RAM.
 * @param device - String containing device name.
 * @param buf - Values to write.
 * @param offset - Offset in memory after the nth chunk of data.
 * @param bytes_count - Number of bytes to write.
 * @return Bytes_count or negative value in case of error.
 */
static int iio_write_buffer(struct iiod_ctx *ctx, const char *device,
			    const char *buf, uint32_t bytes)
{
	struct iio_dev_priv	*dev;
	int32_t			ret;
	uint32_t		available;
	uint32_t		size;

	dev = get_iio_device(ctx->instance, device);
	if (!dev || !dev->buffer.initalized)
		return -EINVAL;

	ret = no_os_cb_size(&dev->buffer.cb, &size);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	available = dev->buffer.public.size - size;
	bytes = no_os_min(available, bytes);
	ret = no_os_cb_write(&dev->buffer.cb, buf, bytes);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	return bytes;
}

int iio_buffer_get_block(struct iio_buffer *buffer, void **addr)
{
	uint32_t size;

	if (!buffer)
		return -EINVAL;

	if (buffer->dir == IIO_DIRECTION_INPUT)
		return no_os_cb_prepare_async_write(buffer->buf, buffer->size, addr, &size);

	return no_os_cb_prepare_async_read(buffer->buf, buffer->size, addr, &size);
}

int iio_buffer_block_done(struct iio_buffer *buffer)
{
	if (!buffer)
		return -EINVAL;

	if (buffer->dir == IIO_DIRECTION_INPUT)
		return no_os_cb_end_async_write(buffer->buf);

	return no_os_cb_end_async_read(buffer->buf);
}

/* Write to buffer iio_buffer.bytes_per_scan bytes from data */
int iio_buffer_push_scan(struct iio_buffer *buffer, void *data)
{
	if (!buffer)
		return -EINVAL;

	return no_os_cb_write(buffer->buf, data, buffer->bytes_per_scan);
}

/* Read from buffer iio_buffer.bytes_per_scan bytes into data */
int iio_buffer_pop_scan(struct iio_buffer *buffer, void *data)
{
	if (!buffer)
		return -EINVAL;

	int ret;

	ret = no_os_cb_read(buffer->buf, data, buffer->bytes_per_scan);

	if (buffer->cyclic_info.is_cyclic) {
		if (buffer->buf->read.idx == buffer->buf->write.idx)
			buffer->buf->read.idx = 0;
	}

	return ret;
}

#if defined(NO_OS_NETWORKING) || defined(NO_OS_LWIP_NETWORKING)

static int32_t accept_network_clients(struct iio_desc *desc)
{
	struct tcp_socket_desc *sock;
	struct iiod_conn_data data;
	int32_t ret;
	uint32_t id;

	do {
		ret = socket_accept(desc->server, &sock);
		if (NO_OS_IS_ERR_VALUE(ret))
			return ret;

		data.conn = sock;
		data.buf = no_os_calloc(1, IIOD_CONN_BUFFER_SIZE);
		data.len = IIOD_CONN_BUFFER_SIZE;

		if (!data.buf) {
			ret = -ENOMEM;
			goto close_socket;
		}

		ret = iiod_conn_add(desc->iiod, &data, &id);
		if (NO_OS_IS_ERR_VALUE(ret))
			goto free_buf;

		ret = _push_conn(desc, id);
		if (NO_OS_IS_ERR_VALUE(ret))
			goto remove_conn;
	} while (true);

	return 0;

remove_conn:
	iiod_conn_remove(desc->iiod, id, &data);
free_buf:
	no_os_free(data.buf);
close_socket:
	socket_remove(sock);

	return ret;
}
#endif

/**
 * @brief Execute an iio step
 * @param desc - IIo descriptor
 * @return 0 in case of success or negative value otherwise.
 */
int iio_step(struct iio_desc *desc)
{
	struct iiod_conn_data data;
	uint32_t conn_id;
	int32_t ret;

	iio_process_async_triggers(desc);

#if defined(NO_OS_NETWORKING) || defined(NO_OS_LWIP_NETWORKING)
	if (desc->server) {
		ret = accept_network_clients(desc);
		if (NO_OS_IS_ERR_VALUE(ret) && ret != -EAGAIN)
			return ret;
#if defined(NO_OS_LWIP_NETWORKING)
		no_os_lwip_step(desc->server->net->net, desc->server->net->net);
#endif
	}
#endif

	ret = _pop_conn(desc, &conn_id);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	ret = iiod_conn_step(desc->iiod, conn_id);
	if (ret == -ENOTCONN) {
#if defined(NO_OS_NETWORKING) || defined(NO_OS_LWIP_NETWORKING)
		iiod_conn_remove(desc->iiod, conn_id, &data);
		socket_remove(data.conn);
		no_os_free(data.buf);
#endif
	} else {
		_push_conn(desc, conn_id);
	}

	return ret;
}

/**
 * @brief Add context attributes into xml string buffer.
 * @param desc - IIo descriptor.
 * @param buff - xml buffer.
 * @param buff_size - size of buffer
 * @return 0 in case of success or negative value otherwise.
 */
static uint32_t iio_add_ctx_attr_in_xml(struct iio_desc *desc, char *buff,
					uint32_t buff_size)
{
	struct iio_ctx_attr *attr;
	char dummy_buff[50];
	int32_t i;
	int32_t j;
	int32_t n;

	if ((int32_t)buff_size == -1)
		n = 0;
	else
		n = buff_size;

	if (buff == NULL)
		/* Set dummy value for buff. It is used only for counting */
		buff = dummy_buff;

	i = 0;

	attr = desc->ctx_attrs;
	if (attr)
		for (j = 0; j < (int32_t)desc->nb_ctx_attr; j++) {
			i += snprintf(buff + i,
				      no_os_max(n - i, 0),
				      "<context-attribute name=\"%s\" ",
				      attr[j].name);

			i += snprintf(buff + i,
				      no_os_max(n - i, 0),
				      "value=\"%s\" />",
				      attr[j].value);
		}

	return i;
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

	i += snprintf(buff + i, no_os_max(n - i, 0),
		      "<device id=\"%s\" name=\"%s\">", id, name);

	/* Write channels */
	if (device->channels)
		for (j = 0; j < device->num_ch; j++) {
			ch = &device->channels[j];
			_print_ch_id(ch_id, ch);
			i += snprintf(buff + i, no_os_max(n - i, 0),
				      "<channel id=\"%s\"",
				      ch_id);
			if(ch->name)
				i += snprintf(buff + i, no_os_max(n - i, 0),
					      " name=\"%s\"",
					      ch->name);
			i += snprintf(buff + i, no_os_max(n - i, 0),
				      " type=\"%s\" >",
				      ch->ch_out ? "output" : "input");

			if (ch->scan_type)
				i += snprintf(buff + i, no_os_max(n - i, 0),
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
					i += snprintf(buff + i, no_os_max(n - i, 0), "<attribute name=\"%s\" ",
						      attr->name);
					if (ch->diferential) {
						switch (attr->shared) {
						case IIO_SHARED_BY_ALL:
							i += snprintf(buff + i, no_os_max(n - i, 0),
								      "filename=\"%s\"",
								      attr->name);
							break;
						case IIO_SHARED_BY_DIR:
							i += snprintf(buff + i, no_os_max(n - i, 0),
								      "filename=\"%s_%s\"",
								      ch->ch_out ? "out" : "in",
								      attr->name);
							break;
						case IIO_SHARED_BY_TYPE:
							i += snprintf(buff + i, no_os_max(n - i, 0),
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
							i += snprintf(buff + i, no_os_max(n - i, 0),
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
							i += snprintf(buff + i, no_os_max(n - i, 0),
								      "filename=\"%s\"",
								      attr->name);
							break;
						case IIO_SHARED_BY_DIR:
							i += snprintf(buff + i, no_os_max(n - i, 0),
								      "filename=\"%s_%s\"",
								      ch->ch_out ? "out" : "in",
								      attr->name);
							break;
						case IIO_SHARED_BY_TYPE:
							i += snprintf(buff + i, no_os_max(n - i, 0),
								      "filename=\"%s_%s_%s\"",
								      ch->ch_out ? "out" : "in",
								      iio_chan_type_string[ch->ch_type],
								      attr->name);
							break;
						case IIO_SEPARATE:
							if (ch->indexed)
								i += snprintf(buff + i, no_os_max(n - i, 0),
									      "filename=\"%s_%s%d_%s\"",
									      ch->ch_out ? "out" : "in",
									      iio_chan_type_string[ch->ch_type],
									      ch->channel,
									      attr->name);
							else
								i += snprintf(buff + i, no_os_max(n - i, 0),
									      "filename=\"%s_%s_%s\"",
									      ch->ch_out ? "out" : "in",
									      iio_chan_type_string[ch->ch_type],
									      attr->name);
							break;
						}
					}
					i += snprintf(buff + i, no_os_max(n - i, 0), " />");
				}

			i += snprintf(buff + i, no_os_max(n - i, 0), "</channel>");
		}

	/* Write device attributes */
	if (device->attributes)
		for (j = 0; device->attributes[j].name; j++)
			i += snprintf(buff + i, no_os_max(n - i, 0),
				      "<attribute name=\"%s\" />",
				      device->attributes[j].name);

	/* Write debug attributes */
	if (device->debug_attributes)
		for (j = 0; device->debug_attributes[j].name; j++)
			i += snprintf(buff + i, no_os_max(n - i, 0),
				      "<debug-attribute name=\"%s\" />",
				      device->debug_attributes[j].name);
	if (device->debug_reg_read || device->debug_reg_write)
		i += snprintf(buff + i, no_os_max(n - i, 0),
			      "<debug-attribute name=\""REG_ACCESS_ATTRIBUTE"\" />");

	/* Write buffer attributes */
	if (device->buffer_attributes)
		for (j = 0; device->buffer_attributes[j].name; j++)
			i += snprintf(buff + i, no_os_max(n - i, 0),
				      "<buffer-attribute name=\"%s\" />",
				      device->buffer_attributes[j].name);

	i += snprintf(buff + i, no_os_max(n - i, 0), "</device>");

	return i;
}

static int32_t iio_init_xml(struct iio_desc *desc)
{
	struct iio_dev_priv *dev;
	struct iio_trig_priv *trig;
	struct iio_device dummy = { 0 };
	uint32_t i, size, of;

	/* -2 because of the 0 character */
	size = sizeof(header) + sizeof(header_end) - 2;
	size += iio_add_ctx_attr_in_xml(desc, NULL, -1);
	for (i = 0; i < desc->nb_devs; i++) {
		dev = desc->devs + i;
		size += iio_generate_device_xml(dev->dev_descriptor,
						(char *)dev->name,
						dev->dev_id, NULL, -1);
	}
	for (i = 0; i < desc->nb_trigs; i++) {
		trig = desc->trigs + i;
		dummy.attributes = trig->descriptor->attributes;
		size += iio_generate_device_xml(&dummy, trig->name, trig->id,
						NULL, -1);
	}

	desc->xml_desc = (char *)no_os_calloc(size + 1, sizeof(*desc->xml_desc));
	if (!desc->xml_desc)
		return -ENOMEM;

	desc->xml_size = size;

	strcpy(desc->xml_desc, header);
	of = sizeof(header) - 1;
	of += iio_add_ctx_attr_in_xml(desc, desc->xml_desc + of, size - of);
	for (i = 0; i < desc->nb_devs; i++) {
		dev = desc->devs + i;
		of += iio_generate_device_xml(dev->dev_descriptor,
					      (char *)dev->name, dev->dev_id,
					      desc->xml_desc + of, size - of);
	}
	for (i = 0; i < desc->nb_trigs; i++) {
		trig = desc->trigs + i;
		dummy.attributes = trig->descriptor->attributes;
		of += iio_generate_device_xml(&dummy, trig->name, trig->id,
					      desc->xml_desc + of, size - of);
	}

	strcpy(desc->xml_desc + of, header_end);

	return 0;
}

static int32_t iio_init_devs(struct iio_desc *desc,
			     struct iio_device_init *devs, uint32_t n)
{
	uint32_t i;
	struct iio_dev_priv *ldev;
	struct iio_device_init *ndev;

	desc->nb_devs = n;
	desc->devs = (struct iio_dev_priv *)no_os_calloc(desc->nb_devs,
			sizeof(*desc->devs));
	if (!desc->devs)
		return -ENOMEM;

	for (i = 0; i < n; i++) {
		ndev = devs + i;
		ldev = desc->devs + i;
		ldev->dev_descriptor = ndev->dev_descriptor;
		sprintf(ldev->dev_id, "iio:device%"PRIu32"", i);
		ldev->trig_idx = iio_get_trig_idx_by_id(desc, ndev->trigger_id);
		ldev->dev_instance = ndev->dev;
		ldev->dev_data.dev = ndev->dev;
		ldev->dev_data.buffer = &ldev->buffer.public;
		ldev->name = ndev->name;
		if (ndev->dev_descriptor->read_dev ||
		    ndev->dev_descriptor->write_dev ||
		    ndev->dev_descriptor->submit ||
		    ndev->dev_descriptor->trigger_handler) {
			ldev->buffer.raw_buf = ndev->raw_buf;
			ldev->buffer.raw_buf_len = ndev->raw_buf_len;
			ldev->buffer.public.buf = &ldev->buffer.cb;
			ldev->buffer.initalized = 1;
		} else {
			ldev->buffer.initalized = 0;
		}
	}

	return 0;
}

/**
 * @brief Initializes IIO triggers.
 * @param desc  - IIO descriptor.
 * @param trigs - Triggers array.
 * @param n     - Number of triggers to be initialized.
 * @return 0 in case of success or negative value otherwise.
 */
static int32_t iio_init_trigs(struct iio_desc *desc,
			      struct iio_trigger_init *trigs, uint32_t n)
{
	uint32_t i;
	struct iio_trig_priv *trig_priv_iter;
	struct iio_trigger_init *trig_init_iter;

	desc->nb_trigs = n;
	desc->trigs = (struct iio_trig_priv *)no_os_calloc(desc->nb_trigs,
			sizeof(*desc->trigs));
	if (!desc->trigs)
		return -ENOMEM;

	for (i = 0; i < n; i++) {
		trig_init_iter = trigs + i;
		trig_priv_iter = desc->trigs + i;
		trig_priv_iter->instance = trig_init_iter->trig;
		trig_priv_iter->name = trig_init_iter->name;
		trig_priv_iter->descriptor = trig_init_iter->descriptor;
		sprintf(trig_priv_iter->id, "trigger%"PRIu32"", i);
	}

	return 0;
}

/**
 * @brief Set communication ops and read/write ops
 * @param desc - iio descriptor.
 * @param init_param - appropriate init param.
 * @return 0 in case of success or negative value otherwise.
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

	ldesc = (struct iio_desc *)no_os_calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return -ENOMEM;

	ldesc->ctx_attrs = init_param->ctx_attrs;
	ldesc->nb_ctx_attr = init_param->nb_ctx_attr;

	ret = iio_init_trigs(ldesc, init_param->trigs, init_param->nb_trigs);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto free_devs;

	ret = iio_init_devs(ldesc, init_param->devs, init_param->nb_devs);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto free_desc;

	ret = iio_init_xml(ldesc);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto free_trigs;

	/* device operations */
	ops = &ldesc->iiod_ops;
	ops->read_attr = iio_read_attr;
	ops->write_attr = iio_write_attr;
	ops->get_trigger = iio_get_trigger;
	ops->set_trigger = iio_set_trigger;
	ops->read_buffer = iio_read_buffer;
	ops->write_buffer = iio_write_buffer;
	ops->refill_buffer = iio_refill_buffer;
	ops->push_buffer = iio_push_buffer;
	ops->open = iio_open_dev;
	ops->close = iio_close_dev;
	ops->send = iio_send;
	ops->recv = iio_recv;
	ops->set_buffers_count = iio_set_buffers_count;

	iiod_param.instance = ldesc;
	iiod_param.ops = ops;
	iiod_param.xml = ldesc->xml_desc;
	iiod_param.xml_len = ldesc->xml_size;
	iiod_param.phy_type = init_param->phy_type;

	ret = iiod_init(&ldesc->iiod, &iiod_param);
	if (NO_OS_IS_ERR_VALUE(ret))
		goto free_xml;

	ret = no_os_cb_init(&ldesc->conns,
			    sizeof(uint32_t) * (IIOD_MAX_CONNECTIONS + 1));
	if (NO_OS_IS_ERR_VALUE(ret))
		goto free_iiod;

	if (init_param->phy_type == USE_UART) {
		ldesc->send = (int (*)())no_os_uart_write;
		ldesc->recv = (int (*)())no_os_uart_read;
		ldesc->uart_desc = init_param->uart_desc;

		struct iiod_conn_data data = {
			.conn = ldesc->uart_desc,
			.buf = uart_buff,
			.len = sizeof(uart_buff)
		};
		ret = iiod_conn_add(ldesc->iiod, &data, &conn_id);
		if (NO_OS_IS_ERR_VALUE(ret))
			goto free_conns;
		_push_conn(ldesc, conn_id);
	}
#if defined(NO_OS_NETWORKING) || defined(NO_OS_LWIP_NETWORKING)
	else if (init_param->phy_type == USE_NETWORK) {
		ldesc->send = (int (*)())socket_send;
		ldesc->recv = (int (*)())socket_recv;
		ret = socket_init(&ldesc->server,
				  init_param->tcp_socket_init_param);
		if (NO_OS_IS_ERR_VALUE(ret))
			goto free_conns;
		ret = socket_bind(ldesc->server, IIOD_PORT);
		if (NO_OS_IS_ERR_VALUE(ret))
			goto free_pylink;
		ret = socket_listen(ldesc->server, MAX_BACKLOG);
		if (NO_OS_IS_ERR_VALUE(ret))
			goto free_pylink;
	}
#endif
	else if (init_param->phy_type == USE_LOCAL_BACKEND) {
		ldesc->recv = init_param->local_backend->local_backend_event_read;
		ldesc->send = init_param->local_backend->local_backend_event_write;

		struct iiod_conn_data data = {
			.conn = NULL,
			.buf = init_param->local_backend->local_backend_buff,
			.len = init_param->local_backend->local_backend_buff_len
		};
		ret = iiod_conn_add(ldesc->iiod, &data, &conn_id);
		if (NO_OS_IS_ERR_VALUE(ret))
			goto free_conns;
		_push_conn(ldesc, conn_id);
	} else {
		ret = -EINVAL;
		goto free_conns;
	}

	*desc = ldesc;

	return 0;

free_pylink:
#if defined(NO_OS_NETWORKING) || defined(NO_OS_LWIP_NETWORKING)
	socket_remove(ldesc->server);
#endif
free_conns:
	no_os_cb_remove(ldesc->conns);
free_iiod:
	iiod_remove(ldesc->iiod);
free_xml:
	no_os_free(ldesc->xml_desc);
free_trigs:
	no_os_free(ldesc->trigs);
free_devs:
	no_os_free(ldesc->devs);
free_desc:
	no_os_free(ldesc);

	return ret;
}

/**
 * @brief Free the resources allocated by "iio_init()".
 * @param desc: iio descriptor.
 * @return 0 in case of success or negative value otherwise.
 */
int iio_remove(struct iio_desc *desc)
{
	struct iiod_conn_data data;
	int ret;

	if (!desc)
		return -EINVAL;

#if defined(NO_OS_NETWORKING) || defined(NO_OS_LWIP_NETWORKING)
	for (int i = 0; i < IIOD_MAX_CONNECTIONS; i++) {
		ret = iiod_conn_remove(desc->iiod, i, &data);
		if (!ret) {
			no_os_free(data.buf);
			socket_remove(data.conn);
		}
	}
	socket_remove(desc->server);
#endif
	no_os_cb_remove(desc->conns);
	iiod_remove(desc->iiod);
	no_os_free(desc->devs);
	no_os_free(desc->trigs);
	no_os_free(desc->xml_desc);
	no_os_free(desc);

	return 0;
}
