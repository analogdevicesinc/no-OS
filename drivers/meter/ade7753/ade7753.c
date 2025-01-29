/***************************************************************************//**
*   @file   ade7753.c
*   @brief  Implementation of ADE7753 Driver.
*   @author REtz (radu.etz@analog.com)
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

#include "ade7753.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "no_os_delay.h"
#include "no_os_units.h"
#include "no_os_alloc.h"
#include "no_os_crc16.h"
#include "no_os_print_log.h"
#include <stdlib.h>
#include <errno.h>
#include <math.h>

/**
 * @brief Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 * @return 0 in case of success, negative error code otherwise.
 */

int ade7753_init(struct ade7753_dev **device,
		 struct ade7753_init_param init_param)
{
	int ret;
	/* ADE7753 device structure */
	struct ade7753_dev *dev;
	/* value read from register */
	uint32_t reg_val;
	/* timeout value */
	uint16_t timeout = 1000;

	dev = (struct ade7753_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* SPI Initialization */
	ret = no_os_spi_init(&dev->spi_desc,
			     init_param.spi_init);
	if (ret)
		goto error_spi;

	/* interrupt */
	dev->irq_ctrl = init_param.irq_ctrl;
	/* reset gpio */
	dev->gpio_reset = init_param.gpio_reset;

	/* reset the device */
	if (dev->gpio_reset) {
		ret = ade7753_hw_reset(dev);
		if (ret)
			goto error_spi;
	} else {
		ret = ade7753_sw_reset(dev);
		if (ret)
			goto error_spi;
	}
	/* wait for reset ack */
	do {
		ret = ade7753_get_int_status(dev, ADE7753_RESET_MSK, &reg_val);
		if (ret)
			goto error_spi;
		timeout--;
	} while ((!reg_val) && (timeout > 0));
	if (timeout <= 0) {
		ret = -ETIMEDOUT;
		goto error_spi;
	}
	/* Read version product */
	ret = ade7753_version_product(dev, &reg_val);
	if (ret)
		goto error_spi;

	if (reg_val != ADE7753_VERSION) {
		pr_err("ERROR device not recognized!!!\n");
		goto error_spi;
	}

	*device = dev;

	return 0;

error_spi:
	no_os_spi_remove(dev->spi_desc);
error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Read device register.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data read from 32bit register.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7753_read(struct ade7753_dev *dev, uint16_t reg_addr,
		 int32_t *reg_data)
{
	int ret;
	/* number of bytes = 2 for 8 bits register data */
	uint8_t no_bytes = 2;
	/* offset of data read in the read buffer */
	uint8_t data_byte_offset = 1;
	/* buffer for data read */
	uint8_t buff[4] = { 0 };
	/* index */
	uint8_t i;
	/* register addres */
	uint32_t addr;

	if (!dev)
		return -ENODEV;
	if (!reg_data)
		return -EINVAL;

	/* set the read command */
	buff[0] = (uint8_t) no_os_field_prep(NO_OS_GENMASK(5, 0), reg_addr);

	/* 8/6 bits registers */
	if (IS_8BITS_REG(reg_addr) || IS_6BITS_REG(reg_addr)) {
		ret = no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes);
		if (ret)
			return ret;
		if (IS_6BITS_REG(reg_addr))
			*reg_data = no_os_sign_extend32(no_os_field_prep(NO_OS_GENMASK(5, 0),
							buff[data_byte_offset]), 5);
		else
			*reg_data = no_os_sign_extend32(buff[data_byte_offset], 7);
	} else if (IS_16BITS_REG(reg_addr) || IS_12BITS_REG(reg_addr)) {
		/* 16/12 bits registers */
		ret = no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes + 1);
		if (ret)
			return ret;
		if (IS_12BITS_REG(reg_addr))
			*reg_data = no_os_sign_extend32(no_os_field_prep(NO_OS_GENMASK(11, 0),
							no_os_get_unaligned_be16(&buff[data_byte_offset])), 11);
		else
			*reg_data = no_os_sign_extend32(no_os_get_unaligned_be16(
								&buff[data_byte_offset]), 15);
	} else {
		/* 24 bits registers */
		ret = no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes + 2);
		if (ret)
			return ret;
		*reg_data = no_os_sign_extend32(no_os_get_unaligned_be24(
							&buff[data_byte_offset]), 23);
	}

	return 0;
}

