/***************************************************************************//**
 *   @file   ltc3220.c
 *   @brief  Implementation of LTC3220 Driver
 *   @author Jan Carlo Roleda (Jancarlo.roleda@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "ltc3220.h"
#include "no_os_i2c.h"
#include "no_os_util.h"
#include "no_os_delay.h"

/***************************************************************************//**
 * @brief Initializes the LTC3220 device structure.
 * @param device - The device structure to initialize.
 * @param init_param - The initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3220_init(struct ltc3220_dev **device,
		 struct ltc3220_init_param init_param)
{
	struct ltc3220_dev *dev;
	int ret;

	dev = (struct ltc3220_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_i2c_init(&dev->i2c_desc, &init_param.i2c_init_param);
	if (ret)
		goto ltc3220_init_err;

	ret = no_os_gpio_get(&dev->gpio_rst_desc, &init_param.gpio_init_param);
	if (ret)
		goto ltc3220_init_err;

	ret = no_os_gpio_direction_output(dev->gpio_rst_desc, NO_OS_GPIO_HIGH);
	if (ret)
		goto ltc3220_init_err;

	*device = dev;

	return 0;

ltc3220_init_err:
	ltc3220_remove(dev);
	return ret;
}

/***************************************************************************//**
 * @brief Deallocates the resources for the device structure.
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3220_remove(struct ltc3220_dev *device)
{
	int ret;

	if (!device)
		return -ENODEV;

	if (device->i2c_desc) {
		ret = no_os_i2c_remove(device->i2c_desc);
		if (ret)
			return ret;
		device->i2c_desc = NULL;
	}

	if (device->gpio_rst_desc) {
		ret = no_os_gpio_remove(device->gpio_rst_desc);
		if (ret)
			return ret;
		device->gpio_rst_desc = NULL;
	}

	no_os_free(device);

	return 0;
}

/***************************************************************************//**
 * @brief Resets the device using the RST pin
 * @param device - the device structure.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3220_reset(struct ltc3220_dev *device)
{
	int ret;

	ret = no_os_gpio_set_value(device->gpio_rst_desc, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	no_os_udelay(LTC3220_RESET_DELAY_USEC); /* at least 20ns */

	ret = no_os_gpio_set_value(device->gpio_rst_desc, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	device->blink_config = blink_reset;
	device->command_config = command_reset;
	device->gradation_config = grad_reset;

	for (int i = 0; i < LTC3220_REG_END_ULED; i++)
		device->uleds[i] = uled_reset;

	return 0;
}

/***************************************************************************//**
 * @brief Writes to the selected register on the device.
 * @param device - The device structure.
 * @param reg_addr - The register sub-address.
 * @param reg_data - The data to write into the register (8-bits wide)
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3220_reg_write(struct ltc3220_dev *device, uint8_t reg_addr,
		      uint8_t reg_data)
{
	uint8_t data_buffer[2] = {0};

	if (reg_addr > LTC3220_REG_BLINK_GRAD)
		return -EINVAL;

	data_buffer[0] = reg_addr;
	data_buffer[1] = reg_data;

	return no_os_i2c_write(device->i2c_desc, data_buffer, 2, 1);
}

/***************************************************************************//**
 * @brief Sets the selected ULED register's operating mode
 * @param device - The device structure.
 * @param uled_number - The ULED to select (1-indexed).
 * @param mode - The mode to set the ULED to.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3220_set_uled_mode(struct ltc3220_dev *device, uint8_t uled_number,
			  enum ltc3220_uled_mode mode)
{
	int ret;

	if (uled_number < LTC3220_REG_START_ULED ||
	    uled_number > LTC3220_REG_END_ULED)
		return -EINVAL;

	ret = ltc3220_update_reg_uled(device, uled_number, mode,
				      device->uleds[uled_number - 1].current_level);
	if (ret)
		return ret;

	device->uleds[uled_number - 1].mode = mode;

	return 0;
}

/***************************************************************************//**
 * @brief Sets the selected ULED register's current strength
 * @param device - The device structure.
 * @param uled_number - The ULED to select (1-indexed).
 * @param current_level - The current strength from 0 to 20mA to set on the
 * corresponding ULED pin (64 levels)
*******************************************************************************/
int ltc3220_set_uled_current(struct ltc3220_dev *device, uint8_t uled_number,
			     uint8_t current_level)
{
	int ret;

	if (uled_number < LTC3220_REG_START_ULED ||
	    uled_number > LTC3220_REG_END_ULED)
		return -EINVAL;

	ret = ltc3220_update_reg_uled(device, uled_number,
				      device->uleds[uled_number - 1].mode, current_level);
	if (ret)
		return ret;

	device->uleds[uled_number - 1].current_level = current_level;

	return 0;
}

