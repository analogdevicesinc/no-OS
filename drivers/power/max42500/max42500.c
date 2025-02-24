/***************************************************************************//**
 *   @file   max42500.c
 *   @brief  Source file of MAX42500 Driver.
 *   @author Mark Sapungan (Mark.Sapungan@analog.com)
 *   @author Joshua Maniti (Joshua.Maniti@analog.com)
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

#include "max42500.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_crc8.h"

#define CRC8_PEC        0x07      /* Implements Polynomial X^8 + X^2 + X^1 +1 */

NO_OS_DECLARE_CRC8_TABLE(max42500_crc8);

/******************************************************************************/

/**
 * @brief Set device state through EN0 and EN1 pins.
 * @param desc - The device structure.
 * @param state - Device state.
 * @return 0 in case of success, error code otherwise.
 */
int max42500_set_state(struct max42500_dev *desc, enum max42500_state state)
{
	int ret;
	uint8_t en0;
	uint8_t en1;

	if (!desc)
		return -EINVAL;

	switch (state) {
	case MAX42500_STATE_OFF:
		en0 = NO_OS_GPIO_LOW;
		en1 = NO_OS_GPIO_LOW;
		break;

	case MAX42500_STATE_SLEEP:
		en0 = NO_OS_GPIO_HIGH;
		en1 = NO_OS_GPIO_LOW;
		break;

	case MAX42500_STATE_ON:
		en0 = NO_OS_GPIO_HIGH;
		en1 = NO_OS_GPIO_HIGH;
		break;

	default:
		return -EINVAL;
	}

	ret = no_os_gpio_set_value(desc->en0, en0);
	if (ret)
		return ret;

	return no_os_gpio_set_value(desc->en1, en1);
}

/**
 * @brief Read a raw value from a register.
 * @param desc - The device structure.
 * @param reg_addr - Address of the register to be written.
 * @param reg_data - Pointer to store the read data.
 * @return 0 in case of success, error code otherwise.
 */
int max42500_reg_read(struct max42500_dev *desc,
		      uint8_t reg_addr,
		      uint8_t *reg_data)
{
	int ret;
	uint8_t i2c_data[MAX42500_I2C_RD_FRAME_SIZE] = {0};
	uint8_t bytes_number;
	uint8_t crc;

	/* PEC is computed over entire I2C frame from the first START condition */
	i2c_data[0] = (desc->comm_desc->slave_address << 1);
	i2c_data[1] = reg_addr;
	i2c_data[2] = (desc->comm_desc->slave_address << 1) | 0x1;

	/* I2C write target address */
	bytes_number = 1;

	ret = no_os_i2c_write(desc->comm_desc, &i2c_data[1], bytes_number, 0);
	if (ret)
		return ret;

	/* Change read byte count if PECE is enabled (1-byte data. 1-byte PEC) */
	bytes_number = (desc->pece) ? 2 : bytes_number;

	ret = no_os_i2c_read(desc->comm_desc, &i2c_data[3], bytes_number, 1);
	if (ret)
		return ret;

	if (desc->pece) {
		/* Compute CRC over entire I2C frame */
		crc = no_os_crc8(max42500_crc8, i2c_data,
				 (MAX42500_I2C_RD_FRAME_SIZE - 1), 0);

		if (i2c_data[4] != crc)
			return -EIO;
	}

	*reg_data = i2c_data[3];

	return 0;
}

/**
 * @brief Write a raw value to a register.
 * @param desc - The device structure.
 * @param reg_addr - Address of the register to be written.
 * @param data - Data to write to register.
 * @return 0 in case of success, negative error code otherwise.
 */
int max42500_reg_write(struct max42500_dev *desc,
		       uint8_t reg_addr,
		       uint8_t data)
{
	uint8_t i2c_data[MAX42500_I2C_WR_FRAME_SIZE] = {0};
	uint8_t bytes_number;

	bytes_number = (desc->pece) ? (MAX42500_I2C_WR_FRAME_SIZE - 1) : 2;
	i2c_data[0] = (desc->comm_desc->slave_address << 1);
	i2c_data[1] = reg_addr;
	i2c_data[2] = (uint8_t)(data & 0xFF);

	if (desc->pece)
		i2c_data[3] = no_os_crc8(max42500_crc8, i2c_data,
					 bytes_number, 0);

	return no_os_i2c_write(desc->comm_desc, &i2c_data[1], bytes_number, 1);
}

