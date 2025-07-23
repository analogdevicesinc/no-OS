/***************************************************************************//**
 *   @file   ad7490.c
 *   @brief  Source file of AD7490 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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
#include "ad7490.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_delay.h"

static int ad7490_transfer(struct ad7490_desc *desc, uint16_t *data)
{
	struct no_os_spi_msg xfer = {
		.bytes_number = 2,
		.cs_change = 1,
	};
	uint8_t buff[2];
	uint16_t reg_val;
	int ret;

	reg_val = *data;
	buff[0] = no_os_field_get(AD7490_LOW_BYTE_MASK, data);
	buff[1] = no_os_field_get(AD7490_HIGH_BYTE_MASK, data);
	xfer.rx_buff = buff;
	xfer.tx_buff = buff;

	ret = no_os_spi_transfer(desc->spi_desc, &xfer, 1);
	if (ret)
		return ret;

	*data = no_os_field_prep(AD7490_HIGH_BYTE_MASK, buff[1]) | buff[0];

	return 0;
}

int ad7490_set_op_mode(struct ad7490_desc *desc, enum ad7490_op_mode op_mode)
{
	uint16_t reg_val;
	int ret;

	reg_val = no_os_field_prep(AD7490_WRITE_MASK, 1)
		  | no_os_field_prep(AD7490_ADD_MASK, 0)
		  | no_os_field_prep(AD7490_PM_MASK, op_mode)
		  | no_os_field_prep(AD7490_CFG_MASK, desc->cfg);

	reg_val <<= 4;
	ret = ad7490_transfer(desc, &reg_val);
	if (ret)
		return ret;

	desc->op_mode = op_mode;

	switch (op_mode) {
	case AD7490_MODE_AUTOSTANDBY:
		reg_val = 0xFF;
		return ad7490_transfer(desc, &reg_val);
	case AD7490_MODE_AUTOSHUTDOWN:
		no_os_udelay(1);

		return 0;
	case AD7490_MODE_FULLSHUTDOWN:
		return 0;
	case AD7490_MODE_NORMAL:
		return 0;
	default:
		return -EINVAL;
	}
}

int ad7490_config(struct ad7490_desc *desc, enum ad7490_dout_state dout_state,
		  enum ad7490_range range, enum ad7490_coding coding)
{
	uint16_t reg_val;
	int ret;

	if (range == AD7490_RANGE_2XREFIN && desc->vdd == AD7490_VDD_3V3)
		return -EINVAL;

	desc->cfg = no_os_field_prep(AD7490_WEAKTRI_MASK, dout_state)
		    | no_os_field_prep(AD7490_RANGE_MASK, range)
		    | no_os_field_prep(AD7490_CODING_BINARY, coding);

	return 0;
}

int ad7490_read_ch(struct ad7490_desc *desc, enum ad7490_address channel,
		   int16_t *val)
{
	uint16_t reg_val;
	uint16_t tmp;
	int ret;

	if (desc->seq_op != AD7490_NO_SEQ)
		return -EINVAL;

	reg_val = no_os_field_prep(AD7490_WRITE_MASK, 1)
		  | no_os_field_prep(AD7490_SEQ_MASK, 0)
		  | no_os_field_prep(AD7490_ADD_MASK, channel)
		  | no_os_field_prep(AD7490_PM_MASK, desc->op_mode)
		  | no_os_field_prep(AD7490_SHADOW_MASK, 0)
		  | no_os_field_prep(AD7490_CFG_MASK, desc->cfg);

	tmp = reg_val << 4;
	reg_val <<= 4;
	ret = ad7490_transfer(desc, &reg_val);
	if (ret)
		return ret;

	ret = ad7490_transfer(desc, &tmp);
	if (ret)
		return ret;

	desc->address = channel;
	*val = no_os_field_get(AD7490_VAL_MASK, reg_val);

	return 0;
}

int ad7490_start_seq(struct ad7490_desc *desc, enum ad7490_seq_op seq_op,
		     uint16_t channels, enum ad7490_address last_chan)
{
	uint16_t reg_val, tmp;
	int ret, i, k;

	k = 0;
	for (i = 0; i < 16; i++) {
		if (no_os_field_get(NO_OS_BIT(i), channels))
			k++;
	}

	if (!k)
		return -EINVAL;

	desc->nb_channels_seq = k;

	switch (seq_op) {
	case AD7490_NO_SEQ:
		return -EINVAL;
	case AD7490_SHADOW_SEQ:
	case AD7490_CONTROL_SEQ:
		reg_val = no_os_field_prep(AD7490_WRITE_MASK, 1)
			  | no_os_field_prep(AD7490_SEQ_MASK, no_os_field_get(NO_OS_BIT(1), seq_op))
			  | no_os_field_prep(AD7490_ADD_MASK, desc->address)
			  | no_os_field_prep(AD7490_PM_MASK, desc->op_mode)
			  | no_os_field_prep(AD7490_SHADOW_MASK, 1)
			  | no_os_field_prep(AD7490_CFG_MASK, desc->cfg);

		reg_val <<= 4;
		tmp = reg_val;
		ret = ad7490_transfer(desc, &reg_val);
		if (ret)
			return ret;

		reg_val = channels;
		ret = ad7490_transfer(desc, &tmp);
		if (ret)
			return ret;

		break;
	case AD7490_CONSECUTIVE_SEQ:
		reg_val = no_os_field_prep(AD7490_WRITE_MASK, 1)
			  | no_os_field_prep(AD7490_SEQ_MASK, no_os_field_get(NO_OS_BIT(1), seq_op))
			  | no_os_field_prep(AD7490_ADD_MASK, last_chan)
			  | no_os_field_prep(AD7490_PM_MASK, desc->op_mode)
			  | no_os_field_prep(AD7490_SHADOW_MASK, 1)
			  | no_os_field_prep(AD7490_CFG_MASK, desc->cfg);

		reg_val <<= 4;
		ret = ad7490_transfer(desc, &reg_val);
		if (ret)
			return ret;

		break;
	default:
		return -EINVAL;
	}

	desc->seq_op = seq_op;

	return 0;
}

int ad7490_change_seq(struct ad7490_desc *desc, enum ad7490_seq_op seq_op,
		      enum ad7490_coding coding, enum ad7490_range range)
{
	uint16_t reg_val;
	int ret;

	switch (seq_op) {
	case AD7490_NO_SEQ:
		return -EINVAL;
	case AD7490_SHADOW_SEQ:
		reg_val = no_os_field_prep(AD7490_WRITE_MASK, 0)
			  | no_os_field_prep(AD7490_SEQ_MASK, no_os_field_get(NO_OS_BIT(1), seq_op))
			  | no_os_field_prep(AD7490_ADD_MASK, desc->address)
			  | no_os_field_prep(AD7490_PM_MASK, desc->op_mode)
			  | no_os_field_prep(AD7490_SHADOW_MASK, no_os_field_get(NO_OS_BIT(0), seq_op))
			  | no_os_field_prep(AD7490_CFG_MASK, desc->cfg);

		break;
	case AD7490_CONTROL_SEQ:
		reg_val = no_os_field_prep(AD7490_WRITE_MASK, 1)
			  | no_os_field_prep(AD7490_SEQ_MASK, 1)
			  | no_os_field_prep(AD7490_ADD_MASK, desc->address)
			  | no_os_field_prep(AD7490_PM_MASK, desc->op_mode)
			  | no_os_field_prep(AD7490_SHADOW_MASK, 0)
			  | no_os_field_prep(AD7490_CFG_MASK, desc->cfg);

		break;
	case AD7490_CONSECUTIVE_SEQ:
		return -EINVAL;
	default:
		return -EINVAL;
	}

	reg_val <<= 4;
	ret = ad7490_transfer(desc, &reg_val);
	if (ret)
		return ret;

	desc->seq_op = seq_op;

	return 0;
}

int ad7490_stop_seq(struct ad7490_desc *desc)
{
	uint16_t reg_val;
	int ret;

	reg_val = no_os_field_prep(AD7490_WRITE_MASK, 1)
		  | no_os_field_prep(AD7490_SEQ_MASK, 0)
		  | no_os_field_prep(AD7490_ADD_MASK, desc->address)
		  | no_os_field_prep(AD7490_PM_MASK, desc->op_mode)
		  | no_os_field_prep(AD7490_SHADOW_MASK, 0)
		  | no_os_field_prep(AD7490_CFG_MASK, desc->cfg);

	reg_val <<= 4;
	ret = ad7490_transfer(desc, &reg_val);
	if (ret)
		return ret;

	desc->seq_op = AD7490_NO_SEQ;

	return 0;
}

int ad7490_read_seq(struct ad7490_desc *desc, int16_t *channels_val)
{
	uint16_t reg_val;
	uint16_t tmp;
	int ret, i;

	switch (desc->seq_op) {
	case AD7490_NO_SEQ:
		return -EINVAL;
	case AD7490_SHADOW_SEQ:
		reg_val = no_os_field_prep(AD7490_WRITE_MASK, 0)
			  | no_os_field_prep(AD7490_SEQ_MASK, no_os_field_get(NO_OS_BIT(1), desc->seq_op))
			  | no_os_field_prep(AD7490_ADD_MASK, desc->address)
			  | no_os_field_prep(AD7490_PM_MASK, desc->op_mode)
			  | no_os_field_prep(AD7490_SHADOW_MASK, no_os_field_get(NO_OS_BIT(0),
					     desc->seq_op))
			  | no_os_field_prep(AD7490_CFG_MASK, desc->cfg);

		break;
	case AD7490_CONTROL_SEQ:
		reg_val = no_os_field_prep(AD7490_WRITE_MASK, 1)
			  | no_os_field_prep(AD7490_SEQ_MASK, 1)
			  | no_os_field_prep(AD7490_ADD_MASK, desc->address)
			  | no_os_field_prep(AD7490_PM_MASK, desc->op_mode)
			  | no_os_field_prep(AD7490_SHADOW_MASK, 0)
			  | no_os_field_prep(AD7490_CFG_MASK, desc->cfg);

		break;
	case AD7490_CONSECUTIVE_SEQ:
		return -EINVAL;
	default:
		return -EINVAL;
	}

	reg_val <<= 4;
	tmp = reg_val;

	for (i = 0; i < desc->nb_channels_seq; i++) {
		reg_val = tmp;

		ret = ad7490_transfer(desc, &reg_val);
		if (ret)
			return ret;

		channels_val[i] = no_os_field_get(AD7490_VAL_MASK, reg_val);
	}

	return 0;
}

int ad7490_init(struct ad7490_desc **desc,
		struct ad7490_init_param *init_param)
{
	struct ad7490_desc *descriptor;
	uint16_t reg_val;
	int ret;

	descriptor = (struct ad7490_desc *)no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_spi_init(&descriptor->spi_desc, init_param->spi_param);
	if (ret)
		goto free_desc;

	reg_val = 0xFF;
	ret = ad7490_transfer(descriptor, &reg_val);
	if (ret)
		goto free_spi;

	ret = ad7490_transfer(descriptor, &reg_val);
	if (ret)
		goto free_spi;

	descriptor->cfg = no_os_field_prep(AD7490_RANGE_MASK, AD7490_RANGE_REFIN);
	ret = ad7490_set_op_mode(descriptor, init_param->op_mode);
	if (ret)
		goto free_spi;

	descriptor->vdd = init_param->vdd;

	*desc = descriptor;

	return 0;

free_spi:
	no_os_spi_remove(descriptor->spi_desc);
free_desc:
	no_os_free(descriptor);

	return ret;
}

int ad7490_remove(struct ad7490_desc *desc)
{
	ad7490_stop_seq(desc);
	ad7490_set_op_mode(desc, AD7490_MODE_FULLSHUTDOWN);
	no_os_spi_remove(desc->spi_desc);
	no_os_free(desc);

	return 0;
}
