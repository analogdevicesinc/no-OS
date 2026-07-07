/***************************************************************************//**
 *   @file   iio_k_regmap.c
 *   @brief  IIO device exposing the chip register API over the JESD CMD path.
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
#include <inttypes.h>
#include "iio_k_regmap.h"
#include "iio.h"
#include "iio_types.h"
#include "no_os_alloc.h"
#include "no_os_error.h"

enum iio_k_regmap_attr_id {
	IIO_K_REGMAP_ATTR_CMD_ADDR,
	IIO_K_REGMAP_ATTR_CMD_NUMWORDS,
	IIO_K_REGMAP_ATTR_STREAMING_READ,
	IIO_K_REGMAP_ATTR_LAST_RESPONSE,
};

/******************************************************************************/
/* Register access: mapped onto the tx_generator CMD channel via k_data.      */
/* Enables libiio `iio_reg`/--direct-reg-access against the remote chip.       */
/******************************************************************************/
static int32_t iio_k_regmap_reg_read(void *device, uint32_t reg,
				     uint32_t *readval)
{
	struct iio_k_regmap_desc *desc = device;
	int ret;

	if (!desc)
		return -ENODEV;

	ret = k_data_reg_read(desc->kd, (uint16_t)reg, readval);
	if (!ret)
		desc->last_response = *readval;

	return ret;
}

static int32_t iio_k_regmap_reg_write(void *device, uint32_t reg,
				      uint32_t writeval)
{
	struct iio_k_regmap_desc *desc = device;

	if (!desc)
		return -ENODEV;

	return k_data_reg_write(desc->kd, (uint16_t)reg, writeval);
}

/******************************************************************************/
/* Attributes                                                                 */
/******************************************************************************/
static int iio_k_regmap_attr_show(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t id)
{
	struct iio_k_regmap_desc *desc = device;

	if (!desc)
		return -ENODEV;

	switch (id) {
	case IIO_K_REGMAP_ATTR_CMD_ADDR:
		return snprintf(buf, len, "0x%04X", desc->cmd_addr);
	case IIO_K_REGMAP_ATTR_CMD_NUMWORDS:
		return snprintf(buf, len, "%u", desc->cmd_numwords);
	case IIO_K_REGMAP_ATTR_LAST_RESPONSE:
		return snprintf(buf, len, "0x%08"PRIX32, desc->last_response);
	case IIO_K_REGMAP_ATTR_STREAMING_READ: {
		/* TODO: format desc->stream_words[0..cmd_numwords-1]. */
		return snprintf(buf, len, "unimplemented");
	}
	default:
		return -EINVAL;
	}
}

static int iio_k_regmap_attr_store(void *device, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t id)
{
	struct iio_k_regmap_desc *desc = device;

	if (!desc)
		return -ENODEV;

	switch (id) {
	case IIO_K_REGMAP_ATTR_CMD_ADDR:
		desc->cmd_addr = (uint16_t)strtoul(buf, NULL, 0);
		return len;
	case IIO_K_REGMAP_ATTR_CMD_NUMWORDS:
		desc->cmd_numwords = (uint8_t)strtoul(buf, NULL, 0);
		return len;
	case IIO_K_REGMAP_ATTR_STREAMING_READ: {
		/* Writing triggers an OP_STREAMING_READ burst using the
		 * previously-set cmd_addr/cmd_numwords. */
		int rc;

		if (desc->cmd_numwords == 0 ||
		    desc->cmd_numwords > IIO_K_REGMAP_MAX_STREAMWORDS)
			return -EINVAL;
		rc = k_data_reg_read_streaming(desc->kd, desc->cmd_addr,
					       desc->cmd_numwords,
					       desc->stream_words);
		return rc ? rc : (int)len;
	}
	default:
		return -EINVAL;
	}
}

static struct iio_attribute iio_k_regmap_attrs[] = {
	{
		.name = "cmd_addr",
		.priv = IIO_K_REGMAP_ATTR_CMD_ADDR,
		.show = iio_k_regmap_attr_show,
		.store = iio_k_regmap_attr_store,
	},
	{
		.name = "cmd_numwords",
		.priv = IIO_K_REGMAP_ATTR_CMD_NUMWORDS,
		.show = iio_k_regmap_attr_show,
		.store = iio_k_regmap_attr_store,
	},
	{
		.name = "streaming_read",
		.priv = IIO_K_REGMAP_ATTR_STREAMING_READ,
		.show = iio_k_regmap_attr_show,
		.store = iio_k_regmap_attr_store,
	},
	{
		.name = "last_response",
		.priv = IIO_K_REGMAP_ATTR_LAST_RESPONSE,
		.show = iio_k_regmap_attr_show,
		.store = NULL,
	},
	END_ATTRIBUTES_ARRAY,
};

static struct iio_device iio_k_regmap_dev = {
	.num_ch = 0,
	.channels = NULL,
	.attributes = iio_k_regmap_attrs,
	.debug_reg_read = iio_k_regmap_reg_read,
	.debug_reg_write = iio_k_regmap_reg_write,
};

/******************************************************************************/
/* Lifecycle                                                                  */
/******************************************************************************/
int iio_k_regmap_init(struct iio_k_regmap_desc **desc,
		      const struct iio_k_regmap_init_param *init)
{
	struct iio_k_regmap_desc *d;

	if (!desc || !init || !init->kd)
		return -EINVAL;

	d = no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	d->kd = init->kd;
	d->cmd_numwords = 1;

	*desc = d;

	return 0;
}

void iio_k_regmap_remove(struct iio_k_regmap_desc *desc)
{
	if (desc)
		no_os_free(desc);
}

void iio_k_regmap_get_dev_descriptor(struct iio_k_regmap_desc *desc,
				     struct iio_device **dev_descriptor)
{
	(void)desc;
	*dev_descriptor = &iio_k_regmap_dev;
}