/**
 * @brief Update a register's value based on a mask.
 * @param desc - The device structure.
 * @param reg_addr - Address of the register to be written.
 * @param mask - The bits that may be modified.
 * @param data - Data to write to register
 * @return 0 in case of success, negative error code otherwise.
 */
int max42500_reg_update(struct max42500_dev *desc,
			uint8_t reg_addr,
			uint8_t mask,
			uint8_t data)
{
	int ret;
	uint8_t reg_data;

	ret = max42500_reg_read(desc, reg_addr, &reg_data);
	if (ret)
		return ret;

	reg_data &= ~mask;
	reg_data |= mask & data;

	return max42500_reg_write(desc, reg_addr, reg_data);
}

/**
 * @brief Set nominal voltage for VM1 to VM5.
 * @param desc - The device structure.
 * @param vm_in - The voltage monitor input.
 * @param voltage - Nominal voltage to set in volts.
 *                  Example: 0.5 to 3.6875 for VM1 to VM4
 *                           0.5 to 5.6 for VM5
 * @return 0 in case of success, negative error code otherwise.
 */
int max42500_set_nominal_voltage(struct max42500_dev *desc,
				 enum max42500_vm_input vm_in,
				 float voltage)
{
	uint8_t reg_val;
	uint8_t reg_addr;

	switch (vm_in) {
	case MAX42500_VM1:
	case MAX42500_VM2:
	case MAX42500_VM3:
	case MAX42500_VM4:
		if ((voltage < MAX42500_MIN_VNOM) ||
		    (voltage > MAX42500_VNOM_MAX_VM1_VM4))
			return -EINVAL;
		reg_val = (uint8_t)((voltage - MAX42500_MIN_VNOM) /
				    MAX42500_VNOM_STEP_VM1_VM4);
		reg_addr = MAX42500_REG_VIN1 + vm_in;
		break;
	case MAX42500_VM5:
		if ((voltage < MAX42500_MIN_VNOM) ||
		    (voltage > MAX42500_VNOM_MAX_VM5))
			return -EINVAL;
		reg_val = (uint8_t)((voltage - MAX42500_MIN_VNOM) /
				    MAX42500_VNOM_STEP_VM5);
		reg_addr = MAX42500_REG_VIN5;
		break;
	default:
		return -EINVAL;
	}

	return max42500_reg_write(desc, reg_addr, reg_val);
}

/**
 * @brief Get the status of the voltage monitor input.
 * @param desc - The device structure.
 * @param vm_in - The voltage monitor input.
 * @param comp_stat - Status to read.
 *                    Example: MAX42500_COMP_STAT_OFF - Comparator off status
 *                             MAX42500_COMP_STAT_UV - Undervoltage status
 *                             MAX42500_COMP_STAT_OV - Overvoltage status
 * @param status - Pointer to store the status.
 *                 Example: 0 - below OV threshold; above UV, OFF threshold
 * 			    1 - above OV threshold; below UV, OFF threshold
 * @return 0 in case of success, negative error code otherwise.
 */
int max42500_get_comp_status(struct max42500_dev *desc,
			     enum max42500_vm_input vm_in,
			     enum max42500_comp_stat comp_stat,
			     uint8_t *status)
{
	int ret;
	uint8_t reg_addr;
	uint8_t vm_in_status;

	switch (comp_stat) {
	case MAX42500_COMP_STAT_OFF:
		reg_addr = MAX42500_REG_STATOFF;
		break;
	case MAX42500_COMP_STAT_UV:
		reg_addr = MAX42500_REG_STATUV;
		break;
	case MAX42500_COMP_STAT_OV:
		reg_addr = MAX42500_REG_STATOV;
		break;
	default:
		return -EINVAL;
	}

	ret = max42500_reg_read(desc, reg_addr, &vm_in_status);
	if (ret)
		return ret;

	*status = (uint8_t)no_os_field_get(NO_OS_BIT(vm_in), vm_in_status);

	return 0;
}


