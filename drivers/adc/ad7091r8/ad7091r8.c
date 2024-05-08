/***************************************************************************//**
 *   @file   ad7091r8.c
 *   @brief  Implementation of AD7091R-8 Driver
 *   @author Marcelo Schmitt (marcelo.schmitt@analog.com)
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

#include <stdlib.h>
#include <errno.h>

#include "ad7091r8.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_delay.h"

/**
 * Pull the CONVST line up then down to signal to the start of a read/write
 * operation.
 * @param dev - The device structure.
 */
int ad7091r8_pulse_convst(struct ad7091r8_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_gpio_set_value(dev->gpio_convst, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	return no_os_gpio_set_value(dev->gpio_convst, NO_OS_GPIO_HIGH);
}

/**
 * Write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7091r8_spi_reg_write(struct ad7091r8_dev *dev,
			   uint8_t reg_addr,
			   uint16_t reg_data)
{
	uint8_t buf[2];

	if (!dev)
		return -EINVAL;

	/*
	 * AD7091R-2/-4/-8 protocol (datasheet page 31) is to do a single SPI
	 * transfer with reg address set in bits B15:B11 and value set in B9:B0.
	 */
	no_os_put_unaligned_be16(
		no_os_field_prep(AD7091R8_REG_DATA_MSK, reg_data) |
		no_os_field_prep(AD7091R8_RD_WR_FLAG_MSK, 1) |
		no_os_field_prep(AD7091R8_REG_ADDR_MSK, reg_addr), buf);

	return no_os_spi_write_and_read(dev->spi_desc, buf, 2);
}

/**
 * Read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7091r8_spi_reg_read(struct ad7091r8_dev *dev,
			  uint8_t reg_addr,
			  uint16_t *reg_data)
{
	uint8_t buf[2];
	int ret;

	if (!dev || !reg_data)
		return -EINVAL;

	if (reg_addr == AD7091R8_REG_RESULT) {
		ret = ad7091r8_pulse_convst(dev);
		if (ret)
			return ret;
	}

	no_os_put_unaligned_be16(
		no_os_field_prep(AD7091R8_RD_WR_FLAG_MSK, 0) |
		no_os_field_prep(AD7091R8_REG_ADDR_MSK, reg_addr), buf);

	/* Reg data only comes out on the next transfer (datasheet figure 52) */
	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 2);
	if (ret)
		return ret;

	if (reg_addr == AD7091R8_REG_RESULT) {
		ret = ad7091r8_pulse_convst(dev);
		if (ret)
			return ret;
	}

	no_os_put_unaligned_be16(
		no_os_field_prep(AD7091R8_RD_WR_FLAG_MSK, 0) |
		no_os_field_prep(AD7091R8_REG_ADDR_MSK, reg_addr), buf);

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 2);
	if (ret)
		return ret;

	*reg_data = no_os_get_unaligned_be16(buf);
	return 0;
}

