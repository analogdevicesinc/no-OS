/*******************************************************************************
*   @file   ltc4162l.c
*   @brief  Source code of the 	LTC4162L driver.
*   @authors Marc Paolo Sosa (marcpaolo.sosa@analog.com)
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
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#include "no_os_util.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_i2c.h"

#include "ltc4162l.h"

/**
 * @brief Write to a register of the LTC4162L
 * @param dev - The LTC4162L device structure.
 * @param addr - The address of the register to write to.
 * @param val - The value to write to the register.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_reg_write(struct ltc4162l_desc * dev, uint8_t addr, uint16_t val)
{
	uint8_t buff [] = {addr, val & 0xFF};

	return no_os_i2c_write(dev->i2c_desc, buff, 2, 1);
}

/**
 * @brief Read from a register of the LTC4162L
 * @param dev - The LTC4162L device structure.
 * @param addr - The address of the register to read from.
 * @param val - The value read from the register.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_reg_read(struct ltc4162l_desc * dev, uint8_t addr, uint16_t *val)
{
	int ret;
	uint8_t uval [2];

	ret = no_os_i2c_write(dev->i2c_desc, &addr, 1, 0);
	if (ret)
		return ret;

	ret = no_os_i2c_read(dev->i2c_desc, uval, 1, 1);
	if (ret)
		return ret;

	*val = uval[0];

	return 0;
}

/**
 * @brief Update a register of the LTC4162L
 * @param dev - The LTC4162L device structure.
 * @param addr - The address of the register to update.
 * @param mask - The mask to apply to the register.
 * @param value - The value to write to the register.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_reg_update(struct ltc4162l_desc * dev, uint8_t addr, uint16_t mask,
			uint16_t value)
{
	int ret;
	uint16_t regval;

	ret = ltc4162l_reg_read(dev, addr, &regval);
	if (ret)
		return ret;

	regval &= ~mask;
	regval |= value & mask;

	return ltc4162l_reg_write(dev, addr, regval);
}

/**
 * @brief Initialize the LTC4162L device
 * @param device - The LTC4162L device structure.
 * @param init_param - The LTC4162L initialization structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_init(struct ltc4162l_desc **device,
		  struct ltc4162l_init_param *init_param)
{
	struct ltc4162l_desc *dev;
	int ret;

	dev = no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_i2c_init(&dev->i2c_desc, &init_param->i2c_init_param);
	if (ret)
		goto free_dev;

	*device = dev;

	return 0;

free_dev:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free the resources allocated by ltc4162l_init()
 * @param dev - The LTC4162L device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_remove(struct ltc4162l_desc *dev)
{
	int ret;

	ret = no_os_i2c_remove(dev->i2c_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return ret;
}

/**
 * @brief Get the number of cells
 * @param dev - The LTC4162L device structure.
 * @param val - The number of cells.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_get_cell_count(struct ltc4162l_desc *dev, uint16_t *val)
{
	uint16_t regval;
	int ret;

	ret = ltc4162l_reg_read(dev, LTC4162L_CHEM_CELLS_REG, &regval);

	if (ret)
		return ret;

	*val = no_os_field_get(LTC4162L_CELL_COUNT_MASK, regval);

	return 0;
}

/**
 * @brief Get the chemistry type
 * @param dev - The LTC4162L device structure.
 * @param val - The chemistry type.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_get_chem_type(struct ltc4162l_desc *dev, uint16_t *val)
{
	int ret;
	uint16_t regval;

	ret = ltc4162l_reg_read(dev, LTC4162L_CHEM_CELLS_REG, &regval);
	if (ret)
		return ret;

	*val = no_os_field_get(LTC4162L_CHEM_TYPE_MASK, regval);

	return 0;
}

/**
 * @brief Get the charger state
 * @param dev - The LTC4162L device structure.
 * @param val - The charger state.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_get_charger_state(struct ltc4162l_desc *dev, uint16_t *val)
{
	int ret;
	uint16_t regval;

	ret = ltc4162l_reg_read(dev, LTC4162L_CHARGE_STATUS, &regval);
	if (ret)
		return ret;

	*val = regval;

	return 0;
}

/**
 * @brief Get the charge status
 * @param dev - The LTC4162L device structure.
 * @param val - The charge status.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_get_charge_status(struct ltc4162l_desc *dev, uint16_t *val)
{
	int ret;
	uint16_t regval;

	ret = ltc4162l_reg_read(dev, LTC4162L_CHARGE_STATUS, &regval);
	if (ret)
		return ret;

	*val = regval;

	return 0;
}

/**
 * @brief Get the online status
 * @param dev - The LTC4162L device structure.
 * @param val - The online status.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_get_online_status(struct ltc4162l_desc *dev, uint16_t *val)
{
	int ret;
	uint16_t regval;

	ret = ltc4162l_reg_read(dev, LTC4162L_SYSTEM_STATUS_REG, &regval);
	if (ret)
		return ret;

	*val = no_os_field_get(LTC4162L_ONLINE_STATUS_BIT, regval);

	return 0;
}

/**
 * @brief Get the battery voltage
 * @param dev - The LTC4162L device structure.
 * @param val - The battery voltage.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_read_vbat(struct ltc4162l_desc *dev, uint16_t *vbat)
{
	uint16_t regval;
	int ret;
	uint16_t cell_count;
	uint16_t chem_type;

	ret = ltc4162l_reg_read(dev, LTC4162L_VBAT, &regval);
	if (ret)
		return ret;

	ret = ltc4162l_get_cell_count(dev, &cell_count);
	if (ret)
		return ret;

	ret = ltc4162l_get_chem_type(dev, &chem_type);
	if (ret)
		return ret;

	switch (dev->type) {
	case ID_LTC4162L:
	case ID_LTC4162F:
		regval *= 1924;
		regval *= cell_count;
		regval /= 10;
		*vbat = regval;
		return 0;
	case ID_LTC4162S:
		regval *= 3848;
		regval *= cell_count / 2;
		regval /= 10;
		*vbat = regval;
		return 0;
	case ID_LTC4015:
		regval *= chem_type <= 6 ? 192264 : 128176;
		regval *= cell_count;
		regval /= 1000;
		*vbat = regval;
		return 0;
	default:
		return -EINVAL;
	}

}

/**
 * @brief Get the average battery voltage
 * @param dev - The LTC4162L device structure.
 * @param val - The average battery voltage.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_read_avg_vbat(struct ltc4162l_desc *dev, uint16_t *vbat)
{
	uint16_t regval;
	int ret;
	uint16_t cell_count;
	uint16_t chem_type;

	ret = ltc4162l_reg_read(dev, LTC4162L_VBAT_FILT, &regval);
	if (ret)
		return ret;

	ret = ltc4162l_get_cell_count(dev, &cell_count);
	if (ret)
		return ret;

	ret = ltc4162l_get_chem_type(dev, &chem_type);
	if (ret)
		return ret;

	switch (dev->type) {
	case ID_LTC4162L:
	case ID_LTC4162F:
		regval *= 1924;
		regval *= cell_count;
		regval /= 10;
		*vbat = regval;
		return 0;
	case ID_LTC4162S:
		regval *= 3848;
		regval *= cell_count / 2;
		regval /= 10;
		*vbat = regval;
		return 0;
	case ID_LTC4015:
		regval *= chem_type <= 6 ? 192264 : 128176;
		regval *= cell_count;
		regval /= 1000;
		*vbat = regval;
		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Get the battery current
 * @param dev - The LTC4162L device structure.
 * @param val - The battery current.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_read_ibat(struct ltc4162l_desc *dev, uint16_t *val)
{
	uint16_t regval;
	int ret;

	ret = ltc4162l_reg_read(dev, LTC4162L_IBAT, &regval);
	if (ret)
		return ret;

	ret = (int16_t)(regval & 0xFFFF);

	*val = 100 * LTC4162L_MULT_FRAC(ret, dev->type == ID_LTC4015 ? 146487 : 14660,
					(int)dev->rsnsb);

	return 0;
}

/**
 * @brief Get the input voltage
 * @param dev - The LTC4162L device structure.
 * @param val - The input voltage.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_read_input_voltage(struct ltc4162l_desc *dev, uint16_t *val)
{
	uint16_t regval;
	int ret;

	ret = ltc4162l_reg_read(dev, LTC4162L_VIN, &regval);
	if (ret)
		return ret;

	*val = regval * (dev->type == ID_LTC4015 ? 38125 : 34125);

	return 0;
}

/**
 * @brief Get the input current
 * @param dev - The LTC4162L device structure.
 * @param val - The input current.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_read_input_current(struct ltc4162l_desc *dev, uint16_t *val)
{
	uint16_t regval;
	int ret;

	ret = ltc4162l_reg_read(dev, LTC4162L_IIN, &regval);
	if (ret)
		return ret;

	ret = (int16_t)(regval & 0xFFFF);
	ret *= dev->type == ID_LTC4015 ? 146487 : 14660;
	ret /= dev->rsnsi;
	ret *= dev->type == ID_LTC4015 ? 1000 : 100;

	*val = ret;

	return 0;
}

/**
 * @brief Get icharge setting
 * @param dev - The LTC4162L device structure.
 * @param val - The icharge setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_get_icharge_setting(struct ltc4162l_desc *dev, uint16_t *val)
{
	uint16_t regval;
	int ret;

	ret = ltc4162l_reg_read(dev, LTC4162L_CHARGE_CURRENT_SETTING, &regval);
	if (ret)
		return ret;

	regval = no_os_field_get(LTC4162L_ICHARGE_DAC_MASK, regval);

	/* The charge current servo level: (icharge_dac + 1) × 1mV/RSNSB */
	++regval;

	*val = 10000u * LTC4162L_MULT_FRAC(regval, 100000u, dev->rsnsb);

	return 0;
}

