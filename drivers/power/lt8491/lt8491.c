/***************************************************************************//**
 *   @file   lt8491.c
 *   @brief  Implementation of LT8491 Driver.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#include "lt8491.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_i2c.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include <string.h>

/**
 * @brief Read a register value
 * @param dev - LT8491 descriptor
 * @param addr - register address
 * @param val - register value
 * @param is_word - true if the register is 16-bit, false if 8-bit
 * @return 0 in case of success, negative error code otherwise
 */
int lt8491_reg_read(struct lt8491_desc *dev, uint8_t addr, uint16_t *val,
		    bool is_word)
{
	int ret;
	uint8_t uval[2];

	ret = no_os_i2c_write(dev->i2c_desc, &addr, 1, 0);
	if (ret)
		return ret;

	if (is_word) {
		ret = no_os_i2c_read(dev->i2c_desc, uval, 2, 1);
		if (ret)
			return ret;

		*val = no_os_get_unaligned_le16(uval);
	} else {
		ret = no_os_i2c_read(dev->i2c_desc, uval, 1, 1);
		if (ret)
			return ret;

		*val = uval[0];
	}

	return 0;
}

/**
 * @brief Write a register value
 * @param dev - LT8491 descriptor
 * @param addr - register address
 * @param val - register value
 * @param is_word - true if the register is 16-bit, false if 8-bit
 * @return 0 in case of success, negative error code otherwise
 */
int lt8491_reg_write(struct lt8491_desc *dev, uint8_t addr, uint16_t val,
		     bool is_word)
{
	uint8_t buff[] = {addr, val & 0xFF};
	int ret;

	if (is_word) {
		ret = no_os_i2c_write(dev->i2c_desc, buff, 2, 1);
		if (ret)
			return ret;

		buff[0] = addr + 1;
		buff[1] = val >> 8;
	}

	return no_os_i2c_write(dev->i2c_desc, buff, 2, 1);
}

/**
 * @brief Device and comm init function
 * @param dev - LT8491 descriptor to be initialized
 * @param init_param - Init parameter for descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int lt8491_init(struct lt8491_desc **dev,
		struct lt8491_init_param *init_param)
{
	struct lt8491_desc *temp_dev;
	int ret;

	temp_dev = no_os_calloc(1, sizeof(*temp_dev));
	if (!temp_dev)
		return -ENOMEM;

	ret = no_os_i2c_init(&temp_dev->i2c_desc, &init_param->i2c_init_param);
	if (ret)
		goto free_dev;

	*dev = temp_dev;

	return 0;

free_dev:
	no_os_free(temp_dev);

	return ret;
}

/**
 * @brief Free resources allocated by the init function
 * @param dev - LT8491 descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int lt8491_remove(struct lt8491_desc *dev)
{
	int ret;

	if (!dev)
		return -ENODEV;

	ret = no_os_i2c_remove(dev->i2c_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}

/**
 * @brief Device and comm init function
 * @param dev - LT8491 descriptor to be initialized
 * @param init_param - Init parameter for descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int lt8491_configure_telemetry(struct lt8491_desc *dev,
			       struct lt8491_init_param *init_param)
{
	int ret;

	ret = lt8491_reg_write(dev, LT8491_CFG_RSENSE1_REG,
			       init_param->rsense1_micro_ohms / 10, true);
	if (ret)
		return ret;

	ret = lt8491_reg_write(dev, LT8491_CFG_RIMON_OUT_REG,
			       init_param->rimon_out_ohms / 10, true);
	if (ret)
		return ret;

	ret = lt8491_reg_write(dev, LT8491_CFG_RSENSE2_REG,
			       init_param->rsense2_micro_ohms / 10, true);
	if (ret)
		return ret;

	ret = lt8491_reg_write(dev, LT8491_CFG_RDACO_REG,
			       init_param->rdaco_ohms / 10, true);
	if (ret)
		return ret;

	ret = lt8491_reg_write(dev, LT8491_CFG_RFBOUT1_REG,
			       init_param->rfbout1_ohms / 100, true);
	if (ret)
		return ret;

	ret = lt8491_reg_write(dev, LT8491_CFG_RFBOUT2_REG,
			       init_param->rfbout2_ohms / 10, true);
	if (ret)
		return ret;

	ret = lt8491_reg_write(dev, LT8491_CFG_RDACI_REG,
			       init_param->rdaci_ohms / 10, true);
	if (ret)
		return ret;

	ret = lt8491_reg_write(dev, LT8491_CFG_RFBIN2_REG,
			       init_param->rfbin2_ohms / 10, true);
	if (ret)
		return ret;

	ret = lt8491_reg_write(dev, LT8491_CFG_RFBIN1_REG,
			       init_param->rfbin1_ohms / 100, true);
	if (ret)
		return ret;

	return 0;
}

static int __lt8491_update_telemetry(struct lt8491_desc *dev)
{
	return lt8491_reg_write(dev, LT8491_CTRL_UPDATE_TELEM, 0xAA, false);
}

/**
 * @brief Read the telemetry battery temperature
 * @param dev - LT8491 descriptor
 * @param val - telemetry battery temperature in mC
 * @return 0 in case of success, errno errors otherwise
 */
