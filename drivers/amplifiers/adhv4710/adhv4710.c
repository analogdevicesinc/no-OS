/***************************************************************************//**
*   @file   adhv4710.c
*   @brief  Implementation of ADHV4710 Driver.
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

#include "adhv4710.h"
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

int adhv4710_init(struct adhv4710_dev **device,
		  struct adhv4710_init_param init_param)
{
	int ret;
	/* ADHV4710 device structure */
	struct adhv4710_dev *dev;
	/* value read from register */
	uint8_t reg_val;

	dev = (struct adhv4710_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	/* SPI Initialization */
	ret = no_os_spi_init(&dev->spi_desc,
			     init_param.spi_init);
	if (ret)
		goto error_dev;

	/* reset gpio */
	dev->gpio_reset = init_param.gpio_reset;

	/* Read version product */
	ret = adhv4710_version_product(dev, &reg_val);
	if (ret)
		goto error_spi;

	if (reg_val != ADHV4710_VERSION_PRODUCT) {
		ret = -ENXIO;
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
int adhv4710_read(struct adhv4710_dev *dev, uint8_t reg_addr,
		  int8_t *reg_data)
{
	int ret;
	/* number of bytes */
	uint8_t no_bytes = 2;
	/* offset of data read in the read buffer */
	uint8_t data_byte_offset = 1;
	/* buffer for data read */
	uint8_t buff[2] = { 0 };
	/* register addres */
	uint32_t addr;

	if (!dev)
		return -ENODEV;
	if (!reg_data)
		return -EINVAL;

	buff[0] = reg_addr;
	buff[0] |= ADHV4710_SPI_READ;

	ret = no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes);
	if (ret)
		return ret;

	*reg_data = buff[data_byte_offset];

	return 0;
}

/**
 * @brief Write device register.
 * @param dev- The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int adhv4710_write(struct adhv4710_dev *dev, uint8_t reg_addr,
		   uint8_t reg_data)
{
	int ret;
	/* number of bytes */
	uint8_t no_bytes = 2;
	/* buffer for data write */
	uint8_t buff[2] = {0};
	/* register addres */
	uint16_t addr;
	/* offset of data to be written in the buffer */
	uint8_t data_byte_offset = 1;

	if (!dev)
		return -ENODEV;

	buff[0] = reg_addr;

	buff[1] = reg_data;

	return no_os_spi_write_and_read(dev->spi_desc, buff, no_bytes);
}

/**
 * @brief Update specific register bits.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - Specific bits mask.
 * @param reg_data - The data to be written.
 * @return 0 in case of success, negative error code otherwise.
 */
int adhv4710_update_bits(struct adhv4710_dev *dev, uint8_t reg_addr,
			 uint8_t mask, uint8_t reg_data)
{
	int ret;
	/* data to be written */
	uint8_t data;

	ret = adhv4710_read(dev, reg_addr, &data);
	if (ret)
		return ret;

	data &= ~mask;
	data |= reg_data & mask;

	return adhv4710_write(dev, reg_addr, data);
}

/**
 * @brief Get indicator of settings.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param msk - Bit that needs to be tested.
 * @param status - Status indicator.
 * @return 0 in case of success, negative error code otherwise.
 */
int adhv4710_get_status(struct adhv4710_dev *dev, uint8_t reg_addr, uint8_t msk,
			uint8_t *status)
{
	int ret;
	/* register value read */
	uint32_t reg_val;

	if (!status)
		return -EINVAL;

	ret = ade7953_read(dev, reg_addr, &reg_val);
	if (ret)
		return ret;

	*status = no_os_test_bit(no_os_find_first_set_bit(msk), &reg_val);

	return 0;
}

