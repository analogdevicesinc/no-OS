/***************************************************************************//**
 *   @file   iio_attr_trig.c
 *   @brief  Demo trigger
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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

#include <stdlib.h>
#include <string.h>
#include "iio_attr_trig.h"
#include "iio_types.h"
#include "iio.h"
#include "no-os/error.h"

#define MAX_NAME_SIZE 20

struct iio_attr_trig {
	struct iio_desc		**iio_desc;
	bool			en;
	char			name[MAX_NAME_SIZE + 1];
};

static void iio_attr_trig_enable(void *trig, char *name)
{
	struct iio_attr_trig *desc = trig;

	desc->en = 1;
}

static void iio_attr_trig_disable(void *trig, char *name)
{
	struct iio_attr_trig *desc = trig;

	desc->en = 0;
}

int32_t iio_attr_trig_init(struct iio_attr_trig **trig,
			   struct iio_attr_trig_init *param)
{
	struct iio_attr_trig *ltrig;
	int32_t ret;

	ltrig = calloc(1, sizeof(*trig));
	if (!ltrig)
		return -ENOMEM;

	ltrig->iio_desc = param->iio_desc;
	strncpy(ltrig->name, param->name, MAX_NAME_SIZE);

	*trig = ltrig;

	return SUCCESS;
error:
//TODO cleanup
	return ret;
}

static int iio_attr_trigger_now(void *device, char *buf, uint32_t len,
				const struct iio_ch_info *channel,
				intptr_t priv)
{
	struct iio_attr_trig *trig = device;

	//Should it be triggered only when in buf is the number 1 ?

	if (trig->en)
		iio_trigger_notify(*trig->iio_desc, trig->name);
}

static struct iio_attribute iio_attr_trig_attributes[] = {
	{
		.name = "trigger_now",
		.store = iio_attr_trigger_now
	},
	END_ATTRIBUTES_ARRAY
};

struct iio_trigger iio_attr_trig_desc = {
	.attributes = iio_attr_trig_attributes,
	.enable = iio_attr_trig_enable,
	.disable = iio_attr_trig_disable
};