/**
 * @brief Write device register.
 * @param dev- The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7753_write(struct ade7753_dev *dev, uint16_t reg_addr,
		  uint32_t reg_data)
{
	int ret;
	/* number of bytes = 2 for 8 bits register data */
	uint8_t no_bytes = 2;
	/* buffer for data write */
	uint8_t buff[4] = {0};
	/* register addres */
	uint16_t addr;
	/* data to be written */
	uint32_t data;
	/* offset of data to be written in the buffer */
	uint8_t data_byte_offset = 1;
	int i;

	if (!dev)
		return -ENODEV;

	buff[0] = (uint8_t) no_os_field_prep(NO_OS_GENMASK(5, 0), reg_addr);
	/* set read bit */
	buff[0] |= ADE7753_SPI_READ;

	/* 8/6 bits registers */
	if (IS_8BITS_REG(reg_addr) || IS_6BITS_REG(reg_addr)) {
		if (IS_6BITS_REG(reg_addr))
			reg_data = no_os_field_prep(NO_OS_GENMASK(5, 0), reg_data);
		buff[1] = (uint8_t)reg_data;

		return no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes);
	} else if (IS_16BITS_REG(reg_addr) || IS_12BITS_REG(reg_addr)) {
		/* 16/12 bits registers */
		if (IS_12BITS_REG(reg_addr))
			reg_data = no_os_field_prep(NO_OS_GENMASK(11, 0), reg_data);
		no_os_put_unaligned_be16(reg_data, &buff[data_byte_offset]);

		return no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes + 1);
	} else {
		/* 24 bits registers */
		no_os_put_unaligned_be24(reg_data, &buff[data_byte_offset]);

		return no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes + 2);
	}
}

/**
 * @brief Update specific register bits.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Specific bits mask.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7753_update_bits(struct ade7753_dev *dev, uint16_t reg_addr,
			uint32_t mask, uint32_t reg_data)
{
	int ret;
	/* data to be written */
	uint32_t data;

	ret = ade7753_read(dev, reg_addr, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= reg_data & mask;

	return ade7753_write(dev, reg_addr, data);
}

/**
 * @brief Remove the device and release resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7753_remove(struct ade7753_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_reset);
	if (ret)
		return ret;

	ret = no_os_irq_ctrl_remove(dev->irq_ctrl);
	if (ret)
		return ret;

	ret = no_os_irq_ctrl_remove(dev->zx_ctrl);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}

/**
 * @brief Reset the device using SW reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7753_sw_reset(struct ade7753_dev *dev)
{
	int ret;

	ret = ade7753_update_bits(dev, ADE7753_REG_MODE, ADE7753_SWRST_MSK,
				  no_os_field_prep(ADE7753_SWRST_MSK, ENABLE));
	if (ret)
		return ret;

	/* Wait for device to initialize */
	no_os_mdelay(ADE7753_RESET_DEL);

	return 0;
}

/**
 * @brief Reset the device using HW reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7753_hw_reset(struct ade7753_dev *dev)
{
	int ret;

	if (!dev || !dev->gpio_reset)
		return -ENODEV;

	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	no_os_mdelay(1);

	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Version product
 * @param dev - The device structure.
 * @param data_read - The version product read value
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7753_version_product(struct ade7753_dev *dev, uint32_t *data_read)
{
	int ret;
	/* version product */
	uint32_t version;

	if (!data_read)
		return -EINVAL;

	ret = ade7753_read(dev, ADE7753_REG_VERSION_PRODUCT, &version);
	if (ret)
		return ret;

	*data_read = version;

	return 0;
}

