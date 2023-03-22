/***************************************************************************//**
 *   @file   mdio_bitbang.c
 *   @brief  Source file for MDIO implementation using gpio bit banging.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include <errno.h>
#include <stdlib.h>
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_gpio.h"
#include "no_os_mdio.h"
#include "mdio_bitbang.h"

struct mdio_bitbang_extra {
	struct no_os_gpio_desc *mdc;
	struct no_os_gpio_desc *mdio;
};

int mdio_bitbang_init(struct no_os_mdio_desc **dev,
		      struct no_os_mdio_init_param *ip)
{
	int ret;
	struct mdio_bitbang_init_param *mbip = ip->extra;

	struct mdio_bitbang_extra *mbe = no_os_calloc(1, sizeof(*mbe));
	if (!mbe)
		return -ENOMEM;

	struct no_os_mdio_desc *d = no_os_calloc(1, sizeof(*dev));
	if (!d)
		goto error;

	ret = no_os_gpio_get(&mbe->mdc, &mbip->mdc);
	if (ret)
		goto error_0;

	ret = no_os_gpio_get(&mbe->mdio, &mbip->mdio);
	if (ret)
		goto error_1;


	ret = no_os_gpio_direction_output(mbe->mdc, NO_OS_GPIO_LOW);
	if (ret)
		goto error_2;

	ret = no_os_gpio_direction_output(mbe->mdio, NO_OS_GPIO_HIGH);
	if (ret)
		goto error_2;

	d->extra = mbe;
	*dev = d;

	return 0;
error_2:
	no_os_gpio_remove(mbe->mdio);
error_1:
	no_os_gpio_remove(mbe->mdc);
error_0:
	no_os_free(d);
error:
	no_os_free(mbe);

	return ret;
}

static int mdio_rw(struct no_os_mdio_desc *dev, bool c45, uint16_t op,
		   uint32_t reg, uint16_t *data)
{
	int i;
	uint32_t frame;
	uint8_t state;
	struct mdio_bitbang_extra *mbe = dev->extra;
	uint8_t start = c45 ? NO_OS_MDIO_C45_START : NO_OS_MDIO_C22_START;
	uint8_t regaddr = c45 ? no_os_field_get(NO_OS_MDIO_C45_DEVADDR_MASK, reg) : reg;
	uint16_t data2;

	frame = no_os_field_prep(NO_OS_MDIO_START_MASK, start) |
		no_os_field_prep(NO_OS_MDIO_OP_MASK, op) |
		no_os_field_prep(NO_OS_MDIO_PHYADDR_MASK, dev->addr) |
		no_os_field_prep(NO_OS_MDIO_REGADDR_MASK, regaddr) |
		no_os_field_prep(NO_OS_MDIO_TURNAROUND_MASK, NO_OS_MDIO_TURNAROUND);

	// preamble
	no_os_gpio_direction_output(mbe->mdio, NO_OS_GPIO_HIGH);
	for (i = 31; i >= 0; i--) {
		no_os_gpio_set_value(mbe->mdc, NO_OS_GPIO_HIGH);
		no_os_gpio_set_value(mbe->mdc, NO_OS_GPIO_LOW);
	}

	// start, read, phyaddr, regaddr
	for (i = 31; i >= 16; i--) {
		no_os_gpio_set_value(mbe->mdio, (bool)(frame & (1u << i)));
		no_os_gpio_set_value(mbe->mdc, NO_OS_GPIO_HIGH);
		no_os_gpio_set_value(mbe->mdc, NO_OS_GPIO_LOW);
	}

	if (op == NO_OS_MDIO_OP_WRITE || op == NO_OS_MDIO_OP_ADDRESS) {
		data2 = op == NO_OS_MDIO_OP_ADDRESS ? (uint16_t)reg : *data;
		for (i = 15; i >= 0; i--) {
			no_os_gpio_set_value(mbe->mdio, (data2 >> i) & 0x1);
			no_os_gpio_set_value(mbe->mdc, NO_OS_GPIO_HIGH);
			no_os_gpio_set_value(mbe->mdc, NO_OS_GPIO_LOW);
		}
	} else {
		*data = 0;
		no_os_gpio_direction_input(mbe->mdio);
		for (i = 15; i >= 0; i--) {
			no_os_gpio_get_value(mbe->mdio, &state);
			if (state)
				*data |= 1;
			if (i)
				*data <<= 1;
			no_os_gpio_set_value(mbe->mdc, NO_OS_GPIO_HIGH);
			no_os_gpio_set_value(mbe->mdc, NO_OS_GPIO_LOW);
		}
	}

	return 0;
}

int mdio_bitbang_write(struct no_os_mdio_desc *dev, uint32_t reg, uint16_t in)
{
	int ret;
	bool c45 = dev->c45 && reg >= NO_OS_MDIO_C22_REGS;

	if (c45) {
		ret = mdio_rw(dev, c45, NO_OS_MDIO_OP_ADDRESS, reg, NULL);
		if (ret)
			return ret;
	}

	return mdio_rw(dev, c45, NO_OS_MDIO_OP_WRITE, reg, &in);
}

int mdio_bitbang_read(struct no_os_mdio_desc *dev, uint32_t reg, uint16_t *out)
{
	int ret;
	bool c45 = dev->c45 && reg >= NO_OS_MDIO_C22_REGS;

	if (c45) {
		ret = mdio_rw(dev, c45, NO_OS_MDIO_OP_ADDRESS, reg, NULL);
		if (ret)
			return ret;
	}

	return mdio_rw(dev, c45, NO_OS_MDIO_OP_READ, reg, out);
}

int mdio_bitbang_remove(struct no_os_mdio_desc *dev)
{
	struct mdio_bitbang_extra *mbe = dev->extra;

	no_os_gpio_remove(mbe->mdio);
	no_os_gpio_remove(mbe->mdc);
	no_os_free(mbe);
	no_os_free(dev);

	return 0;
}

struct no_os_mdio_ops mdio_bitbang_ops = {
	.init = mdio_bitbang_init,
	.write = mdio_bitbang_write,
	.read = mdio_bitbang_read,
	.remove = mdio_bitbang_remove,
};
