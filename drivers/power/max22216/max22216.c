/***************************************************************************//**
 *   @file   max22216.c
 *   @brief  Current control
 *   @author Robert Budai (robert.budai@analog.com)
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

#include "max22216.h"

int max22216_write_reg(struct max22216_desc *desc, uint8_t reg_addr,
		       uint16_t data)
{
	int ret = 0;

	if (!desc || !desc->spi_desc)
		return -EINVAL;
	no_os_field_get(data, MP_OS_GEN_MASK())
	uint8_t tx[3] = {
		(uint8_t)(reg_addr | NO_OS_BIT(8)),
		(uint8_t)no_os_field_get(0xFF00, data),
		(uint8_t)no_os_field_get(0x00FF, data)
	};

	return no_os_spi_write_and_read(desc->spi_desc, tx, 3);
}

int max22216_read_reg(struct max22216_desc *desc, uint8_t reg_addr,
		      uint16_t *data)
{
	int ret;
	uint8_t tx[3] = { reg_addr, 0x00, 0x00};

	if (!desc || !desc->spi_desc)
		return -EINVAL;

	ret = no_os_spi_write_and_read(desc->spi_desc, tx, 3);
	if (ret)
		return ret;
	ret = no_os_spi_write_and_read(desc->spi_desc, tx, 3);
	if (ret)
		return ret;

	desc->status_reg = tx[0];

	*data = (tx[1] << 8) | tx[2];

	return 0;
}

int max22216_write_reg_list(struct max22216_desc *desc,
			    max22216_reg_setting_t* list, uint8_t elem_nr)
{
	int ret;
	uint8_t i;

	if (!desc || !desc->spi_desc || !list)
		return -1;

	for (i = 0; i < elem_nr; i++) {
		ret = max22216_write_reg(desc, list[i].reg_addr, list[i].data);
		if (ret)
			return ret;
	}
	return 0;
}

int erase_fault_reg(struct max22216_desc *desc)
{
	int ret;
	uint16_t data = 0;

	ret = max22216_write_reg(desc, MAX22216_FAULT1, 0xFFFF);
	if (ret)
		return ret;
	ret = max22216_read_reg(desc, MAX22216_FAULT0, &data);
	if (ret)
		return ret;
	return max22216_read_reg(desc, MAX22216_FAULT1, &data);
}

int max22216_set_enable_pin(struct max22216_desc *desc, bool value)
{
	int ret;

	if (!desc || !desc->drv_en_gpio)
		return -EINVAL;

	return no_os_gpio_set_value(desc->drv_en_gpio,
				    value ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);
}

int max22216_check_fault_pin(struct max22216_desc *desc, bool *fault_status)
{
	int ret;
	uint8_t value;

	if (!desc || !desc->fault_gpio || !fault_status)
		return -EINVAL;

	ret = no_os_gpio_get_value(desc->fault_gpio, &value);
	if (ret)
		return ret;

	*fault_status = (value == NO_OS_GPIO_LOW); // Assuming LOW indicates a fault
	return 0;
}

int max22216_current_reg_control(struct max22216_desc *desc, uint8_t channel_nr,
				 uint16_t value)
{
	int ret;
	uint16_t test_value;
	max22216_reg_setting_t reg_setting;
	// Prepare register settings for current control
	uint8_t reg_addr = MAX22216_CFG_DC_H_0 + (channel_nr *
			   MAX22216_CHANNEL_CONFIG_REG_SHIFT);

	if (!desc || !desc->spi_desc)
		return -EINVAL;


	if (channel_nr >= MAX22216_NR_OF_CHANNELS)
		return -EINVAL;

	// Write the register settings
	ret = max22216_write_reg(desc, reg_addr, value);
	if (ret)
		return ret;

	ret = max22216_read_reg(desc, reg_addr, &test_value);
	if (ret)
		return ret;

	if (test_value != value)
		return -EINVAL;

	return 0;
}

int max22216_turn_on(struct max22216_desc *desc, uint8_t channel_nr)
{
	int ret;
	uint16_t data;

	if (!desc || !desc->spi_desc)
		return -EINVAL;

	if (channel_nr >= MAX22216_NR_OF_CHANNELS)
		return -EINVAL;

	ret = max22216_read_reg(desc,
				MAX22216_CFG_IND_0_0 + (channel_nr * MAX22216_CHANNEL_CONFIG_REG_SHIFT), &data);
	if (ret)
		return ret;
	ret = max22216_write_reg(desc,
				 MAX22216_CFG_IND_0_0 + (channel_nr * MAX22216_CHANNEL_CONFIG_REG_SHIFT),
				 data | NO_OS_BIT(MAX22216_CFG_IND_DITHERING_BIT));
	if (ret)
		return ret;
	ret = max22216_read_reg(desc,
				MAX22216_CFG_IND_0_0 + (channel_nr * MAX22216_CHANNEL_CONFIG_REG_SHIFT), &data);
	if (ret)
		return ret;
	if (!(data | NO_OS_BIT(MAX22216_CFG_IND_DITHERING_BIT)))
		return -1;

	ret = max22216_read_reg(desc, MAX22216_GLOBAL_CTRL, &data);
	if (ret)
		return ret;
	ret = max22216_write_reg(desc, MAX22216_GLOBAL_CTRL,
				 data | NO_OS_BIT(channel_nr));
	if (ret)
		return ret;
	ret = max22216_read_reg(desc, MAX22216_GLOBAL_CTRL, &data);
	if (ret)
		return ret;
	if (!(data | NO_OS_BIT(channel_nr)))
		return -EINVAL;

	return 0;

}

int max22216_turn_off(struct max22216_desc *desc, uint8_t channel_nr)
{
	int ret;
	uint16_t data;

	if (!desc || !desc->spi_desc)
		return -EINVAL;
	if (channel_nr >= MAX22216_NR_OF_CHANNELS)
		return -EINVAL;
	ret = max22216_read_reg(desc, MAX22216_GLOBAL_CTRL, &data);
	if (ret)
		return ret;
	ret = max22216_write_reg(desc, MAX22216_GLOBAL_CTRL,
				 data & ~(NO_OS_BIT(channel_nr)));
	if (ret)
		return ret;
	ret = max22216_read_reg(desc, MAX22216_GLOBAL_CTRL, &data);
	if (ret)
		return ret;
	if (data & (NO_OS_BIT(channel_nr)))
		// Indicates that the relevant bits for the channel were not cleared as expected
		return -EINVAL;
	ret = max22216_read_reg(desc,
				MAX22216_CFG_IND_0_0 + (channel_nr * MAX22216_CHANNEL_CONFIG_REG_SHIFT), &data);
	if (ret)
		return ret;
	ret = max22216_write_reg(desc,
				 MAX22216_CFG_IND_0_0 + (channel_nr * MAX22216_CHANNEL_CONFIG_REG_SHIFT),
				 data & ~NO_OS_BIT(MAX22216_CFG_IND_DITHERING_BIT));
	if (ret)
		return ret;
	ret = max22216_read_reg(desc,
				MAX22216_CFG_IND_0_0 + (channel_nr * MAX22216_CHANNEL_CONFIG_REG_SHIFT), &data);
	if (ret)
		return ret;
	if (data & NO_OS_BIT(MAX22216_CFG_IND_DITHERING_BIT))
		return -EINVAL;

	return 0;
}

int max22216_set_current_ma(struct max22216_desc *desc, uint8_t channel_nr,
			    uint16_t current_ma)
{
	int ret;
	uint16_t reg_value = (uint16_t)(current_ma / (MAX22216_K_CDR * MAX22216_GAIN *
					MAX22216_SNSF));

	if (!desc)
		return -EINVAL;

	ret = max22216_current_reg_control(desc, channel_nr, reg_value);
	if (ret)
		return ret;

	return 0;
}

int max22216_init(struct max22216_desc **desc,
		  struct max22216_init_param *param)
{
	int ret;

	if (!desc || !param || !param->spi_ip)
		return -EINVAL;

	struct max22216_desc *dev = calloc(1, sizeof(*dev));

	if (!dev)
		return -ENOMEM;

	struct no_os_gpio_desc *max22216_drv_en_desc = calloc(1,
			sizeof(*max22216_drv_en_desc));
	if (!max22216_drv_en_desc)
		goto error1;
	struct no_os_gpio_desc *max22216_fault_desc = calloc(1,
			sizeof(*max22216_fault_desc));

	if (!max22216_fault_desc)
		goto error2;

	ret = no_os_spi_init(&dev->spi_desc, param->spi_ip);
	if (ret)
		goto error1;

	// Setup for enable pin
	ret = no_os_gpio_get(&max22216_drv_en_desc, param->drv_en_gpio_ip);
	if (ret)
		goto error1;

	dev->drv_en_gpio = max22216_drv_en_desc;
	ret = no_os_gpio_direction_output(max22216_drv_en_desc, NO_OS_GPIO_HIGH);
	if (ret)
		goto error1;

	ret = no_os_gpio_set_value(max22216_drv_en_desc, NO_OS_GPIO_HIGH);
	if (ret)
		goto error1;

	// Setup for fault pin polling
	ret = no_os_gpio_get(&max22216_fault_desc, param->fault_gpio_ip);
	if (ret)
		goto error1;

	dev->fault_gpio = max22216_fault_desc;

	ret = no_os_gpio_direction_input(max22216_fault_desc);
	if (ret)
		goto error1;

	dev->status_reg = 0;
	*desc = dev;
	return 0;

error1:
	free(max22216_fault_desc);
error2:
	free(max22216_drv_en_desc);
error3:
	free(dev);
	return ret;
}

int max22216_remove(struct max22216_desc *desc)
{
	if (!desc)
		return -1;
	if (desc->spi_desc)
		no_os_spi_remove(desc->spi_desc);
	if (desc->drv_en_gpio)
		no_os_gpio_remove(desc->drv_en_gpio);
	if (desc->fault_gpio)
		no_os_gpio_remove(desc->fault_gpio);
	free(desc);
	return 0;
}
