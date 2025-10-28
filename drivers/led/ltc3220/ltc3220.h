/***************************************************************************//**
 *   @file   ltc3220.h
 *   @brief  Header file of LTC3220 Driver
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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __LTC3220_H__
#define __LTC3220_H__

#include "no_os_i2c.h"
#include "no_os_gpio.h"
#include "no_os_util.h"

/*ltc3220 register addresses*/
#define LTC3220_REG_COMMAND					0x00
#define LTC3220_REG_START_ULED				0x01 /*ULED1 */
#define LTC3220_REG_END_ULED				0x12 /*ULED18*/
#define LTC3220_REG_BLINK_GRAD				0x13

/*command register (0x00) */
#define LTC3220_COMMAND_QUICK_WRITE_MASK	NO_OS_BIT(0)
#define LTC3220_COMMAND_1P5X_MASK			NO_OS_BIT(1)
#define LTC3220_COMMAND_2X_MASK				NO_OS_BIT(2)
#define LTC3220_COMMAND_SHUTDOWN			NO_OS_BIT(3)

/* ULEDxx registers (0x01-0x12) */
#define LTC3220_ULED_MODE_MASK				NO_OS_GENMASK(7,6)
#define LTC3220_ULED_CURRENT_MASK			NO_OS_GENMASK(5,0)
#define LTC3220_ULED_CURRENT_MAX_STEP		64

/* Blink/Gradation (0x13) */
#define LTC3220_BLINK_LONG_MASK				NO_OS_BIT(4)
#define LTC3220_BLINK_FAST_MASK				NO_OS_BIT(3)
#define LTC3220_GRAD_SPD_MASK				NO_OS_GENMASK(2,1)
#define LTC3220_GRAD_DIRECTION_MASK			NO_OS_BIT(0)
#define LTC3220_GRAD_MAX_SPD				3

/* Reset timing */
#define LTC3220_RESET_DELAY_USEC			1 /*20ns min */

enum ltc3220_variant {
	LTC3220 = 0x1C,
	LTC3220_1 = 0x1D
};

enum ltc3220_uled_mode {
	LTC3220_MODE_NORMAL = 0,
	LTC3220_MODE_BLINK,
	LTC3220_MODE_GRADATION,
	LTC3220_MODE_GPO
};

struct ltc3220_command_cfg {
	bool is_quick_write;
	bool is_force_cpo_1p5x;
	bool is_force_cpo_2x;
	bool is_shutdown;
};

static const struct ltc3220_command_cfg command_reset;

struct ltc3220_uled_cfg {
	enum ltc3220_uled_mode mode;
	uint8_t current_level;
};

static const struct ltc3220_uled_cfg uled_reset;

struct ltc3220_blink_cfg {
	bool is_fast_on;
	bool is_long_period;
};

static const struct ltc3220_blink_cfg blink_reset;

struct ltc3220_grad_cfg {
	uint8_t speed;
	bool is_increasing;
};

static const struct ltc3220_grad_cfg grad_reset;

struct ltc3220_dev {
	struct no_os_i2c_desc *i2c_desc;

	struct no_os_gpio_desc *gpio_rst_desc;

	/* Contains configuration of each ULED */
	struct ltc3220_uled_cfg uleds[LTC3220_REG_END_ULED];

	/* Contains configuration of device */
	struct ltc3220_command_cfg command_config;

	/* Blink mode configuration */
	struct ltc3220_blink_cfg blink_config;

	/* Gradation mode configuration */
	struct ltc3220_grad_cfg gradation_config;
};

struct ltc3220_init_param {
	struct no_os_i2c_init_param i2c_init_param;
	struct no_os_gpio_init_param gpio_init_param;	/* for device reset */
};

