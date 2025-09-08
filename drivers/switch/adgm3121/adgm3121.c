/***************************************************************************//**
 *   @file   adgm3121.c
 *   @brief  Implementation of ADGM3121 Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#include <stdlib.h>
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_util.h"
#include "adgm3121.h"

/**
 * @brief Write to device register via SPI
 * @param dev - The device structure
 * @param reg_addr - Register address
 * @param data - Data to write
 * @return 0 in case of success, negative error code otherwise
 */
int adgm3121_write_register(struct adgm3121_dev *dev, uint8_t reg_addr,
			    uint8_t data)
{
	uint8_t buf[2];
	uint16_t cmd;

	if (!dev || dev->mode != ADGM3121_MODE_SPI || !dev->spi_desc)
		return -EINVAL;

	if (dev->daisy_chain_mode) {
		buf[0] = data;
		return no_os_spi_write_and_read(dev->spi_desc, buf, 1);
	}

	/* Build 16-bit SPI command: [RW][ADDR][DATA] */
	cmd = ADGM3121_SPI_WRITE |
	      NO_OS_FIELD_PREP(ADGM3121_SPI_ADDR_MSK, reg_addr) |
	      data;

	buf[0] = cmd >> 8;
	buf[1] = cmd & 0xFF;

	return no_os_spi_write_and_read(dev->spi_desc, buf, 2);
}

/**
 * @brief Read from device register via SPI
 * @param dev - The device structure
 * @param reg_addr - Register address
 * @param data - Pointer to store read data
 * @return 0 in case of success, negative error code otherwise
 */
int adgm3121_read_register(struct adgm3121_dev *dev, uint8_t reg_addr,
			   uint8_t *data)
{
	uint8_t buf[2];
	uint16_t cmd;
	int ret;

	if (!dev || !data || dev->mode != ADGM3121_MODE_SPI || !dev->spi_desc)
		return -EINVAL;

	if (dev->daisy_chain_mode)
		return -ENOTSUP; /* Read not supported in daisy chain mode */

	/* Build 16-bit SPI command: [RW][ADDR][DUMMY] */
	cmd = ADGM3121_SPI_READ |
	      NO_OS_FIELD_PREP(ADGM3121_SPI_ADDR_MSK, reg_addr);

	buf[0] = cmd >> 8;
	buf[1] = cmd & 0xFF;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 2);
	if (ret)
		return ret;

	*data = buf[1];

	return 0;
}

/**
 * @brief Set switch state using parallel GPIO mode
 * @param dev - The device structure
 * @param sw - Switch to control
 * @param state - Desired state
 * @return 0 in case of success, negative error code otherwise
 */
static int adgm3121_set_switch_parallel(struct adgm3121_dev *dev,
					enum adgm3121_switch sw,
					enum adgm3121_state state)
{
	struct no_os_gpio_desc *gpio;
	uint8_t value;

	switch (sw) {
	case ADGM3121_SW1:
		gpio = dev->gpio_in1;
		break;
	case ADGM3121_SW2:
		gpio = dev->gpio_in2;
		break;
	case ADGM3121_SW3:
		gpio = dev->gpio_in3;
		break;
	case ADGM3121_SW4:
		gpio = dev->gpio_in4;
		break;
	default:
		return -EINVAL;
	}

	if (!gpio)
		return -EINVAL;

	value = (state == ADGM3121_ENABLE) ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW;

	return no_os_gpio_set_value(gpio, value);
}

/**
 * @brief Get switch state using parallel GPIO mode
 * @param dev - The device structure
 * @param sw - Switch to read
 * @param state - Pointer to store current state
 * @return 0 in case of success, negative error code otherwise
 */
static int adgm3121_get_switch_parallel(struct adgm3121_dev *dev,
					enum adgm3121_switch sw,
					enum adgm3121_state *state)
{
	struct no_os_gpio_desc *gpio;
	uint8_t value;
	int ret;

	switch (sw) {
	case ADGM3121_SW1:
		gpio = dev->gpio_in1;
		break;
	case ADGM3121_SW2:
		gpio = dev->gpio_in2;
		break;
	case ADGM3121_SW3:
		gpio = dev->gpio_in3;
		break;
	case ADGM3121_SW4:
		gpio = dev->gpio_in4;
		break;
	default:
		return -EINVAL;
	}

