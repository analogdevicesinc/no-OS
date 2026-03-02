/***************************************************************************//**
 *   @file   ad4692.c
 *   @brief  Implementation of ad4692 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ad4692.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_units.h"

const int ad4692_int_osc_val[16] = {
	[AD4692_OSC_1MHZ] = 1000000,
	[AD4692_OSC_500KHZ] = 500000,
	[AD4692_OSC_400KHZ] = 400000,
	[AD4692_OSC_250KHZ] = 250000,
	[AD4692_OSC_200KHZ] = 200000,
	[AD4692_OSC_167KHZ] = 167000,
	[AD4692_OSC_133KHZ] = 133000,
	[AD4692_OSC_125KHZ] = 125000,
	[AD4692_OSC_100KHZ] = 100000,
	[AD4692_OSC_50KHZ] = 50000,
	[AD4692_OSC_25KHZ] = 25000,
	[AD4692_OSC_12P5KHZ] = 12500,
	[AD4692_OSC_10KHZ] = 10000,
	[AD4692_OSC_5KHZ] = 5000,
	[AD4692_OSC_2P5KHZ] = 2500,
	[AD4692_OSC_1P25KHZ] = 1250,
};

static int ad4692_product_id[] = {
	[ID_AD4692] = 0x12,
	[ID_AD4691] = 0x11,
	[ID_AD4694] = 0x14,
	[ID_AD4693] = 0x13,
};

static int ad4692_max_rate[] = {
	[ID_AD4692] = 1000000,
	[ID_AD4691] = 500000,
	[ID_AD4694] = 1000000,
	[ID_AD4693] = 500000,
};

static int ad4692_num_channels[] = {
	[ID_AD4692] = 16,
	[ID_AD4691] = 16,
	[ID_AD4694] = 8,
	[ID_AD4693] = 8,
};

static int ad4692_config(struct ad4692_desc *desc, enum ad4692_spi_mode mode)
{
	uint32_t reg_val;
	int ret;

	ret = ad4692_reg_read(desc, AD4692_PRODUCT_ID_LSB_REG, &reg_val);
	if (ret)
		return ret;

	if (reg_val != ad4692_product_id[desc->id])
		return -ENODEV;

	ret = ad4692_reg_write(desc, AD4692_INTERFACE_STATUS_A_REG,
			       AD4692_CLEAR_IA_STATUS);
	if (ret)
		return ret;

	ret = ad4692_reg_read(desc, AD4692_INTERFACE_STATUS_A_REG, &reg_val);
	if (ret)
		return ret;

	if (no_os_field_get(AD4692_REG_ACCESS_ERR_MASK, reg_val))
		return -EINVAL;

	/* Performing a STATE RESET for all channels. */
	ret = ad4692_reg_write(desc, AD4692_STATE_RESET_REG,
			       AD4692_STATE_RESET_ALL);
	if (ret)
		return ret;

	/* Clearing STATUS Register. */
	ret = ad4692_reg_read(desc, AD4692_STATUS_REG, &reg_val);
	if (ret)
		return ret;

	/* Configuring Voltage Reference. */
	switch (desc->vref) {
	case AD4692_VREF_MIN ... AD4692_VREF_2P75:
		ret = ad4692_reg_write(desc, AD4692_REF_CTRL_REG,
				       AD4692_REF_VOLTAGE_2P75);
		break;
	case (AD4692_VREF_2P75 + 1) ... AD4692_VREF_3P25:
		ret = ad4692_reg_write(desc, AD4692_REF_CTRL_REG,
				       AD4692_REF_VOLTAGE_3P25);
		break;
	case (AD4692_VREF_3P25 + 1) ... AD4692_VREF_3P75:
		ret = ad4692_reg_write(desc, AD4692_REF_CTRL_REG,
				       AD4692_REF_VOLTAGE_3P75);
		break;
	case (AD4692_VREF_3P75 + 1) ... AD4692_VREF_4P50:
		ret = ad4692_reg_write(desc, AD4692_REF_CTRL_REG,
				       AD4692_REF_VOLTAGE_4P50);
		break;
	case (AD4692_VREF_4P50 + 1) ... AD4692_VREF_MAX:
		ret = ad4692_reg_write(desc, AD4692_REF_CTRL_REG,
				       AD4692_REF_VOLTAGE_5);
		break;
	default:
		return -EINVAL;
	}
	if (ret)
		return ret;

	/* Configuring GPIO Output for GPI0 as interrupt for each mode. */
	switch (mode) {
	case AD4692_CNV_CLOCK:
	case AD4692_CNV_BURST:
	case AD4692_SPI_BURST:
		ret = ad4692_reg_update(desc, AD4692_GPIO_MODE1_REG,
					AD4692_GPIO0_MASK,
					AD4692_GPIO_OUTPUT_DATA_READYb);
		if (ret)
			return ret;

		ret = ad4692_reg_write(desc, AD4692_ADC_SETUP_REG,
				       mode);
		if (ret)
			return ret;

		break;
	case AD4692_AUTONOMOUS:
		ret = ad4692_reg_update(desc, AD4692_GPIO_MODE1_REG,
					AD4692_GPIO0_MASK,
					AD4692_GPIO_OUTPUT_ADC_BUSY);
		if (ret)
			return ret;

		ret = ad4692_reg_write(desc, AD4692_ADC_SETUP_REG,
				       mode);
		if (ret)
			return ret;

		break;
	case AD4692_MANUAL_MODE:
		ret = ad4692_reg_update(desc, AD4692_GPIO_MODE1_REG,
					AD4692_GPIO0_MASK,
					AD4692_GPIO_OUTPUT_ADC_BUSY);
		if (ret)
			return ret;

		ret = ad4692_reg_write(desc, AD4692_DEVICE_SETUP_REG,
				       AD4692_DEVICE_MANUAL);
		if (ret)
			return ret;

		break;
	default:
		return -EINVAL;
	}

	desc->mode = mode;

	return 0;
}

