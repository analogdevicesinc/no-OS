/***************************************************************************//**
 *   @file   ad5592r-base.c
 *   @brief  Implementation of AD5592R Base Driver.
 *   @author Mircea Caprioru (mircea.caprioru@analog.com)
********************************************************************************
 * Copyright 2018, 2020(c) Analog Devices, Inc.
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
#include "no_os_error.h"
#include "ad5592r-base.h"

/**
 * Write register.
 *
 * @param dev - The device structure.
 * @param reg - The register address.
 * @param value - register value
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5592r_base_reg_write(struct ad5592r_dev *dev, uint8_t reg,
			       uint16_t value)
{
	return dev->ops->reg_write(dev, reg, value);
}

/**
 * Read register.
 *
 * @param dev - The device structure.
 * @param reg - The register address.
 * @param value - register value
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5592r_base_reg_read(struct ad5592r_dev *dev, uint8_t reg,
			      uint16_t *value)
{
	return dev->ops->reg_read(dev, reg, value);
}

/**
 * Get GPIO value
 *
 * @param dev - The device structure.
 * @param offset - The channel number.
 * @return 0 or 1 depending on the GPIO value.
 */
int32_t ad5592r_gpio_get(struct ad5592r_dev *dev, uint8_t offset)
{
	int32_t ret = 0;
	uint8_t val;

	if (!dev)
		return -1;

	if (dev->gpio_out & NO_OS_BIT(offset))
		val = dev->gpio_val;
	else
		ret = dev->ops->gpio_read(dev, &val);

	if (ret < 0)
		return ret;

	return !!(val & NO_OS_BIT(offset));
}

/**
 * Set GPIO value
 *
 * @param dev - The device structure.
 * @param offset - The channel number.
 * @param value - the GPIO value (0 or 1)
 */
int32_t ad5592r_gpio_set(struct ad5592r_dev *dev, uint8_t offset, int32_t value)
{
	if (!dev)
		return -1;

	if (value)
		dev->gpio_val |= NO_OS_BIT(offset);
	else
		dev->gpio_val &= ~NO_OS_BIT(offset);

	return ad5592r_base_reg_write(dev, AD5592R_REG_GPIO_SET,
				      dev->gpio_val);
}

/**
 * Set GPIO as input
 *
 * @param dev - The device structure.
 * @param offset - The channel number.
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5592r_gpio_direction_input(struct ad5592r_dev *dev, uint8_t offset)
{
	int32_t ret;

	if (!dev)
		return -1;

	dev->gpio_out &= ~NO_OS_BIT(offset);
	dev->gpio_in |= NO_OS_BIT(offset);

	ret = ad5592r_base_reg_write(dev, AD5592R_REG_GPIO_OUT_EN,
				     dev->gpio_out);
	if (ret < 0)
		return ret;

	return ad5592r_base_reg_write(dev, AD5592R_REG_GPIO_IN_EN,
				      dev->gpio_in);
}

/**
 * Set GPIO as output
 *
 * @param dev - The device structure.
 * @param offset - The channel number.
 * @param value - GPIO value to set.
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5592r_gpio_direction_output(struct ad5592r_dev *dev,
				      uint8_t offset, int32_t value)
{
	int32_t ret;

	if (!dev)
		return -1;

	if (value)
		dev->gpio_val |= NO_OS_BIT(offset);
	else
		dev->gpio_val &= ~NO_OS_BIT(offset);

	dev->gpio_in &= ~NO_OS_BIT(offset);
	dev->gpio_out |= NO_OS_BIT(offset);

	ret = ad5592r_base_reg_write(dev, AD5592R_REG_GPIO_SET, dev->gpio_val);
	if (ret < 0)
		return ret;

	ret = ad5592r_base_reg_write(dev, AD5592R_REG_GPIO_OUT_EN,
				     dev->gpio_out);
	if (ret < 0)
		return ret;

	ret = ad5592r_base_reg_write(dev, AD5592R_REG_GPIO_IN_EN,
				     dev->gpio_in);

	return ret;
}

/**
 * Software reset device.
 *
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5592r_software_reset(struct ad5592r_dev *dev)
{
	int32_t ret;

	if (!dev)
		return -1;

	/* Writing this magic value resets the device */
	ret = ad5592r_base_reg_write(dev, AD5592R_REG_RESET, 0xdac);

	no_os_mdelay(10);

	return ret;
}