	if (!gpio)
		return -EINVAL;

	ret = no_os_gpio_get_value(gpio, &value);
	if (ret)
		return ret;

	*state = (value == NO_OS_GPIO_HIGH) ? ADGM3121_ENABLE : ADGM3121_DISABLE;

	return 0;
}

/**
 * @brief Set switch state using SPI mode
 * @param dev - The device structure
 * @param sw - Switch to control
 * @param state - Desired state
 * @return 0 in case of success, negative error code otherwise
 */
static int adgm3121_set_switch_spi(struct adgm3121_dev *dev,
				   enum adgm3121_switch sw,
				   enum adgm3121_state state)
{
	uint8_t switch_data;

	if (state == ADGM3121_ENABLE)
		switch_data = dev->switch_states | NO_OS_BIT(sw);
	else
		switch_data = dev->switch_states & ~NO_OS_BIT(sw);

	int32_t ret = adgm3121_write_register(dev, ADGM3121_REG_SWITCH_DATA,
					      switch_data);
	if (ret)
		return ret;

	dev->switch_states = switch_data;

	/* Wait for switching time */
	no_os_udelay(ADGM3121_SWITCHING_TIME_US);

	return 0;
}

/**
 * @brief Get switch state using SPI mode
 * @param dev - The device structure
 * @param sw - Switch to read
 * @param state - Pointer to store current state
 * @return 0 in case of success, negative error code otherwise
 */
static int adgm3121_get_switch_spi(struct adgm3121_dev *dev,
				   enum adgm3121_switch sw,
				   enum adgm3121_state *state)
{
	uint8_t switch_data;
	int ret;

	ret = adgm3121_read_register(dev, ADGM3121_REG_SWITCH_DATA, &switch_data);
	if (ret)
		return ret;

	dev->switch_states = switch_data & 0x0F; /* Only bits 3:0 are switches */

	*state = (switch_data & NO_OS_BIT(sw)) ? ADGM3121_ENABLE : ADGM3121_DISABLE;

	return 0;
}

/**
 * @brief Set the state of a specific switch
 * @param dev - The device structure
 * @param sw - The switch to control (SW1 to SW4)
 * @param state - The desired state (ADGM3121_ENABLE or ADGM3121_DISABLE)
 * @return 0 in case of success, negative error code otherwise
 */
int adgm3121_set_switch_state(struct adgm3121_dev *dev,
			      enum adgm3121_switch sw,
			      enum adgm3121_state state)
{
	if (!dev || sw > ADGM3121_SW4)
		return -EINVAL;

	if (dev->mode == ADGM3121_MODE_PARALLEL)
		return adgm3121_set_switch_parallel(dev, sw, state);
	else
		return adgm3121_set_switch_spi(dev, sw, state);
}

/**
 * @brief Get the current state of a specific switch
 * @param dev - The device structure
 * @param sw - The switch to read (SW1 to SW4)
 * @param state - Pointer to store the current state
 * @return 0 in case of success, negative error code otherwise
 */
int adgm3121_get_switch_state(struct adgm3121_dev *dev,
			      enum adgm3121_switch sw,
			      enum adgm3121_state *state)
{
	if (!dev || !state || sw > ADGM3121_SW4)
		return -EINVAL;

	if (dev->mode == ADGM3121_MODE_PARALLEL)
		return adgm3121_get_switch_parallel(dev, sw, state);
	else
		return adgm3121_get_switch_spi(dev, sw, state);
}

/**
 * @brief Set multiple switches at once using bitmask
 * @param dev - The device structure
 * @param switch_mask - Bitmask of switches to enable (bits 0-3)
 * @return 0 in case of success, negative error code otherwise
 */
