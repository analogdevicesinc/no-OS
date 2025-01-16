/***************************************************************************//**
 *   @file   ad5592r-base.c
 *   @brief  Implementation of AD5592R Base Driver.
 *   @author Mircea Caprioru (mircea.caprioru@analog.com)
********************************************************************************
 * Copyright 2018, 2020, 2025(c) Analog Devices, Inc.
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

/**
 * Register update
 *
 * @param dev - The device structure.
 * @param reg_addr - The Register address
 * @param data - The data to be written
 * @param mask - The mask
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5592r_base_reg_update(struct ad5592r_dev *dev, uint16_t reg_addr,
				uint16_t data, uint16_t mask)
{

	uint16_t temp_reg_val;
	int32_t ret;

	if (!dev)
		return -1;

	ret = ad5592r_base_reg_read(dev, reg_addr, &temp_reg_val);
	if (ret < 0)
		return ret;

	temp_reg_val &= ~mask;
	temp_reg_val |= data;

	ret = ad5592r_base_reg_write(dev, reg_addr, temp_reg_val);
	if (ret < 0)
		return ret;

	return 0;
}

/**
 * Set ADC Range of the device
 *
 * @param dev - The device structure.
 * @param adc_range - ADC Range
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5592r_set_adc_range(struct ad5592r_dev *dev,
			      enum ad559xr_range adc_range)
{
	int32_t ret;
	uint16_t status = 0;

	if (!dev)
		return -1;

	status = adc_range ? AD5592R_REG_CTRL_ADC_RANGE : 0;

	ret = ad5592r_base_reg_update(dev, AD5592R_REG_CTRL, status,
				      AD5592R_REG_CTRL_ADC_RANGE);
	if (ret < 0)
		return ret;

	dev->adc_range = adc_range;

	return 0;
}

/**
 * Set DAC Range of the device
 *
 * @param dev - The device structure.
 * @param dac_range - DAC Range
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5592r_set_dac_range(struct ad5592r_dev *dev,
			      enum ad559xr_range dac_range)
{
	int32_t ret;
	uint16_t status = 0;

	if (!dev)
		return -1;

	status = dac_range ? AD5592R_REG_CTRL_DAC_RANGE : 0;

	ret = ad5592r_base_reg_update(dev, AD5592R_REG_CTRL, status,
				      AD5592R_REG_CTRL_DAC_RANGE);
	if (ret < 0)
		return ret;

	dev->dac_range = dac_range;

	return 0;
}

/**
 * Set Power Down DAC Channel of the device
 *
 * @param dev - The device structure.
 * @param chan - The channel number.
 * @param enable - Status to enable/disable power down.
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5592r_power_down(struct ad5592r_dev *dev, uint8_t chan, bool enable)
{
	int ret;
	uint16_t temp_reg_val = 0;

	if (!dev)
		return -1;

	temp_reg_val = enable ? NO_OS_BIT(chan) : 0;

	ret = ad5592r_base_reg_update(dev, AD5592R_REG_PD, temp_reg_val,
				      NO_OS_BIT(chan));
	if (ret < 0)
		return ret;

	dev->power_down[chan] = enable;

	return 0;
}

/**
 * Set Reference Select option for the device
 *
 * @param dev - The device structure.
 * @param enable - Status to enable/disable internal reference.
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5592r_set_int_ref(struct ad5592r_dev *dev, bool enable)
{
	uint16_t temp_reg_val = 0;
	int ret;

	if (!dev)
		return -1;

	temp_reg_val = enable ? AD5592R_REG_PD_EN_REF : 0;

	ret = ad5592r_base_reg_update(dev, AD5592R_REG_PD, temp_reg_val,
				      AD5592R_REG_PD_EN_REF);
	if (ret < 0)
		return ret;

	dev->int_ref = enable;

	return 0;
}

/**
 * Set ADC Buffer for the device
 *
 * @param dev - The device structure.
 * @param enable - Status to enable/disable adc buffer.
 * @return 0 in case of success, negative error code otherwise
 */
int32_t ad5592r_set_adc_buffer(struct ad5592r_dev *dev, bool enable)
{
	uint16_t temp_reg_val = 0;
	int ret;

	if (!dev)
		return -1;

	temp_reg_val = enable ? AD5592R_REG_CTRL_ADC_BUFF_EN : 0;

	ret = ad5592r_base_reg_update(dev, AD5592R_REG_CTRL, temp_reg_val,
				      AD5592R_REG_CTRL_ADC_BUFF_EN);
	if (ret < 0)
		return ret;

	dev->adc_buf = enable;

	return 0;
}
