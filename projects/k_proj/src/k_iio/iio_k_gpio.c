/***************************************************************************//**
 *   @file   iio_k_gpio.c
 *   @brief  IIO device exposing GPIO lines (read/write + direction).
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
#include "iio_k_gpio.h"
#include "iio.h"
#include "iio_types.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_gpio.h"

enum iio_k_gpio_attr_id {
	IIO_K_GPIO_ATTR_RAW,
	IIO_K_GPIO_ATTR_DIRECTION,
};

/*
 * The device descriptor, channel array and channel names are built at init
 * time because the number of lines is configurable. Keep them next to the
 * runtime state so a single allocation owns everything.
 *
 * NOTE: `pub` MUST be the first member so a struct iio_k_gpio_desc * can be
 * cast back to the enclosing priv (no_os has no container_of helper).
 */
struct iio_k_gpio_priv {
	struct iio_k_gpio_desc	pub;
	struct iio_device	dev;
	struct iio_channel	channels[IIO_K_GPIO_MAX_LINES];
	char			names[IIO_K_GPIO_MAX_LINES][16];
};

/******************************************************************************/
/* Attributes (per channel = per GPIO line)                                   */
/******************************************************************************/
static int iio_k_gpio_attr_show(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel, intptr_t id)
{
	struct iio_k_gpio_priv *p = device;
	uint8_t val;
	int32_t ret;

	if (!p || !channel || channel->ch_num >= (int)p->pub.num_lines)
		return -EINVAL;

	switch (id) {
	case IIO_K_GPIO_ATTR_RAW:
		ret = no_os_gpio_get_value(p->pub.gpio[channel->ch_num], &val);
		if (ret)
			return ret;
		return snprintf(buf, len, "%u", val);
	case IIO_K_GPIO_ATTR_DIRECTION: {
		uint8_t dir;
		ret = no_os_gpio_get_direction(p->pub.gpio[channel->ch_num], &dir);
		if (ret)
			return ret;
		return snprintf(buf, len, "%s",
				dir == NO_OS_GPIO_OUT ? "out" : "in");
	}
	default:
		return -EINVAL;
	}
}

static int iio_k_gpio_attr_store(void *device, char *buf, uint32_t len,
				 const struct iio_ch_info *channel, intptr_t id)
{
	struct iio_k_gpio_priv *p = device;
	int32_t ret;

	if (!p || !channel || channel->ch_num >= (int)p->pub.num_lines)
		return -EINVAL;

	switch (id) {
	case IIO_K_GPIO_ATTR_RAW:
		ret = no_os_gpio_set_value(p->pub.gpio[channel->ch_num],
					   strtoul(buf, NULL, 0) ?
					   NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);
		return ret ? ret : (int)len;
	case IIO_K_GPIO_ATTR_DIRECTION:
		if (strncmp(buf, "out", 3) == 0)
			ret = no_os_gpio_direction_output(
				p->pub.gpio[channel->ch_num], NO_OS_GPIO_LOW);
		else
			ret = no_os_gpio_direction_input(
				p->pub.gpio[channel->ch_num]);
		return ret ? ret : (int)len;
	default:
		return -EINVAL;
	}
}

static struct iio_attribute iio_k_gpio_ch_attrs[] = {
	{
		.name = "raw",
		.priv = IIO_K_GPIO_ATTR_RAW,
		.show = iio_k_gpio_attr_show,
		.store = iio_k_gpio_attr_store,
	},
	{
		.name = "direction",
		.priv = IIO_K_GPIO_ATTR_DIRECTION,
		.show = iio_k_gpio_attr_show,
		.store = iio_k_gpio_attr_store,
	},
	END_ATTRIBUTES_ARRAY,
};

/******************************************************************************/
/* Lifecycle                                                                  */
/******************************************************************************/
int iio_k_gpio_init(struct iio_k_gpio_desc **desc,
		    const struct iio_k_gpio_init_param *init)
{
	struct iio_k_gpio_priv *p;
	unsigned int i;
	int32_t ret;

	if (!desc || !init || !init->num_lines ||
	    init->num_lines > IIO_K_GPIO_MAX_LINES)
		return -EINVAL;

	p = no_os_calloc(1, sizeof(*p));
	if (!p)
		return -ENOMEM;

	p->pub.num_lines = init->num_lines;

	for (i = 0; i < init->num_lines; i++) {
		ret = no_os_gpio_get(&p->pub.gpio[i], init->lines[i].init_param);
		if (ret)
			goto err;

		if (init->lines[i].name)
			strncpy(p->names[i], init->lines[i].name,
				sizeof(p->names[i]) - 1);
		else
			snprintf(p->names[i], sizeof(p->names[i]), "gpio%u", i);

		p->channels[i].name = p->names[i];
		p->channels[i].ch_type = IIO_VOLTAGE; /* generic scalar line */
		p->channels[i].channel = i;
		p->channels[i].attributes = iio_k_gpio_ch_attrs;
		p->channels[i].ch_out = true;
		p->channels[i].indexed = true;
	}

	p->dev.num_ch = init->num_lines;
	p->dev.channels = p->channels;

	*desc = &p->pub;

	return 0;

err:
	while (i--)
		no_os_gpio_remove(p->pub.gpio[i]);
	no_os_free(p);

	return ret;
}

void iio_k_gpio_remove(struct iio_k_gpio_desc *desc)
{
	struct iio_k_gpio_priv *p;
	unsigned int i;

	if (!desc)
		return;

	p = (struct iio_k_gpio_priv *)desc;	/* pub is first member */
	for (i = 0; i < p->pub.num_lines; i++)
		no_os_gpio_remove(p->pub.gpio[i]);
	no_os_free(p);
}

void iio_k_gpio_get_dev_descriptor(struct iio_k_gpio_desc *desc,
				   struct iio_device **dev_descriptor)
{
	struct iio_k_gpio_priv *p = (struct iio_k_gpio_priv *)desc;

	*dev_descriptor = &p->dev;
}