/***************************************************************************//**
 * @brief Updates the configuration set on the ULED based on the device's
 * config
 * @param device - The device structure.
 * @param uled_number - The ULED to select (1-indexed).
 * @param mode - The mode to set the ULED to.
 * @param current_level - The current strength from 0 to 20mA to set on the
 * corresponding ULED pin (64 levels)
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3220_update_reg_uled(struct ltc3220_dev *device, uint8_t uled_number,
			    enum ltc3220_uled_mode mode, uint8_t current_level)
{
	uint8_t data, quick_mode, quick_current;

	if (uled_number < LTC3220_REG_START_ULED ||
	    uled_number > LTC3220_REG_END_ULED)
		return -EINVAL;

	/* Copy config of ULED1 to all other ULEDs */
	if (device->command_config.is_quick_write
	    && uled_number == LTC3220_REG_START_ULED) {

		quick_mode = device->uleds[0].mode;
		quick_current = device->uleds[0].current_level;

		for (int uled = LTC3220_REG_START_ULED;
		     uled < LTC3220_REG_END_ULED;
		     uled++) {
			device->uleds[uled].mode = quick_mode;
			device->uleds[uled].current_level = quick_current;
		}
	}

	data = no_os_field_prep(LTC3220_ULED_MODE_MASK, mode) |
	       no_os_field_prep(LTC3220_ULED_CURRENT_MASK, current_level);

	return ltc3220_reg_write(device, uled_number, data);
}