/**
 * @brief Set the overvoltage threshold of VM1 to VM5.
 * @param desc - The device structure.
 * @param vm_in - The voltage monitor input.
 * @param thresh - The overvoltage threshold in percentage (2.5% to 10%).
 * @return 0 in case of success, negative error code otherwise.
 */
int max42500_set_ov_thresh1(struct max42500_dev *desc,
			    enum max42500_vm_input vm_in,
			    float thresh)
{
	uint8_t ov_val;

	if ((thresh < MAX42500_MIN_THRESH_VM1_VM5) ||
	    (thresh > MAX42500_MAX_THRESH_VM1_VM5))
		return -EINVAL;

	switch (vm_in) {
	case MAX42500_VM1:
	case MAX42500_VM2:
	case MAX42500_VM3:
	case MAX42500_VM4:
	case MAX42500_VM5:
		/* Compute the value of OV to be written in the register*/
		ov_val = (uint8_t)
			 NO_OS_DIV_ROUND_CLOSEST(((1 + (thresh / 100)) - 1.025),
						 0.005);
		return max42500_reg_update(desc,
					   MAX42500_REG_OVUV1 + vm_in,
					   NO_OS_GENMASK(7, 4),
					   no_os_field_prep(NO_OS_GENMASK(7, 4),
							   ov_val));
	default:
		return -EINVAL;
	}
}

/**
 * @brief Set the overvoltage threshold of VM6 and VM7.
 * @param desc - The device structure.
 * @param vm_in - The voltage monitor input.
 * @param thresh - The overvoltage threshold in volts (0.5V to 1.775V).
 * @return 0 in case of success, negative error code otherwise.
 */
int max42500_set_ov_thresh2(struct max42500_dev *desc,
			    enum max42500_vm_input vm_in,
			    float thresh)
{
	uint8_t reg_addr;
	uint8_t ov_val;

	if ((thresh < MAX42500_MIN_THRESH_VM6_V7) ||
	    (thresh > MAX42500_MAX_THRESH_VM6_V7))
		return -EINVAL;

	switch (vm_in) {
	case MAX42500_VM6:
		reg_addr = MAX42500_REG_VINO6;
		break;
	case MAX42500_VM7:
		reg_addr = MAX42500_REG_VINO7;
		break;
	default:
		return -EINVAL;
	}

	ov_val = (uint8_t)NO_OS_DIV_ROUND_CLOSEST((thresh - 0.5), 0.005);

	return max42500_reg_write(desc, reg_addr, ov_val);
}

/**
 * @brief Set the undervoltage threshold of VM1 to VM5.
 * @param desc - The device structure.
 * @param vm_in - The voltage monitor input.
 * @param thresh - The undervoltage threshold in percentage (2.5% to 10%).
 * @return 0 in case of success, negative error code otherwise.
 */
int max42500_set_uv_thresh1(struct max42500_dev *desc,
			    enum max42500_vm_input vm_in,
			    float thresh)
{
	uint8_t uv_val;

	if ((thresh < MAX42500_MIN_THRESH_VM1_VM5) ||
	    (thresh > MAX42500_MAX_THRESH_VM1_VM5))
		return -EINVAL;

	switch (vm_in) {
	case MAX42500_VM1:
	case MAX42500_VM2:
	case MAX42500_VM3:
	case MAX42500_VM4:
	case MAX42500_VM5:
		uv_val = (uint8_t)
			 NO_OS_DIV_ROUND_CLOSEST(((1 - (thresh / 100)) - 0.975),
						 -0.005);
		return max42500_reg_update(desc,
					   MAX42500_REG_OVUV1 + vm_in,
					   NO_OS_GENMASK(3, 0),
					   uv_val);
	default:
		return -EINVAL;
	}
}

/**
 * @brief Set the undervoltage threshold of VM6 and VM7.
 * @param desc - The device structure.
 * @param vm_in - The voltage monitor input.
 * @param thresh - The overvoltage threshold in volts (0.5V to 1.775V).
 * @return 0 in case of success, negative error code otherwise.
 */