int adgm3121_set_switches(struct adgm3121_dev *dev, uint8_t switch_mask)
{
	int ret;

	if (!dev)
		return -EINVAL;

	switch_mask &= 0x0F; /* Only 4 switches available */

	if (dev->mode == ADGM3121_MODE_PARALLEL) {
		/* Set each switch individually in parallel mode */
		for (int i = 0; i < 4; i++) {
			enum adgm3121_state state = (switch_mask & NO_OS_BIT(i)) ?
						    ADGM3121_ENABLE : ADGM3121_DISABLE;
			ret = adgm3121_set_switch_parallel(dev, i, state);
			if (ret)
				return ret;
		}
	} else {
		/* Set all switches at once in SPI mode */
		ret = adgm3121_write_register(dev, ADGM3121_REG_SWITCH_DATA,
					      switch_mask);
		if (ret)
			return ret;

		dev->switch_states = switch_mask;

		/* Wait for switching time */
		no_os_udelay(ADGM3121_SWITCHING_TIME_US);
	}

	return 0;
}

/**
 * @brief Get all switch states as bitmask
 * @param dev - The device structure
 * @param switch_mask - Pointer to store switch states bitmask
 * @return 0 in case of success, negative error code otherwise
 */
int adgm3121_get_switches(struct adgm3121_dev *dev, uint8_t *switch_mask)
{
	int ret;

	if (!dev || !switch_mask)
		return -EINVAL;

	if (dev->mode == ADGM3121_MODE_PARALLEL) {
		/* Read each switch individually in parallel mode */
		*switch_mask = 0;
		for (int i = 0; i < 4; i++) {
			enum adgm3121_state state;
			ret = adgm3121_get_switch_parallel(dev, i, &state);
			if (ret)
				return ret;
			if (state == ADGM3121_ENABLE)
				*switch_mask |= NO_OS_BIT(i);
		}
	} else {
		/* Read all switches at once in SPI mode */
		uint8_t switch_data;
		ret = adgm3121_read_register(dev, ADGM3121_REG_SWITCH_DATA,
					     &switch_data);
		if (ret)
			return ret;

		*switch_mask = switch_data & 0x0F;
		dev->switch_states = *switch_mask;
	}

	return 0;
}

/**
 * @brief Reset all switches to off state
 * @param dev - The device structure
 * @return 0 in case of success, negative error code otherwise
 */
int adgm3121_reset_switches(struct adgm3121_dev *dev)
{
	return adgm3121_set_switches(dev, 0x00);
}

/**
 * @brief Enter SPI daisy chain mode
 * @param dev - The device structure
 * @return 0 in case of success, negative error code otherwise
 */
int adgm3121_enter_daisy_chain_mode(struct adgm3121_dev *dev)
{
	uint8_t buf[2];
	int ret;

	if (!dev || dev->mode != ADGM3121_MODE_SPI || !dev->spi_desc)
		return -EINVAL;

	if (dev->daisy_chain_mode)
		return 0; /* Already in daisy chain mode */

	buf[0] = (ADGM3121_SPI_DAISY_CHAIN_CMD >> 8) & 0xFF;
	buf[1] = ADGM3121_SPI_DAISY_CHAIN_CMD & 0xFF;

	ret = no_os_spi_write_and_read(dev->spi_desc, buf, 2);
	if (ret)
		return ret;

	dev->daisy_chain_mode = true;

	return 0;
}

/**
 * @brief Check for internal errors
 * @param dev - The device structure
 * @param error_status - Pointer to store error status (bits 7:6)
 * @return 0 in case of success, negative error code otherwise
 */
int adgm3121_check_internal_error(struct adgm3121_dev *dev,
				  uint8_t *error_status)
{
	uint8_t switch_data;
	int ret;

	if (!dev || !error_status || dev->mode != ADGM3121_MODE_SPI)
		return -EINVAL;

	ret = adgm3121_read_register(dev, ADGM3121_REG_SWITCH_DATA, &switch_data);
	if (ret)
		return ret;

	*error_status = NO_OS_FIELD_GET(ADGM3121_INTERNAL_ERROR, switch_data);

	return 0;
}

/**
 * @brief Initialize the device
 * @param device - Pointer to the device structure pointer
 * @param init_param - Pointer to the initialization parameters
 * @return 0 in case of success, negative error code otherwise
 */
int adgm3121_init(struct adgm3121_dev **device,
		  struct adgm3121_init_param *init_param)
{
	struct adgm3121_dev *dev;
	int ret;

	if (!device || !init_param)
		return -EINVAL;

