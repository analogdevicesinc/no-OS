/***************************************************************************//**
 *   @file   ad7091r5.c
 *   @brief  Implementation of ad7091r5 Driver.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "ad7091r5.h"
#include <string.h>
#include <errno.h>
#include "stdlib.h"
#include "no_os_error.h"
#include "no_os_delay.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * Read from device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7091r5_i2c_reg_read(struct ad7091r5_dev *dev,
			      uint8_t reg_addr,
			      uint16_t *reg_data)
{
	uint8_t buf[2];
	int32_t ret;

	if (!dev || !reg_data)
		return -EINVAL;

	ret = no_os_i2c_write(dev->i2c_desc, &reg_addr, 1, 1);
	if (ret < 0)
		return ret;

	ret = no_os_i2c_read(dev->i2c_desc, buf, 2, 1);
	if (ret < 0)
		return ret;

	*reg_data = (buf[0] << 8) | buf[1];

	return ret;
}

/**
 * Write to device.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7091r5_i2c_reg_write(struct ad7091r5_dev *dev,
			       uint8_t reg_addr,
			       uint16_t reg_data)
{
	uint8_t buf[3];

	if (!dev)
		return -EINVAL;

	buf[0] = reg_addr;
	buf[1] = (reg_data & 0xFF00) >> 8;
	buf[2] = reg_data & 0xFF;

	return no_os_i2c_write(dev->i2c_desc, buf, NO_OS_ARRAY_SIZE(buf), 1);
}

/**
 * I2C read from device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7091r5_i2c_read_mask(struct ad7091r5_dev *dev,
			       uint8_t reg_addr,
			       uint8_t mask,
			       uint16_t *data)
{
	uint16_t reg_data;
	int32_t ret;

	if (!dev || !data)
		return -EINVAL;

	ret = ad7091r5_i2c_reg_read(dev, reg_addr, &reg_data);
	*data = (reg_data & mask);

	return ret;
}

/**
 * I2C write to device using a mask.
 * @param dev - The device structure.
 * @param reg_addr - The register address.
 * @param mask - The mask.
 * @param data - The register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7091r5_i2c_write_mask(struct ad7091r5_dev *dev,
				uint8_t reg_addr,
				uint16_t mask,
				uint16_t data)
{
	uint16_t reg_data;
	int32_t ret;

	if (!dev)
		return -EINVAL;

	ret = ad7091r5_i2c_reg_read(dev, reg_addr, &reg_data);
	if (ret < 0)
		return ret;

	reg_data &= ~mask;
	reg_data |= data;

	return ad7091r5_i2c_reg_write(dev, reg_addr, reg_data);
}

/**
 * Set sleep mode.
 * @param dev - The device structure.
 * @param mode - Sleep mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7091r5_sleep_mode(struct ad7091r5_dev *dev,
			    enum ad7091r5_sleep_mode mode)
{
	if (!dev)
		return -EINVAL;

	return ad7091r5_i2c_write_mask(dev, AD7091R5_REG_CONF,
				       REG_CONF_SLEEP_MODE_MASK,
				       REG_CONF_SLEEP_MODE(mode));
}

/**
 * Set output port value.
 * @param dev - The device structure.
 * @param port - Port number.
 * @param value - Value.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7091r5_set_port(struct ad7091r5_dev *dev, enum ad7091r5_port port,
			  bool value)
{
	uint16_t mask;
	uint16_t val;

	if (!dev)
		return -EINVAL;

	switch (port) {
	case AD7091R5_GPO0:
		mask = REG_CONF_GPO0_MASK;
		val = REG_CONF_GPO0(value);
		break;
	case AD7091R5_GPO1:
		mask = REG_CONF_GPO1_MASK;
		val = REG_CONF_GPO1(value);
		break;
	case AD7091R5_GPO2:
		mask = REG_CONF_GPO2_MASK;
		val = REG_CONF_GPO2(value);
		break;
	default:
		return -EINVAL;
	}

	return ad7091r5_i2c_write_mask(dev, AD7091R5_REG_CONF,
				       mask, val);
}

/**
 * Set GPO0 mode.
 * @param dev - The device structure.
 * @param mode - GPO0 new mode.
 * @param is_cmos - 0: GPO0 is open drain
 * 		  - 1: GPO0 is CMOS.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7091r5_set_gpo0_mode(struct ad7091r5_dev *dev,
			       enum ad7091r5_gpo0_mode mode, bool is_cmos)
{
	uint16_t value;

	if (!dev)
		return -EINVAL;

	switch (mode) {
	case AD7091R5_GPO0_ENABLED:
		value = 0;
		break;
	case AD7091R5_GPO0_ALERT:
		value = REG_CONF_GPO0_ALERT(1);
		break;
	case AD7091R5_GPO0_BUSY:
		value = REG_CONF_GPO0_BUSY(1) | REG_CONF_GPO0_ALERT(1);
		break;
	default:
		return -EINVAL;
		break;
	}
	value |= REG_CONF_GPO0_DRIVE_TYPE(is_cmos);

	return ad7091r5_i2c_write_mask(dev, AD7091R5_REG_CONF,
				       REG_CONF_GPO0_MODE_MASK, value);
}

/**
 * Set cycle timer for autocycle mode.
 * @param dev - The device structure.
 * @param timer - Converter sampling period.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7091r5_set_cycle_timer(struct ad7091r5_dev *dev,
				 enum ad7091r5_cycle_timer timer)
{
	if (!dev)
		return -EINVAL;

	return ad7091r5_i2c_write_mask(dev, AD7091R5_REG_CONF,
				       REG_CONF_CYCLE_TIMER_MASK,
				       REG_CONF_CYCLE_TIMER(timer));
}

/**
 * Get alert.
 * @param dev - The device structure.
 * @param channel - Channel.
 * @param alert - Alert type.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7091r5_get_alert(struct ad7091r5_dev *dev, uint8_t channel,
			   enum ad7091r5_alert_type *alert)
{
	int32_t ret;
	uint16_t data;

	if (!dev || !alert)
		return -EINVAL;

	if (channel >= AD7091R5_CHANNEL_NO)
		return -EINVAL;

	ret = ad7091r5_i2c_reg_read(dev, AD7091R5_REG_ALERT, &data);
	if (ret)
		return ret;

	*alert = REG_ALERT_MASK(data, channel);

	return 0;
}


/**
 * Set high limit, low limit, hysteresis.
 * @param dev - The device structure.
 * @param limit - Limit.
 * @param channel - Channel.
 * @param value - Value.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7091r5_set_limit(struct ad7091r5_dev *dev,
			   enum ad7091r5_limit limit,
			   uint8_t channel,
			   uint16_t value)
{
	uint16_t reg;

	if (!dev)
		return -EINVAL;

	switch (limit) {
	case AD7091R5_LOW_LIMIT:
		reg = AD7091R5_REG_CH_LOW_LIMIT(channel);
		break;
	case AD7091R5_HIGH_LIMIT:
		reg = AD7091R5_REG_CH_HIGH_LIMIT(channel);
		break;
	case AD7091R5_HYSTERESIS:
		reg = AD7091R5_REG_CH_HYSTERESIS(channel);
		break;
	default:
		return -EINVAL;
	}

	return ad7091r5_i2c_reg_write(dev, reg, REG_RESULT_CONV_DATA(value));
}

/**
 * Get high limit, low limit, hysteresis.
 * @param dev - The device structure.
 * @param limit - Limit.
 * @param channel - Channel.
 * @param value - Value.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7091r5_get_limit(struct ad7091r5_dev *dev,
			   enum ad7091r5_limit limit,
			   uint8_t channel,
			   uint16_t *value)
{
	int32_t ret;
	uint16_t reg, data;

	if (!dev || !value)
		return -EINVAL;

	switch (limit) {
	case AD7091R5_LOW_LIMIT:
		reg = AD7091R5_REG_CH_LOW_LIMIT(channel);
		break;
	case AD7091R5_HIGH_LIMIT:
		reg = AD7091R5_REG_CH_HIGH_LIMIT(channel);
		break;
	case AD7091R5_HYSTERESIS:
		reg = AD7091R5_REG_CH_HYSTERESIS(channel);
		break;
	default:
		return -EINVAL;
	}

	ret = ad7091r5_i2c_reg_read(dev, reg, &data);
	if (ret)
		return ret;

	*value = REG_RESULT_CONV_DATA(data);

	return 0;
}

/**
 * Set mode.
 * @param dev - The device structure.
 * @param mode - Converter mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7091r5_set_mode(struct ad7091r5_dev *dev, enum ad7091r5_mode mode)
{
	uint16_t data;

	if (!dev)
		return -EINVAL;

	switch (mode) {
	case AD7091R5_MODE_SAMPLE:
		data = 0;
		break;
	case AD7091R5_MODE_COMMAND:
		data = REG_CONF_CMD(1);
		break;
	case AD7091R5_MODE_AUTOCYCLE:
		data = REG_CONF_AUTO(1);
		break;
	default:
		return -EINVAL;
	}
	return ad7091r5_i2c_write_mask(dev, AD7091R5_REG_CONF,
				       REG_CONF_MODE_MASK, data);
}

/**
 * Set device channel.
 * @param dev - The device structure.
 * @param channel - Channel.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7091r5_set_channel(struct ad7091r5_dev *dev, uint8_t channel)
{
	uint16_t foo;
	int32_t ret;

	if (!dev)
		return -EINVAL;

	if (channel >= AD7091R5_CHANNEL_NO)
		return -EINVAL;

	/* AD7091R5_REG_CHANNEL is a 8-bit register */
	ret = ad7091r5_i2c_reg_write(dev, AD7091R5_REG_CHANNEL,
				     (NO_OS_BIT(channel) << 8));
	if (ret)
		return ret;

	/* There is a latency of one conversion before the channel conversion
	 * sequence is updated */
	return ad7091r5_i2c_reg_read(dev, AD7091R5_REG_RESULT, &foo);
}