/**
 * @brief Set icharge setting
 * @param dev - The LTC4162L device structure.
 * @param val - The icharge setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_set_icharge_setting(struct ltc4162l_desc *dev, uint16_t val)
{
	uint16_t regval;

	/* The charge current servo level: (icharge_dac + 1) × 1mV/RSNSB */
	regval = LTC4162L_MULT_FRAC(val, dev->rsnsb, 100000u);
	regval /= 10000u;

	if (regval)
		--regval;

	if (regval > 31)
		return -EINVAL;

	return ltc4162l_reg_write(dev, LTC4162L_CHARGE_CURRENT_SETTING, regval);
}

/**
 * @brief Get icharge value
 * @param dev - The LTC4162L device structure.
 * @param val - The icharge value.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_get_icharge(struct ltc4162l_desc *dev, uint16_t *val)
{
	uint16_t regval;
	int ret;

	ret = ltc4162l_reg_read(dev, LTC4162L_ICHARGE_DAC, &regval);
	if (ret)
		return ret;

	regval = no_os_field_get(LTC4162L_ICHARGE_DAC_MASK, regval);

	/* The charge current servo level: (icharge_dac + 1) × 1mV/RSNSB */
	++regval;

	*val = 10000u * LTC4162L_MULT_FRAC(regval, 100000u, dev->rsnsb);

	return 0;
}

