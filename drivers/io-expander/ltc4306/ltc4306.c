/***************************************************************************//**
 *   @file   ltc4306.c
 *   @brief  Implementation of ltc4306 driver.
 *   @author Jose Ramon San Buenaventura (jose.sanbuenaventura@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include <stdint.h>
#include <stdlib.h>
#include "no_os_i2c.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "no_os_alloc.h"
#include "ltc4306.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/* Pin configurable LTC4306 addresses */
const uint8_t ltc4306_addresses[] = {
	0x88,
	0x8E,
	0x8C,
	0xB2,
	0x8A,
	0x82,
	0x80,
	0x86,
	0x84,
	0xA8,
	0xAE,
	0xAC,
	0xB0,
	0xAA,
	0xA2,
	0xA0,
	0xA6,
	0xA4,
	0x98,
	0x9E,
	0x9C,
	0xB4,
	0x9A,
	0x92,
	0x90,
	0x96,
	0x94,
};

/***************************************************************************//**
 * @brief Initializes the communication peripheral and checks if the ltc4306
 *		  part is present.
 *
 * @param device - The device structure.
 * @param init_param - The structure that contains the device initial
 * 		       parameters.
 *
 * @return status - Result of the initialization procedure.
 *                  Example: -1 - I2C peripheral was not initialized or
 *                                ltc4306 part is not present.
 *                            0 - I2C peripheral is initialized and ltc4306
 *                                part is present.
*******************************************************************************/
int ltc4306_init(struct ltc4306_dev **device,
		 struct ltc4306_init_param init_param)
{
	struct ltc4306_dev *dev;
	int ret;

	dev = (struct ltc4306_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_i2c_init(&dev->i2c_desc, &init_param.i2c_init);
	if (ret)
		goto error_dev;

	*device = dev;

	return 0;

error_dev:
	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Free the resources allocated by ltc4306_init().
 *
 * @param dev - The device structure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_remove(struct ltc4306_dev *dev)
{
	int ret;

	ret = no_os_i2c_remove(dev->i2c_desc);

	no_os_free(dev);

	return ret;
}

/***************************************************************************//**
 * @brief Writes data into a register (I2C).
 *
 * @param dev - The device structure.
 * @param addr - Starting / selected register address.
 * @param write_data - Write data buffer
 * @param bytes - number of bytes to write
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_write(struct ltc4306_dev *dev, uint8_t addr, uint8_t *write_data,
		  uint8_t bytes)
{
	int i;
	uint8_t buf[bytes + 1];

	/* Check if valid writable register */
	if (addr < LTC4306_CTRL_REG1 || addr > LTC4306_CTRL_REG3)
		return -EINVAL;

	/* Check if write will go out of bounds*/
	if (addr + bytes > LTC4306_OUT_OF_BOUNDS)
		return -EINVAL;

	buf[0] = addr;

	for (i = 0; i < bytes; i++)
		buf[i + 1] = write_data[i];

	return no_os_i2c_write(dev->i2c_desc, buf, bytes + 1, 1);
}

/***************************************************************************//**
 * @brief Reads the value of a register.
 *
 * @param dev - The device structure.
 * @param addr - Address of the register.
 * @param read_data - Read data bufer
 * @param bytes - Number of bytes to read from starting register address
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_read(struct ltc4306_dev *dev, uint8_t addr, uint8_t *read_data,
		 uint8_t bytes)
{
	int ret;

	/* Check if valid readable register */
	if (addr < LTC4306_CTRL_REG0 || addr > LTC4306_CTRL_REG3)
		return -EINVAL;

	/* Check if read will go out of bounds*/
	if (addr + bytes > LTC4306_OUT_OF_BOUNDS)
		return -EINVAL;

	ret = no_os_i2c_write(dev->i2c_desc, &addr, 1, 0);
	if (ret)
		return ret;

	return no_os_i2c_read(dev->i2c_desc, read_data, bytes, 1);
}