/**
 * @brief Initialize GPIO driver handlers for the GPIOs in the system.
 *        ad7091r5_init() helper function.
 * @param dev - ad7091r5_dev device handler.
 * @param is_software - true: Software reset
 * 		      - false: hardware reset
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7091r5_reset(struct ad7091r5_dev *dev, bool is_software)
{
	int32_t ret;

	if (!dev)
		return -EINVAL;

	if (is_software) {
		/* Bit is cleared automatically after reset */
		return ad7091r5_i2c_write_mask(dev, AD7091R5_REG_CONF,
					       REG_CONF_RESET_MASK,
					       REG_CONF_RESET(1));
	} else {
		ret = no_os_gpio_set_value(dev->gpio_resetn, NO_OS_GPIO_LOW);
		if (ret < 0)
			return ret;

		/* reset pulse width, at least 10 ns*/
		no_os_udelay(1);
		return no_os_gpio_set_value(dev->gpio_resetn, NO_OS_GPIO_HIGH);
	}
}

/**
 * @brief Initialize GPIO driver handlers for the GPIOs in the system.
 *        ad7091r5_init() helper function.
 * @param dev - ad7091r5_dev device handler.
 * @param init_param - Pointer to the initialization structure.
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t ad7091r5_init_gpio(struct ad7091r5_dev *dev,
				  struct ad7091r5_init_param *init_param)
{
	int32_t ret;

	if (!dev || !init_param)
		return -EINVAL;

	ret = no_os_gpio_get_optional(&dev->gpio_resetn, init_param->gpio_resetn);
	if (ret < 0)
		return ret;

	ret = no_os_gpio_direction_output(dev->gpio_resetn, NO_OS_GPIO_LOW);
	if (ret < 0)
		return ret;

	/** Reset to configure pins */
	return ad7091r5_reset(dev, init_param->gpio_resetn);
}