/**
 * @brief read with reset Ipeak val
 * @param dev - The device structure.
 * @param val - Iapeak val
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7753_read_reset_iapk_val(struct ade7753_dev *dev, uint32_t *val)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!val)
		return -EINVAL;

	ret = ade7753_read(dev, ADE7753_REG_RSTIPEAK, &reg_val);
	if (ret)
		return ret;
	*val = reg_val;

	return 0;
}

/**
 * @brief read with reset Vpeak val
 * @param dev - The device structure.
 * @param val - Vpeak val
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7753_reset_vpk_val(struct ade7753_dev *dev, uint32_t *val)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!val)
		return -EINVAL;

	ret = ade7753_read(dev, ADE7753_REG_RSTVPEAK, &reg_val);
	if (ret)
		return ret;
	*val = reg_val;

	return 0;
}

/**
 * @brief Get interrupt indicator from STATUS register.
 * @param dev - The device structure.
 * @param msk - Interrupt mask.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7753_get_int_status(struct ade7753_dev *dev, uint32_t msk,
			   uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!status)
		return -EINVAL;

	ret = ade7753_read(dev, ADE7753_REG_STATUS, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(msk), &reg_val);

	return 0;
}


/**
 * @brief Clear irq status flags.
 * @param dev - The device structure.
 * @param reg_data - value of the status register
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7753_clear_irq_status(struct ade7753_dev *dev, int32_t *reg_data)
{
	int ret;
	uint32_t data;

	if (!reg_data)
		return -EINVAL;

	ret = ade7753_read(dev, ADE7753_REG_RSTSTATUS, &data);
	if (ret)
		return ret;

	*reg_data = data;

	return 0;
}

/**
 * @brief Enable/Disable interrupt.
 * @param dev - The device structure.
 * @param msk - Interrupt mask.
 * @param en -Enable/Disable
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7753_enable_irq(struct ade7753_dev *dev, uint32_t msk, uint8_t en)
{

	return ade7753_update_bits(dev, ADE7753_REG_IRQEN, msk,
				   no_os_field_prep(msk, en));

}

/**
 * @brief Select waveform register update rate.
 * @param dev - The device structure.
 * @param sel - Update rate
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7753_wave_update_rate(struct ade7753_dev *dev,
			     enum ade7753_dtrt_e sel)
{
	int ret;

	switch (sel) {
	case ADE7753_UPDATE_3_5KSPS:
		ret = ade7753_update_bits(dev, ADE7753_REG_MODE, ADE7753_DTRT_MSK,
					  no_os_field_prep(ADE7753_DTRT_MSK, ADE7753_UPDATE_3_5KSPS));
		break;
	case ADE7753_UPDATE_7KSPS:
		ret = ade7753_update_bits(dev, ADE7753_REG_MODE, ADE7753_DTRT_MSK,
					  no_os_field_prep(ADE7753_DTRT_MSK, ADE7753_UPDATE_7KSPS));
		break;
	case ADE7753_UPDATE_14KSPS:
		ret = ade7753_update_bits(dev, ADE7753_REG_MODE, ADE7753_DTRT_MSK,
					  no_os_field_prep(ADE7753_DTRT_MSK, ADE7753_UPDATE_14KSPS));
		break;
	default:
		ret = ade7753_update_bits(dev, ADE7753_REG_MODE, ADE7753_DTRT_MSK,
					  no_os_field_prep(ADE7753_DTRT_MSK, ADE7753_UPDATE_27_9KSPS));
		break;
	}

	return ret;
}

/**
 * @brief Select source of sampled data for wave register.
 * @param dev - The device structure.
 * @param sel - Update rate
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7753_wave_sample_data_source(struct ade7753_dev *dev,
				    enum ade7753_wavsel_e sel)
{
	int ret;

	switch (sel) {
	case ADE7753_24BITS_CH2:
		ret = ade7753_update_bits(dev, ADE7753_REG_MODE, ADE7753_WAVSEL_MSK,
					  no_os_field_prep(ADE7753_WAVSEL_MSK, ADE7753_24BITS_CH2));
		break;
	case ADE7753_24BITS_CH1:
		ret = ade7753_update_bits(dev, ADE7753_REG_MODE, ADE7753_WAVSEL_MSK,
					  no_os_field_prep(ADE7753_WAVSEL_MSK, ADE7753_24BITS_CH1));
		break;
	default:
		ret = ade7753_update_bits(dev, ADE7753_REG_MODE, ADE7753_WAVSEL_MSK,
					  no_os_field_prep(ADE7753_WAVSEL_MSK, ADE7753_24BITS_ACTIVE_POWER));
		break;
	}

	return ret;
}

/**
 * @brief Read energy values
 * @param dev - The device structure.
 * @param data - Structure to store energy values
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7753_energy_vals(struct ade7753_dev *dev,
			struct ade7753_energy_values *data)
{
	int ret;
	/* temporary value read from register */
	uint32_t temp_val;

	if (!data)
		return -EINVAL;

	/* read accumulated active energy */
	ret = ade7753_read(dev, ADE7753_REG_AENERGY, &temp_val);
	if (ret)
		return ret;
	data->active_energy_reg_val = (int32_t)temp_val;

	/* read accumulated apparent energy MSBs */
	ret = ade7753_read(dev, ADE7753_REG_VAENERGY, &temp_val);
	if (ret)
		return ret;
	data->apparent_energy_reg_val = (int32_t)temp_val;

	return 0;
}

/**
 * @brief Read rms values
 * @param dev - The device structure.
 * @param data - Structure to store rms values
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7753_rms_vals(struct ade7753_dev *dev,
		     struct ade7753_rms_values *data)
{
	int ret;
	/* temporary value read from register */
	uint32_t temp_val;

	if (!data)
		return -EINVAL;

	/* read voltage rms value */
	ret = ade7753_read(dev, ADE7753_REG_VRMS, &temp_val);
	if (ret)
		return ret;
	data->voltage_rms_reg_val = temp_val;

	/* read current rms value */
	ret = ade7753_read(dev, ADE7753_REG_IRMS, &temp_val);
	if (ret)
		return ret;
	data->current_rms_reg_val = temp_val;

	return 0;
}

/**
 * @brief Read period value
 * @param dev - The device structure.
 * @param data - Structure to store the period val
 * @return 0 in case of success, negative error code otherwise.
 */
int ade7753_period_val(struct ade7753_dev *dev,
		       struct ade7753_period_value *data)
{
	int ret;
	/* temporary value read from register */
	int32_t temp_val;

	if (!data)
		return -EINVAL;

	/* read periode value */
	ret = ade7753_read(dev, ADE7753_REG_PERIOD, &temp_val);
	if (ret)
		return ret;
	data->period_reg_val = (uint32_t)temp_val;

	return 0;
}
