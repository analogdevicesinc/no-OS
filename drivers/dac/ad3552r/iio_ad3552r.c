/**************************************************************************//**
*   @file   iio_ad3552r.c
*   @brief  IIO implementation for ad3552r Driver
*   @author Mihail Chindris (Mihail.Chindris@analog.com)
*
*******************************************************************************
* Copyright 2021(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification,
* are permitted provided that the following conditions are met:
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
* INCIDENTAL,SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
* DAMAGE.
******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdio.h>
#include "iio_types.h"
#include "iio.h"
#include "ad3552r.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/*****************************************************************************/
/******************** Macros and Constants Definitions ***********************/
/*****************************************************************************/

#define AD3552R_ATTR(_name, _priv) {\
	.name = _name,\
	.priv = _priv,\
	.show = iio_ad3552r_attr_get,\
	.store = iio_ad3552r_attr_set\
}

#define AD3552R_DAC_CH(_idx)  (struct iio_channel){\
	.ch_type = IIO_VOLTAGE,\
	.ch_out = 1,\
	.indexed = true,\
	.channel = _idx,\
	.scan_index = _idx,\
	.scan_type = &ad3552r_dac_scan_type,\
	.attributes = iio_ad3552r_ch_attributes}

enum ad3552r_iio_attrs {
	AD3552R_IIO_ATTR_EN,
	AD3552R_IIO_ATTR_OFFSET,
	AD3552R_IIO_ATTR_RAW,
	AD3552R_IIO_ATTR_SCALE,
};

struct iio_ad3552r_desc {
	struct iio_channel channels[AD3552R_MAX_NUM_CH];
	struct iio_device iio_desc;
	struct ad3552r_desc *dac;
	uint32_t mask;
};

/*****************************************************************************/
/************************* Functions Definitions *****************************/
/*****************************************************************************/

static int iio_ad3552r_attr_get(void *device, char *buf, size_t len,
				const struct iio_ch_info *channel, intptr_t priv)
{
	struct iio_ad3552r_desc *iio_dac = device;
	int32_t vals[2], err;
	uint16_t val16;

	switch (priv) {
	case AD3552R_IIO_ATTR_EN:
		err = ad3552r_get_ch_value(iio_dac->dac,
					   AD3552R_CH_DAC_POWERDOWN,
					   channel->ch_num, &val16);
		if (NO_OS_IS_ERR_VALUE(err))
			return err;

		vals[0] = !val16;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, vals);

	case AD3552R_IIO_ATTR_RAW:
		err = ad3552r_get_ch_value(iio_dac->dac, AD3552R_CH_CODE,
					   channel->ch_num, &val16);
		if (NO_OS_IS_ERR_VALUE(err))
			return err;

		vals[0] = val16;

		return iio_format_value(buf, len, IIO_VAL_INT, 1, vals);

	case AD3552R_IIO_ATTR_OFFSET:
		err = ad3552r_get_offset(iio_dac->dac, channel->ch_num,
					 &vals[0], &vals[1]);
		if (NO_OS_IS_ERR_VALUE(err))
			return err;

		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2,
					vals);
	case AD3552R_IIO_ATTR_SCALE:
		err = ad3552r_get_scale(iio_dac->dac, channel->ch_num,
					&vals[0], &vals[1]);
		if (NO_OS_IS_ERR_VALUE(err))
			return err;

		return iio_format_value(buf, len, IIO_VAL_INT_PLUS_MICRO, 2,
					vals);
	default:
		return -EINVAL;
	}
}

static int iio_ad3552r_attr_set(void *device, char *buf, size_t len,
				const struct iio_ch_info *channel, intptr_t priv)
{
	struct iio_ad3552r_desc *iio_dac = device;
	uint32_t val;
	int32_t err;

	val = no_os_str_to_uint32(buf);
	switch (priv) {
	case AD3552R_IIO_ATTR_EN:
		err = ad3552r_set_ch_value(iio_dac->dac,
					   AD3552R_CH_DAC_POWERDOWN,
					   channel->ch_num, !val);
		if (NO_OS_IS_ERR_VALUE(err))
			return err;
	case AD3552R_IIO_ATTR_RAW:
		err = ad3552r_set_ch_value(iio_dac->dac, AD3552R_CH_CODE,
					   channel->ch_num, val);
		if (NO_OS_IS_ERR_VALUE(err))
			return err;
	default:
		return -EINVAL;
	}

	return len;
}

