/***************************************************************************//**
 *   @file   ad4692.c
 *   @brief  Implementation of ad4692 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ad4692.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_units.h"

static int ad4692_resolution[] = {
	[ID_AD4692] = 16,
	[ID_AD4691] = 18,
	[ID_AD4694] = 14,
	[ID_AD4693] = 20,
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

/* NOTE: Add MACROS for hard-coded values. */
static int ad4692_config(struct ad4692_desc *desc, enum ad4692_spi_mode mode)
{
	uint32_t reg_val;
	int ret;

	ret = ad4692_reg_update(desc, AD4692_INTERFACE_CONFIG_A_REG,
				AD4692_ADDR_ASCENSION,
				no_os_field_prep(AD4692_ADDR_ASCENSION, 1));
	if (ret)
		return ret;

	ret = ad4692_reg_read(desc, AD4692_STATUS_REG, &reg_val);
	if (ret)
		return ret;

	ret = ad4692_reg_write(desc, AD4692_STATE_RESET_REG,
			       AD4692_STATE_RESET_ALL);
	if (ret)
		return ret;

	switch (desc->vref) {
	case AD4692_VREF_MIN ... 2750000:
		ret = ad4692_reg_write(desc, AD4692_REFERENCE_CONTROL_REG,
				       0x00);
		break;
	case 2750001 ... 3250000:
		ret = ad4692_reg_write(desc, AD4692_REFERENCE_CONTROL_REG,
				       0x04);
		break;
	case 3250001 ... 3750000:
		ret = ad4692_reg_write(desc, AD4692_REFERENCE_CONTROL_REG,
				       0x08);
		break;
	case 3750001 ... 4500000:
		ret = ad4692_reg_write(desc, AD4692_REFERENCE_CONTROL_REG,
				       0x0C);
		break;
	case 4500001 ... AD4692_VREF_MAX:
		ret = ad4692_reg_write(desc, AD4692_REFERENCE_CONTROL_REG,
				       0x10);
		break;
	default:
		return -EINVAL;
	}
	if (ret)
		return ret;

	switch (desc->mode) {
	case AD4692_CNV_CLOCK:
	case AD4692_CNV_BURST:
	case AD4692_AUTONOMOUS:
	case AD4692_SPI_BURST:
		ret = ad4692_reg_write(desc, AD4692_ADC_OPERATION_REG, desc->mode);
		if (ret)
			return ret;

		return ad4692_reg_update(desc, AD4692_GPIO_MODE1_REG,
					 AD4692_GPIO0_MASK,
					 AD4692_GPIO_OUTPUT_DATA_READYb);
	case AD4692_MANUAL_MODE:
		ret = ad4692_reg_write(desc, AD4692_ADC_SETUP_REG, 0x14);
		if (ret)
			return ret;

		desc->manual = true;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/* Used by Manual Mode. */
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

	if (!desc->manual)
		return -EOPNOTSUPP;

	if (command > AD4692_MAX_COMMAND_VAL)
		return -EINVAL;

	memset(desc->buff, 0, xfer.bytes_number);

	desc->buff[0] = command;
	ret = no_os_spi_transfer(desc->comm_desc, &xfer, 1);
	if (ret)
		return ret;

	*val = no_os_get_unaligned_be24(&desc->buff[0]);

	return 0;
}

/* Add SPI Engine support. */
int ad4692_reg_read(struct ad4692_desc *desc, uint32_t reg, uint32_t *val)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.rx_buff = desc->buff,
		.cs_change = 1,
	};
	int ret;

	if (desc->manual)
		return -EOPNOTSUPP;

	switch (reg) {
	case 0 ... AD4692_INTERNAL_OSCILLATOR_REG:
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
	case AD4692_STD_SEQ_CONFIG:
	case AD4692_ACC16B(0) ... AD4692_ACC16B(15):
		xfer.bytes_number = 4;
		memset(desc->buff, 0, xfer.bytes_number);
		desc->buff[0] = AD4692_RW_ADDR_MASK | AD4692_MSB_MASK(reg);
		desc->buff[1] = AD4692_LSB_MASK(reg);
		ret = no_os_spi_transfer(desc->comm_desc, &xfer, 1);
		if (ret)
			return ret;

		*val = no_os_get_unaligned_le16(&desc->buff[2]);
		break;
	case AD4692_ACC16B_STATUS(0) ... AD4692_ACC24B(15):
		xfer.bytes_number = 5;
		memset(desc->buff, 0, xfer.bytes_number);
		desc->buff[0] = AD4692_RW_ADDR_MASK | AD4692_MSB_MASK(reg);
		desc->buff[1] = AD4692_LSB_MASK(reg);
		ret = no_os_spi_transfer(desc->comm_desc, &xfer, 1);
		if (ret)
			return ret;

		*val = no_os_get_unaligned_le24(&desc->buff[2]);
		break;
	case AD4692_ACC24B_STATUS(0) ... AD4692_ACC24B_STATUS(15):
		xfer.bytes_number = 6;
		memset(desc->buff, 0, xfer.bytes_number);
		desc->buff[0] = AD4692_RW_ADDR_MASK | AD4692_MSB_MASK(reg);
		desc->buff[1] = AD4692_LSB_MASK(reg);
		ret = no_os_spi_transfer(desc->comm_desc, &xfer, 1);
		if (ret)
			return ret;

		*val = no_os_get_unaligned_le32(&desc->buff[2]);
		break;
	default: /* Test Register Access. */
		memset(desc->buff, 0, xfer.bytes_number);
		desc->buff[0] = AD4692_RW_ADDR_MASK | AD4692_MSB_MASK(reg);
		desc->buff[1] = AD4692_LSB_MASK(reg);
		ret = no_os_spi_transfer(desc->comm_desc, &xfer, 1);
		if (ret)
			return ret;

		*val = desc->buff[2];
		break;
	}

	return 0;
}