/***************************************************************************//**
 * @brief Update selected I2C register contents
 *
 * @param dev   	  - The device structure.
 * @param addr 		  - Starting / selected register address.
 * @param update_mask - Mask to update to selected register bit/s
 * @param update_val  - Value used to update on selected register
 *
 * @return            - 0 if I2C write is successful, error otherwise.
*******************************************************************************/
int ltc4306_reg_update(struct ltc4306_dev *dev, uint8_t addr, int update_mask,
		       int update_val)
{
	int ret;
	uint8_t temp;

	ret = ltc4306_read(dev, addr, &temp, 1);
	if (ret)
		return ret;

	temp &= ~update_mask;
	temp |= update_val;

	return ltc4306_write(dev, addr, &temp, 1);
}

/***************************************************************************//**
 * @brief Gives the equivalent hex device address based on the input combination
 * 		of addr0, addr1, and addr2. (27 possible device address)
 *
 * @param init_param - init param structure.
 * @param addr0 - addr0 logic level
 * @param addr1 - addr1 logic level
 * @param addr2 - addr2 logic level
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_addr_gen (struct ltc4306_init_param *init_param,
		      enum ltc4306_addr_conn addr0, enum ltc4306_addr_conn addr1,
		      enum ltc4306_addr_conn addr2)
{
	int loc;

	if (addr0 < LTC4306_LOW || addr0 > LTC4306_NO_CONN)
		return -EINVAL;

	if (addr1 < LTC4306_LOW || addr1 > LTC4306_NO_CONN)
		return -EINVAL;

	if (addr2 < LTC4306_LOW || addr2 > LTC4306_NO_CONN)
		return -EINVAL;

	loc = (int)addr0 * 1 + (int)addr1 * 3 + (int)addr2 * 9;

	init_param->i2c_init.slave_address = ltc4306_addresses[loc];
	init_param->i2c_init.slave_address >>= 1;

	return 0;
}

/***************************************************************************//**
 * @brief Get downstream connect bit status to see if any downstream bus connected
 *
 * @param dev - The device structure.
 * @param downstream_conn - Stores status of downstream connect bit.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_downstream_check(struct ltc4306_dev *dev, bool *downstream_conn)
{
	int ret;
	uint8_t temp;

	ret = ltc4306_read(dev, LTC4306_CTRL_REG0, &temp, 1);
	if (ret)
		return ret;

	*downstream_conn = (bool)no_os_field_get(LTC4306_DOWNSTREAM_CONNECT, temp);

	return 0;
}

/***************************************************************************//**
 * @brief Reads Alert Logic State of selected ALERT pin.
 *
 * @param dev - The device structure.
 * @param alert_pin_number - ALERT pin to be read (1 to 4).
 * @param is_high - Stores status of selected ALERT pin.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_read_alert_logic_state(struct ltc4306_dev *dev,
				   uint8_t alert_pin_number, bool *is_high)
{
	int ret;
	uint8_t temp;

	if (alert_pin_number < LTC4306_MIN_CHANNEL_INDEX
	    || alert_pin_number > LTC4306_MAX_CHANNEL_INDEX)
		return -EINVAL;

	ret = ltc4306_read(dev, LTC4306_CTRL_REG0, &temp, 1);
	if (ret)
		return ret;

	*is_high = (bool)no_os_field_get(LTC4306_ALERT_LOGIC(alert_pin_number), temp);

	return 0;
}

/***************************************************************************//**
 * @brief Reads Failed Connection Attempt bit.
 *
 * @param dev - The device structure.
 * @param is_high - Stores status of Failed Connection Attempt bit
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_read_failed_conn(struct ltc4306_dev *dev, bool *is_high)
{
	int ret;
	uint8_t temp;

	ret = ltc4306_read(dev, LTC4306_CTRL_REG0, &temp, 1);
	if (ret)
		return ret;

	*is_high = (bool)no_os_field_get(LTC4306_FAILED_CONN, temp);

	return 0;
}

/***************************************************************************//**
 * @brief Reads Latched Timeout status.
 *
 * @param dev - The device structure.
 * @param timed_out - Stores status of Latched Timeout bit.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_get_latched_timeout(struct ltc4306_dev *dev, bool *timed_out)
{
	int ret;
	uint8_t temp;

	ret = ltc4306_read(dev, LTC4306_CTRL_REG0, &temp, 1);
	if (ret)
		return ret;

	*timed_out = (bool)no_os_field_get(LTC4306_LATCHED_TOUT, temp);

	return 0;
}

/***************************************************************************//**
 * @brief Reads Realtime Timeout status.
 *
 * @param dev - The device structure.
 * @param timed_out - Stores status of Realtime Timeout bit.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_get_realtime_timeout(struct ltc4306_dev *dev, bool *timed_out)
{
	int ret;
	uint8_t temp;

	ret = ltc4306_read(dev, LTC4306_CTRL_REG0, &temp, 1);
	if (ret)
		return ret;

	*timed_out = (bool)no_os_field_get(LTC4306_REALTIME_TOUT, temp);

	return 0;
}

/***************************************************************************//**
 * @brief En/Disables upstream accelerator.
 *
 * @param dev - The device structure.
 * @param upstream_en - Set true to enable upstream rise time accelerator, false otherwise.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_set_upstream_accel(struct ltc4306_dev *dev, bool upstream_en)
{
	dev->upstream = upstream_en;

	if (upstream_en)
		return ltc4306_reg_update(dev, LTC4306_CTRL_REG1, LTC4306_UPSTREAM_EN,
					  no_os_field_prep(LTC4306_UPSTREAM_EN, 1));

	return ltc4306_reg_update(dev, LTC4306_CTRL_REG1, LTC4306_UPSTREAM_EN, 0);
}

/***************************************************************************//**
 * @brief Get status of upstream accelerator enable bit field
 *
 * @param dev - The device structure.
 * @param upstream_en - Stores status of Upstream Connected bit.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_get_upstream_accel(struct ltc4306_dev *dev, bool *upstream_en)
{
	int ret;
	uint8_t temp;

	ret = ltc4306_read(dev, LTC4306_CTRL_REG1, &temp, 1);
	if (ret)
		return ret;

	*upstream_en = (bool)no_os_field_get(LTC4306_UPSTREAM_EN, temp);

	return 0;
}

/***************************************************************************//**
 * @brief En/Disables downstream accelerator.
 *
 * @param dev - The device structure.
 * @param downstream_en - Set true to enable downstream rise time accelerator; false
 * 		to disable
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_set_downstream_accel(struct ltc4306_dev *dev, bool downstream_en)
{
	dev->downstream = downstream_en;

	if (downstream_en)
		return ltc4306_reg_update(dev, LTC4306_CTRL_REG1, LTC4306_DOWNSTREAM_EN,
					  no_os_field_prep(LTC4306_DOWNSTREAM_EN, 1));

	return ltc4306_reg_update(dev, LTC4306_CTRL_REG1, LTC4306_DOWNSTREAM_EN, 0);
}

/***************************************************************************//**
 * @brief Get status of downstream accelerator enable bit field
 *
 * @param dev - The device structure.
 * @param downstream_en - Stores status of Downstream Connected bit.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_get_downstream_accel(struct ltc4306_dev *dev, bool *downstream_en)
{
	int ret;
	uint8_t temp;

	ret = ltc4306_read(dev, LTC4306_CTRL_REG1, &temp, 1);
	if (ret)
		return ret;

	*downstream_en = (bool)no_os_field_get(LTC4306_DOWNSTREAM_EN, temp);

	return 0;
}

/***************************************************************************//**
 * @brief Sets GPIO Output Driver state.
 *
 * @param dev - The device structure.
 * @param gpio - GPIO to set / reset.
 * @param is_high - GPIO logic level.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_set_gpio_output_state(struct ltc4306_dev *dev, int gpio,
				  bool is_high)
{
	if (gpio < LTC4306_GPIO_MIN || gpio > LTC4306_GPIO_MAX)
		return -EINVAL;

	if (is_high)
		return ltc4306_reg_update(dev, LTC4306_CTRL_REG1, LTC4306_OUT_DRV_STATE(gpio),
					  no_os_field_prep(LTC4306_OUT_DRV_STATE(gpio), 1));

	return ltc4306_reg_update(dev, LTC4306_CTRL_REG1,
				  LTC4306_OUT_DRV_STATE(gpio), 0);
}

/***************************************************************************//**
 * @brief Gets GPIO Output Driver state.
 *
 * @param dev - The device structure.
 * @param gpio - GPIO to set / reset.
 * @param is_high - Stores status of selected GPIO.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_get_gpio_output_state(struct ltc4306_dev *dev, int gpio,
				  bool *is_high)
{
	int ret;
	uint8_t temp;

	if (gpio < LTC4306_GPIO_MIN || gpio > LTC4306_GPIO_MAX)
		return -EINVAL;

	ret = ltc4306_read(dev, LTC4306_CTRL_REG1, &temp, 1);
	if (ret)
		return ret;

	*is_high = (bool)no_os_field_get(LTC4306_OUT_DRV_STATE(gpio), temp);

	return 0;
}

/***************************************************************************//**
 * @brief Reads logic of selected GPIO.
 *
 * @param dev - The device structure.
 * @param gpio - GPIO number (only 1 or 2)
 * @param is_high - Stores logic state value of selected GPIO.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_read_gpio_logic_state(struct ltc4306_dev *dev, int gpio,
				  bool *is_high)
{
	int ret;
	uint8_t temp;

	if (gpio < LTC4306_GPIO_MIN || gpio > LTC4306_GPIO_MAX)
		return -EINVAL;

	ret = ltc4306_read(dev, LTC4306_CTRL_REG1, &temp, 1);
	if (ret)
		return ret;

	*is_high = (bool)no_os_field_get(LTC4306_GPIO_LOGIC(gpio), temp);

	return 0;
}

/***************************************************************************//**
 * @brief Sets Connection Requirement bit field.
 *
 * @param dev - The device structure.
 * @param connect_regardless - Set true to configure controller to connect to
 * 		downstream bus regardless of bus logic state; otherwise, set false.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_set_conn_req(struct ltc4306_dev *dev, bool connect_regardless)
{
	if (connect_regardless)
		return ltc4306_reg_update(dev, LTC4306_CTRL_REG2, LTC4306_CONN_REQ,
					  no_os_field_prep(LTC4306_CONN_REQ, 1));

	return ltc4306_reg_update(dev, LTC4306_CTRL_REG2, LTC4306_CONN_REQ, 0);
}

/***************************************************************************//**
 * @brief Gets Connection Requirement bit field.
 *
 * @param dev - The device structure.
 * @param connect_regardless - Stores connection requirement bit status
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_get_conn_req(struct ltc4306_dev *dev, bool *connect_regardless)
{
	int ret;
	uint8_t temp;

	ret = ltc4306_read(dev, LTC4306_CTRL_REG2, &temp, 1);
	if (ret)
		return ret;

	*connect_regardless = (bool)no_os_field_get(LTC4306_CONN_REQ, temp);

	return 0;
}

/***************************************************************************//**
 * @brief Configures GPIO1 and GPIO2 as input or output mode. If selected as
 * 		output mode, can be configured as open-drain or push-pull.
 *
 * @param dev - The device structure.
 * @param gpio1 - Set true to configure GPIO1 to input mode;
 * 		false for output mode.
 * @param gpio2 - Set true to configure GPIO2 to input mode;
 * 		false for output mode.
 * @param gpio1_is_pushpull - Set true to configure GPIO1 output mode
 * 		to push-pull; false for open-drain.
 * @param gpio2_is_pushpull - Set true to configure GPIO2 output mode
 * 		to push-pull; false for open-drain.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_gpio_configure(struct ltc4306_dev *dev, bool gpio1,
			   bool gpio2, bool gpio1_is_pushpull, bool gpio2_is_pushpull)
{
	int i, ret;
	bool gpio_mode[LTC4306_GPIO_MAX] = {gpio1, gpio2};
	bool gpio_conf[LTC4306_GPIO_MAX] = {gpio1_is_pushpull, gpio2_is_pushpull};

	for (i = 0; i < LTC4306_GPIO_MAX; i++) {
		/* Allow push pull / open drain if GPIOx is set to output. Otherwise skip. */
		if (gpio_mode[i]) {
			ret = ltc4306_reg_update(dev, LTC4306_CTRL_REG2,
						 LTC4306_GPIO_MODE_CONFIG(i + 1),
						 no_os_field_prep(LTC4306_GPIO_MODE_CONFIG(i + 1), 1));
			if (ret)
				return ret;
		} else {
			ret = ltc4306_reg_update(dev, LTC4306_CTRL_REG2,
						 LTC4306_GPIO_MODE_CONFIG(i + 1),
						 0);
			if (ret)
				return ret;

			if (gpio_conf[i])
				ret = ltc4306_reg_update(dev, LTC4306_CTRL_REG2,
							 LTC4306_OUT_MODE_CONFIG(i + 1),
							 no_os_field_prep(LTC4306_OUT_MODE_CONFIG(i + 1), 1));
			else
				ret =  ltc4306_reg_update(dev, LTC4306_CTRL_REG2,
							  LTC4306_OUT_MODE_CONFIG(i + 1), 0);

			if (ret)
				return ret;

			dev->is_push_pull[i] = gpio_conf[i];
		}

		dev->is_input[i] = gpio_mode[i];
	}

	return 0;
}

