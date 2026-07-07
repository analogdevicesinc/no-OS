/***************************************************************************//**
 *   @file   iio_ad9081k.c
 *   @brief  IIO driver for AD9081K — exposes a "load_file" attribute
 *           that prints received text data to UART.
 *   @author Analog Devices Inc.
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
#include <inttypes.h>
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_print_log.h"
#include "iio_ad9081k.h"

enum iio_ad9081k_attr_id {
	IIO_AD9081K_ATTR_LOAD_FILE,
	IIO_AD9081K_ATTR_LOADED_BYTES,
};

static int iio_ad9081k_attr_get(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t attr_id)
{
	struct iio_ad9081k_desc *desc = device;

	if (!desc)
		return -ENODEV;

	switch (attr_id) {
	case IIO_AD9081K_ATTR_LOADED_BYTES:
		return snprintf(buf, len, "%"PRIu32, desc->loaded_bytes);
	case IIO_AD9081K_ATTR_LOAD_FILE:
		return snprintf(buf, len, "%"PRIu32, desc->loaded_bytes);
	default:
		return -EINVAL;
	}
}

static int iio_ad9081k_attr_set(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t attr_id)
{
	struct iio_ad9081k_desc *desc = device;

	if (!desc)
		return -ENODEV;

	if (attr_id != IIO_AD9081K_ATTR_LOAD_FILE)
		return -EINVAL;

	if (len == 0)
		return -EINVAL;

	/* Print the received file contents to UART */
	printf("--- load_file (%"PRIu32" bytes) ---\n", (uint32_t)len);
	fwrite(buf, 1, len, stdout);
	printf("\n--- end ---\n");

	desc->loaded_bytes = len;

	return (int)len;
}

static struct iio_attribute iio_ad9081k_global_attrs[] = {
	{
		.name = "load_file",
		.priv = IIO_AD9081K_ATTR_LOAD_FILE,
		.show = iio_ad9081k_attr_get,
		.store = iio_ad9081k_attr_set,
	},
	{
		.name = "loaded_bytes",
		.priv = IIO_AD9081K_ATTR_LOADED_BYTES,
		.show = iio_ad9081k_attr_get,
		.store = NULL,
	},
	END_ATTRIBUTES_ARRAY,
};

static struct iio_device iio_ad9081k_dev_descriptor = {
	.num_ch = 0,
	.channels = NULL,
	.attributes = iio_ad9081k_global_attrs,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
};

int iio_ad9081k_init(struct iio_ad9081k_desc **desc)
{
	struct iio_ad9081k_desc *d;

	if (!desc)
		return -EINVAL;

	d = no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->loaded_bytes = 0;
	*desc = d;

	return 0;
}

void iio_ad9081k_remove(struct iio_ad9081k_desc *desc)
{
	if (desc)
		no_os_free(desc);
}

void iio_ad9081k_get_dev_descriptor(struct iio_ad9081k_desc *desc,
				    struct iio_device **dev_descriptor)
{
	*dev_descriptor = &iio_ad9081k_dev_descriptor;
}