int ad4692_send_command(struct ad4692_desc *desc, uint32_t command,
			uint32_t *val)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.rx_buff = desc->buff,
		.bytes_number = 3,
		.cs_change = 1,
	};
	int ret;

	if (desc->mode != AD4692_MANUAL_MODE)
		return -EOPNOTSUPP;

	memset(desc->buff, 0, xfer.bytes_number);

	desc->buff[0] = command;
	ret = no_os_spi_transfer(desc->comm_desc, &xfer, 1);
	if (ret)
		return ret;

	*val = no_os_get_unaligned_be24(&desc->buff[0]);

	return 0;
}

int ad4692_reg_read(struct ad4692_desc *desc, uint32_t reg, uint32_t *val)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.rx_buff = desc->buff,
		.cs_change = 1,
	};
	int ret;

	if (desc->mode == AD4692_MANUAL_MODE)
		return -EOPNOTSUPP;

	switch (reg) {
	case 0 ... AD4692_OSC_FREQ_REG:
	case AD4692_SPARE_CONTROL_REG ... AD4692_ACC_SAT_OVR_REG(15):
		xfer.bytes_number = 3;
		memset(desc->buff, 0, xfer.bytes_number);
		desc->buff[0] = AD4692_RW_ADDR_MASK | AD4692_MSB_MASK(reg);
		desc->buff[1] = AD4692_LSB_MASK(reg);
		ret = no_os_spi_transfer(desc->comm_desc, &xfer, 1);
		if (ret)
			return ret;

		*val = desc->buff[2];
		break;
	case AD4692_STD_SEQ_CONFIG_REG:
	case (AD4692_STD_SEQ_CONFIG_REG + 1):
	case AD4692_AVG_IN_REG(0) ...(AD4692_AVG_IN_REG(15) + 1):
		xfer.bytes_number = 4;
		memset(desc->buff, 0, xfer.bytes_number);

		desc->buff[0] = AD4692_RW_ADDR_MASK | AD4692_MSB_MASK(reg);
		desc->buff[1] = AD4692_LSB_MASK(reg);
		ret = no_os_spi_transfer(desc->comm_desc, &xfer, 1);
		if (ret)
			return ret;

		*val = no_os_get_unaligned_be16(&desc->buff[2]);
		break;
	case AD4692_AVG_STS_IN_REG(0) ...(AD4692_ACC_IN_REG(15) + 2):
		xfer.bytes_number = 5;
		memset(desc->buff, 0, xfer.bytes_number);

		desc->buff[0] = AD4692_RW_ADDR_MASK | AD4692_MSB_MASK(reg);
		desc->buff[1] = AD4692_LSB_MASK(reg);
		ret = no_os_spi_transfer(desc->comm_desc, &xfer, 1);
		if (ret)
			return ret;

		*val = no_os_get_unaligned_be24(&desc->buff[2]);
		break;
	case AD4692_ACC_STS_DATA_REG(0) ...(AD4692_ACC_STS_DATA_REG(15) + 3):
		xfer.bytes_number = 6;
		memset(desc->buff, 0, xfer.bytes_number);

		desc->buff[0] = AD4692_RW_ADDR_MASK | AD4692_MSB_MASK(reg);
		desc->buff[1] = AD4692_LSB_MASK(reg);
		ret = no_os_spi_transfer(desc->comm_desc, &xfer, 1);
		if (ret)
			return ret;

		*val = no_os_get_unaligned_be32(&desc->buff[2]);
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

int ad4692_reg_write(struct ad4692_desc *desc, uint32_t reg, uint32_t val)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.cs_change = 1,
	};

	if (desc->mode == AD4692_MANUAL_MODE)
		return -EOPNOTSUPP;

	switch (reg) {
	case 0 ... AD4692_OSC_FREQ_REG:
	case AD4692_SPARE_CONTROL_REG ... AD4692_GPIO_MODE2_REG:
		if (val > 0xFF)
			return -EINVAL;

		desc->buff[0] = AD4692_MSB_MASK(reg);
		desc->buff[1] = AD4692_LSB_MASK(reg);

		xfer.bytes_number = 3;
		desc->buff[2] = val;
		break;
	case AD4692_STD_SEQ_CONFIG_REG:
		if (val > 0xFFFF)
			return -EINVAL;

		desc->buff[0] = AD4692_MSB_MASK(reg);
		desc->buff[1] = AD4692_LSB_MASK(reg);

		xfer.bytes_number = 4;

		no_os_put_unaligned_be16(val, &desc->buff[2]);
		break;
	default:
		return -EINVAL;
	}

	return no_os_spi_transfer(desc->comm_desc, &xfer, 1);
}