/**
 * SPI write to device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7091r8_spi_write_mask(struct ad7091r8_dev *dev,
			    uint8_t reg_addr,
			    uint16_t mask,
			    uint16_t data)
{
	uint16_t reg_data;
	int ret;

	ret = ad7091r8_spi_reg_read(dev, reg_addr, &reg_data);
	if (ret)
		return ret;

	reg_data &= ~mask;
	reg_data |= data;

	return ad7091r8_spi_reg_write(dev, reg_addr, reg_data);
}

/**
 * @brief Set device sleep mode.
 *
 * @param dev - The device structure.
 * @param mode - The device sleep mode to set.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7091r8_set_sleep_mode(struct ad7091r8_dev *dev,
			    enum ad7091r8_sleep_mode mode)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad7091r8_spi_write_mask(dev, AD7091R8_REG_CONF,
				      REG_CONF_SLEEP_MODE_MASK, mode);
	if (ret)
		return ret;

	dev->sleep_mode = mode;

	return 0;
}

/**
 * Set output port value.
 * @param dev - The device structure.
 * @param port - Port number.
 * @param value - Value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7091r8_set_port(struct ad7091r8_dev *dev, enum ad7091r8_port port,
		      bool value)
{
	uint16_t mask;
	uint16_t val;

	switch (port) {
	case AD7091R8_GPO0:
		mask = REG_CONF_GPO0_MASK;
		val = no_os_field_prep(REG_CONF_GPO0_MASK, value);
		break;
	case AD7091R8_GPO1:
		mask = REG_CONF_GPO1_MASK;
		val = no_os_field_prep(REG_CONF_GPO1_MASK, value);
		break;
	default:
		return -EINVAL;
	}

	return ad7091r8_spi_write_mask(dev, AD7091R8_REG_CONF, mask, val);
}

/**
 * Set GPO0 mode.
 * @param dev - The device structure.
 * @param mode - GPO0 new mode.
 * @param is_cmos - 0: GPO0 is open drain
 * 		  - 1: GPO0 is CMOS.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7091r8_set_gpo0_mode(struct ad7091r8_dev *dev,
			   enum ad7091r8_gpo0_mode mode, bool is_cmos)
{
	uint16_t value = mode;

	if (is_cmos)
		value |= NO_OS_BIT(6);

	return ad7091r8_spi_write_mask(dev, AD7091R8_REG_CONF,
				       REG_CONF_GPO0_MODE_MASK, value);
}

/**
 * Set high limit, low limit, hysteresis.
 * Device accepts 9 bits provided by the user and sets them as the MSBs.
 * The 3 LSBs of the internal 12-bit registers are set either to 000 or 111.
 * Round user input according to each case.
 * @param dev - The device structure.
 * @param limit - Limit.
 * @param channel - Channel.
 * @param value - Value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7091r8_set_limit(struct ad7091r8_dev *dev,
		       enum ad7091r8_limit limit,
		       uint8_t channel,
		       uint16_t value)
{
	uint16_t reg;

	switch (limit) {
	case AD7091R8_LOW_LIMIT:
		reg = AD7091R8_REG_CH_LOW_LIMIT(channel);
		break;
	case AD7091R8_HIGH_LIMIT:
		reg = AD7091R8_REG_CH_HIGH_LIMIT(channel);
		break;
	case AD7091R8_HYSTERESIS:
		reg = AD7091R8_REG_CH_HYSTERESIS(channel);
		break;
	default:
		return -EINVAL;
	}

	return ad7091r8_spi_reg_write(dev, reg,
				      no_os_field_get(AD7091R8_CONV_MASK, value));
}

/**
 * Get alert.
 * @param dev - The device structure.
 * @param channel - Channel.
 * @param alert - Alert type.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7091r8_get_alert(struct ad7091r8_dev *dev, uint8_t channel,
		       enum ad7091r8_alert_type *alert)
{
	int ret;
	uint16_t data;

	if (!dev || !alert)
		return -EINVAL;

	if (channel >= AD7091R_NUM_CHANNELS(dev->device_id))
		return -EINVAL;

	ret = ad7091r8_spi_reg_read(dev, AD7091R8_REG_ALERT, &data);
	if (ret)
		return ret;

	*alert = REG_ALERT_MASK(data, channel);

	return 0;
}

/**
 * Get high limit, low limit, hysteresis.
 * The 3 LSBs of the internal 12-bit registers are set either to 000 or 111.
 * Adjust limit data to reflect actual limit in use by the device.
 * @param dev - The device structure.
 * @param limit - Limit.
 * @param channel - Channel.
 * @param value - Value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7091r8_get_limit(struct ad7091r8_dev *dev,
		       enum ad7091r8_limit limit,
		       uint8_t channel,
		       uint16_t *value)
{
	uint16_t reg;

	switch (limit) {
	case AD7091R8_LOW_LIMIT:
		reg = AD7091R8_REG_CH_LOW_LIMIT(channel);
		break;
	case AD7091R8_HIGH_LIMIT:
		reg = AD7091R8_REG_CH_HIGH_LIMIT(channel);
		break;
	case AD7091R8_HYSTERESIS:
		reg = AD7091R8_REG_CH_HYSTERESIS(channel);
		break;
	default:
		return -EINVAL;
	}

	return ad7091r8_spi_reg_read(dev, reg, value);
}

/**
 * @brief Initiate a software reset or hardware reset through the RESET pin.
 * @param dev - ad7091r8_dev device handler.
 * @param is_software - true: Software reset
 * 		      - false: hardware reset
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7091r8_reset(struct ad7091r8_dev *dev, bool is_software)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (is_software) {
		/* Bit is cleared automatically after reset */
		return ad7091r8_spi_write_mask(dev, AD7091R8_REG_CONF,
					       REG_CONF_RESET_MASK,
					       NO_OS_BIT(9));
	}
	ret = no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	no_os_udelay(1);
	return no_os_gpio_set_value(dev->gpio_reset, NO_OS_GPIO_HIGH);
}