/***************************************************************************//**
 * @brief Sets the configuration for blinking mode of ULEDs.
 * @param device - The device structure.
 * @param is_fast_on - Sets whether blinking should turn on fast (0.156s)
 * or not (0.625s)
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3220_set_blink_fast(struct ltc3220_dev *device, bool is_fast_on)
{
	int ret;

	ret = ltc3220_update_reg_blink_grad(device, is_fast_on,
					    device->blink_config.is_long_period,
					    device->gradation_config.is_increasing,
					    device->gradation_config.speed);
	if (ret)
		return ret;

	device->blink_config.is_fast_on = is_fast_on;

	return 0;
}

/***************************************************************************//**
 * @brief Sets the configuration for blinking mode of ULEDs.
 * @param device - The device structure.
 * @param is_long_period - Sets whether blinking should have a shorter (1.25s)
 * or longer (2.5s) period
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3220_set_blink_long(struct ltc3220_dev *device, bool is_long_period)
{
	int ret;

	ret = ltc3220_update_reg_blink_grad(device, device->blink_config.is_fast_on,
					    is_long_period, device->gradation_config.is_increasing,
					    device->gradation_config.speed);
	if (ret)
		return ret;

	device->blink_config.is_long_period = is_long_period;

	return 0;
}

/***************************************************************************//**
 * @brief Sets the configuration for the device's direction of gradation
 * @param device - The device structure.
 * @param is_increasing - Set for increasing gradation, unset if
 * decreasing gradation.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3220_set_grad_increasing(struct ltc3220_dev *device, bool is_increasing)
{
	int ret;

	ret = ltc3220_update_reg_blink_grad(device, device->blink_config.is_fast_on,
					    device->blink_config.is_long_period,
					    is_increasing, device->gradation_config.speed);
	if (ret)
		return ret;

	device->gradation_config.is_increasing = is_increasing;

	return 0;
}

/***************************************************************************//**
 * @brief Sets the configuration of the device's speed of gradation
 * @param device - The device structure.
 * @param speed - Sets the speed of gradation, with slowest speed at
 * max setting (3). Set to 0 to disable gradation.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3220_set_grad_speed(struct ltc3220_dev *device, uint8_t speed)
{
	int ret;

	if (speed > LTC3220_GRAD_MAX_SPD)
		return -EINVAL;

	ret = ltc3220_update_reg_blink_grad(device, device->blink_config.is_fast_on,
					    device->blink_config.is_long_period,
					    device->gradation_config.is_increasing, speed);
	if (ret)
		return ret;

	device->gradation_config.speed = speed;

	return 0;
}

/***************************************************************************//**
 * @brief Updates the configuration set on the device's Blink/Gradation
 * based on the config
 * @param device - The device's structure.
 * @param is_blink_fast - Sets whether blinking should turn on fast (0.156s)
 * or not (0.625s)
 * @param is_blink_long - Sets whether blinking should have a shorter (1.25s)
 * or longer (2.5s) period
 * @param is_grad_inc - Set for increasing gradation, unset if decreasing
 * gradation.
 * @param grad_speed - Sets the speed of gradation, with slowest speed at
 * max setting (3). Set to 0 to disable gradation.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3220_update_reg_blink_grad(struct ltc3220_dev *device,
				  bool is_blink_fast, bool is_blink_long, bool is_grad_inc,
				  uint8_t grad_speed)
{
	uint8_t data = no_os_field_prep(LTC3220_BLINK_LONG_MASK, is_blink_long) |
		       no_os_field_prep(LTC3220_BLINK_FAST_MASK, is_blink_fast) |
		       no_os_field_prep(LTC3220_GRAD_SPD_MASK, grad_speed) |
		       no_os_field_prep(LTC3220_GRAD_DIRECTION_MASK, is_grad_inc);

	return ltc3220_reg_write(device, LTC3220_REG_BLINK_GRAD, data);
}

/***************************************************************************//**
 * @brief Sets the configuration to perform a quick write to the device
 * @param device - The device structure.
 * @param is_quick_write - Set for quick write operation. This copies the
 * configuration set on ULED1 to all other ULED registers.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3220_set_quick_write(struct ltc3220_dev *device, bool is_quick_write)
{
	int ret;

	ret = ltc3220_update_reg_command(device, device->command_config.is_shutdown,
					 device->command_config.is_force_cpo_2x,
					 device->command_config.is_force_cpo_1p5x,
					 is_quick_write);
	if (ret)
		return ret;

	device->command_config.is_quick_write = is_quick_write;

	return 0;
}

/***************************************************************************//**
 * @brief Sets the device's charge pump to output 1x of operating voltage
 * @param device - The device structure.
 * @param is_force_cpo_1x - Set if the device's is to use 1x voltage.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3220_set_cpo_1x(struct ltc3220_dev *device, bool is_force_cpo_1x)
{
	int ret;

	ret = ltc3220_update_reg_command(device, device->command_config.is_shutdown,
					 is_force_cpo_1x, is_force_cpo_1x,
					 device->command_config.is_quick_write);
	if (ret)
		return ret;

	device->command_config.is_force_cpo_1p5x = is_force_cpo_1x;
	device->command_config.is_force_cpo_2x = is_force_cpo_1x;

	return 0;
}

/***************************************************************************//**
 * @brief Sets the device's charge pump to output 1.5x of operating voltage
 * @param device - The device structure.
 * @param is_force_cpo_1p5x - Sets the device's to use 1.5x voltage.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3220_set_cpo_1p5x(struct ltc3220_dev *device, bool is_force_cpo_1p5x)
{
	int ret;

	ret = ltc3220_update_reg_command(device, device->command_config.is_shutdown,
					 false, is_force_cpo_1p5x,
					 device->command_config.is_quick_write);
	if (ret)
		return ret;

	device->command_config.is_force_cpo_1p5x = is_force_cpo_1p5x;
	device->command_config.is_force_cpo_2x = false;

	return 0;
}

/***************************************************************************//**
 * @brief Sets the device's charge pump to output 2x of operating voltage
 * @param device - The device structure.
 * @param is_force_cpo_2x - Sets the device's to use 2x voltage.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3220_set_cpo_2x(struct ltc3220_dev *device, bool is_force_cpo_2x)
{
	int ret;

	ret = ltc3220_update_reg_command(device, device->command_config.is_shutdown,
					 is_force_cpo_2x, false,
					 device->command_config.is_quick_write);
	if (ret)
		return ret;

	device->command_config.is_force_cpo_1p5x = false;
	device->command_config.is_force_cpo_2x = is_force_cpo_2x;

	return 0;
}

/***************************************************************************//**
 * @brief Sets the device to shutdown mode, while retaining the register
 * configurations
 * @param device - The device structure.
 * @param is_shutdown - Signals the device to shutdown.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3220_set_shutdown(struct ltc3220_dev *device, bool is_shutdown)
{
	int ret;

	ret = ltc3220_update_reg_command(device, is_shutdown,
					 device->command_config.is_force_cpo_2x,
					 device->command_config.is_force_cpo_1p5x,
					 device->command_config.is_quick_write);
	if (ret)
		return ret;

	device->command_config.is_shutdown = is_shutdown;

	return 0;
}

/***************************************************************************//**
 * @brief Updates the configuration set on the
 * @param device - The device's structure.
 * @param is_shutdown - Signals the device to shutdown.
 * @param is_force_cpo_1p5x - Sets the device's to use 1.5x voltage.
 * @param is_force_cpo_2x - Sets the device's to use 2x voltage.
 * @param is_quick_write - Set for quick write operation. This copies the
 * configuration set on ULED1 to all other ULED registers.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3220_update_reg_command(struct ltc3220_dev *device, bool is_shutdown,
			       bool is_force_cpo_2x, bool is_force_cpo_1p5x,
			       bool is_quick_write)
{
	uint8_t data, quick_mode, quick_current;
	int ret;

	data = no_os_field_prep(LTC3220_COMMAND_SHUTDOWN, is_shutdown) |
	       no_os_field_prep(LTC3220_COMMAND_1P5X_MASK, is_force_cpo_1p5x) |
	       no_os_field_prep(LTC3220_COMMAND_2X_MASK, is_force_cpo_2x) |
	       no_os_field_prep(LTC3220_COMMAND_QUICK_WRITE_MASK, is_quick_write);

	ret = ltc3220_reg_write(device, LTC3220_REG_COMMAND, data);
	if (ret)
		return ret;

	/* Copy config of ULED1 to all other ULEDs */
	if (is_quick_write) {
		quick_mode = device->uleds[0].mode;
		quick_current = device->uleds[0].current_level;
		for (int uled = LTC3220_REG_START_ULED;
		     uled < LTC3220_REG_END_ULED;
		     uled++) {
			device->uleds[uled].mode = quick_mode;
			device->uleds[uled].current_level = quick_current;
		}
	}

	return 0;
}