/**
 * @brief Get the vcharge setting
 * @param dev - The LTC4162L device structure.
 * @param val - The vcharge setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_get_vcharge_setting(struct ltc4162l_desc *dev, uint16_t *val)
{
	uint16_t regval;
	int ret;
	uint32_t voltage;
	uint16_t cell_count;
	uint16_t chem_type;

	ret = ltc4162l_reg_read(dev, LTC4162L_VCHARGE_SETTING, &regval);

	if (ret)
		return ret;

	regval = no_os_field_get(LTC4162L_VCHARGE_SETTING_MASK, regval);

	ret = ltc4162l_get_cell_count(dev, &cell_count);
	if (ret)
		return ret;

	ret = ltc4162l_get_chem_type(dev, &chem_type);
	if (ret)
		return ret;

	switch (dev->type) {
	case ID_LTC4162L:
		voltage = 3812500 + (regval * 12500);
		voltage *= cell_count;
		*val = voltage;

		return 0;
	case ID_LTC4162F:
		voltage = 3412500 + (regval * 12500);
		voltage *= cell_count;
		*val = voltage;

		return 0;
	case ID_LTC4162S:
		voltage = 6000000 + (regval * 28571);
		voltage *= cell_count/2;
		*val = voltage;

		return 0;
	case ID_LTC4015:
		if (chem_type <= 3)
			voltage = 3812500 + (regval * 12500);
		else if (chem_type > 3 || chem_type < 7)
			voltage = 3412500 + (regval * 12500);
		else
			voltage = 2000000 + (regval * 9523);

		voltage *= cell_count;
		*val = voltage;
		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Set the vcharge setting
 * @param dev - The LTC4162L device structure.
 * @param val - The vcharge setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_set_vcharge_setting(struct ltc4162l_desc *dev, uint16_t val)
{
	uint32_t base_voltage, scale_factor, range;
	uint16_t cell_count;
	uint16_t chem_type;
	int ret;

	ret = ltc4162l_get_cell_count(dev, &cell_count);
	if (ret)
		return ret;

	ret = ltc4162l_get_chem_type(dev, &chem_type);
	if (ret)
		return ret;

	switch (dev->type) {
	case ID_LTC4162L:
		base_voltage = 3812500;
		scale_factor = 12500;
		range = 31;
		break;
	case ID_LTC4162F:
		base_voltage = 3412500;
		scale_factor = 12500;
		range = 31;
		break;
	case ID_LTC4162S:
		base_voltage = 6000000;
		scale_factor = 28571;
		range = 31;
		break;
	case ID_LTC4015:
		if (chem_type <= 3) {
			base_voltage = 3812500;
			scale_factor = 12500;
			range = 31;
		} else if (chem_type < 7) {
			base_voltage = 3412500;
			scale_factor = 12500;
			range = 31;
		} else {
			base_voltage = 2000000;
			scale_factor = 9523;
			range = 35;
		}
		break;
	default:
		return -EINVAL;
	}

	cell_count = dev->type == ID_LTC4162S ? cell_count / 2 : cell_count;

	if (!cell_count)
		return -EBUSY; /* Not available yet, try again later */

	/* Adjust value based on cell count and limits */
	val /= cell_count;
	if (val < base_voltage)
		return -EINVAL;

	val = (val - base_voltage) / scale_factor;
	if (val > range)
		return -EINVAL;

	return ltc4162l_reg_write(dev, LTC4162L_VCHARGE_SETTING, val);
}