/***************************************************************************//**
 * Initializes the communication peripheral and the initial Values for
 * AD7092R-8 Board.
 * @param device     - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return ret - The result of the initialization procedure.
 *               Example: -1 - SPI peripheral was not initialized or the
 *                             device is not present.
 *                         0 - SPI peripheral was initialized and the
 *                             device is present.
*******************************************************************************/
int ad7091r8_init(struct ad7091r8_dev **device,
		  struct ad7091r8_init_param *init_param)
{
	struct ad7091r8_dev *dev;
	int ret;

	if (!device)
		return -EINVAL;

	dev = (struct ad7091r8_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret)
		goto err_free_dev;

	dev->device_id = init_param->device_id;

	ret = no_os_gpio_get(&dev->gpio_convst, init_param->gpio_convst);
	if (ret)
		goto err_release_spi;

	ret = no_os_gpio_direction_output(dev->gpio_convst, NO_OS_GPIO_HIGH);
	if (ret)
		goto err_release_convst;

	ret = no_os_gpio_get_optional(&dev->gpio_reset, init_param->gpio_reset);
	if (!ret && dev->gpio_reset) {
		ret = no_os_gpio_direction_output(dev->gpio_reset,
						  NO_OS_GPIO_HIGH);
		if (ret)
			goto err_release_reset;
	}

	ret = ad7091r8_reset(dev, !dev->gpio_reset);
	if (ret)
		goto err_release_reset;

	/* Set device sleep-mode */
	ret = ad7091r8_set_sleep_mode(dev, init_param->sleep_mode);
	if (ret)
		goto err_release_reset;

	/* Use external vref or enable internal vref */
	dev->vref_mv = init_param->vref_mv;
	if (!dev->vref_mv) {
		dev->vref_mv = 2500;
		ret = ad7091r8_set_sleep_mode(dev, AD7091R8_SLEEP_MODE_1);
		if (ret)
			goto err_release_reset;
	}

	*device = dev;

	return 0;

err_release_reset:
	no_os_gpio_remove(dev->gpio_reset);

err_release_convst:
	no_os_gpio_remove(dev->gpio_convst);

err_release_spi:
	no_os_spi_remove(dev->spi_desc);

err_free_dev:
	no_os_free(dev);
	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ad7091r8_init().
 *
 * @param dev - The device structure.
 *
 * @return ret - The result of the remove procedure.
*******************************************************************************/
int ad7091r8_remove(struct ad7091r8_dev *dev)
{
	int ret;

	ret = no_os_gpio_remove(dev->gpio_reset);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_convst);
	if (ret)
		return ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	no_os_free(dev);
	return 0;
}

/**
 * Set device channel.
 * @param dev - The device structure.
 * @param channel - Channel.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7091r8_set_channel(struct ad7091r8_dev *dev, uint8_t channel)
{
	int ret;

	if (!dev)
		return -EINVAL;

	if (channel >= AD7091R_NUM_CHANNELS(dev->device_id))
		return -EINVAL;

	ret = ad7091r8_pulse_convst(dev);
	if (ret)
		return ret;

	return ad7091r8_spi_reg_write(dev, AD7091R8_REG_CHANNEL,
				      NO_OS_BIT(channel));
}

/**
 * Read one sample.
 * @param dev - The device structure.
 * @param channel - Channel.
 * @param read_val - Value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7091r8_read_one(struct ad7091r8_dev *dev, uint8_t channel,
		      uint16_t *read_val)
{
	int ret;

	if (!dev || !read_val)
		return -EINVAL;

	if (channel >= AD7091R_NUM_CHANNELS(dev->device_id))
		return -EINVAL;

	ret = ad7091r8_set_channel(dev, channel);
	if (ret)
		return ret;

	return ad7091r8_spi_reg_read(dev, AD7091R8_REG_RESULT, read_val);
}

/**
 * Read the next channel set in the channel sequencer (channel register).
 * @param dev - The device structure.
 * @param read_val - Value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ad7091r8_sequenced_read(struct ad7091r8_dev *dev, uint16_t *read_val)
{

	uint8_t buf[2];
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ad7091r8_pulse_convst(dev);
	if (ret)
		return ret;

	no_os_put_unaligned_be16(0xf800, buf); /* NOP command */

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 2);
	if (ret)
		return ret;

	*read_val = no_os_get_unaligned_be16(buf);
	return 0;
}