/**
 * @brief Remove the device and release resources.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int adhv4710_remove(struct adhv4710_dev *dev)
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

	no_os_free(dev);

	return 0;
}

/**
 * @brief Reset the device using SW reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int adhv4710_sw_reset(struct adhv4710_dev *dev)
{
	return adhv4710_update_bits(dev, ADHV4710_REG_CTRL_REG_03,
				    ADHV4710_SOFT_RESET_MSK,
				    no_os_field_prep(ADHV4710_SOFT_RESET_MSK, 1));
}

/**
 * @brief Reset the device using HW reset.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int adhv4710_hw_reset(struct adhv4710_dev *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	if (dev->gpio_reset) {
		ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_LOW);
		if (ret)
			return ret;
	}
	/* delay for toggeling the reset pin (minimum 10ns see datasheet) */
	no_os_mdelay(1);

	if (dev->gpio_reset) {
		ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_HIGH);
		if (ret)
			return ret;
	}

	return 0;
}

/**
 * @brief Clear shutdown latch and re-enable amplifier from shutdown.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int adhv4710_clear_shutdown_latch(struct adhv4710_dev *dev)
{
	int ret;

	ret = adhv4710_update_bits(dev, ADHV4710_REG_CTRL_REG_00, ADHV4710_HV_RESET_MSK,
				   no_os_field_prep(ADHV4710_HV_RESET_MSK, 1));
	if (ret)
		return ret;

	return adhv4710_update_bits(dev, ADHV4710_REG_CTRL_REG_00,
				    ADHV4710_HV_RESET_MSK,
				    no_os_field_prep(ADHV4710_HV_RESET_MSK, 0));
}

/**
 * @brief Shutdown the amplifier.
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int adhv4710_shutdown_amplifier(struct adhv4710_dev *dev)
{
	return adhv4710_update_bits(dev, ADHV4710_REG_CTRL_REG_00,
				    ADHV4710_HV_SLEEP_MSK,
				    no_os_field_prep(ADHV4710_HV_SLEEP_MSK, 0));
}

/**
 * @brief Program quiescent current of amplifier
 * @param dev - The device structure.
 * @param value - The quiescent current value to be set (0x0 for nominal supply current).
 * @param direction - The direction of the quiescent current (0 for reduction, 1 for increase).
 * @return 0 in case of success, negative error code otherwise.
 */
int adhv4710_set_quiescent_current(struct adhv4710_dev *dev, uint8_t value,
				   uint8_t direction)
{
	if (direction == 0)
		value &= ~ADHV4710_INC_DEC_MSK;
	else
		value |= ADHV4710_INC_DEC_MSK;

	return adhv4710_write(dev, ADHV4710_REG_CTRL_REG_04, value);
}

/**
 * @brief Enable source overcurrent protection.
 * @param dev - The device structure.
 */
int adhv4710_enable_source_overcurrent_protection(struct adhv4710_dev *dev)
{
	/* Enable overcurrent protection */
	return adhv4710_update_bits(dev, ADHV4710_REG_CTRL_REG_08,
				    ADHV4710_OC_SRC_ARM_MSK,
				    no_os_field_prep(ADHV4710_OC_SRC_ARM_MSK, 1));
}

/**
 * @brief Set level of overcurrent source protection.
 * @param dev - The device structure.
 * @param level - The level of overcurrent source protection (level = round(threshold[mA] / 15.625[mA/LSB])).
 */
int adhv4710_set_source_overcurrent_level(struct adhv4710_dev *dev,
		uint8_t level)
{
	/* Set overcurrent level */
	return adhv4710_update_bits(dev, ADHV4710_REG_CTRL_REG_08,
				    ADEHV4710_OC_SRC_LIMIT_MSK,
				    no_os_field_prep(ADEHV4710_OC_SRC_LIMIT_MSK, level));
}

/**
 * @brief Enable sink overcurrent protection.
 * @param dev - The device structure.
 */
int adhv4710_enable_sink_overcurrent_protection(struct adhv4710_dev *dev)
{
	/* Enable overcurrent protection */
	return adhv4710_update_bits(dev, ADHV4710_REG_CTRL_REG_09,
				    ADHV4710_OC_SNK_ARM_MSK,
				    no_os_field_prep(ADHV4710_OC_SNK_ARM_MSK, 1));
}

/**
 * @brief Set level of overcurrent sink protection.
 * @param dev - The device structure.
 * @param level - The level of overcurrent source protection (level = round(threshold[mA] / 15.625[mA/LSB])).
 */
