/***************************************************************************//**
 *   @file   ltc3208.c
 *   @brief  Implementation of LTC3208 Driver
 *   @author Edelweise Escala (edelweise.escala@analog.com)
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
#include "ltc3208.h"
#include "no_os_i2c.h"
#include "no_os_util.h"
#include "no_os_delay.h"

/***************************************************************************//**
 * @brief Initializes the LTC3208 device structure.
 * @param device - The device structure to initialize.
 * @param init_param - The initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3208_init(struct ltc3208_dev **device,
		 struct ltc3208_init_param init_param)
{
	struct ltc3208_dev *dev;
	int ret;

	dev = (struct ltc3208_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_i2c_init(&dev->i2c_desc, &init_param.i2c_init_param);
	if (ret)
		goto ltc3208_init_err;

	ret = no_os_gpio_get(&dev->gpio_enrgbs_desc, &init_param.gpio_init_param);
	if (ret)
		goto ltc3208_init_err;

	ret = no_os_gpio_direction_output(dev->gpio_enrgbs_desc, NO_OS_GPIO_HIGH);
	if (ret)
		goto ltc3208_init_err;

	*device = dev;

	return 0;

ltc3208_init_err:
	ltc3208_remove(dev);
	return ret;
}

/***************************************************************************//**
 * @brief Deallocates the resources for the device structure.
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3208_remove(struct ltc3208_dev *device)
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

	if (device->gpio_enrgbs_desc) {
		ret = no_os_gpio_remove(device->gpio_enrgbs_desc);
		if (ret)
			return ret;

		device->gpio_enrgbs_desc = NULL;
	}

	no_os_free(device);

	return 0;
}

/***************************************************************************//**
 * @brief Writes to the selected register on the device.
 * @param device - The device structure.
 * @param reg_addr - The register sub-address.
 * @param reg_data - The data to write into the register (8-bits wide)
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3208_reg_write(struct ltc3208_dev *device, uint8_t reg_addr,
		      uint8_t reg_data)
{
	uint8_t data_buffer[2] = {0};

	if (reg_addr > LTC3208_REG_OPTIONS)
		return -EINVAL;

	data_buffer[0] = reg_addr;
	data_buffer[1] = reg_data;

	return no_os_i2c_write(device->i2c_desc, data_buffer, 2, 1);
}

/***************************************************************************//**
 * @brief Resets all register of the device.
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3208_reset(struct ltc3208_dev *device)
{
	int ret;

	ret = ltc3208_set_8_bit_dac(device, MAIN_REG, 0);
	if (ret)
		return ret;

	ret = ltc3208_set_8_bit_dac(device, SUB_REG, 0);
	if (ret)
		return ret;

	ret = ltc3208_set_4_bit_dac(device, GREEN_RED_REG, 0, 0);
	if (ret)
		return ret;

	ret = ltc3208_set_4_bit_dac(device, AUX_BLUE_REG, 0, 0);
	if (ret)
		return ret;

	ret = ltc3208_set_4_bit_dac(device, CAM_REG, 0, 0);
	if (ret)
		return ret;

	ret = ltc3208_update_options(device, 0, 0, 0, 0, 0);
	if (ret)
		return ret;

	return 0;
}

/***************************************************************************//**
 * @brief Sets the selected LED register's current strength
 * @param device - The device structure.
 * @param reg_selected - The enumeration value of the selected register.
 * @param current_level - The current level for the LED group with 256 levels
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3208_set_8_bit_dac(struct ltc3208_dev *device,
			  enum reg_options reg_selected,
			  uint8_t current_level)
{
	uint8_t data;
	uint8_t reg_addr;

	data = no_os_field_prep(LTC3208_8_BIT_MASK, current_level);

	switch (reg_selected) {
	case MAIN_REG:
		reg_addr = LTC3208_REG_MAIN;
		break;
	case SUB_REG:
		reg_addr = LTC3208_REG_SUB;
		break;
	default:
		return -EINVAL;
	}

	return ltc3208_reg_write(device, reg_addr, data);
}

/***************************************************************************//**
 * @brief Sets the selected LED register's current strength
 * @param device - The device structure.
 * @param reg_selected - The enumeration value of the selected register.
 * @param current_level_high - The current level for the high bits with 16 levels
 * @param current_level_low - The current level for the low bits with 16 levels
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3208_set_4_bit_dac(struct ltc3208_dev *device,
			  enum reg_options reg_selected,
			  uint8_t current_level_high,
			  uint8_t current_level_low)
{
	uint8_t data;
	uint8_t reg_addr;

	data = no_os_field_prep(LTC3208_4_BIT_HIGH_MASK, current_level_high) |
	       no_os_field_prep(LTC3208_4_BIT_LOW_MASK, current_level_low);

	switch (reg_selected) {
	case GREEN_RED_REG:
		reg_addr = LTC3208_REG_RED_GREEN;
		break;
	case AUX_BLUE_REG:
		reg_addr = LTC3208_REG_BLUE_AUX;
		break;
	case CAM_REG:
		reg_addr = LTC3208_REG_CAM_HIGH_LOW;
		break;
	default:
		return -EINVAL;
	}

	return ltc3208_reg_write(device, reg_addr, data);
}

/***************************************************************************//**
 * @brief Sets the DAC settings for each AUX LED
 * @param device - The device structure.
 * @param aux_dac_1 - The enumeration value of the selected DAC for AUX 1.
 * @param aux_dac_2 - The enumeration value of the selected DAC for AUX 2.
 * @param aux_dac_3 - The enumeration value of the selected DAC for AUX 3.
 * @param aux_dac_4 - The enumeration value of the selected DAC for AUX 4.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3208_set_all_aux_led_dac(struct ltc3208_dev *device,
				enum dac_options aux_dac_1,
				enum dac_options aux_dac_2,
				enum dac_options aux_dac_3,
				enum dac_options aux_dac_4)
{
	int ret;
	uint8_t data;

	data = no_os_field_prep(LTC3208_AUX1_MASK, aux_dac_1) |
	       no_os_field_prep(LTC3208_AUX2_MASK, aux_dac_2) |
	       no_os_field_prep(LTC3208_AUX3_MASK, aux_dac_3) |
	       no_os_field_prep(LTC3208_AUX4_MASK, aux_dac_4);

	ret = ltc3208_reg_write(device, LTC3208_REG_AUX_DAC_SEL, data);
	if (ret)
		return ret;

	device->aux_dac[3] = aux_dac_4;
	device->aux_dac[2] = aux_dac_3;
	device->aux_dac[1] = aux_dac_2;
	device->aux_dac[0] = aux_dac_1;
	return 0;
}

/***************************************************************************//**
 * @brief Sets the DAC settings for a specific AUX LED
 * @param device - The device structure.
 * @param aux_dac - The enumeration value of the selected DAC for AUX.
 * @param aux_index - The auxillary led index (0..3).
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3208_set_aux_led_dac(struct ltc3208_dev *device,
			    enum dac_options aux_dac,
			    int aux_index)
{
	int i;

	enum dac_options aux_dac_values[4];

	for	(i = 0; i < 4; i++) {
		if (i == aux_index)
			aux_dac_values[i] = aux_dac;
		else
			aux_dac_values[i] = device->aux_dac[i];
	}

	return ltc3208_set_all_aux_led_dac(device, aux_dac_values[0], aux_dac_values[1],
					   aux_dac_values[2], aux_dac_values[3]);
}

/***************************************************************************//**
 * @brief Updates the options set on the device
 * @param device - The device's structure.
 * @param is_force_2x - Signals the device to use 2x voltage.
 * @param is_force_1p5x - Sets the device to use 1.5x voltage.
 * @param is_dropout_disable - Sets the device to use dropout.
 * @param is_cam_high_enable - Sets the device to enable camera high current.
 * @param is_sub_enable - Sets the device to enable rgb.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3208_update_options(struct ltc3208_dev *device, bool is_force_2x,
			   bool is_force_1p5x, bool is_dropout_disable,
			   bool is_cam_high_enable, bool is_sub_enable)
{
	int ret;
	uint8_t data;

	data = no_os_field_prep(LTC3208_CPO_2X_BIT, is_force_2x) |
	       no_os_field_prep(LTC3208_CPO_1P5X_BIT, is_force_1p5x) |
	       no_os_field_prep(LTC3208_TEST_HOOK_1_MASK, 0) |
	       no_os_field_prep(LTC3208_TEST_HOOK_2_MASK, 0) |
	       no_os_field_prep(LTC3208_DROPOUT_BIT, is_dropout_disable) |
	       no_os_field_prep(LTC3208_CAM_BIT, is_cam_high_enable) |
	       no_os_field_prep(LTC3208_RGB_BIT, is_sub_enable);

	ret = ltc3208_reg_write(device, LTC3208_REG_OPTIONS, data);

	if (ret)
		return ret;

	device->options_cfg.is_force_2x = is_force_2x;
	device->options_cfg.is_force_1p5x = is_force_1p5x;
	device->options_cfg.is_dropout_disable = is_dropout_disable;
	device->options_cfg.is_cam_high = is_cam_high_enable;
	device->options_cfg.is_sub_enable = is_sub_enable;
	return 0;
}

/***************************************************************************//**
 * @brief Sets the device's charge pump to output 2x of operating voltage
 * @param device - The device structure.
 * @param is_force_2x - Signals the device to use 2x voltage.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3208_set_cpo_2x(struct ltc3208_dev *device, bool is_force_2x)
{
	return ltc3208_update_options(device,
				      is_force_2x,
				      device->options_cfg.is_force_1p5x,
				      device->options_cfg.is_dropout_disable,
				      device->options_cfg.is_cam_high,
				      device->options_cfg.is_sub_enable);
}

/***************************************************************************//**
 * @brief Sets the device's charge pump to output 1.5x of operating voltage
 * @param device - The device structure.
 * @param is_force_1p5x - Sets the device to use 1.5x voltage.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3208_set_1p5x(struct ltc3208_dev *device, bool is_force_1p5x)
{
	return ltc3208_update_options(device,
				      device->options_cfg.is_force_2x,
				      is_force_1p5x,
				      device->options_cfg.is_dropout_disable,
				      device->options_cfg.is_cam_high,
				      device->options_cfg.is_sub_enable);
}

/***************************************************************************//**
 * @brief Enables RGB and AUX4 dropout signals
 * @param device - The device structure.
 * @param is_dropout_disable - Sets the device to disable RGB and AUX4 dropout signals.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3208_set_droupout_disable(struct ltc3208_dev *device,
				 bool is_dropout_disable)
{
	return ltc3208_update_options(device,
				      device->options_cfg.is_force_2x,
				      device->options_cfg.is_force_1p5x,
				      is_dropout_disable,
				      device->options_cfg.is_cam_high,
				      device->options_cfg.is_sub_enable);
}

/***************************************************************************//**
 * @brief Selects which CAM register to use
 * @param device - The device structure.
 * @param is_cam_high_enable - Sets the device to enable camera high current.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3208_set_cam_high(struct ltc3208_dev *device, bool is_cam_high_enable)
{
	return ltc3208_update_options(device,
				      device->options_cfg.is_force_2x,
				      device->options_cfg.is_force_1p5x,
				      device->options_cfg.is_dropout_disable,
				      is_cam_high_enable,
				      device->options_cfg.is_sub_enable);
}

/***************************************************************************//**
 * @brief Selects if RGB or SUB is controled by ENRGBS pin
 * @param device - The device structure.
 * @param is_sub_enable - Sets the device to enable SUB displays.
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc3208_set_sub_enable(struct ltc3208_dev *device, bool is_sub_enable)
{
	return ltc3208_update_options(device,
				      device->options_cfg.is_force_2x,
				      device->options_cfg.is_force_1p5x,
				      device->options_cfg.is_dropout_disable,
				      device->options_cfg.is_cam_high,
				      is_sub_enable);
}