/* Add SPI Engine support. */
int ad4692_reg_write(struct ad4692_desc *desc, uint32_t reg, uint32_t val)
{
	struct no_os_spi_msg xfer = {
		.tx_buff = desc->buff,
		.cs_change = 1,
	};

	desc->buff[0] = AD4692_MSB_MASK(reg);
	desc->buff[1] = AD4692_LSB_MASK(reg);

	if (desc->manual)
		return -EOPNOTSUPP;

	switch (reg) {
	case 0 ... AD4692_INTERNAL_OSCILLATOR_REG:
	case AD4692_SPARE_CONTROL_REG ... AD4692_GPIO_MODE2_REG:
		if (val > 0xFF)
			return -EINVAL;
		xfer.bytes_number = 3;
		desc->buff[2] = val;
		break;
	case AD4692_STD_SEQ_CONFIG:
		if (val > 0xFFFF)
			return -EINVAL;
		xfer.bytes_number = 4;
		no_os_put_unaligned_le16(val, &desc->buff[2]);
		break;
	default:
		if (val > 0xFF)
			return -EINVAL;
		xfer.bytes_number = 3;
		desc->buff[2] = val;
		break;
	}

	return no_os_spi_transfer(desc->comm_desc, &xfer, 1);
}

int ad4692_reg_update(struct ad4692_desc *desc, uint32_t reg, uint32_t mask,
		      uint32_t val)
{
	int ret;
	uint32_t reg_val = 0;

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

		return ad4692_reg_write(desc, AD4692_STD_SEQ_CONFIG, ch_mask);
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
		return ad4692_reg_write(desc, AD4692_CONV_START_REG, enable);
	case AD4692_MANUAL_MODE:
		return no_os_gpio_set_value(desc->conv_gpio,
					    enable ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);
	default:
		return -EINVAL;
	}
}