int adhv4710_set_sink_overcurrent_level(struct adhv4710_dev *dev, uint8_t level)
{
	/* Set overcurrent level */
	return adhv4710_update_bits(dev, ADHV4710_REG_CTRL_REG_09,
				    ADEHV4710_OC_SNK_LIMIT_MSK,
				    no_os_field_prep(ADEHV4710_OC_SNK_LIMIT_MSK, level));
}

/**
 * @brief Enable overvoltage positive protection.
 * @param dev - The device structure.
 */
int adhv4710_enable_overvoltage_pos_protection(struct adhv4710_dev *dev)
{
	/* Enable overvoltage postivie protection */
	return adhv4710_update_bits(dev, ADHV4710_REG_CTRL_REG_10,
				    ADHV4710_OV_POS_ARM_MSK,
				    no_os_field_prep(ADHV4710_OV_POS_ARM_MSK, 1));
}

/**
 * @brief Set level of overvoltage positive protection.
 * @param dev - The device structure.
 * @param level - The level of overvoltage positive protection (level = round(threshold[V] / 1.953[V/LSB])).
 */
int adhv4710_set_overvoltage_pos_level(struct adhv4710_dev *dev, uint8_t level)
{
	/* Set overvoltage positive level */
	return adhv4710_update_bits(dev, ADHV4710_REG_CTRL_REG_10,
				    ADEHV4710_OV_POS_LIMIT_MSK,
				    no_os_field_prep(ADEHV4710_OV_POS_LIMIT_MSK, level));
}

/**
 * @brief Enable overvoltage negative protection.
 * @param dev - The device structure.
 */
int adhv4710_enable_overvoltage_neg_protection(struct adhv4710_dev *dev)
{
	/* Enable overvoltage negative protection */
	return  adhv4710_update_bits(dev, ADHV4710_REG_CTRL_REG_11,
				     ADHV4710_OV_NEG_ARM_MSK,
				     no_os_field_prep(ADHV4710_OV_NEG_ARM_MSK, 1));
}

/**
 * @brief Set level of overvoltage negative protection.
 * @param dev - The device structure.
 * @param level - The level of overvoltage negative protection (level = round(threshold[V] / 1.953[V/LSB])).
 */
int adhv4710_set_overvoltage_neg_level(struct adhv4710_dev *dev, uint8_t level)
{
	/* Set overvoltage negative level */
	return adhv4710_update_bits(dev, ADHV4710_REG_CTRL_REG_11,
				    ADEHV4710_OV_NEG_LIMIT_MSK,
				    no_os_field_prep(ADEHV4710_OV_NEG_LIMIT_MSK, level));
}

/**
 * @brief Enable overtemperature protection.
 * @param dev - The device structure.
 */
int adhv4710_enable_overtemperature_protection(struct adhv4710_dev *dev)
{
	/* Enable overtemperature protection */
	return adhv4710_update_bits(dev, ADHV4710_REG_CTRL_REG_12, ADHV4710_OT_ARM_MSK,
				    no_os_field_prep(ADHV4710_OT_ARM_MSK, 1));
}

/**
 * @brief Set level of overtemperature protection.
 * @param dev - The device structure.
 * @param level - The level of overtemperature protection (level = round( (threshold[℃] + 266.64[℃]) / 6.51[℃/LSB]) ).
 */
int adhv4710_set_overtemperature_level(struct adhv4710_dev *dev, uint8_t level)
{
	/* Set overtemperature level */
	return adhv4710_update_bits(dev, ADHV4710_REG_CTRL_REG_12,
				    ADEHV4710_OT_LIMIT_MSK,
				    no_os_field_prep(ADEHV4710_OT_LIMIT_MSK, level));
}
/**
 * @brief Version product
 * @param dev - The device structure.
 * @param data_read - The version product read value
 * @return 0 in case of success, negative error code otherwise.
 */
int adhv4710_version_product(struct adhv4710_dev *dev, uint8_t *data_read)
{
	int ret;
	/* version product */
	uint8_t version;

	if (!data_read)
		return -EINVAL;

	ret = adhv4710_read(dev, ADHV4710_REG_CTRL_REG_26, &version);
	if (ret)
		return ret;

	*data_read = version;

	return 0;
}
