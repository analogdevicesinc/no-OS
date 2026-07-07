/***************************************************************************//**
 *   @file   iio_k_time.c
 *   @brief  IIO device exposing time and data-plane statistics.
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
#include <inttypes.h>
#include "iio_k_time.h"
#include "iio.h"
#include "iio_types.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_delay.h"

enum iio_k_time_attr_id {
	IIO_K_TIME_ATTR_UPTIME_US,
	IIO_K_TIME_ATTR_LAST_TX_US,
	IIO_K_TIME_ATTR_LAST_RX_US,
	IIO_K_TIME_ATTR_TX_BYTES,
	IIO_K_TIME_ATTR_RX_BYTES,
	IIO_K_TIME_ATTR_LINK_STATUS,
};

static int iio_k_time_attr_show(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t id)
{
	struct iio_k_time_desc *desc = device;
	struct k_data_stats *st;

	if (!desc)
		return -ENODEV;

	st = &desc->kd->stats;

	switch (id) {
	case IIO_K_TIME_ATTR_UPTIME_US: {
		struct no_os_time t = no_os_get_time();
		uint64_t us = (uint64_t)t.s * 1000000ULL + t.us;
		return snprintf(buf, len, "%"PRIu64, us);
	}
	case IIO_K_TIME_ATTR_LAST_TX_US:
		return snprintf(buf, len, "%"PRIu64, st->last_tx_us);
	case IIO_K_TIME_ATTR_LAST_RX_US:
		return snprintf(buf, len, "%"PRIu64, st->last_rx_us);
	case IIO_K_TIME_ATTR_TX_BYTES:
		return snprintf(buf, len, "%"PRIu64, st->tx_bytes);
	case IIO_K_TIME_ATTR_RX_BYTES:
		return snprintf(buf, len, "%"PRIu64, st->rx_bytes);
	case IIO_K_TIME_ATTR_LINK_STATUS:
		/* TODO: cache axi_jesd204_{tx,rx}_status_read() results. */
		return snprintf(buf, len, "unknown");
	default:
		return -EINVAL;
	}
}

static struct iio_attribute iio_k_time_attrs[] = {
	{
		.name = "uptime_us",
		.priv = IIO_K_TIME_ATTR_UPTIME_US,
		.show = iio_k_time_attr_show,
		.store = NULL,
	},
	{
		.name = "last_tx_us",
		.priv = IIO_K_TIME_ATTR_LAST_TX_US,
		.show = iio_k_time_attr_show,
		.store = NULL,
	},
	{
		.name = "last_rx_us",
		.priv = IIO_K_TIME_ATTR_LAST_RX_US,
		.show = iio_k_time_attr_show,
		.store = NULL,
	},
	{
		.name = "tx_bytes",
		.priv = IIO_K_TIME_ATTR_TX_BYTES,
		.show = iio_k_time_attr_show,
		.store = NULL,
	},
	{
		.name = "rx_bytes",
		.priv = IIO_K_TIME_ATTR_RX_BYTES,
		.show = iio_k_time_attr_show,
		.store = NULL,
	},
	{
		.name = "link_status",
		.priv = IIO_K_TIME_ATTR_LINK_STATUS,
		.show = iio_k_time_attr_show,
		.store = NULL,
	},
	END_ATTRIBUTES_ARRAY,
};

static struct iio_device iio_k_time_dev = {
	.num_ch = 0,
	.channels = NULL,
	.attributes = iio_k_time_attrs,
};

int iio_k_time_init(struct iio_k_time_desc **desc,
		    const struct iio_k_time_init_param *init)
{
	struct iio_k_time_desc *d;

	if (!desc || !init || !init->kd)
		return -EINVAL;

	d = no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->kd = init->kd;
	*desc = d;

	return 0;
}

void iio_k_time_remove(struct iio_k_time_desc *desc)
{
	if (desc)
		no_os_free(desc);
}

void iio_k_time_get_dev_descriptor(struct iio_k_time_desc *desc,
				   struct iio_device **dev_descriptor)
{
	(void)desc;
	*dev_descriptor = &iio_k_time_dev;
}