/******************************************************************************/
/************************** IIO Types Declarations *****************************/
/******************************************************************************/

static struct scan_type ad3552r_dac_scan_type = {
	.realbits = 16,
	.storagebits = 16,
	.shift = 0,
	.sign = 'u',
	.is_big_endian = true
};

static struct iio_attribute iio_ad3552r_ch_attributes[] = {
	AD3552R_ATTR("en", AD3552R_IIO_ATTR_EN),
	AD3552R_ATTR("offset", AD3552R_IIO_ATTR_OFFSET),
	AD3552R_ATTR("raw", AD3552R_IIO_ATTR_RAW),
	AD3552R_ATTR("scale", AD3552R_IIO_ATTR_SCALE),
	END_ATTRIBUTES_ARRAY,
};

static int32_t iio_ad3552r_write_reg(struct iio_ad3552r_desc *iio_dac,
				     uint32_t addr, uint32_t val)
{
	return ad3552r_write_reg(iio_dac->dac, addr, val);
}


static int32_t iio_ad3552r_read_reg(struct iio_ad3552r_desc *iio_dac,
				    uint32_t addr, uint32_t *val)
{
	uint16_t v2;
	int32_t err;

	err = ad3552r_read_reg(iio_dac->dac, addr, &v2);
	*val = v2;

	return err;
}

static int32_t iio_ad3552r_prep_wr(struct iio_ad3552r_desc *iio_dac,
				   uint32_t mask)
{
	iio_dac->mask = mask;

	return 0;
}

static int32_t iio_ad3552r_wr_dev(struct iio_ad3552r_desc *iio_dac,
				  uint16_t *buff, uint32_t nb_samples)
{
	int32_t i;

	static int c = 0;
	c++;

	for (i = 0; i < nb_samples * no_os_hweight32(iio_dac->mask); ++i)
		buff[i] = no_os_get_unaligned_be16((uint8_t *)&buff[i]);

	return ad3552r_write_samples(iio_dac->dac, buff, nb_samples,
				     iio_dac->mask,
				     AD3552R_WRITE_INPUT_REGS_AND_TRIGGER_LDAC);
}


int32_t iio_ad3552r_init(struct iio_ad3552r_desc **iio_dac,
			 struct ad3552r_init_param *param)
{
	struct iio_ad3552r_desc *liio_dac;
	uint32_t i, j;
	int32_t err;

	if (!iio_dac || !param)
		return -EINVAL;

	liio_dac = (struct iio_ad3552r_desc *)no_os_calloc(1, sizeof(*liio_dac));
	if (!liio_dac)
		return -ENOMEM;

	err = ad3552r_init(&liio_dac->dac, param);
	if (NO_OS_IS_ERR_VALUE(err)) {
		no_os_free(liio_dac);
		return err;
	}

	j = 0;
	for (i = 0; i < AD3552R_MAX_NUM_CH; ++i)
		if (param->channels[i].en)
			liio_dac->channels[j++] = AD3552R_DAC_CH(i);

	liio_dac->iio_desc.num_ch = j;
	liio_dac->iio_desc.channels = liio_dac->channels;
	liio_dac->iio_desc.write_dev = (int32_t (*)())iio_ad3552r_wr_dev;
	liio_dac->iio_desc.pre_enable = (int32_t (*)())iio_ad3552r_prep_wr;
	liio_dac->iio_desc.debug_reg_read = (int32_t (*)())iio_ad3552r_read_reg;
	liio_dac->iio_desc.debug_reg_write = (int32_t (*)())iio_ad3552r_write_reg;

	*iio_dac = liio_dac;

	return 0;
}

int32_t iio_ad3552r_remove(struct iio_ad3552r_desc *iio_dac)
{
	if (!iio_dac)
		return -EINVAL;

	ad3552r_remove(iio_dac->dac);

	no_os_free(iio_dac);

	return 0;
}

void iio_ad3552r_get_descriptor(struct iio_ad3552r_desc *iio_dac,
				struct iio_device **desc)
{
	if (!iio_dac || !desc)
		return ;

	*desc = &iio_dac->iio_desc;
}
