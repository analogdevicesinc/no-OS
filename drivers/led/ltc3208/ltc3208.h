/***************************************************************************//**
 *   @file   ltc3208.h
 *   @brief  Header file of LTC3208 Driver
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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __LTC3208_H__
#define __LTC3208_H__

#include "no_os_i2c.h"
#include "no_os_gpio.h"
#include "no_os_util.h"

/* ltc3208 register addresses */
#define LTC3208_REG_RED_GREEN				0x01
#define LTC3208_REG_BLUE_AUX				0x02
#define LTC3208_REG_MAIN					0x03
#define LTC3208_REG_SUB						0X04
#define LTC3208_REG_AUX_DAC_SEL				0x05
#define LTC3208_REG_CAM_HIGH_LOW			0x06
#define LTC3208_REG_OPTIONS					0x07

#define LTC3208_8_BIT_MASK					NO_OS_GENMASK(7,0)
#define LTC3208_4_BIT_HIGH_MASK				NO_OS_GENMASK(7,4)
#define LTC3208_4_BIT_LOW_MASK				NO_OS_GENMASK(3,0)

/* aux dac select register (0x05) */
#define LTC3208_AUX4_MASK					NO_OS_GENMASK(7,6)
#define LTC3208_AUX3_MASK					NO_OS_GENMASK(5,4)
#define LTC3208_AUX2_MASK					NO_OS_GENMASK(3,2)
#define LTC3208_AUX1_MASK					NO_OS_GENMASK(1,0)

/* options register (0x07) */
#define LTC3208_CPO_2X_BIT					NO_OS_BIT(7)
#define LTC3208_CPO_1P5X_BIT				NO_OS_BIT(6)
#define LTC3208_TEST_HOOK_1_MASK			NO_OS_BIT(5)
#define LTC3208_TEST_HOOK_2_MASK			NO_OS_BIT(4)
#define LTC3208_DROPOUT_BIT					NO_OS_BIT(3)
#define LTC3208_CAM_BIT						NO_OS_BIT(2)
#define LTC3208_RGB_BIT						NO_OS_BIT(1)

enum dac_options {
	AUX,
	MAIN,
	SUB,
	CAM
};

enum reg_options {
	GREEN_RED_REG,
	AUX_BLUE_REG,
	MAIN_REG,
	SUB_REG,
	CAM_REG
};

struct ltc3208_options_cfg {
	bool is_force_2x;
	bool is_force_1p5x;
	bool is_dropout_disable;
	bool is_cam_high;
	bool is_sub_enable;
};

struct ltc3208_dev {
	enum dac_options aux_dac[4];
	struct no_os_i2c_desc *i2c_desc;
	struct no_os_gpio_desc *gpio_enrgbs_desc;
	struct ltc3208_options_cfg options_cfg;
};

struct ltc3208_init_param {
	struct no_os_i2c_init_param i2c_init_param;
	struct no_os_gpio_init_param gpio_init_param;
};

/**
 * @brief Initializes the LTC3208 device structure.
 * @param device - The device structure to initialize.
 * @param init_param - The initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3208_init(struct ltc3208_dev **device,
		 struct ltc3208_init_param init_param);

/**
 * @brief Deallocates the resources for the device structure.
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3208_remove(struct ltc3208_dev *device);

/**
 * @brief Writes to the selected register on the device.
 * @param device - The device structure.
 * @param reg_addr - The register sub-address.
 * @param reg_data - The data to write into the register (8-bits wide)
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3208_reg_write(struct ltc3208_dev *device, uint8_t reg_addr,
		      uint8_t reg_data);

/**
 * @brief Resets all register of the device.
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3208_reset(struct ltc3208_dev *device);

/**
 * @brief Sets the selected 8 bit LED register's current strength
 * @param device - The device structure.
 * @param reg_selected - The enumeration value of the selected register.
 * @param current_level - The current level for the LED group with 256 levels
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3208_set_8_bit_dac(struct ltc3208_dev *device,
			  enum reg_options reg_selected,
			  uint8_t current_level);

/**
 * @brief Sets the selected 4 bit LED register's current strength
 * @param device - The device structure.
 * @param reg_selected - The enumeration value of the selected register.
 * @param current_level_high - The current level for the high bits with 16 levels
 * @param current_level_low - The current level for the low bits with 16 levels
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3208_set_4_bit_dac(struct ltc3208_dev *device,
			  enum reg_options reg_selected,
			  uint8_t current_level_high,
			  uint8_t current_level_low);

/**
 * @brief Sets the DAC settings for each AUX LED
 * @param device - The device structure.
 * @param aux_dac_1 - The enumeration value of the selected DAC for AUX 1.
 * @param aux_dac_2 - The enumeration value of the selected DAC for AUX 2.
 * @param aux_dac_3 - The enumeration value of the selected DAC for AUX 3.
 * @param aux_dac_4 - The enumeration value of the selected DAC for AUX 4.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3208_set_all_aux_led_dac(struct ltc3208_dev *device,
				enum dac_options aux_dac_1,
				enum dac_options aux_dac_2,
				enum dac_options aux_dac_3,
				enum dac_options aux_dac_4);
/**
 * @brief Sets the DAC settings for a specific AUX LED
 * @param device - The device structure.
 * @param aux_dac - The enumeration value of the selected DAC for AUX.
 * @param aux_index - The auxillary led index (0..3).
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3208_set_aux_led_dac(struct ltc3208_dev *device,
			    enum dac_options aux_dac,
			    int aux_index);

/**
 * @brief Updates the options set on the device
 * @param device - The device's structure.
 * @param is_force_2x - Signals the device to use 2x voltage.
 * @param is_force_1p5x - Sets the device to use 1.5x voltage.
 * @param is_dropout_disable - Sets the device to disable RGB and AUX4 dropout signals.
 * @param is_cam_high_enable - Sets the device to enable camera high current.
 * @param is_sub_enable - Sets the device to enable SUB displays.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3208_update_options(struct ltc3208_dev *device, bool is_force_2x,
			   bool is_force_1p5x, bool is_dropout_disable,
			   bool is_cam_high_enable, bool is_sub_enable);

/**
 * @brief Sets the device's charge pump to output 2x of operating voltage
 * @param device - The device structure.
 * @param is_force_2x - Signals the device to use 2x voltage.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3208_set_cpo_2x(struct ltc3208_dev *device, bool is_force_2x);

/**
 * @brief Sets the device's charge pump to output 1.5x of operating voltage
 * @param device - The device structure.
 * @param is_force_1p5x - Sets the device to use 1.5x voltage.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3208_set_1p5x(struct ltc3208_dev *device, bool is_force_1p5x);

/**
 * @brief Enables RGB and AUX4 dropout signals
 * @param device - The device structure.
 * @param is_dropout_disable - Sets the device to disable RGB and AUX4 dropout signals.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3208_set_droupout_disable(struct ltc3208_dev *device,
				 bool is_dropout_disable);

/**
 * @brief Selects which CAM register to use
 * @param device - The device structure.
 * @param is_cam_high_enable - Sets the device to enable camera high current.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3208_set_cam_high(struct ltc3208_dev *device, bool is_cam_high_enable);

/**
 * @brief Selects if RGB or SUB is controled by ENRGBS pin
 * @param device - The device structure.
 * @param is_sub_enable - Sets the device to enable SUB displays.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3208_set_sub_enable(struct ltc3208_dev *device, bool is_sub_enable);


#endif  /*__LTC3208_H__*/

#ifdef __cplusplus
}
#endif