int max42500_set_uv_thresh2(struct max42500_dev *desc,
			    enum max42500_vm_input vm_in,
			    float thresh)
{
	uint8_t reg_addr;
	uint8_t uv_val;

	if ((thresh < MAX42500_MIN_THRESH_VM6_V7) ||
	    (thresh > MAX42500_MAX_THRESH_VM6_V7))
		return -EINVAL;

	switch (vm_in) {
	case MAX42500_VM6:
		reg_addr = MAX42500_REG_VINU6;
		break;
	case MAX42500_VM7:
		reg_addr = MAX42500_REG_VINU7;
		break;
	default:
		return -EINVAL;
	}

	uv_val = (uint8_t)NO_OS_DIV_ROUND_CLOSEST((thresh - 0.5), 0.005);

	return max42500_reg_write(desc, reg_addr, uv_val);
}

/**
 * @brief Get the FPS clock divider value.
 * @param desc - The device structure.
 * @param fps_clk_div Pointer to a variable where the FPS clock divider value
 *                    will be stored.
 * @return 0 in case of success, negative error code otherwise.
 */
static int max42500_get_fps_clk_div(struct max42500_dev *desc,
				    uint8_t *fps_clk_div)
{
	int ret;
	uint8_t reg_val;

	ret = max42500_reg_read(desc, MAX42500_REG_FPSCFG1, &reg_val);
	if (ret)
		return ret;

	*fps_clk_div = (uint8_t)no_os_field_get(NO_OS_GENMASK(2, 0), reg_val);

	return 0;
}

/**
 * @brief Get the power-up timestamp for a specified voltage monitor input.
 * @param desc - The device structure.
 * @param vm_in - The voltage monitor input for which to get the timestamp.
 * @param timestamp - The timestamp in microseconds.
 *                    If the input voltage never rose above the UV threshold,
 *                    the timestamp will be set to 0.
 * @return 0 in case of success, negative error code otherwise.
 */
int max42500_get_power_up_timestamp(struct max42500_dev *desc,
				    enum max42500_vm_input vm_in,
				    uint8_t *timestamp)
{
	int ret;
	uint8_t reg_val;
	uint8_t fps_clk_div;

	ret = max42500_reg_read(desc, MAX42500_REG_UTIME1 + vm_in, &reg_val);
	if (ret)
		return ret;

	// Check if the input voltage rose above the UV threshold
	if (reg_val == 0) {
		// Input voltage never rose above UV threshold
		*timestamp = 0;
		return 0;
	}

	ret = max42500_get_fps_clk_div(desc, &fps_clk_div);
	if (ret)
		return ret;

	*timestamp = (reg_val - 1) * 25 * (1 << fps_clk_div);

	return 0;
}

/**
 * @brief Get the power-down timestamp for a specified voltage monitor input.
 * @param desc - The device structure.
 * @param vm_in - The voltage monitor input for which to get the timestamp.
 * @param timestamp - The timestamp in microseconds.
 *                    If the input voltage never rose above the UV threshold,
 *                    the timestamp will be set to 0.
 * @return 0 in case of success, negative error code otherwise.
 */
int max42500_get_power_down_timestamp(struct max42500_dev *desc,
				      enum max42500_vm_input vm_in,
				      uint8_t *timestamp)
{
	int ret;
	uint8_t reg_val;
	uint8_t fps_clk_div;

	ret = max42500_reg_read(desc, MAX42500_REG_DTIME1 + vm_in, &reg_val);
	if (ret)
		return ret;

	// Check if the input voltage fell below the the OFF threshold
	if (reg_val == 0) {
		// Input voltage never fell below OFF threshold
		*timestamp = 0;
		return 0;
	}

	ret = max42500_get_fps_clk_div(desc, &fps_clk_div);
	if (ret)
		return ret;

	*timestamp = (reg_val - 1) * 25 * (1 << fps_clk_div);

	return 0;
}

/**
 * @brief Enable/Disable watchdog
 * @param desc - The device structure.
 * @param wd_enable - Enable/disable MAX42500 watchdog.
 * @return 0 in case of success, error code otherwise
 */