int ad4692_get_ch(struct ad4692_desc *desc, uint8_t ch_index, uint32_t *val)
{
	int ret;

	if (ch_index > desc->num_channels)
		return -EINVAL;

	switch (desc->mode) {
	case AD4692_CNV_CLOCK:
	case AD4692_CNV_BURST:
	case AD4692_AUTONOMOUS:
	case AD4692_SPI_BURST:
		return ad4692_reg_read(desc,
				       desc->resolution > 16 ? AD4692_ACC24B(ch_index) : AD4692_ACC16B(ch_index), val);
	case AD4692_MANUAL_MODE:
		ret = ad4692_send_command(desc, AD4692_IN_COMMAND(ch_index), val);
		if (ret)
			return ret;

		ret = ad4692_sampling_enable(desc, true);
		if (ret)
			return ret;

		ret = ad4692_sampling_enable(desc, false);
		if (ret)
			return ret;

		ret = ad4692_send_command(desc, AD4692_NOOP, val);
		if (ret)
			return ret;

		ret = ad4692_sampling_enable(desc, true);
		if (ret)
			return ret;

		ret = ad4692_sampling_enable(desc, false);
		if (ret)
			return ret;

		*val = *val >> 8;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/* TBD if kept.
int ad4692_get_temp(struct ad4692_desc *desc, uint32_t *val)
{
	int ret;

	if (!desc->manual)
		return -EOPNOTSUPP;

	ret = ad4692_send_command(desc, AD4692_TEMPERATURE_SENSOR, val);
	if (ret)
		return ret;

	return ad4692_send_command(desc, AD4692_NOOP, val);
}
*/

int ad4692_set_osc(struct ad4692_desc *desc, enum ad4692_int_osc_sel osc)
{
	return ad4692_reg_update(desc, AD4692_INTERNAL_OSCILLATOR_REG,
				 AD4692_INTERNAL_OSCILLATOR_MASK,
				 no_os_field_prep(AD4692_INTERNAL_OSCILLATOR_MASK, osc));
}

int ad4692_get_osc(struct ad4692_desc *desc, enum ad4692_int_osc_sel *osc)
{
	uint32_t reg_val;
	int ret;

	ret = ad4692_reg_read(desc, AD4692_INTERNAL_OSCILLATOR_REG, &reg_val);
	if (ret)
		return ret;

	*osc = no_os_field_get(AD4692_INTERNAL_OSCILLATOR_MASK, reg_val);

	return 0;
}

int ad4692_set_ref(struct ad4692_desc *desc, enum ad4692_ref_ctrl_sel ref)
{
	return ad4692_reg_update(desc, AD4692_REFERENCE_CONTROL_REG,
				 AD4692_REFERENCE_CONTROL_MASK,
				 no_os_field_prep(AD4692_REFERENCE_CONTROL_MASK, ref));
}

int ad4692_get_ref(struct ad4692_desc *desc, enum ad4692_ref_ctrl_sel *ref)
{
	uint32_t reg_val;
	int ret;

	ret = ad4692_reg_read(desc, AD4692_REFERENCE_CONTROL_REG, &reg_val);
	if (ret)
		return ret;

	*ref = no_os_field_get(AD4692_REFERENCE_CONTROL_MASK, reg_val);

	return 0;
}

int ad4692_gpio_set(struct ad4692_desc *desc, enum ad4692_gpio_num index,
		    enum ad4692_gpio_sel mode)
{
	switch (index) {
	case AD4692_GPIO0:
		return ad4692_reg_update(desc, AD4692_GPIO_MODE1_REG, AD4692_GPIO0_MASK, mode);
	case AD4692_GPIO1:
		return ad4692_reg_update(desc, AD4692_GPIO_MODE1_REG, AD4692_GPIO1_MASK, mode);
	case AD4692_GPIO2:
		return ad4692_reg_update(desc, AD4692_GPIO_MODE2_REG, AD4692_GPIO2_MASK, mode);
	case AD4692_GPIO3:
		return ad4692_reg_update(desc, AD4692_GPIO_MODE2_REG, AD4692_GPIO3_MASK, mode);
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
			      index / 2 ? AD4692_GPIO_MODE2_REG : AD4692_GPIO_MODE1_REG, &reg_val);
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

	descriptor->mode = init_param->mode;
	descriptor->max_rate = ad4692_max_rate[init_param->id];
	descriptor->num_channels = ad4692_num_channels[init_param->id];
	descriptor->resolution = ad4692_resolution[init_param->id];
	descriptor->vref = init_param->vref;
	descriptor->manual = false;

	ret = no_os_gpio_get(&descriptor->reset_desc, init_param->reset_param);
	if (ret)
		goto spi_err;

	ret = no_os_gpio_direction_output(descriptor->reset_desc, NO_OS_GPIO_HIGH);
	if (ret)
		goto reset_err;

	no_os_udelay(3200);

	switch (descriptor->mode) {
	case AD4692_CNV_CLOCK:
		if (init_param->id == ID_AD4692 || init_param->id == ID_AD4694) {
			if (init_param->conv_param->period_ns < 1000
			    || init_param->conv_param->period_ns > 1000000)
				return -EINVAL;
		} else {
			if (init_param->conv_param->period_ns < 2000
			    || init_param->conv_param->period_ns > 1000000)
				return -EINVAL;
		}

		init_param->conv_param->duty_cycle_ns = AD4692_CNV_CLOCK_DUTY_CYCLE;

		ret = no_os_pwm_init(&descriptor->conv_desc,
				     init_param->conv_param);
		if (ret)
			goto reset_err;

		no_os_pwm_disable(descriptor->conv_desc);

		descriptor->conv_gpio = NULL;
		break;
	case AD4692_MANUAL_MODE:
		ret = no_os_gpio_get(&descriptor->conv_gpio, init_param->conv_gpio_param);
		if (ret)
			goto reset_err;

		ret = no_os_gpio_direction_output(descriptor->conv_gpio, NO_OS_GPIO_LOW);
		if (ret)
			goto conv_err;

		descriptor->conv_desc = NULL;
		break;
	case AD4692_CNV_BURST:
		/* In case of CNV_BURST the sampling frequency should be a low
		 * one since in fact we need only on pulse of the PWM.
		 */
		init_param->conv_param->period_ns = 16 * NO_OS_DIV_ROUND_CLOSEST_ULL(NANO,
						    AD4692_DEFAULT_OSC_FREQ);
		init_param->conv_param->duty_cycle_ns = AD4692_CNV_CLOCK_DUTY_CYCLE;

		ret = no_os_pwm_init(&descriptor->conv_desc,
				     init_param->conv_param);
		if (ret)
			goto reset_err;

		no_os_pwm_disable(descriptor->conv_desc);

		descriptor->conv_gpio = NULL;

		break;
	case AD4692_AUTONOMOUS:
	case AD4692_SPI_BURST:
		descriptor->conv_desc = NULL;
		descriptor->conv_gpio = NULL;
		break;
	default:
		return -EINVAL;
	}
	ret = ad4692_config(descriptor, descriptor->mode);
	if (ret)
		goto pwm_err;

	switch (descriptor->mode) {
	case AD4692_MANUAL_MODE:
		break;
	case AD4692_CNV_CLOCK:
	case AD4692_CNV_BURST:
	case AD4692_AUTONOMOUS:
	case AD4692_SPI_BURST:
		ret = no_os_gpio_get(&descriptor->gpio0_desc, init_param->gpio0_param);
		if (ret)
			goto pwm_err;

		ret = no_os_gpio_direction_input(descriptor->gpio0_desc);
		if (ret)
			goto gpi0_err;

		ret = ad4692_gpio_set(descriptor, 0, AD4692_GPIO_OUTPUT_DATA_READYb);
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

		break;
	default:
		return -EINVAL;
	}

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
conv_err:
	no_os_gpio_remove(descriptor->conv_gpio);
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
	int ret;

	if (!desc)
		return -ENODEV;

	ret = ad4692_reg_write(desc, AD4692_STD_SEQ_CONFIG,
			       AD4692_SEQ_ALL_CHANNELS_OFF);
	if (ret)
		return ret;

	no_os_pwm_remove(desc->conv_desc);
	no_os_gpio_remove(desc->conv_gpio);
	no_os_gpio_remove(desc->gpio3_desc);
	no_os_gpio_remove(desc->gpio2_desc);
	no_os_gpio_remove(desc->gpio1_desc);
	no_os_gpio_remove(desc->gpio0_desc);
	no_os_gpio_remove(desc->reset_desc);
	no_os_spi_remove(desc->comm_desc);
	no_os_free(desc);

	return 0;
}