/**
 * Set channels modes.
 *
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5592r_set_channel_modes(struct ad5592r_dev *dev)
{
	int32_t ret;
	uint8_t i;
	uint8_t pulldown = 0, tristate = 0, dac = 0, adc = 0;
	uint16_t read_back;

	if (!dev)
		return -1;

	dev->gpio_in = 0;
	dev->gpio_out = 0;

	for (i = 0; i < dev->num_channels; i++) {
		switch (dev->channel_modes[i]) {
		case CH_MODE_DAC:
			dac |= NO_OS_BIT(i);
			break;

		case CH_MODE_ADC:
			adc |= NO_OS_BIT(i);
			break;

		case CH_MODE_DAC_AND_ADC:
			dac |= NO_OS_BIT(i);
			adc |= NO_OS_BIT(i);
			break;

		case CH_MODE_GPI:
			dev->gpio_in |= NO_OS_BIT(i);
			break;

		case CH_MODE_GPO:
			dev->gpio_out |= NO_OS_BIT(i);
			break;

		case CH_MODE_UNUSED:
		/* fall-through */
		default:
			switch (dev->channel_offstate[i]) {
			case CH_OFFSTATE_OUT_TRISTATE:
				tristate |= NO_OS_BIT(i);
				break;

			case CH_OFFSTATE_OUT_LOW:
				dev->gpio_out |= NO_OS_BIT(i);
				break;

			case CH_OFFSTATE_OUT_HIGH:
				dev->gpio_out |= NO_OS_BIT(i);
				dev->gpio_val |= NO_OS_BIT(i);
				break;

			case CH_OFFSTATE_PULLDOWN:
			/* fall-through */
			default:
				pulldown |= NO_OS_BIT(i);
				break;
			}
		}
	}

	/* Pull down unused pins to GND */
	ret = ad5592r_base_reg_write(dev, AD5592R_REG_PULLDOWN, pulldown);
	if (ret < 0)
		return ret;

	ret = ad5592r_base_reg_write(dev, AD5592R_REG_TRISTATE, tristate);
	if (ret < 0)
		return ret;

	/* Configure pins that we use */
	ret = ad5592r_base_reg_write(dev, AD5592R_REG_DAC_EN, dac);
	if (ret < 0)
		return ret;

	ret = ad5592r_base_reg_write(dev, AD5592R_REG_ADC_EN, adc);
	if (ret < 0)
		return ret;

	ret = ad5592r_base_reg_write(dev, AD5592R_REG_GPIO_SET, dev->gpio_val);
	if (ret < 0)
		return ret;

	ret = ad5592r_base_reg_write(dev, AD5592R_REG_GPIO_OUT_EN,
				     dev->gpio_out);
	if (ret < 0)
		return ret;

	ret = ad5592r_base_reg_write(dev, AD5592R_REG_GPIO_IN_EN,
				     dev->gpio_in);
	if (ret < 0)
		return ret;

	/* Verify that we can read back at least one register */
	ret = ad5592r_base_reg_read(dev, AD5592R_REG_ADC_EN, &read_back);
	if (!ret && (read_back & 0xff) != adc)
		return -1;

	return ret;
}

/**
 * Reset channels and set GPIO to unused.
 *
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5592r_reset_channel_modes(struct ad5592r_dev *dev)
{
	uint32_t i;

	if (!dev)
		return -1;

	for (i = 0; i < sizeof(dev->channel_modes); i++)
		dev->channel_modes[i] = CH_MODE_UNUSED;

	return ad5592r_set_channel_modes(dev);
}