int lt8491_read_tbat(struct lt8491_desc *dev, int32_t *val)
{
	int ret;
	uint16_t uval;

	ret = __lt8491_update_telemetry(dev);
	if (ret)
		return ret;

	ret = lt8491_reg_read(dev, LT8491_TELE_TBAT_REG, &uval, true);
	if (ret)
		return ret;

	*val = no_os_sign_extend32(uval, 15) * 100;

	return 0;
}

/**
 * @brief Read the telemetry battery output power
 * @param dev - LT8491 descriptor
 * @param val - telemetry battery output power in mW
 * @return 0 in case of success, errno errors otherwise
 */
int lt8491_read_pout(struct lt8491_desc *dev, uint32_t *val)
{
	int ret;
	uint16_t uval;

	ret = __lt8491_update_telemetry(dev);
	if (ret)
		return ret;

	ret = lt8491_reg_read(dev, LT8491_TELE_POUT_REG, &uval, true);
	if (ret)
		return ret;

	*val = uval * 10;

	return 0;
}

/**
 * @brief Read the telemetry battery input power
 * @param dev - LT8491 descriptor
 * @param val - telemetry battery input power in mW
 * @return 0 in case of success, errno errors otherwise
 */
int lt8491_read_pin(struct lt8491_desc *dev, uint32_t *val)
{
	int ret;
	uint16_t uval;

	ret = __lt8491_update_telemetry(dev);
	if (ret)
		return ret;

	ret = lt8491_reg_read(dev, LT8491_TELE_PIN_REG, &uval, true);
	if (ret)
		return ret;

	*val = uval * 10;

	return 0;
}

/**
 * @brief Read the telemetry battery efficiency
 * @param dev - LT8491 descriptor
 * @param val - telemetry battery efficiency in m%
 * @return 0 in case of success, errno errors otherwise
 */
int lt8491_read_efficiency(struct lt8491_desc *dev, uint32_t *val)
{
	int ret;
	uint16_t uval;

	ret = __lt8491_update_telemetry(dev);
	if (ret)
		return ret;

	ret = lt8491_reg_read(dev, LT8491_TELE_EFF_REG, &uval, true);
	if (ret)
		return ret;

	*val = uval * 10;

	return 0;
}

/**
 * @brief Read the telemetry battery output current
 * @param dev - LT8491 descriptor
 * @param val - telemetry battery output current in mA
 * @return 0 in case of success, errno errors otherwise
 */
int lt8491_read_iout(struct lt8491_desc *dev, uint32_t *val)
{
	int ret;
	uint16_t uval;

	ret = __lt8491_update_telemetry(dev);
	if (ret)
		return ret;

	ret = lt8491_reg_read(dev, LT8491_TELE_IOUT_REG, &uval, true);
	if (ret)
		return ret;

	*val = uval;

	return 0;
}

/**
 * @brief Read the telemetry battery input current
 * @param dev - LT8491 descriptor
 * @param val - telemetry battery input current in mA
 * @return 0 in case of success, errno errors otherwise
 */
int lt8491_read_iin(struct lt8491_desc *dev, uint32_t *val)
{
	int ret;
	uint16_t uval;

	ret = __lt8491_update_telemetry(dev);
	if (ret)
		return ret;

	ret = lt8491_reg_read(dev, LT8491_TELE_IIN_REG, &uval, true);
	if (ret)
		return ret;

	*val = uval;

	return 0;
}

/**
 * @brief Read the telemetry battery voltage
 * @param dev - LT8491 descriptor
 * @param val - telemetry battery voltage in mV
 * @return 0 in case of success, errno errors otherwise
 */
int lt8491_read_vbat(struct lt8491_desc *dev, uint32_t *val)
{
	int ret;
	uint16_t uval;

	ret = __lt8491_update_telemetry(dev);
	if (ret)
		return ret;

	ret = lt8491_reg_read(dev, LT8491_TELE_VBAT_REG, &uval, true);
	if (ret)
		return ret;

	*val = uval * 10;

	return 0;
}

/**
 * @brief Read the telemetry input voltage
 * @param dev - LT8491 descriptor
 * @param val - telemetry input voltage in mV
 * @return 0 in case of success, errno errors otherwise
 */