/**
 * Read one sample.
 * @param dev - The device structure.
 * @param channel - Channel.
 * @param read_val - Value.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7091r5_read_one(struct ad7091r5_dev *dev,
			  uint8_t channel, uint16_t *read_val)
{
	uint16_t val;
	int32_t ret;

	if (!dev || !read_val)
		return -EINVAL;

	if (channel >= AD7091R5_CHANNEL_NO)
		return -EINVAL;

	ret = ad7091r5_set_channel(dev, channel);
	if (ret)
		return ret;

	ret = ad7091r5_i2c_reg_read(dev, AD7091R5_REG_RESULT, &val);
	if (ret)
		return ret;

	if (REG_RESULT_CH_ID(val) != channel)
		return -1;

	*read_val = REG_RESULT_CONV_DATA(val);

	return 0;
}

/**
 * Initialize the device.
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7091r5_init(struct ad7091r5_dev **device,
		      struct ad7091r5_init_param *init_param)
{
	struct ad7091r5_dev *dev;
	int32_t ret;

	if (!device || !init_param)
		return -EINVAL;

	dev = (struct ad7091r5_dev *)no_os_malloc(sizeof(*dev));
	if (!dev)
		return -ENOMEM;


	ret = ad7091r5_init_gpio(dev, init_param);
	if (ret < 0)
		goto error_dev;

	ret = no_os_i2c_init(&dev->i2c_desc, init_param->i2c_init);
	if (ret < 0)
		goto error_gpio;

	/* Use command mode by default */
	ret = ad7091r5_set_mode(dev, AD7091R5_MODE_COMMAND);
	if (ret < 0)
		goto error_i2c;

	*device = dev;

	return 0;

error_i2c:
	no_os_i2c_remove(dev->i2c_desc);
error_gpio:
	no_os_gpio_remove(dev->gpio_resetn);
error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free the memory allocated by ad7091r5_init().
 * @param dev - Pointer to the device handler.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t ad7091r5_remove(struct ad7091r5_dev *dev)
{
	int32_t ret;

	if (!dev)
		return -EINVAL;

	ret = no_os_i2c_remove(dev->i2c_desc);
	if (ret < 0)
		return ret;

	ret = no_os_gpio_remove(dev->gpio_resetn);
	if (ret < 0)
		return ret;

	no_os_free(dev);

	return ret;
}