	dev = no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	dev->mode = init_param->mode;
	dev->switch_states = 0;
	dev->daisy_chain_mode = false;

	/* Initialize PIN/SPI mode select GPIO */
	ret = no_os_gpio_get(&dev->gpio_pin_spi, &init_param->gpio_pin_spi);
	if (ret)
		goto error_dev;

	ret = no_os_gpio_direction_output(dev->gpio_pin_spi,
					  (dev->mode == ADGM3121_MODE_SPI) ?
					  NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);
	if (ret)
		goto error_pin_spi;

	if (dev->mode == ADGM3121_MODE_SPI) {
		/* Initialize SPI interface */
		ret = no_os_spi_init(&dev->spi_desc, &init_param->spi_init);
		if (ret)
			goto error_pin_spi;
	} else {
		/* Initialize GPIO interfaces for parallel mode */
		ret = no_os_gpio_get(&dev->gpio_in1, &init_param->gpio_in1);
		if (ret)
			goto error_pin_spi;

		ret = no_os_gpio_get(&dev->gpio_in2, &init_param->gpio_in2);
		if (ret)
			goto error_gpio1;

		ret = no_os_gpio_get(&dev->gpio_in3, &init_param->gpio_in3);
		if (ret)
			goto error_gpio2;

		ret = no_os_gpio_get(&dev->gpio_in4, &init_param->gpio_in4);
		if (ret)
			goto error_gpio3;

		/* Set all GPIO pins as outputs, initially low */
		ret = no_os_gpio_direction_output(dev->gpio_in1, NO_OS_GPIO_LOW);
		if (ret)
			goto error_gpio4;

		ret = no_os_gpio_direction_output(dev->gpio_in2, NO_OS_GPIO_LOW);
		if (ret)
			goto error_gpio4;

		ret = no_os_gpio_direction_output(dev->gpio_in3, NO_OS_GPIO_LOW);
		if (ret)
			goto error_gpio4;

		ret = no_os_gpio_direction_output(dev->gpio_in4, NO_OS_GPIO_LOW);
		if (ret)
			goto error_gpio4;
	}

	/* Wait for power-up time */
	no_os_mdelay(ADGM3121_POWER_UP_TIME_MS);

	/* Reset all switches to off state */
	ret = adgm3121_reset_switches(dev);
	if (ret) {
		if (dev->mode == ADGM3121_MODE_SPI)
			goto error_spi;
		else
			goto error_gpio4;
	}

	*device = dev;

	return 0;

error_spi:
	if (dev->spi_desc)
		no_os_spi_remove(dev->spi_desc);
	goto error_pin_spi;

error_gpio4:
	if (dev->gpio_in4)
		no_os_gpio_remove(dev->gpio_in4);
error_gpio3:
	if (dev->gpio_in3)
		no_os_gpio_remove(dev->gpio_in3);
error_gpio2:
	if (dev->gpio_in2)
		no_os_gpio_remove(dev->gpio_in2);
error_gpio1:
	if (dev->gpio_in1)
		no_os_gpio_remove(dev->gpio_in1);
error_pin_spi:
	no_os_gpio_remove(dev->gpio_pin_spi);
error_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free resources allocated by adgm3121_init()
 * @param dev - The device structure
 * @return 0 in case of success, negative error code otherwise
 */
int adgm3121_remove(struct adgm3121_dev *dev)
{
	if (!dev)
		return -EINVAL;

	/* Turn off all switches before removing */
	adgm3121_reset_switches(dev);

	/* Free mode-specific resources */
	if (dev->mode == ADGM3121_MODE_SPI) {
		if (dev->spi_desc)
			no_os_spi_remove(dev->spi_desc);
	} else {
		if (dev->gpio_in1)
			no_os_gpio_remove(dev->gpio_in1);
		if (dev->gpio_in2)
			no_os_gpio_remove(dev->gpio_in2);
		if (dev->gpio_in3)
			no_os_gpio_remove(dev->gpio_in3);
		if (dev->gpio_in4)
			no_os_gpio_remove(dev->gpio_in4);
	}

	/* Free common resources */
	if (dev->gpio_pin_spi)
		no_os_gpio_remove(dev->gpio_pin_spi);

	no_os_free(dev);

	return 0;
}