int lt8491_read_vin(struct lt8491_desc *dev, uint32_t *val)
{
	int ret;
	uint16_t uval;

	ret = __lt8491_update_telemetry(dev);
	if (ret)
		return ret;

	ret = lt8491_reg_read(dev, LT8491_STAT_SUPPLY_REG, &uval, false);
	if (ret)
		return ret;

	if (no_os_field_get(LT8491_PS_OR_SOLAR_MASK, uval))
		ret = lt8491_reg_read(dev, LT8491_TELE_VIN_REG, &uval, true);
	else
		ret = lt8491_reg_read(dev, LT8491_TELE_VINR_REG, &uval, true);
	if (ret)
		return ret;

	*val = uval * 10;

	return 0;
}

/**
 * @brief Read the telemetry battery charging stage
 * @param dev - LT8491 descriptor
 * @param uval - telemetry battery charging stage
 * @return 0 in case of success, errno errors otherwise
 */
int lt8491_read_charging_stage(struct lt8491_desc *dev, uint32_t *uval)
{
	int ret;
	uint16_t temp;

	ret = lt8491_reg_read(dev, LT8491_STAT_CHARGER_REG, &temp, false);
	if (ret)
		return ret;

	*uval = no_os_field_get(LT8491_CHRG_STAGE_MASK, temp);

	return 0;
}

/**
 * @brief Read the telemetry battery charging status
 * @param dev - LT8491 descriptor
 * @param charging - true if charging, false otherwise
 * @return 0 in case of success, errno errors otherwise
 */
int lt8491_read_charging_status(struct lt8491_desc *dev, uint32_t *charging)
{
	int ret;
	uint16_t uval;

	ret = lt8491_reg_read(dev, LT8491_STAT_CHARGER_REG, &uval, false);
	if (ret)
		return ret;

	*charging = !!no_os_field_get(LT8491_CHARGING_MASK, uval);

	return 0;
}

/**
 * @brief Read the telemetry battery charging enable
 * @param dev - LT8491 descriptor
 * @param enable - true if charging enable, false otherwise
 * @return 0 in case of success, errno errors otherwise
 */
int lt8491_read_enable(struct lt8491_desc *dev, uint32_t *enable)
{
	int ret;
	uint16_t uval;

	ret = lt8491_reg_read(dev, LT8491_CTRL_CHRG_EN_REG, &uval, false);
	if (ret)
		return ret;

	*enable = !!uval;

	return 0;
}

/**
 * @brief Write the telemetry battery charging enable
 * @param dev - LT8491 descriptor
 * @param enable - true if charging enable, false otherwise
 * @return 0 in case of success, errno errors otherwise
 */
int lt8491_write_enable(struct lt8491_desc *dev, uint32_t enable)
{
	return lt8491_reg_write(dev, LT8491_CTRL_CHRG_EN_REG, enable, false);
}

/**
 * @brief Restart the chip
 * @param dev - LT8491 descriptor
 * @return 0 in case of success, errno errors otherwise
 */
int lt8491_restart_chip(struct lt8491_desc *dev)
{
	int ret;

	ret = lt8491_write_enable(dev, false);
	if (ret)
		return ret;

	return lt8491_reg_write(dev, LT8491_CTRL_RESTART_CHIP_REG, 0x99, false);
}

/**
 * @brief Read the scratch register
 * @param dev - LT8491 descriptor
 * @param val - scratch register value
 * @return 0 in case of success, errno errors otherwise
 */
int lt8491_read_scratch(struct lt8491_desc *dev, uint32_t *val)
{
	uint16_t uval;
	int ret;

	ret = lt8491_reg_read(dev, LT8491_CFG_USER_CODE, &uval, true);
	if (ret)
		return ret;

	*val = uval;

	return 0;
}

/**
 * @brief Write the scratch register
 * @param dev - LT8491 descriptor
 * @param val - scratch register value
 * @return 0 in case of success, errno errors otherwise
 */
int lt8491_write_scratch(struct lt8491_desc *dev, uint32_t val)
{
	uint16_t uval = val;

	return lt8491_reg_write(dev, LT8491_CFG_USER_CODE, uval, true);
}

/**
 * @brief Read the serial ID
 * @param dev - LT8491 descriptor
 * @param val - serial ID value
 * @return 0 in case of success, errno errors otherwise
 */
int lt8491_read_serial_id(struct lt8491_desc *dev, uint32_t *val)
{
	uint16_t uval[3];
	int ret;

	ret = lt8491_reg_read(dev, LT8491_MFR_DATA1_REG, &uval[0], true);
	if (ret)
		return ret;

	val[0] = uval[0];

	ret = lt8491_reg_read(dev, LT8491_MFR_DATA2_REG, &uval[1], true);
	if (ret)
		return ret;

	val[1] = uval[1];

	ret = lt8491_reg_read(dev, LT8491_MFR_DATA3_REG, &uval[2], true);
	if (ret)
		return ret;

	val[2] = uval[2];

	return 0;
}