/***************************************************************************//**
 * @brief Enables Mass Write.
 *
 * @param dev - The device structure.
 * @param mass_write_en - Set true to enable mass write address; false to disable.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_set_mass_write(struct ltc4306_dev *dev, bool mass_write_en)
{
	if (mass_write_en)
		return ltc4306_reg_update(dev, LTC4306_CTRL_REG2, LTC4306_MASS_WRITE,
					  no_os_field_prep(LTC4306_MASS_WRITE,1));

	return ltc4306_reg_update(dev, LTC4306_CTRL_REG2, LTC4306_MASS_WRITE, 0);
}

/***************************************************************************//**
 * @brief Gets Mass Write Bit Field status.
 *
 * @param dev - The device structure.
 * @param mass_write_en - Stores the status of the mass write bit field.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_get_mass_write(struct ltc4306_dev *dev, bool *mass_write_en)
{
	int ret;
	uint8_t temp;

	ret = ltc4306_read(dev, LTC4306_CTRL_REG2, &temp, 1);
	if (ret)
		return ret;

	*mass_write_en = (bool)no_os_field_get(LTC4306_MASS_WRITE, temp);

	return 0;
}

/***************************************************************************//**
 * @brief Sets LTC4306's Timeout Mode.
 *
 * @param dev - The device structure.
 * @param tout - Timeout mode to configure.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_set_timeout_mode(struct ltc4306_dev *dev,
			     enum ltc4306_timeout_mode tout)
{
	if (tout < LTC4306_DISABLED || tout > LTC4306_7P5MS)
		return -EINVAL;

	return ltc4306_reg_update(dev, LTC4306_CTRL_REG2, LTC4306_TOUT, tout);
}

/***************************************************************************//**
 * @brief Gets LTC4306's Timeout Mode.
 *
 * @param dev - The device structure.
 * @param tout - Stores the timeout read.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_get_timeout_mode(struct ltc4306_dev *dev,
			     enum ltc4306_timeout_mode *tout)
{
	int ret;
	uint8_t temp;

	ret = ltc4306_read(dev, LTC4306_CTRL_REG2, &temp, 1);
	if (ret)
		return ret;

	*tout = no_os_field_get(LTC4306_TOUT, temp);

	return 0;
}

/***************************************************************************//**
 * @brief Reads bus logic state.
 *
 * @param dev - The device structure.
 * @param bus_num - Selects what bus to read (any number from 1 to 4)
 * @param is_high - Stores logic status of selected bus.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_read_bus_logic_state(struct ltc4306_dev *dev, uint8_t bus_num,
				 bool *is_high)
{
	int ret;
	uint8_t temp;

	if (bus_num < LTC4306_MIN_CHANNEL_INDEX || bus_num > LTC4306_MAX_CHANNEL_INDEX)
		return -EINVAL;

	ret = ltc4306_read(dev, LTC4306_CTRL_REG3, &temp, 1);
	if (ret)
		return ret;

	/* If FET is connected, logic read is invalid. Therefore exit */
	if (no_os_field_get(LTC4306_FET_STATE(bus_num), temp))
		return -EINVAL;

	*is_high = (bool)no_os_field_get(LTC4306_LOGIC_STATE(bus_num), temp);

	return 0;
}