int max42500_set_watchdog_enable(struct max42500_dev *desc, bool wd_enable)
{
	int ret;
	uint8_t reg_val;

	ret = max42500_reg_read(desc, MAX42500_REG_WDCFG2, &reg_val);
	if (ret)
		return ret;

	if (wd_enable)
		reg_val |= NO_OS_BIT(3);
	else
		reg_val &= ~NO_OS_BIT(3);

	return max42500_reg_write(desc, MAX42500_REG_WDCFG2, reg_val);
}

/**
 * @brief 8-bit watchdog key computation.
 * @param desc - The device structure.
 * @param new_wd_key - New watchdog key.
 * @return 0 in case of success, negative error code otherwise.
 */
static int max42500_new_watchdog_key(struct max42500_dev *desc,
				     uint8_t *new_wd_key)
{
	int ret;
	uint8_t curr_wd_key;

	ret = max42500_reg_read(desc, MAX42500_REG_WDKEY, &curr_wd_key);
	if (ret)
		return ret;

	/* Calculate the new bit using the LFSR polynomial */
	uint8_t new_bit = ((curr_wd_key >> 7) ^
			   (curr_wd_key >> 5) ^
			   (curr_wd_key >> 4) ^
			   (curr_wd_key >> 3)) & 0x01;

	/* Shift existing bits upwards toward MSb and insert the new bit as LSb */
	*new_wd_key = (curr_wd_key << 1) | new_bit;

	return 0;
}

/**
 * @brief Update the watchdog key based on the mode and current value.
 * @param desc - The device structure.
 * @return 0 in case of success, error code otherwise.
 */
int max42500_set_watchdog_key(struct max42500_dev *desc)
{
	int ret;
	uint8_t reg_val;
	uint8_t wd_key;
	uint8_t wd_mode;

	ret = max42500_reg_read(desc, MAX42500_REG_WDKEY, &wd_key);
	if (ret)
		return ret;

	ret = max42500_reg_read(desc, MAX42500_REG_WDCDIV, &reg_val);
	if (ret)
		return ret;

	wd_mode = (uint8_t)no_os_field_get(NO_OS_BIT(6), reg_val);

	/* Compute new watchdog key for challenge/response mode */
	if (wd_mode == MAX42500_WD_MODE_CH_RESP)
		max42500_new_watchdog_key(desc, &wd_key);

	return max42500_reg_write(desc, MAX42500_REG_WDKEY, wd_key);
}

/** @brief Set watchdog reset hold time
 * @param desc - The device structure.
 * @param rhld - Reset hold time to set.
 * @return 0 in case of success, error code otherwise
 */
int max42500_set_watchdog_rhld(struct max42500_dev *desc,
			       enum max42500_wd_rhld rhld)
{
	return max42500_reg_update(desc,
				   MAX42500_REG_RSTCTRL,
				   NO_OS_GENMASK(1, 0),
				   rhld);
}

