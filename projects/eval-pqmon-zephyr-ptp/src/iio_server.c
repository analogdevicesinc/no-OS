/*******************************************************************************
 *   @file   iio_server.c
 *   @brief  IIOD server with selectable transport for Zephyr
 *   @author Radu Etz (radu.etz@analog.com)
 ********************************************************************************
 * Copyright (c) 2026 Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

/*
 * Provides the glue between:
 *   - Transport: USB CDC/ACM or TCP socket (compile-time selected via Kconfig)
 *   - no-OS iiod.c text protocol engine (parser)
 *   - iio_pqm.c PQM device attributes (data)
 *
 * Transport is selected at build time:
 *   CONFIG_IIO_TRANSPORT_USB  -> USB CDC/ACM (existing, default)
 *   CONFIG_IIO_TRANSPORT_TCP  -> TCP socket on port 30431 over T1L
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>

#ifdef CONFIG_IIO_TRANSPORT_USB
#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usbd.h>
#endif

#ifdef CONFIG_IIO_TRANSPORT_TCP
#include <zephyr/net/socket.h>
#include <zephyr/net/net_if.h>
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "iio_server.h"
#include "iiod.h"
#include "iio_types.h"
#include "iio_pqm.h"
#include "pqm.h"

LOG_MODULE_REGISTER(iio_server, LOG_LEVEL_INF);

/* Mutex protecting PQLib output from torn reads */
K_MUTEX_DEFINE(pqm_data_mutex);

/* IIOD descriptor */
static struct iiod_desc *iiod;

/* Payload buffer size for attribute data (per connection) */
#define IIO_PAYLOAD_BUF_SIZE 8192

#ifdef CONFIG_IIO_TRANSPORT_USB
/* USB: single connection, single buffer */
static uint32_t usb_conn_id;
static char usb_payload_buf[IIO_PAYLOAD_BUF_SIZE];
#endif

/* XML context string (generated at init) */
#define IIO_XML_BUF_SIZE 16384
static char iio_xml_buf[IIO_XML_BUF_SIZE];
static uint32_t iio_xml_len;

/* IIOD thread */
#define IIO_THREAD_STACK_SIZE 8192
#define IIO_THREAD_PRIORITY K_PRIO_PREEMPT(10)
static K_THREAD_STACK_DEFINE(iio_thread_stack, IIO_THREAD_STACK_SIZE);
static struct k_thread iio_thread_data;

/*
 * Channel type strings for XML generation (matches iio.c)
 */