int ad4692_reg_update(struct ad4692_desc *desc, uint32_t reg, uint32_t mask,
		      uint32_t val)
{
	uint32_t reg_val = 0;
	int ret;

	ret = ad4692_reg_read(desc, reg, &reg_val);
	if (ret)
		return ret;

	reg_val &= ~mask;
	reg_val |= mask & val;

	return ad4692_reg_write(desc, reg, reg_val);
}

int ad4692_std_seq_ch(struct ad4692_desc *desc, uint16_t ch_mask, bool std,
		      uint8_t len)
{
	int ret;

	if (std) {
		ret = ad4692_reg_write(desc, AD4692_SEQUENCER_CONTROL_REG,
				       AD4692_STANDARD_SEQ_MODE);
		if (ret)
			return ret;

		return ad4692_reg_write(desc, AD4692_STD_SEQ_CONFIG_REG, ch_mask);
	} else {
		return ad4692_reg_write(desc, AD4692_SEQUENCER_CONTROL_REG,
					no_os_field_prep(AD4692_ADV_SEQ_MODE_MASK, len));
	}
}

int ad4692_sampling_enable(struct ad4692_desc *desc, bool enable)
{
	switch (desc->mode) {
	case AD4692_CNV_CLOCK:
	case AD4692_CNV_BURST:
		if (enable)
			return no_os_pwm_enable(desc->conv_desc);
		else
			return no_os_pwm_disable(desc->conv_desc);
	case AD4692_AUTONOMOUS:
	case AD4692_SPI_BURST:
		return ad4692_reg_write(desc, AD4692_OSC_EN_REG,
					(uint32_t)enable);
	default:
		return -EINVAL;
	}
}