/**
 * @brief Initialize the device structure.
 * @param desc - The device structure to be initialized.
 * @param init_param - Initialization parameter for the device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int max42500_init(struct max42500_dev **desc,
		  struct max42500_init_param *init_param)
{
	int ret;
	struct max42500_dev *descriptor;
	uint8_t device_id;

	no_os_crc8_populate_msb(max42500_crc8, CRC8_PEC);

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	descriptor->pece = 0x00;

	/* Initialize GPIOs for device pins */
	ret = no_os_gpio_get(&descriptor->en0, &init_param->en0_param);
	if (ret)
		goto free_desc;

	ret = no_os_gpio_get(&descriptor->en1, &init_param->en1_param);
	if (ret)
		goto free_en0;

	ret = no_os_gpio_get(&descriptor->addr, &init_param->addr_param);
	if (ret)
		goto free_en1;

	/* Enable EN0 and EN1 GPIO output with default values */
	ret = no_os_gpio_direction_output(descriptor->en0, NO_OS_GPIO_HIGH);
	if (ret)
		goto free_addr;

	ret = no_os_gpio_direction_output(descriptor->en1, NO_OS_GPIO_HIGH);
	if (ret)
		goto free_addr;

	/* Enable I2C addr selection */
	ret = no_os_gpio_direction_output(descriptor->addr, init_param->addr_sel);
	if (ret)
		goto free_addr;

	/* Initialize I2C communication */
	ret = no_os_i2c_init(&descriptor->comm_desc, &init_param->comm_param);
	if (ret)
		goto free_addr;

	/* Check device silicon ID */
	ret = max42500_reg_read(descriptor, MAX42500_REG_ID, &device_id);
	if (ret)
		goto free_i2c;

	if (device_id != MAX42500_SILICON_ID) {
		ret = -ENODEV;
		goto free_i2c;
	}

	/* Configure PEC */
	ret = max42500_reg_update(descriptor,
				  MAX42500_REG_CONFIG1,
				  NO_OS_BIT(0),
				  init_param->pece);
	if (ret)
		goto free_i2c;

	/* Set PEC enable/disable for subsequent register access */
	descriptor->pece = init_param->pece;

	/* Enable voltage monitor inputs */
	ret = max42500_reg_update(descriptor,
				  MAX42500_REG_VMON,
				  NO_OS_GENMASK(6, 0),
				  init_param->vmon_en);
	if (ret)
		goto free_i2c;

	/* Enable voltage monitor power-down */
	ret = max42500_reg_update(descriptor,
				  MAX42500_REG_VMON,
				  NO_OS_BIT(7),
				  init_param->vmon_vmpd);
	if (ret)
		goto free_i2c;

	/* Enable input OV/UV mapping to reset pin */
	ret = max42500_reg_update(descriptor,
				  MAX42500_REG_RSTMAP,
				  NO_OS_GENMASK(6, 0),
				  init_param->reset_map);
	if (ret)
		goto free_i2c;

	/* Set watchdog mode */
	ret = max42500_reg_update(descriptor,
				  MAX42500_REG_WDCDIV,
				  NO_OS_BIT(6),
				  init_param->wd_mode << 6);
	if (ret)
		goto free_i2c;

	/* Set watchdog clock div */
	ret = max42500_reg_update(descriptor,
				  MAX42500_REG_WDCDIV,
				  NO_OS_GENMASK(5, 0),
				  init_param->wd_cdiv);
	if (ret)
		goto free_i2c;

	/* Set watchdog open window */
	ret = max42500_reg_update(descriptor,
				  MAX42500_REG_WDCFG1,
				  NO_OS_GENMASK(3, 0),
				  init_param->wd_open);
	if (ret)
		goto free_i2c;

	/* Set watchdog close window */
	ret = max42500_reg_update(descriptor,
				  MAX42500_REG_WDCFG1,
				  NO_OS_GENMASK(7, 4),
				  init_param->wd_close);
	if (ret)
		goto free_i2c;

	/* Set watchdog first update window */
	ret = max42500_reg_update(descriptor,
				  MAX42500_REG_WDCFG2,
				  NO_OS_GENMASK(2, 0),
				  init_param->wd_1ud);
	if (ret)
		goto free_i2c;

	/* Set watchdog enable */
	ret = max42500_reg_update(descriptor,
				  MAX42500_REG_WDCFG2,
				  NO_OS_BIT(3),
				  init_param->wd_en);
	if (ret)
		goto free_i2c;

	/* Update parameters */
	descriptor->wd_mode = init_param->wd_mode;
	descriptor->wd_cdiv = init_param->wd_cdiv;
	descriptor->wd_close = init_param->wd_close;
	descriptor->wd_open = init_param->wd_open;
	descriptor->wd_1ud = init_param->wd_1ud;
	descriptor->wd_en = init_param->wd_en;

	/* Update descriptor */
	*desc = descriptor;

	return 0;

free_i2c:
	no_os_i2c_remove(descriptor->comm_desc);
free_addr:
	no_os_gpio_remove(descriptor->addr);
free_en1:
	no_os_gpio_remove(descriptor->en1);
free_en0:
	no_os_gpio_remove(descriptor->en0);
free_desc:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free the device descriptor.
 * @param desc - The device structure.
 * @return 0 in case of success, -EINVAL otherwise.
 */
int max42500_remove(struct max42500_dev *desc)
{
	int ret;

	if (!desc)
		return -EINVAL;

	ret = no_os_i2c_remove(desc->comm_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(desc->addr);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(desc->en1);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(desc->en0);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}