static const char *const iio_chan_type_string[] = {
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

/* ================================================================
 * USB CDC/ACM Transport
 * ================================================================ */
#ifdef CONFIG_IIO_TRANSPORT_USB

static const struct device *cdc_dev;

USBD_DEVICE_DEFINE(iio_usbd,
		   DEVICE_DT_GET(DT_NODELABEL(zephyr_udc0)),
		   0x0B6B,  /* Maxim VID */
		   0x003C); /* PQM PID */

USBD_DESC_LANG_DEFINE(iio_lang);
USBD_DESC_MANUFACTURER_DEFINE(iio_mfr, "Analog Devices");
USBD_DESC_PRODUCT_DEFINE(iio_product, "MAX32650 CDC-ACM");

USBD_DESC_CONFIG_DEFINE(iio_fs_cfg_desc, "FS Configuration");
USBD_DESC_CONFIG_DEFINE(iio_hs_cfg_desc, "HS Configuration");

USBD_CONFIGURATION_DEFINE(iio_fs_config, 0, 250, &iio_fs_cfg_desc);
USBD_CONFIGURATION_DEFINE(iio_hs_config, 0, 250, &iio_hs_cfg_desc);

static K_SEM_DEFINE(dtr_sem, 0, 1);

static void usbd_msg_cb(struct usbd_context *const ctx,
			const struct usbd_msg *msg)
{
	if (usbd_can_detect_vbus(ctx)) {
		if (msg->type == USBD_MSG_VBUS_READY) {
			usbd_enable(ctx);
		}
		if (msg->type == USBD_MSG_VBUS_REMOVED) {
			usbd_disable(ctx);
		}
	}

	if (msg->type == USBD_MSG_CDC_ACM_CONTROL_LINE_STATE) {
		uint32_t dtr = 0U;

		uart_line_ctrl_get(msg->dev, UART_LINE_CTRL_DTR, &dtr);
		if (dtr) {
			k_sem_give(&dtr_sem);
		}
	}
}

static int usb_cdc_init(void)
{
	int err;

	err = usbd_add_descriptor(&iio_usbd, &iio_lang);
	if (err) return err;
	err = usbd_add_descriptor(&iio_usbd, &iio_mfr);
	if (err) return err;
	err = usbd_add_descriptor(&iio_usbd, &iio_product);
	if (err) return err;

	if (USBD_SUPPORTS_HIGH_SPEED &&
	    usbd_caps_speed(&iio_usbd) == USBD_SPEED_HS) {
		err = usbd_add_configuration(&iio_usbd, USBD_SPEED_HS,
					     &iio_hs_config);
		if (err) return err;
		err = usbd_register_all_classes(&iio_usbd, USBD_SPEED_HS, 1,
						NULL);
		if (err) return err;
		usbd_device_set_code_triple(&iio_usbd, USBD_SPEED_HS,
					    USB_BCC_MISCELLANEOUS, 0x02, 0x01);
	}

	err = usbd_add_configuration(&iio_usbd, USBD_SPEED_FS, &iio_fs_config);
	if (err) return err;
	err = usbd_register_all_classes(&iio_usbd, USBD_SPEED_FS, 1, NULL);
	if (err) return err;
	usbd_device_set_code_triple(&iio_usbd, USBD_SPEED_FS,
				    USB_BCC_MISCELLANEOUS, 0x02, 0x01);

	err = usbd_msg_register_cb(&iio_usbd, usbd_msg_cb);
	if (err) return err;

	err = usbd_init(&iio_usbd);
	if (err) return err;

	/* Always force-enable USB. The UDC driver hardcodes
	 * can_detect_vbus=true, but if VBUS is already present at init
	 * time the event may never fire. Force enable to be safe.
	 * -EALREADY is OK: means VBUS callback already enabled it. */
	err = usbd_enable(&iio_usbd);
	if (err && err != -EALREADY) return err;

	return 0;
}

static int usb_iio_send(struct iiod_ctx *ctx, uint8_t *buf, uint32_t len)
{
	uint32_t sent = 0;

	while (sent < len) {
		/* Send in chunks to avoid overflowing the CDC/ACM TX ring
		 * buffer.  With flow_ctrl=false (default), uart_poll_out()
		 * silently DISCARDS bytes when the ring buffer is full.
		 * Yielding between chunks lets the USB work queue drain the
		 * ring buffer before we add more data. */
		uint32_t chunk = (len - sent > 4096) ? 4096 : (len - sent);

		for (uint32_t i = 0; i < chunk; i++) {
			uart_poll_out(cdc_dev, buf[sent + i]);
		}
		sent += chunk;

		if (sent < len) {
			/* Give USB work queue time to drain the ring buffer.
			 * CDC/ACM schedules TX work with K_MSEC(1) delay;
			 * at HS bulk rate, ~6KB drains per millisecond. */
			k_msleep(1);
		}
	}
	return len;
}

static int usb_iio_recv(struct iiod_ctx *ctx, uint8_t *buf, uint32_t len)
{
	int ret;
	uint32_t i;

	for (i = 0; i < len; i++) {
		ret = uart_poll_in(cdc_dev, &buf[i]);
		if (ret == -1) {
			/* No data available */
			if (i == 0)
				return -EAGAIN;
			return i;
		}
	}
	return len;
}

#endif /* CONFIG_IIO_TRANSPORT_USB */

/* ================================================================
 * TCP Transport (T1L) — multi-client via zsock_poll()
 * ================================================================ */
#ifdef CONFIG_IIO_TRANSPORT_TCP

#define IIOD_TCP_PORT 30431
#define IIO_TCP_MAX_CLIENTS 4

/* Per-connection context: fd + iiod payload buffer.
 * Pointer is stored in iiod_conn_data.conn so send/recv can
 * identify which socket to use. */
struct tcp_client {
	int fd;
	uint32_t conn_id;
	bool active;
	char payload_buf[IIO_PAYLOAD_BUF_SIZE];
};

static int tcp_server_fd = -1;
static struct tcp_client tcp_clients[IIO_TCP_MAX_CLIENTS];

static int tcp_iio_send(struct iiod_ctx *ctx, uint8_t *buf, uint32_t len)
{
	struct tcp_client *client = (struct tcp_client *)ctx->conn;
	int ret = zsock_send(client->fd, buf, len, 0);
	if (ret < 0) {
		return -errno;
	}
	return ret;
}

static int tcp_iio_recv(struct iiod_ctx *ctx, uint8_t *buf, uint32_t len)
{
	struct tcp_client *client = (struct tcp_client *)ctx->conn;
	int ret = zsock_recv(client->fd, buf, len, ZSOCK_MSG_DONTWAIT);
	if (ret < 0) {
		return (errno == EAGAIN || errno == EWOULDBLOCK) ?
			-EAGAIN : -errno;
	}
	if (ret == 0) {
		return -ENOTCONN;  /* peer closed */
	}
	return ret;
}

static int tcp_server_setup(void)
{
	struct sockaddr_in addr;
	int opt = 1;
	int ret;

	if (tcp_server_fd >= 0) {
		zsock_close(tcp_server_fd);
		tcp_server_fd = -1;
	}

	tcp_server_fd = zsock_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (tcp_server_fd < 0) {
		LOG_ERR("TCP: socket() failed: %d", errno);
		return -errno;
	}

	ret = zsock_setsockopt(tcp_server_fd, SOL_SOCKET, SO_REUSEADDR,
			       &opt, sizeof(opt));
	if (ret < 0) {
		LOG_ERR("TCP: setsockopt(REUSEADDR) failed: %d", errno);
		zsock_close(tcp_server_fd);
		tcp_server_fd = -1;
		return -errno;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(IIOD_TCP_PORT);

	ret = zsock_bind(tcp_server_fd, (struct sockaddr *)&addr, sizeof(addr));
	if (ret < 0) {
		LOG_ERR("TCP: bind() failed: %d", errno);
		zsock_close(tcp_server_fd);
		tcp_server_fd = -1;
		return -errno;
	}

	ret = zsock_listen(tcp_server_fd, IIO_TCP_MAX_CLIENTS);
	if (ret < 0) {
		LOG_ERR("TCP: listen() failed: %d", errno);
		zsock_close(tcp_server_fd);
		tcp_server_fd = -1;
		return -errno;
	}

	LOG_INF("TCP: listening on port %d (max %d clients)",
		IIOD_TCP_PORT, IIO_TCP_MAX_CLIENTS);
	return 0;
}

static int tcp_server_init(void)
{
	struct net_if *iface = net_if_get_default();
	int wait = 0;

	/* Wait for T1L link carrier (physical link up) before binding.
	 * net_if_is_up() only means administratively up, not link up. */
	while (iface && !net_if_is_carrier_ok(iface) && wait < 30) {
		if (wait % 5 == 0) {
			LOG_INF("TCP: waiting for T1L carrier... (%ds)", wait);
		}
		k_sleep(K_SECONDS(1));
		wait++;
	}

	if (!iface || !net_if_is_carrier_ok(iface)) {
		LOG_WRN("TCP: no carrier after %ds, starting server anyway",
			wait);
	} else {
		LOG_INF("TCP: carrier detected after %ds", wait);
	}

	memset(tcp_clients, 0, sizeof(tcp_clients));
	return tcp_server_setup();
}

static void tcp_client_close(struct tcp_client *client)
{
	struct iiod_conn_data cdata;

	iiod_conn_remove(iiod, client->conn_id, &cdata);
	zsock_close(client->fd);
	client->fd = -1;
	client->active = false;
	LOG_INF("TCP: client closed (conn_id=%u)", client->conn_id);
}

static struct tcp_client *tcp_client_find_free(void)
{
	for (int i = 0; i < IIO_TCP_MAX_CLIENTS; i++) {
		if (!tcp_clients[i].active) {
			return &tcp_clients[i];
		}
	}
	return NULL;
}

static int tcp_client_count(void)
{
	int n = 0;
	for (int i = 0; i < IIO_TCP_MAX_CLIENTS; i++) {
		if (tcp_clients[i].active) {
			n++;
		}
	}
	return n;
}

#endif /* CONFIG_IIO_TRANSPORT_TCP */

/* ================================================================
 * Shared: XML generation + attribute routing
 * ================================================================ */

static void print_ch_id(char *buf, struct iio_channel *ch)
{
	if (ch->indexed) {
		sprintf(buf, "%s%d", iio_chan_type_string[ch->ch_type],
			(int)ch->channel);
	} else {
		sprintf(buf, "%s", iio_chan_type_string[ch->ch_type]);
	}
}

static int generate_xml(char *buf, uint32_t buf_size)
{
	struct iio_device *dev = &pqm_iio_descriptor;
	struct iio_channel *ch;
	struct iio_attribute *attr;
	char ch_id[64];
	int i = 0;
	int n = buf_size;
	uint16_t j, k;

	/* XML header + DTD + context open */
	i += snprintf(buf + i, n - i,
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
		"<context name=\"xml\" description=\"pqmon-zephyr-iio\" >");

	/* Single device: pqm */
	i += snprintf(buf + i, n - i, "<device id=\"iio:device0\" name=\"pqm\">");

	/* Channels */
	for (j = 0; j < dev->num_ch; j++) {
		ch = &dev->channels[j];
		print_ch_id(ch_id, ch);

		i += snprintf(buf + i, n - i, "<channel id=\"%s\"", ch_id);
		if (ch->name)
			i += snprintf(buf + i, n - i, " name=\"%s\"", ch->name);
		i += snprintf(buf + i, n - i, " type=\"%s\" >",
			      ch->ch_out ? "output" : "input");

		/* Scan element */
		if (ch->scan_type)
			i += snprintf(buf + i, n - i,
				"<scan-element index=\"%d\""
				" format=\"%s:%c%d/%d>>%d\" />",
				ch->scan_index,
				ch->scan_type->is_big_endian ? "be" : "le",
				ch->scan_type->sign,
				ch->scan_type->realbits,
				ch->scan_type->storagebits,
				ch->scan_type->shift);

		/* Channel attributes */
		if (ch->attributes) {
			for (k = 0; ch->attributes[k].name; k++) {
				attr = &ch->attributes[k];
				i += snprintf(buf + i, n - i,
					"<attribute name=\"%s\" ", attr->name);
				/* Generate filename for IIO_SEPARATE (default) */
				if (ch->indexed) {
					i += snprintf(buf + i, n - i,
						"filename=\"%s_%s%d_%s\"",
						ch->ch_out ? "out" : "in",
						iio_chan_type_string[ch->ch_type],
						(int)ch->channel,
						attr->name);
				} else {
					i += snprintf(buf + i, n - i,
						"filename=\"%s_%s_%s\"",
						ch->ch_out ? "out" : "in",
						iio_chan_type_string[ch->ch_type],
						attr->name);
				}
				i += snprintf(buf + i, n - i, " />");
			}
		}
		i += snprintf(buf + i, n - i, "</channel>");
	}

	/* Device (global) attributes */
	if (dev->attributes) {
		for (j = 0; dev->attributes[j].name; j++) {
			i += snprintf(buf + i, n - i,
				"<attribute name=\"%s\" />",
				dev->attributes[j].name);
		}
	}

	i += snprintf(buf + i, n - i, "</device>");
	i += snprintf(buf + i, n - i, "</context>");

	return i;
}

static struct iio_channel *find_channel(const char *ch_name, bool ch_out)
{
	struct iio_device *dev = &pqm_iio_descriptor;
	char ch_id[64];

	for (uint16_t i = 0; i < dev->num_ch; i++) {
		print_ch_id(ch_id, &dev->channels[i]);
		if (strcmp(ch_id, ch_name) == 0 &&
		    dev->channels[i].ch_out == ch_out) {
			return &dev->channels[i];
		}
	}
	return NULL;
}

static int read_one_attr(struct iio_attribute *attr, void *dev_instance,
			 struct iio_ch_info *ch_info, char *buf, uint32_t len)
{
	if (!attr->show)
		return -ENOENT;
	return attr->show(dev_instance, buf, len, ch_info, attr->priv);
}

static int write_one_attr(struct iio_attribute *attr, void *dev_instance,
			  struct iio_ch_info *ch_info, char *buf, uint32_t len)
{
	if (!attr->store)
		return -ENOENT;
	return attr->store(dev_instance, buf, len, ch_info, attr->priv);
}

static int read_all_attrs(struct iio_attribute *attrs, void *dev_instance,
			  struct iio_ch_info *ch_info, char *buf, uint32_t len)
{
	int total = 0;
	char tmp[1024];
	int ret;

	if (!attrs)
		return 0;

	for (int i = 0; attrs[i].name; i++) {
		ret = read_one_attr(&attrs[i], dev_instance, ch_info, tmp, sizeof(tmp));
		if (ret < 0) {
			ret = 0;
			tmp[0] = '\0';
		}
		if (total >= (int)len)
			break;
		int n = snprintf(buf + total, len - total, "%d\n%s", ret, tmp);
		if (n < 0)
			break;
		/* snprintf returns what WOULD be written; clamp to actual */
		if (n > (int)(len - total))
			n = (int)(len - total);
		total += n;
	}
	return total;
}

static int iio_read_attr(struct iiod_ctx *ctx, const char *device,
			 struct iiod_attr *attr, char *buf, uint32_t len)
{
	struct iio_device *dev = &pqm_iio_descriptor;
	void *dev_instance = pqm_desc_inst;
	struct iio_ch_info ch_info;
	struct iio_channel *channel;
	struct iio_attribute *attrs;
	int ret;

	LOG_DBG("READ type=%d ch=%s attr=%s", attr->type,
		attr->channel ? attr->channel : "",
		attr->name ? attr->name : "(all)");

	k_mutex_lock(&pqm_data_mutex, K_FOREVER);

	/* Device attribute */
	if (attr->type == IIO_ATTR_TYPE_DEVICE) {
		attrs = dev->attributes;
		if (!attrs) {
			ret = -ENOENT;
			goto out;
		}

		if (attr->name[0] == '\0') {
			ret = read_all_attrs(attrs, dev_instance, NULL,
					     buf, len);
			goto out;
		}

		for (int i = 0; attrs[i].name; i++) {
			if (strcmp(attrs[i].name, attr->name) == 0) {
				ret = read_one_attr(&attrs[i], dev_instance,
						    NULL, buf, len);
				goto out;
			}
		}
		ret = -ENOENT;
		goto out;
	}

	/* Channel attribute */
	if (attr->type == IIO_ATTR_TYPE_CH_IN ||
	    attr->type == IIO_ATTR_TYPE_CH_OUT) {
		bool ch_out = (attr->type == IIO_ATTR_TYPE_CH_OUT);

		channel = find_channel(attr->channel, ch_out);
		if (!channel) {
			ret = -ENOENT;
			goto out;
		}

		ch_info.ch_num = channel->channel;
		ch_info.ch_out = channel->ch_out;
		ch_info.type = channel->ch_type;

		attrs = channel->attributes;
		if (!attrs) {
			ret = -ENOENT;
			goto out;
		}

		if (attr->name[0] == '\0') {
			ret = read_all_attrs(attrs, dev_instance, &ch_info,
					     buf, len);
			goto out;
		}

		for (int i = 0; attrs[i].name; i++) {
			if (strcmp(attrs[i].name, attr->name) == 0) {
				ret = read_one_attr(&attrs[i], dev_instance,
						    &ch_info, buf, len);
				goto out;
			}
		}
		ret = -ENOENT;
		goto out;
	}

	ret = -EINVAL;
out:
	k_mutex_unlock(&pqm_data_mutex);
	return ret;
}

static int iio_write_attr(struct iiod_ctx *ctx, const char *device,
			  struct iiod_attr *attr, char *buf, uint32_t len)
{
	struct iio_device *dev = &pqm_iio_descriptor;
	void *dev_instance = pqm_desc_inst;
	struct iio_ch_info ch_info;
	struct iio_channel *channel;
	struct iio_attribute *attrs;
	int ret;

	k_mutex_lock(&pqm_data_mutex, K_FOREVER);

	if (attr->type == IIO_ATTR_TYPE_DEVICE) {
		attrs = dev->attributes;
		if (!attrs) {
			ret = -ENOENT;
			goto out;
		}

		for (int i = 0; attrs[i].name; i++) {
			if (strcmp(attrs[i].name, attr->name) == 0) {
				ret = write_one_attr(&attrs[i], dev_instance,
						     NULL, buf, len);
				goto out;
			}
		}
		ret = -ENOENT;
		goto out;
	}

	if (attr->type == IIO_ATTR_TYPE_CH_IN ||
	    attr->type == IIO_ATTR_TYPE_CH_OUT) {
		bool ch_out = (attr->type == IIO_ATTR_TYPE_CH_OUT);

		channel = find_channel(attr->channel, ch_out);
		if (!channel) {
			ret = -ENOENT;
			goto out;
		}

		ch_info.ch_num = channel->channel;
		ch_info.ch_out = channel->ch_out;
		ch_info.type = channel->ch_type;

		attrs = channel->attributes;
		if (!attrs) {
			ret = -ENOENT;
			goto out;
		}

		for (int i = 0; attrs[i].name; i++) {
			if (strcmp(attrs[i].name, attr->name) == 0) {
				ret = write_one_attr(&attrs[i], dev_instance,
						     &ch_info, buf, len);
				goto out;
			}
		}
		ret = -ENOENT;
		goto out;
	}

	ret = -EINVAL;
out:
	k_mutex_unlock(&pqm_data_mutex);
	return ret;
}

static int iio_get_trigger(struct iiod_ctx *ctx, const char *device,
			   char *trigger, uint32_t len)
{
	/* No trigger assigned — return empty string (0 bytes) */
	trigger[0] = '\0';
	return 0;
}

/* ================================================================
 * IIOD ops + thread (transport-specific)
 * ================================================================ */

#ifdef CONFIG_IIO_TRANSPORT_USB

static struct iiod_ops iio_ops = {
	.send = usb_iio_send,
	.recv = usb_iio_recv,
	.read_attr = iio_read_attr,
	.write_attr = iio_write_attr,
	.get_trigger = iio_get_trigger,
};

static void iio_thread_fn(void *p1, void *p2, void *p3)
{
	int32_t ret;

	LOG_DBG("USB: thread started, waiting for DTR...");

	/* Wait for host to open the CDC/ACM port (DTR signal).
	 * Timeout after 5s and proceed anyway — some serial tools
	 * don't trigger DTR on Linux CDC/ACM. */
	if (k_sem_take(&dtr_sem, K_SECONDS(5)) == 0) {
		LOG_INF("USB: DTR set by host");
	} else {
		LOG_DBG("USB: DTR timeout, proceeding anyway");
	}

	/* Small delay for host settle */
	k_msleep(200);

	/* Kick-start USB bulk OUT receive pipeline.
	 * The CDC/ACM driver only queues the initial USB receive buffer
	 * when uart_irq_rx_enable() is called. Without this, poll_in()
	 * always returns -1 because no data can arrive from the host.
	 * We still use poll_in() for reading — this just primes the pump. */
	uart_irq_rx_enable(cdc_dev);

	LOG_DBG("USB: server active, polling for commands");

	while (1) {
		ret = iiod_conn_step(iiod, usb_conn_id);
		if (ret == 0) {
			/* Command completed OK */
		} else if (ret == -EAGAIN) {
			/* No data available, yield */
			k_msleep(1);
		} else if (ret == -ENOTCONN) {
			/* EXIT command received — reset connection */
			LOG_INF("USB: client disconnected");

			/* Remove and re-add connection for clean state */
			struct iiod_conn_data cdata;
			iiod_conn_remove(iiod, usb_conn_id, &cdata);

			cdata.conn = NULL;
			cdata.buf = usb_payload_buf;
			cdata.len = IIO_PAYLOAD_BUF_SIZE;
			iiod_conn_add(iiod, &cdata, &usb_conn_id);

			k_msleep(200);
			LOG_INF("USB: ready for next connection");
		} else if (ret < 0) {
			/* Only log occasionally to avoid flooding */
			static int err_count;
			if (++err_count % 1000 == 1) {
				LOG_WRN("USB: iiod error %d (count=%d)",
					ret, err_count);
			}
			k_msleep(1);
		}
	}
}

#endif /* CONFIG_IIO_TRANSPORT_USB */

#ifdef CONFIG_IIO_TRANSPORT_TCP

static struct iiod_ops iio_ops = {
	.send = tcp_iio_send,
	.recv = tcp_iio_recv,
	.read_attr = iio_read_attr,
	.write_attr = iio_write_attr,
	.get_trigger = iio_get_trigger,
};

static void iio_thread_fn(void *p1, void *p2, void *p3)
{
	/* pollfd[0] = server socket, pollfd[1..N] = client sockets */
	struct zsock_pollfd fds[1 + IIO_TCP_MAX_CLIENTS];
	int accept_fails = 0;

	LOG_INF("TCP: thread started, waiting for connections...");

	while (1) {
		/* Build poll set */
		int nfds = 0;

		/* Server socket: watch for incoming connections */
		fds[nfds].fd = tcp_server_fd;
		fds[nfds].events = ZSOCK_POLLIN;
		fds[nfds].revents = 0;
		nfds++;

		/* Active client sockets */
		for (int i = 0; i < IIO_TCP_MAX_CLIENTS; i++) {
			if (tcp_clients[i].active) {
				fds[nfds].fd = tcp_clients[i].fd;
				fds[nfds].events = ZSOCK_POLLIN;
				fds[nfds].revents = 0;
				nfds++;
			}
		}

		int ready = zsock_poll(fds, nfds, 10);
		if (ready < 0) {
			LOG_ERR("TCP: poll() error: %d", errno);
			k_msleep(100);
			continue;
		}

		/* Check server socket for new connections */
		if (fds[0].revents & ZSOCK_POLLIN) {
			struct sockaddr_in client_addr;
			socklen_t addr_len = sizeof(client_addr);

			int new_fd = zsock_accept(tcp_server_fd,
						  (struct sockaddr *)&client_addr,
						  &addr_len);
			if (new_fd < 0) {
				int err = errno;
				accept_fails++;
				if (accept_fails <= 3 ||
				    accept_fails % 10 == 0) {
					LOG_WRN("TCP: accept() failed: %d "
						"(count=%d)",
						err, accept_fails);
				}
				if (accept_fails >= 5) {
					LOG_WRN("TCP: re-creating server "
						"socket");
					tcp_server_setup();
					accept_fails = 0;
				}
			} else {
				accept_fails = 0;

				struct tcp_client *client =
					tcp_client_find_free();
				if (!client) {
					LOG_WRN("TCP: max clients reached, "
						"rejecting");
					zsock_close(new_fd);
				} else {
					/* Disable Nagle */
					int opt = 1;
					zsock_setsockopt(new_fd, IPPROTO_TCP,
							 ZSOCK_TCP_NODELAY,
							 &opt, sizeof(opt));

					client->fd = new_fd;
					client->active = true;

					/* Register with iiod — each client
					 * gets its own payload buffer and
					 * conn pointer */
					struct iiod_conn_data cdata = {
						.conn = client,
						.buf = client->payload_buf,
						.len = IIO_PAYLOAD_BUF_SIZE,
					};

					int ret = iiod_conn_add(iiod, &cdata,
								&client->conn_id);
					if (ret) {
						LOG_ERR("TCP: conn_add "
							"failed: %d", ret);
						zsock_close(new_fd);
						client->active = false;
					} else {
						LOG_INF("TCP: client "
							"connected (fd=%d, "
							"conn=%u, total=%d)",
							new_fd,
							client->conn_id,
							tcp_client_count());
					}
				}
			}
		}

		/* Check for error/hangup on client sockets */
		int fdi = 1;
		for (int i = 0; i < IIO_TCP_MAX_CLIENTS; i++) {
			if (!tcp_clients[i].active)
				continue;
			if (fdi < nfds && fds[fdi].fd == tcp_clients[i].fd) {
				if (fds[fdi].revents &
				    (ZSOCK_POLLERR | ZSOCK_POLLHUP)) {
					LOG_WRN("TCP: client error/hangup "
						"(fd=%d)",
						tcp_clients[i].fd);
					tcp_client_close(&tcp_clients[i]);
				}
				fdi++;
			}
		}

		/* Drive iiod state machine for ALL active clients.
		 * Each client is stepped in a loop until EAGAIN (no
		 * data to recv) or error. This is necessary because
		 * iiod reads commands byte-by-byte, then transitions
		 * through parse → execute → send-response states that
		 * don't need recv. A single conn_step call is not
		 * enough to complete a full command cycle. */
		for (int i = 0; i < IIO_TCP_MAX_CLIENTS; i++) {
			if (!tcp_clients[i].active)
				continue;

			struct tcp_client *client = &tcp_clients[i];
			int ret;

			do {
				ret = iiod_conn_step(iiod, client->conn_id);
			} while (ret == 0);

			if (ret == -ENOTCONN) {
				LOG_INF("TCP: client disconnected "
					"(fd=%d)", client->fd);
				tcp_client_close(client);
			} else if (ret != -EAGAIN) {
				LOG_WRN("TCP: iiod error %d (fd=%d)",
					ret, client->fd);
				tcp_client_close(client);
			}
		}
	}
}

#endif /* CONFIG_IIO_TRANSPORT_TCP */

/* ================================================================
 * Public API
 * ================================================================ */
int iio_server_init(void)
{
	int ret;

#ifdef CONFIG_IIO_TRANSPORT_USB
	/* Get CDC/ACM UART device */
	cdc_dev = DEVICE_DT_GET_ONE(zephyr_cdc_acm_uart);
	if (!device_is_ready(cdc_dev)) {
		printf("IIO: CDC ACM device not ready\n");
		return -ENODEV;
	}

	/* Initialize USB device stack */
	ret = usb_cdc_init();
	if (ret) {
		printf("IIO: USB CDC init failed: %d\n", ret);
		return ret;
	}
	printf("IIO: USB CDC/ACM initialized\n");
#endif

#ifdef CONFIG_IIO_TRANSPORT_TCP
	/* Initialize TCP server socket */
	ret = tcp_server_init();
	if (ret) {
		printf("IIO: TCP server init failed: %d\n", ret);
		return ret;
	}
#endif

	/* Generate IIO XML context */
	iio_xml_len = generate_xml(iio_xml_buf, IIO_XML_BUF_SIZE);
	LOG_DBG("IIO XML generated: %u bytes", iio_xml_len);

	/* Initialize IIOD */
	struct iiod_init_param param = {
		.ops = &iio_ops,
		.instance = NULL,
		.xml = iio_xml_buf,
		.xml_len = iio_xml_len,
	};

	ret = iiod_init(&iiod, &param);
	if (ret) {
		printf("IIO: IIOD init failed: %d (calloc?)\n", ret);
		return ret;
	}

#ifdef CONFIG_IIO_TRANSPORT_USB
	/* USB: pre-add a connection (single persistent connection) */
	struct iiod_conn_data cdata = {
		.conn = NULL,
		.buf = usb_payload_buf,
		.len = IIO_PAYLOAD_BUF_SIZE,
	};

	ret = iiod_conn_add(iiod, &cdata, &usb_conn_id);
	if (ret) {
		printf("IIO: IIOD conn_add failed: %d\n", ret);
		return ret;
	}
#endif
	/* TCP: connections are added in the thread on accept() */

	/* Start IIOD thread */
	k_thread_create(&iio_thread_data, iio_thread_stack,
			IIO_THREAD_STACK_SIZE, iio_thread_fn,
			NULL, NULL, NULL,
			IIO_THREAD_PRIORITY, 0, K_NO_WAIT);
	k_thread_name_set(&iio_thread_data, "iio_server");

#ifdef CONFIG_IIO_TRANSPORT_USB
	printf("IIO: USB server started (conn_id=%u)\n", usb_conn_id);
#else
	printf("IIO: TCP server started (port %d)\n", IIOD_TCP_PORT);
#endif
	return 0;
}