int ad4692_get_ch(struct ad4692_desc *desc, uint8_t ch, uint32_t *val)
{
	uint32_t reg;
	int ret;

	if (ch > desc->num_channels)
		return -EINVAL;

	if (desc->mode == AD4692_MANUAL_MODE) {
		ret = ad4692_send_command(desc, AD4692_IN_COMMAND(ch), val);
		if (ret)
			return ret;

		ret = ad4692_send_command(desc, AD4692_NOOP, val);
		if (ret)
			return ret;

		*val = no_os_field_get(AD4692_MANUAL_DATA_MASK, *val);

		return 0;
	}

	ret = ad4692_std_seq_ch(desc, AD4692_CHANNEL_MASK(ch), true, 0);
	if (ret)
		return ret;

	if (ch > 7) {
		ret = ad4692_reg_write(desc, AD4692_ACC_MASK1_REG, 0xFF);
		if (ret)
			return ret;

		ret = ad4692_reg_write(desc, AD4692_ACC_MASK2_REG,
				       AD4692_LSB_MASK(~AD4692_CHANNEL_MASK(ch)));
		if (ret)
			return ret;
	} else {
		ret = ad4692_reg_write(desc, AD4692_ACC_MASK1_REG,
				       AD4692_LSB_MASK(~AD4692_CHANNEL_MASK(ch)));
		if (ret)
			return ret;

		ret = ad4692_reg_write(desc, AD4692_ACC_MASK2_REG, 0xFF);
		if (ret)
			return ret;
	}

	ret = ad4692_reg_write(desc, AD4692_ACC_COUNT_LIMIT_REG(ch), 0x1);
	if (ret)
		return ret;

	ret = ad4692_sampling_enable(desc, true);
	if (ret)
		return ret;

	no_os_udelay(AD4692_MAX_CONV_PERIOD_US);

	ret = ad4692_sampling_enable(desc, false);
	if (ret)
		return ret;

	reg = AD4692_AVG_IN_REG(ch);

	ret = ad4692_reg_read(desc, reg, val);
	if (ret)
		return ret;

	return ad4692_reg_write(desc, AD4692_STATE_RESET_REG,
				AD4692_STATE_RESET_ALL);
}

int ad4692_get_temp(struct ad4692_desc *desc, uint32_t *val)
{
	int ret;

	ret = ad4692_send_command(desc, AD4692_TEMPERATURE_SENSOR, val);
	if (ret)
		return ret;

	ret = ad4692_send_command(desc, AD4692_NOOP, val);
	if (ret)
		return ret;

	*val = no_os_field_get(AD4692_MANUAL_DATA_MASK, *val);

	return 0;
}

int ad4692_set_osc(struct ad4692_desc *desc, enum ad4692_int_osc_sel osc)
{
	return ad4692_reg_update(desc, AD4692_OSC_FREQ_REG,
				 AD4692_OSC_FREQ_MASK,
				 no_os_field_prep(AD4692_OSC_FREQ_MASK, osc));
}

int ad4692_get_osc(struct ad4692_desc *desc, enum ad4692_int_osc_sel *osc)
{
	uint32_t reg_val;
	int ret;

	ret = ad4692_reg_read(desc, AD4692_OSC_FREQ_REG, &reg_val);
	if (ret)
		return ret;

	*osc = no_os_field_get(AD4692_OSC_FREQ_MASK, reg_val);

	return 0;
}

int ad4692_set_ref(struct ad4692_desc *desc, enum ad4692_ref_ctrl_sel ref)
{
	return ad4692_reg_update(desc, AD4692_REF_CTRL_REG,
				 AD4692_REF_CTRL_MASK,
				 no_os_field_prep(AD4692_REF_CTRL_MASK,
						 ref));
}

int ad4692_get_ref(struct ad4692_desc *desc, enum ad4692_ref_ctrl_sel *ref)
{
	uint32_t reg_val;
	int ret;

	ret = ad4692_reg_read(desc, AD4692_REF_CTRL_REG, &reg_val);
	if (ret)
		return ret;

	*ref = no_os_field_get(AD4692_REF_CTRL_MASK, reg_val);

	return 0;
}

int ad4692_gpio_set(struct ad4692_desc *desc, enum ad4692_gpio_num index,
		    enum ad4692_gpio_sel mode)
{
	switch (index) {
	case AD4692_GPIO0:
		return ad4692_reg_update(desc, AD4692_GPIO_MODE1_REG,
					 AD4692_GPIO0_MASK,
					 no_os_field_prep(AD4692_GPIO0_MASK, mode));
	case AD4692_GPIO1:
		return ad4692_reg_update(desc, AD4692_GPIO_MODE1_REG,
					 AD4692_GPIO1_MASK,
					 no_os_field_prep(AD4692_GPIO1_MASK, mode));
	case AD4692_GPIO2:
		return ad4692_reg_update(desc, AD4692_GPIO_MODE2_REG,
					 AD4692_GPIO2_MASK,
					 no_os_field_prep(AD4692_GPIO2_MASK, mode));
	case AD4692_GPIO3:
		return ad4692_reg_update(desc, AD4692_GPIO_MODE2_REG,
					 AD4692_GPIO3_MASK,
					 no_os_field_prep(AD4692_GPIO3_MASK, mode));
	default:
		return -EINVAL;
	}
}

