/***************************************************************************//**
 *   @file   iio_k_tx.c
 *   @brief  IIO OUTPUT device for large bitstream transmit (K project).
 *   @author Analog Devices Inc.
 ********************************************************************************
 * Copyright (C) 2026 Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "iio_k_tx.h"
#include "iio.h"
#include "iio_types.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_util.h"

enum iio_k_tx_attr_id {
	IIO_K_TX_ATTR_STREAM_ID,
	IIO_K_TX_ATTR_TX_MODE,
	IIO_K_TX_ATTR_PAYLOAD_MODE,
	IIO_K_TX_ATTR_TIMEOUT_MS,
};

/******************************************************************************/
/* Attribute show/store                                                       */
/******************************************************************************/
static int iio_k_tx_attr_show(void *device, char *buf, uint32_t len,
			      const struct iio_ch_info *channel, intptr_t id)
{
	struct iio_k_tx_desc *desc = device;

	if (!desc)
		return -ENODEV;

	switch (id) {
	case IIO_K_TX_ATTR_STREAM_ID:
		return snprintf(buf, len, "%u", (unsigned)desc->stream_id);
	case IIO_K_TX_ATTR_TX_MODE:
		return snprintf(buf, len, "%s",
				desc->tx_mode == K_TX_CYCLIC ? "cyclic" : "oneshot");
	case IIO_K_TX_ATTR_PAYLOAD_MODE:
		return snprintf(buf, len, "%s",
				desc->payload_mode == K_PAYLOAD_RAW ? "raw" : "samples");
	case IIO_K_TX_ATTR_TIMEOUT_MS:
		return snprintf(buf, len, "%u", (unsigned)desc->timeout_ms);
	default:
		return -EINVAL;
	}
}

static int iio_k_tx_attr_store(void *device, char *buf, uint32_t len,
			       const struct iio_ch_info *channel, intptr_t id)
{
	struct iio_k_tx_desc *desc = device;

	if (!desc)
		return -ENODEV;

	switch (id) {
	case IIO_K_TX_ATTR_STREAM_ID:
		desc->stream_id = (enum k_stream_id)strtoul(buf, NULL, 0);
		return len;
	case IIO_K_TX_ATTR_TX_MODE:
		desc->tx_mode = strncmp(buf, "cyclic", 6) == 0 ?
				K_TX_CYCLIC : K_TX_ONESHOT;
		return len;
	case IIO_K_TX_ATTR_PAYLOAD_MODE:
		desc->payload_mode = strncmp(buf, "raw", 3) == 0 ?
				     K_PAYLOAD_RAW : K_PAYLOAD_SAMPLES;
		return len;
	case IIO_K_TX_ATTR_TIMEOUT_MS:
		desc->timeout_ms = strtoul(buf, NULL, 0);
		return len;
	default:
		return -EINVAL;
	}
}

/******************************************************************************/
/* Buffer callbacks                                                           */
/******************************************************************************/
static int32_t iio_k_tx_pre_enable(void *device, uint32_t mask)
{
	struct iio_k_tx_desc *desc = device;

	if (!desc)
		return -ENODEV;

	desc->active_mask = mask;

	/* TODO: reset + arm data-offload for the selected stream/mode. */

	return 0;
}

static int32_t iio_k_tx_submit(struct iio_device_data *dev_data)
{
	struct iio_k_tx_desc *desc = dev_data->dev;

	if (!desc)
		return -ENODEV;

	/* TODO(phase 3):
	 *  - iio_buffer_pop_scan()/get_block() one chunk from the client buffer
	 *  - if payload_mode == samples: interleave active channels
	 *    else: pass octets through unchanged
	 *  - k_data_tx_block(desc->kd, chunk, len, stream_id, tx_mode, timeout)
	 *  - ping-pong across staging sub-windows for large buffers
	 */
	(void)dev_data;

	return -ENOSYS;
}

static int32_t iio_k_tx_post_disable(void *device)
{
	struct iio_k_tx_desc *desc = device;

	if (!desc)
		return -ENODEV;

	/* TODO: quiesce data-offload, stop tx_dmac. */

	return 0;
}

/******************************************************************************/
/* Descriptor tables                                                          */
/******************************************************************************/
static struct iio_attribute iio_k_tx_attrs[] = {
	{
		.name = "stream_id",
		.priv = IIO_K_TX_ATTR_STREAM_ID,
		.show = iio_k_tx_attr_show,
		.store = iio_k_tx_attr_store,
	},
	{
		.name = "tx_mode",
		.priv = IIO_K_TX_ATTR_TX_MODE,
		.show = iio_k_tx_attr_show,
		.store = iio_k_tx_attr_store,
	},
	{
		.name = "payload_mode",
		.priv = IIO_K_TX_ATTR_PAYLOAD_MODE,
		.show = iio_k_tx_attr_show,
		.store = iio_k_tx_attr_store,
	},
	{
		.name = "timeout_ms",
		.priv = IIO_K_TX_ATTR_TIMEOUT_MS,
		.show = iio_k_tx_attr_show,
		.store = iio_k_tx_attr_store,
	},
	END_ATTRIBUTES_ARRAY,
};

/* Sample-mode scan type: 8 channels of le:s16/16. */
static struct scan_type iio_k_tx_scan_type = {
	.sign = 's',
	.realbits = 16,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = false,
};

#define K_TX_CHANNEL(idx) {			\
	.ch_type = IIO_VOLTAGE,			\
	.channel = (idx),			\
	.scan_index = (idx),			\
	.scan_type = &iio_k_tx_scan_type,	\
	.ch_out = true,				\
	.indexed = true,			\
}

static struct iio_channel iio_k_tx_channels[] = {
	K_TX_CHANNEL(0), K_TX_CHANNEL(1), K_TX_CHANNEL(2), K_TX_CHANNEL(3),
	K_TX_CHANNEL(4), K_TX_CHANNEL(5), K_TX_CHANNEL(6), K_TX_CHANNEL(7),
};

static struct iio_device iio_k_tx_dev = {
	.num_ch = NO_OS_ARRAY_SIZE(iio_k_tx_channels),
	.channels = iio_k_tx_channels,
	.attributes = iio_k_tx_attrs,
	.pre_enable = iio_k_tx_pre_enable,
	.submit = iio_k_tx_submit,
	.post_disable = iio_k_tx_post_disable,
};

/******************************************************************************/
/* Lifecycle                                                                  */
/******************************************************************************/
int iio_k_tx_init(struct iio_k_tx_desc **desc,
		  const struct iio_k_tx_init_param *init)
{
	struct iio_k_tx_desc *d;

	if (!desc || !init || !init->kd)
		return -EINVAL;

	d = no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->kd		= init->kd;
	d->stream_id	= init->stream_id ? init->stream_id : K_STREAM_PASS;
	d->tx_mode	= init->tx_mode;
	d->payload_mode	= init->payload_mode;
	d->timeout_ms	= init->timeout_ms ? init->timeout_ms : 2000;

	*desc = d;

	return 0;
}

void iio_k_tx_remove(struct iio_k_tx_desc *desc)
{
	if (desc)
		no_os_free(desc);
}

void iio_k_tx_get_dev_descriptor(struct iio_k_tx_desc *desc,
				 struct iio_device **dev_descriptor)
{
	(void)desc;
	*dev_descriptor = &iio_k_tx_dev;
}
