/***************************************************************************//**
 *   @file   iio_k_hdlregs.c
 *   @brief  IIO device for windowed local AXI/PL IP register access.
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
#include "iio_k_hdlregs.h"
#include "iio.h"
#include "iio_types.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_axi_io.h"

enum iio_k_hdlregs_attr_id {
	IIO_K_HDLREGS_ATTR_WINDOW,
	IIO_K_HDLREGS_ATTR_WINDOW_BASE,
};

static const char *const k_hdl_window_name[K_HDL_WIN_COUNT] = {
	[K_HDL_WIN_JESD_TX]	= "jesd_tx",
	[K_HDL_WIN_JESD_RX]	= "jesd_rx",
	[K_HDL_WIN_ADXCVR_TX]	= "adxcvr_tx",
	[K_HDL_WIN_ADXCVR_RX]	= "adxcvr_rx",
	[K_HDL_WIN_TPL_TX]	= "tpl_tx",
	[K_HDL_WIN_TPL_RX]	= "tpl_rx",
	[K_HDL_WIN_OFFLOAD_TX]	= "offload_tx",
	[K_HDL_WIN_OFFLOAD_RX]	= "offload_rx",
};

/******************************************************************************/
/* Register access: direct AXI, offset relative to the selected window base.  */
/******************************************************************************/
static int32_t iio_k_hdlregs_reg_read(void *device, uint32_t reg,
				      uint32_t *readval)
{
	struct iio_k_hdlregs_desc *desc = device;

	if (!desc)
		return -ENODEV;

	no_os_axi_io_read(desc->base[desc->window], reg, readval);

	return 0;
}

static int32_t iio_k_hdlregs_reg_write(void *device, uint32_t reg,
				       uint32_t writeval)
{
	struct iio_k_hdlregs_desc *desc = device;

	if (!desc)
		return -ENODEV;

	no_os_axi_io_write(desc->base[desc->window], reg, writeval);

	return 0;
}

/******************************************************************************/
/* Attributes                                                                 */
/******************************************************************************/
static int iio_k_hdlregs_attr_show(void *device, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t id)
{
	struct iio_k_hdlregs_desc *desc = device;

	if (!desc)
		return -ENODEV;

	switch (id) {
	case IIO_K_HDLREGS_ATTR_WINDOW:
		return snprintf(buf, len, "%s", k_hdl_window_name[desc->window]);
	case IIO_K_HDLREGS_ATTR_WINDOW_BASE:
		return snprintf(buf, len, "0x%08lX",
				(unsigned long)desc->base[desc->window]);
	default:
		return -EINVAL;
	}
}

static int iio_k_hdlregs_attr_store(void *device, char *buf, uint32_t len,
				    const struct iio_ch_info *channel,
				    intptr_t id)
{
	struct iio_k_hdlregs_desc *desc = device;
	unsigned int i;

	if (!desc)
		return -ENODEV;

	if (id != IIO_K_HDLREGS_ATTR_WINDOW)
		return -EINVAL;

	for (i = 0; i < K_HDL_WIN_COUNT; i++) {
		if (strncmp(buf, k_hdl_window_name[i],
			    strlen(k_hdl_window_name[i])) == 0) {
			desc->window = (enum k_hdl_window)i;
			return len;
		}
	}

	return -EINVAL;
}

static struct iio_attribute iio_k_hdlregs_attrs[] = {
	{
		.name = "window",
		.priv = IIO_K_HDLREGS_ATTR_WINDOW,
		.show = iio_k_hdlregs_attr_show,
		.store = iio_k_hdlregs_attr_store,
	},
	{
		.name = "window_base",
		.priv = IIO_K_HDLREGS_ATTR_WINDOW_BASE,
		.show = iio_k_hdlregs_attr_show,
		.store = NULL,
	},
	END_ATTRIBUTES_ARRAY,
};

static struct iio_device iio_k_hdlregs_dev = {
	.num_ch = 0,
	.channels = NULL,
	.attributes = iio_k_hdlregs_attrs,
	.debug_reg_read = iio_k_hdlregs_reg_read,
	.debug_reg_write = iio_k_hdlregs_reg_write,
};

/******************************************************************************/
/* Lifecycle                                                                  */
/******************************************************************************/
int iio_k_hdlregs_init(struct iio_k_hdlregs_desc **desc,
		       const struct iio_k_hdlregs_init_param *init)
{
	struct iio_k_hdlregs_desc *d;

	if (!desc || !init)
		return -EINVAL;

	d = no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	memcpy(d->base, init->base, sizeof(d->base));
	d->window = K_HDL_WIN_JESD_TX;

	*desc = d;

	return 0;
}

void iio_k_hdlregs_remove(struct iio_k_hdlregs_desc *desc)
{
	if (desc)
		no_os_free(desc);
}

void iio_k_hdlregs_get_dev_descriptor(struct iio_k_hdlregs_desc *desc,
				      struct iio_device **dev_descriptor)
{
	(void)desc;
	*dev_descriptor = &iio_k_hdlregs_dev;
}