/**
 * @brief Get the iin limit
 * @param dev - The LTC4162L device structure.
 * @param val - The iin limit.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_get_iin_limit(struct ltc4162l_desc *dev, uint16_t *val)
{
	uint16_t regval;
	int ret;

	ret = ltc4162l_reg_read(dev, LTC4162L_IIN_LIMIT_DAC, &regval);
	if (ret)
		return ret;

	regval = no_os_field_get(LTC4162L_IIN_LIMIT_DAC_MASK, regval);

	++regval;
	regval *= 5000000u;
	regval /= dev->rsnsi;
	*val = 100u * regval;

	return 0;
}

/**
 * @brief Set the iin limit
 * @param dev - The LTC4162L device structure.
 * @param val - The iin limit.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_set_iin_limit(struct ltc4162l_desc *dev, uint16_t val)
{
	uint16_t regval;

	regval = LTC4162L_MULT_FRAC(val, dev->rsnsi, 50000u);
	regval /= 10000u;
	if (regval)
		--regval;

	return ltc4162l_reg_update(dev, LTC4162L_IIN_LIMIT_DAC,
				   LTC4162L_IIN_LIMIT_DAC_MASK, regval);
}

/**
 * @brief Get the die temperature
 * @param dev - The LTC4162L device structure.
 * @param val - The die temperature.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_get_die_temp(struct ltc4162l_desc *dev, uint16_t *val)
{
	uint16_t regval;
	int ret;

	ret = ltc4162l_reg_read(dev, LTC4162L_DIE_TEMPERATURE, &regval);
	if (ret)
		return ret;

	switch (dev->type) {
	case ID_LTC4162L:
	case ID_LTC4162F:
	case ID_LTC4162S:
		ret = (int16_t)(regval & 0xFFFF);
		ret *= 215;
		ret /= 100;
		ret -= 26440;
		*val = ret;

		return 0;
	case ID_LTC4015:
		ret = (int16_t)(regval & 0xFFFF);
		ret -= 12010;
		ret = no_os_div_s64((int16_t)ret * 1000, 45600);
		*val = ret;

		return 0;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Get the term current
 * @param dev - The LTC4162L device structure.
 * @param val - The term current.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_get_term_current(struct ltc4162l_desc *dev, uint16_t *val)
{
	uint16_t regval;
	int ret;

	ret = ltc4162l_reg_read(dev, LTC4162L_CHARGER_CONFIG, &regval);
	if (ret)
		return ret;

	if (!(regval & LTC4162L_CHARGER_CONFIG_BIT)) {
		*val = 0;
		return 0;
	}

	ret = ltc4162l_reg_read(dev, LTC4162L_C_OVER_X_THRESHOLD, &regval);
	if (ret)
		return ret;

	regval *= 14660u;
	regval /= dev->rsnsb;
	*val = 100 * regval;

	return 0;
}

/**
 * @brief Set the term current
 * @param dev - The LTC4162L device structure.
 * @param val - The term current.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_set_term_current(struct ltc4162l_desc *dev, uint16_t val)
{
	int ret;
	uint16_t regval;

	if (!val) {
		return ltc4162l_reg_update(dev, LTC4162L_CHARGER_CONFIG,
					   LTC4162L_EN_C_OVER_X_TERM_BIT, 0);
	}

	regval = LTC4162L_MULT_FRAC(val, dev->rsnsb, 14660u);
	regval /= 100u;

	ret = ltc4162l_reg_write(dev, LTC4162L_C_OVER_X_THRESHOLD, regval);
	if (ret)
		return ret;

	return ltc4162l_reg_update(dev, LTC4162L_CHARGER_CONFIG,
				   LTC4162L_EN_C_OVER_X_TERM_BIT, LTC4162L_EN_C_OVER_X_TERM_BIT);
}

/**
 * @brief Get the telemetry
 * @param dev - The LTC4162L device structure.
 * @param val - The telemetry.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_get_telemetry(struct ltc4162l_desc *dev, uint16_t *val)
{
	uint16_t regval;
	int ret;

	ret = ltc4162l_reg_read(dev, LTC4162L_CONFIG_BITS_REG, &regval);
	if (ret)
		return ret;

	if (dev->type == ID_LTC4015) {
		*val = (no_os_field_get(LTC4162l_RUN_BSR_BIT, regval)) ? 1 : 0;

	} else {
		*val = (no_os_field_get(LTC4162L_FORCE_TELEMETRY_BIT, regval)) ? 1 : 0;
	}

	return 0;
}

/**
 * @brief Set the telemetry
 * @param dev - The LTC4162L device structure.
 * @param val - The telemetry.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_set_telemetry(struct ltc4162l_desc *dev, uint16_t val)
{
	int ret;
	uint16_t regval;

	ret = ltc4162l_reg_read(dev, LTC4162L_CONFIG_BITS_REG, &regval);
	if (ret)
		return ret;

	if (dev->type == ID_LTC4015)
		ret = ltc4162l_reg_update(dev, LTC4162L_CONFIG_BITS_REG, NO_OS_BIT(4),
					  val ? NO_OS_BIT(4) : 0);

	ret = ltc4162l_reg_update(dev, LTC4162L_CONFIG_BITS_REG, NO_OS_BIT(2),
				  val ? NO_OS_BIT(2) : 0);

	return ret;
}