/***************************************************************************//**
 * @brief Dis/connects to a downstream bus. Bus logic state must be 1 for connection
 * 		to occur EXCEPT when conn_req is 0. Controller will connect to
 * 		downstream bus regardless of its logic state if conn_req is 1.
 *
 * @param dev - The device structure.
 * @param bus_num - Bus number to dis/connect (1 to 4).
 * @param connect - Set true to connect selecetd downstream to upstream, false otherwise.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_set_downstream_channel(struct ltc4306_dev *dev,
				   uint8_t bus_num, bool connect)
{
	int ret;
	bool conn_reg, bus_check;

	if (bus_num < LTC4306_MIN_CHANNEL_INDEX || bus_num > LTC4306_MAX_CHANNEL_INDEX)
		return -EINVAL;

	ret = ltc4306_get_conn_req(dev, &conn_reg);
	if (ret)
		return ret;

	dev->is_closed[bus_num - 1] = connect;

	if (connect) {
		if (!conn_reg) {
			ret = ltc4306_read_bus_logic_state(dev, bus_num, &bus_check);
			if (ret)
				return ret;

			if (!bus_check)
				return -EINVAL;
		}

		return ltc4306_reg_update(dev, LTC4306_CTRL_REG3, LTC4306_FET_STATE(bus_num),
					  no_os_field_prep(LTC4306_FET_STATE(bus_num), 1));
	} else
		return ltc4306_reg_update(dev, LTC4306_CTRL_REG3, LTC4306_FET_STATE(bus_num),
					  0);
}

/***************************************************************************//**
 * @brief Get the connection status of the selected FET.
 *
 * @param dev - The device structure.
 * @param bus_num - Bus number that the user wants to connect to.
 * 		(can be any value from 1 to 4)
 * @param connect - Stores the status of the bus FET state.
 *
 * @return 0 in case of success, negative error code otherwise.
*******************************************************************************/
int ltc4306_get_downstream_channel(struct ltc4306_dev *dev,
				   uint8_t bus_num, bool *connect)
{
	int ret;
	uint8_t temp;

	if (bus_num < LTC4306_MIN_CHANNEL_INDEX || bus_num > LTC4306_MAX_CHANNEL_INDEX)
		return -EINVAL;

	ret = ltc4306_read(dev, LTC4306_CTRL_REG3, &temp, 1);
	if (ret)
		return ret;

	*connect = (bool)no_os_field_get(LTC4306_FET_STATE(bus_num), temp);

	return 0;
}