int ad4692_gpio_get_value(struct ad4692_desc *desc, enum ad4692_gpio_num index,
			  enum no_os_gpio_values *value)
{
	enum ad4692_gpio_sel mode;
	uint32_t reg_val;
	int ret;

	ret = ad4692_reg_read(desc,
			      index / 2 ? AD4692_GPIO_MODE2_REG : AD4692_GPIO_MODE1_REG,
			      &reg_val);
	if (ret)
		return ret;

	mode = no_os_field_get(index % 2 ? AD4692_GPIO1_MASK : AD4692_GPIO0_MASK,
			       reg_val);
	if (mode != AD4692_GPIO_INPUT)
		return -EINVAL;

	ret = ad4692_reg_read(desc, AD4692_GPIO_READ_REG, &reg_val);
	if (ret)
		return ret;

	*value = no_os_field_get(AD4692_GP_READ_MASK(index), reg_val);

	return 0;
}

int ad4692_hardware_reset(struct ad4692_desc *desc)
{
	uint32_t reg_val;
	int ret;

	ret = no_os_gpio_set_value(desc->reset_desc, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	no_os_udelay(1);

	ret = no_os_gpio_set_value(desc->reset_desc, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	no_os_udelay(300);

	/* Clearing STATUS Register. */
	ret = ad4692_reg_read(desc, AD4692_STATUS_REG, &reg_val);
	if (ret)
		return ret;

	return 0;
}

int ad4692_software_reset(struct ad4692_desc *desc)
{
	int ret;

	ret = ad4692_reg_update(desc, AD4692_INTERFACE_CONFIG_A_REG,
				AD4692_SW_RST_MASK, AD4692_SW_RST_MASK);
	if (ret)
		return ret;

	no_os_udelay(300);

	return 0;
}

int ad4692_cnv_burst_period_ns(struct ad4692_desc *desc, int n_active,
			       uint32_t *period_ns)
{
	uint32_t osc_idx = AD4692_OSC_1MHZ;
	uint64_t osc_freq, conv_time_ns, spi_bits, spi_time_ns;
	int ret;

	ret = ad4692_reg_read(desc, AD4692_OSC_FREQ_REG, &osc_idx);
	if (ret)
		return ret;

	osc_idx = no_os_field_get(AD4692_OSC_FREQ_MASK, osc_idx);
	if (osc_idx >= NO_OS_ARRAY_SIZE(ad4692_int_osc_val))
		osc_idx = AD4692_OSC_1MHZ;

	osc_freq = ad4692_int_osc_val[osc_idx];
	conv_time_ns = (uint64_t)(n_active + 1) * NANO / osc_freq;

	/*
	 * Non-offload sequence per trigger: n_active AVG_IN_REG reads
	 * (4 B: 2 cmd + 2 data each) + STATE_RESET (3 B).
	 */
	spi_bits = (uint64_t)n_active * 32 + 24;
	spi_time_ns = spi_bits * NANO / desc->comm_desc->max_speed_hz;

	/* 50% margin on SPI time absorbs OS scheduling jitter. */
	*period_ns = conv_time_ns + spi_time_ns * 3 / 2;

	return 0;
}

static int ad4692_pwm_init(struct ad4692_desc *desc,
			   struct ad4692_init_param *init_param)
{
	struct no_os_pwm_init_param *conv_param = init_param->conv_param;
	int ret;

	if (!conv_param)
		return -EINVAL;

	switch (init_param->mode) {
	case AD4692_CNV_CLOCK:
		switch (desc->id) {
		case ID_AD4692:
		case ID_AD4694:
			if (conv_param->period_ns < 1000)
				return -EINVAL;
			break;
		case ID_AD4691:
		case ID_AD4693:
			if (conv_param->period_ns < 2000)
				return -EINVAL;
			break;
		default:
			return -EINVAL;
		}
		break;
	case AD4692_CNV_BURST:
		ret = ad4692_cnv_burst_period_ns(desc, desc->num_channels,
						 &conv_param->period_ns);
		if (ret)
			return ret;
		break;
	default:
		return -EINVAL;
	}

	conv_param->duty_cycle_ns = AD4692_CNV_DUTY_CYCLE_NS;

	ret = no_os_pwm_init(&desc->conv_desc, conv_param);
	if (ret)
		return ret;

	ret = no_os_pwm_disable(desc->conv_desc);
	if (ret) {
		no_os_pwm_remove(desc->conv_desc);
		desc->conv_desc = NULL;
		return ret;
	}

	return 0;
}

int ad4692_init(struct ad4692_desc **desc, struct ad4692_init_param *init_param)
{
	struct ad4692_desc *descriptor;
	int ret;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	ret = no_os_spi_init(&descriptor->comm_desc, init_param->comm_param);
	if (ret)
		goto err;

	descriptor->max_rate = ad4692_max_rate[init_param->id];
	descriptor->num_channels = ad4692_num_channels[init_param->id];
	descriptor->vref = init_param->vref;
	descriptor->id = init_param->id;

	ret = no_os_gpio_get(&descriptor->reset_desc, init_param->reset_param);
	if (ret)
		goto spi_err;

	ret = no_os_gpio_direction_output(descriptor->reset_desc,
					  NO_OS_GPIO_HIGH);
	if (ret)
		goto reset_err;

	ret = ad4692_hardware_reset(descriptor);
	if (ret)
		goto reset_err;

	switch (init_param->mode) {
	case AD4692_CNV_CLOCK:
	case AD4692_CNV_BURST:
		ret = ad4692_pwm_init(descriptor, init_param);
		if (ret)
			goto reset_err;
		break;
	case AD4692_MANUAL_MODE:
	case AD4692_AUTONOMOUS:
	case AD4692_SPI_BURST:
		break;
	default:
		ret = -EINVAL;
		goto reset_err;
	}

	ret = ad4692_config(descriptor, init_param->mode);
	if (ret)
		goto pwm_err;

	ret = no_os_gpio_get(&descriptor->gpio0_desc, init_param->gpio0_param);
	if (ret)
		goto pwm_err;

	ret = no_os_gpio_direction_input(descriptor->gpio0_desc);
	if (ret)
		goto gpi0_err;

	ret = no_os_gpio_get_optional(&descriptor->gpio1_desc, init_param->gpio1_param);
	if (ret)
		goto gpi0_err;

	ret = no_os_gpio_get_optional(&descriptor->gpio2_desc, init_param->gpio2_param);
	if (ret)
		goto gpi1_err;

	ret = no_os_gpio_get_optional(&descriptor->gpio3_desc, init_param->gpio3_param);
	if (ret)
		goto gpi2_err;

	*desc = descriptor;

	return 0;

gpi2_err:
	no_os_gpio_remove(descriptor->gpio2_desc);
gpi1_err:
	no_os_gpio_remove(descriptor->gpio1_desc);
gpi0_err:
	no_os_gpio_remove(descriptor->gpio0_desc);
pwm_err:
	no_os_pwm_remove(descriptor->conv_desc);
reset_err:
	no_os_gpio_remove(descriptor->reset_desc);
spi_err:
	no_os_spi_remove(descriptor->comm_desc);
err:
	no_os_free(descriptor);

	return ret;
}

int ad4692_remove(struct ad4692_desc *desc)
{
	if (!desc)
		return -ENODEV;

	ad4692_reg_write(desc, AD4692_STD_SEQ_CONFIG_REG,
			 AD4692_SEQ_ALL_CHANNELS_OFF);
	no_os_gpio_remove(desc->gpio3_desc);
	no_os_gpio_remove(desc->gpio2_desc);
	no_os_gpio_remove(desc->gpio1_desc);
	no_os_gpio_remove(desc->gpio0_desc);
	no_os_pwm_remove(desc->conv_desc);
	no_os_gpio_remove(desc->reset_desc);
	no_os_spi_remove(desc->comm_desc);
	no_os_free(desc);

	return 0;
}