/**
 * @brief Initializes the LTC3220 device structure.
 * @param device - The device structure to initialize.
 * @param init_param - The initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3220_init(struct ltc3220_dev **device,
		 struct ltc3220_init_param init_param);

/**
 * @brief Deallocates the resources for the device structure.
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3220_remove(struct ltc3220_dev *device);

/**
 * @brief Resets the device using the RST pin
 * @param device - the device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3220_reset(struct ltc3220_dev *device);

/**
 * @brief Writes to the selected register on the device.
 * @param device - The device structure.
 * @param reg_addr - The register sub-address.
 * @param reg_data - The data to write into the register (8-bits wide)
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3220_reg_write(struct ltc3220_dev *device, uint8_t reg_addr,
		      uint8_t reg_data);

/**
 * @brief Sets the selected ULED register's operating mode
 * @param device - The device structure.
 * @param uled_number - The ULED to select (1-indexed).
 * @param mode - The mode to set the ULED to.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3220_set_uled_mode(struct ltc3220_dev *device, uint8_t uled_number,
			  enum ltc3220_uled_mode mode);

/**
 * @brief Sets the selected ULED register's current strength
 * @param device - The device structure.
 * @param uled_number - The ULED to select (1-indexed).
 * @param current_level - The current strength from 0 to 20mA to set on the corresponding ULED pin (64 levels)
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3220_set_uled_current(struct ltc3220_dev *device, uint8_t uled_number,
			     uint8_t current_level);

/**
 * @brief Updates the configuration set on the device's ULED based on the config
 * @param device - The device structure.
 * @param uled_number - The ULED to select (1-indexed).
 * @param mode - The mode to set the ULED to.
 * @param current_level - The current strength from 0 to 20mA to set on the corresponding ULED pin (64 levels)
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3220_update_reg_uled(struct ltc3220_dev *device, uint8_t uled_number,
			    enum ltc3220_uled_mode mode, uint8_t current_level);

/**
 * @brief Sets the configuration for blinking mode of ULEDs.
 * @param device - The device structure.
 * @param is_fast_on - Sets whether blinking should turn on fast (0.156s) or not (0.625s)
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3220_set_blink_fast(struct ltc3220_dev *device, bool is_fast_on);

/**
 * @brief Sets the configuration for blinking mode of ULEDs.
 * @param device - The device structure.
 * @param is_long_period - Sets whether blinking should have a shorter (1.25s) or longer (2.5s) period
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3220_set_blink_long(struct ltc3220_dev *device, bool is_long_period);

/**
 * @brief Sets the configuration of the device's speed of gradation
 * @param device - The device structure.
 * @param speed - Sets the speed of gradation, with slowest speed at max setting (3). Set to 0 to disable gradation.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3220_set_grad_speed(struct ltc3220_dev *device, uint8_t speed);

/**
 * @brief Sets the configuration for the device's direction of gradation
 * @param device - The device structure.
 * @param is_increasing - Set for increasing gradation, unset if decreasing gradation.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3220_set_grad_increasing(struct ltc3220_dev *device, bool is_increasing);

/**
 * @brief Updates the configuration set on the device's Blink/Gradation based on the config
 * @param device - The device's structure.
 * @param is_blink_fast - Sets whether blinking should turn on fast (0.156s) or not (0.625s)
 * @param is_blink_long - Sets whether blinking should have a shorter (1.25s) or longer (2.5s) period
 * @param is_grad_inc - Set for increasing gradation, unset if decreasing gradation.
 * @param grad_speed - Sets the speed of gradation, with slowest speed at max setting (3). Set to 0 to disable gradation.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3220_update_reg_blink_grad(struct ltc3220_dev *device,
				  bool is_blink_fast, bool is_blink_long, bool is_grad_inc,
				  uint8_t grad_speed);

/**
 * @brief Sets the configuration to perform a quick write to the device
 * @param device - The device structure.
 * @param is_quick_write - Set for quick write operation. This copies the configuration set on ULED1 to all other ULED registers.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3220_set_quick_write(struct ltc3220_dev *device, bool is_quick_write);

/**
 * @brief Sets the device's charge pump to output 1x of operating voltage
 * @param device - The device structure.
 * @param is_force_cpo_1x - Sets the device's to use 1x voltage.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3220_set_cpo_1x(struct ltc3220_dev *device, bool is_force_cpo_1x);

/**
 * @brief Sets the device's charge pump to output 1.5x of operating voltage
 * @param device - The device structure.
 * @param is_force_cpo_1p5x - Sets the device's to use 1.5x voltage.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3220_set_cpo_1p5x(struct ltc3220_dev *device, bool is_force_cpo_1p5x);

/**
 * @brief Sets the device's charge pump to output 2x of operating voltage
 * @param device - The device structure.
 * @param is_force_cpo_2x - Sets the device's to use 2x voltage.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3220_set_cpo_2x(struct ltc3220_dev *device, bool is_force_cpo_2x);

/**
 * @brief Sets the device to shutdown mode, while retaining the register configurations
 * @param device - The device structure.
 * @param is_shutdown - Signals the device to shutdown.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3220_set_shutdown(struct ltc3220_dev *device, bool is_shutdown);


/**
 * @brief Updates the configuration set on the
 * @param device - The device's structure.
 * @param is_shutdown - Signals the device to shutdown.
 * @param is_force_cpo_1p5x - Sets the device's to use 1.5x voltage.
 * @param is_force_cpo_2x - Sets the device's to use 2x voltage.
 * @param is_quick_write - Set for quick write operation. This copies the configuration set on ULED1 to all other ULED registers.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc3220_update_reg_command(struct ltc3220_dev *device, bool is_shutdown,
			       bool is_force_cpo_2x, bool is_force_cpo_1p5x,
			       bool is_quick_write);

#endif  /*__LTC3220_H__*/

#ifdef __cplusplus
}
#endif
