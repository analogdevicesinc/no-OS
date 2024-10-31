/*******************************************************************************
*   @file   ltc4162l.c
*   @brief  Source code of the 	LTC4162L driver.
*   @authors Marc Paolo Sosa (marcpaolo.sosa@analog.com)
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
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#include "no_os_util.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_i2c.h"

#include "ltc4162l.h"

/**
 * @brief Computations for the LTC4162x Driver
 */
static uint16_t ltc4162l_get_vbat_calc(uint16_t reg, uint16_t cell_count,
				       uint16_t chem_type)
{
	return ((reg) * (cell_count) * 1924 / 10);
}

static uint16_t ltc4162s_get_vbat_calc(uint16_t reg, uint16_t cell_count,
				       uint16_t chem_type)
{
	return ((reg) * (cell_count) * 3848 / 10);
}

static uint16_t ltc4015_get_vbat_calc(uint16_t reg, uint16_t cell_count,
				      uint16_t chem_type)
{
	return ((reg) * (chem_type <= 6 ? 192264 : 128176) * (cell_count)) / 1000;
}

static uint16_t ltc4162l_get_vcharge_setting_calc(uint16_t reg,
		uint16_t cell_count,
		uint16_t chem_type)
{
	return (LTC4162L_BASE_VOLTAGE + (reg * LTC4162L_SCALE_FACTOR)) * cell_count;
}

static uint16_t ltc4162f_get_vcharge_setting_calc(uint16_t reg,
		uint16_t cell_count,
		uint16_t chem_type)
{
	return (LTC4162F_BASE_VOLTAGE + (reg * LTC4162L_SCALE_FACTOR)) * cell_count;
}

static uint16_t ltc4162s_get_vcharge_setting_calc(uint16_t reg,
		uint16_t cell_count,
		uint16_t chem_type)
{
	return (LTC4162S_BASE_VOLTAGE + (reg * LTC4162S_SCALE_FACTOR)) * cell_count / 2;
}

static uint16_t ltc4015_get_vcharge_setting_calc(uint16_t reg,
		uint16_t cell_count,
		uint16_t chem_type)
{
	switch (chem_type) {
	case 0 ... 3:
		return (LTC4015_BASE_VOLTAGE_TYPE1 + reg * LTC4015_SCALE_FACTOR_TYPE1) *
		       cell_count;
	case 4 ... 6:
		return (LTC4015_BASE_VOLTAGE_TYPE2 + reg * LTC4015_SCALE_FACTOR_TYPE2) *
		       cell_count;
	default:
		return (LTC4015_BASE_VOLTAGE_TYPE3 + reg * LTC4015_SCALE_FACTOR_TYPE3) *
		       cell_count;
	}
}

static uint16_t ltc4162l_get_die_temp_calc(uint16_t reg, uint16_t cell_count,
		uint16_t chem_type)
{
	return ((reg * 215 / 100) - 26440);
}

static uint16_t ltc4015_get_die_temp_calc(uint16_t reg, uint16_t cell_count,
		uint16_t chem_type)
{
	return (reg - (12010 * 1000 / 456));
}

static uint16_t ltc4162l_read_ibat_calc(struct ltc4162l_desc *dev,
					uint16_t signed_regval)
{
	return 100 * ((signed_regval * LTC4162L_MULTIPLIER) / (int)dev->rsnsb);
}

static uint16_t ltc4015_read_ibat_calc(struct ltc4162l_desc *dev,
				       uint16_t signed_regval)
{
	return 100 * ((signed_regval * LTC4015_MULTIPLIER) / (int)dev->rsnsb);
}

static uint16_t ltc4162l_read_input_current_calc(struct ltc4162l_desc *dev,
		uint16_t signed_regval)
{
	return signed_regval * LTC4162L_MULTIPLIER / dev->rsnsi * 100;
}

static uint16_t ltc4015_read_input_current_calc(struct ltc4162l_desc *dev,
		uint16_t signed_regval)
{
	return signed_regval * LTC4015_MULTIPLIER / dev->rsnsi * 100;
}

/**
 * @brief LTC4162x chip information
 */
const struct ltc4162l_chip_info ltc4162l_infos[] = {
	[ID_LTC4162L] = {
		.base_voltage = LTC4162L_BASE_VOLTAGE,
		.scale_factor = LTC4162L_SCALE_FACTOR,
		.range = LTC4162L_RANGE,
		.get_vbat = ltc4162l_get_vbat_calc,
		.get_die_temp = ltc4162l_get_die_temp_calc,
		.get_vcharge_setting = ltc4162l_get_vcharge_setting_calc,
		.read_ibat = ltc4162l_read_ibat_calc,
		.read_input_current = ltc4162l_read_input_current_calc,
	},
	[ID_LTC4162F] = {
		.base_voltage = LTC4162F_BASE_VOLTAGE,
		.scale_factor = LTC4162F_SCALE_FACTOR,
		.range = LTC4162F_RANGE,
		.get_vbat = ltc4162l_get_vbat_calc,
		.get_die_temp = ltc4162l_get_die_temp_calc,
		.get_vcharge_setting = ltc4162f_get_vcharge_setting_calc,
		.read_ibat = ltc4162l_read_ibat_calc,
		.read_input_current = ltc4162l_read_input_current_calc,
	},
	[ID_LTC4162S] = {
		.base_voltage = LTC4162S_BASE_VOLTAGE,
		.scale_factor = LTC4162S_SCALE_FACTOR,
		.range = LTC4162S_RANGE,
		.get_vbat = ltc4162s_get_vbat_calc,
		.get_die_temp = ltc4162l_get_die_temp_calc,
		.get_vcharge_setting = ltc4162s_get_vcharge_setting_calc,
		.read_ibat = ltc4162l_read_ibat_calc,
		.read_input_current = ltc4162l_read_input_current_calc,
	},
	[ID_LTC4015] = {
		.base_voltage = LTC4015_BASE_VOLTAGE_TYPE1,
		.scale_factor = LTC4015_SCALE_FACTOR_TYPE1,
		.range = LTC4015_RANGE_TYPE1,
		.get_vbat = ltc4015_get_vbat_calc,
		.get_die_temp = ltc4015_get_die_temp_calc,
		.get_vcharge_setting = ltc4015_get_vcharge_setting_calc,
		.read_ibat = ltc4015_read_ibat_calc,
		.read_input_current = ltc4015_read_input_current_calc,
	},
};

/**
 * @brief Write to a register of the LTC4162L
 * @param dev - The LTC4162L device structure.
 * @param addr - The address of the register to write to.
 * @param val - The value to write to the register.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_reg_write(struct ltc4162l_desc *dev, uint8_t addr, uint16_t val)
{
	uint8_t buff[] = {addr, (uint8_t)(val & 0xFF), (uint8_t)((val >> 8) & 0xFF)};

	return no_os_i2c_write(dev->i2c_desc, buff, 3, 1);
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
	uint8_t uval[2];

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
 * @param val - The value to write to the register.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_reg_update(struct ltc4162l_desc * dev, uint8_t addr, uint16_t mask,
			uint16_t val)
{
	int ret;
	uint16_t regval;

	ret = ltc4162l_reg_read(dev, addr, &regval);
	if (ret)
		return ret;

	regval &= ~mask;
	regval |= val & mask;

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

	dev->id = init_param->id;
	dev->info = &ltc4162l_infos[init_param->id];
	dev->rsnsb = init_param->rsnsb;
	dev->rsnsi = init_param->rsnsi;

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
	int ret;
	uint16_t regval;

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
int ltc4162l_read_vbat(struct ltc4162l_desc *dev, uint16_t *val)
{
	int ret;
	uint16_t regval, cell_count, chem_type;

	ret = ltc4162l_reg_read(dev, LTC4162L_VBAT, &regval);
	if (ret)
		return ret;

	ret = ltc4162l_get_cell_count(dev, &cell_count);
	if (ret)
		return ret;

	ret = ltc4162l_get_chem_type(dev, &chem_type);
	if (ret)
		return ret;

	*val = dev->info->get_vbat(regval, cell_count, chem_type);

	return 0;
}

/**
 * @brief Get the average battery voltage
 * @param dev - The LTC4162L device structure.
 * @param val - The average battery voltage.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_read_avg_vbat(struct ltc4162l_desc *dev, uint16_t *val)
{
	int ret;
	uint16_t regval, cell_count, chem_type;

	ret = ltc4162l_reg_read(dev, LTC4162L_VBAT_FILT, &regval);
	if (ret)
		return ret;

	ret = ltc4162l_get_cell_count(dev, &cell_count);
	if (ret)
		return ret;

	ret = ltc4162l_get_chem_type(dev, &chem_type);
	if (ret)
		return ret;

	*val = dev->info->get_vbat(regval, cell_count, chem_type);

	return 0;
}

/**
 * @brief Get the battery current
 * @param dev - The LTC4162L device structure.
 * @param val - The battery current.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_read_ibat(struct ltc4162l_desc *dev, uint16_t *val)
{
	int ret;
	int16_t signed_regval;
	uint16_t regval;

	ret = ltc4162l_reg_read(dev, LTC4162L_IBAT, &regval);
	if (ret)
		return ret;

	signed_regval = (int16_t)(regval & 0xFFFF);

	*val = dev->info->read_ibat(dev, signed_regval);

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
	int ret;
	uint16_t regval;

	ret = ltc4162l_reg_read(dev, LTC4162L_VIN, &regval);
	if (ret)
		return ret;

	if (dev->id == ID_LTC4015)
		*val = regval * LTC4015_VCHARGE_RANGE;
	else
		*val = regval * LTC4162L_VCHARGE_RANGE;

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
	int ret;
	int16_t signed_regval;
	uint16_t regval;

	ret = ltc4162l_reg_read(dev, LTC4162L_IIN, &regval);
	if (ret)
		return ret;

	signed_regval = (int16_t)(regval & 0xFFFF);

	*val = dev->info->read_input_current(dev, signed_regval);

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
	int ret;
	uint16_t regval;

	ret = ltc4162l_reg_read(dev, LTC4162L_CHARGE_CURRENT_SETTING, &regval);
	if (ret)
		return ret;

	regval = no_os_field_get(LTC4162L_ICHARGE_DAC_MASK, regval);

	/* The charge current servo level: (icharge_dac + 1) × 1mV/RSNSB */
	++regval;

	*val = 10000u * ((regval * 100000u) / dev->rsnsb);

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
	regval = (val * dev->rsnsb) / 100000u;
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
	int ret;
	uint16_t regval;

	ret = ltc4162l_reg_read(dev, LTC4162L_ICHARGE_DAC, &regval);
	if (ret)
		return ret;

	regval = no_os_field_get(LTC4162L_ICHARGE_DAC_MASK, regval);

	/* The charge current servo level: (icharge_dac + 1) × 1mV/RSNSB */
	++regval;

	*val = 10000u * ((regval * 100000u) / dev->rsnsb);

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
	int ret;
	uint16_t cell_count, regval, chem_type;

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

	*val = dev->info->get_vcharge_setting(regval, cell_count, chem_type);

	return 0;
}

/**
 * @brief Set the vcharge setting
 * @param dev - The LTC4162L device structure.
 * @param val - The vcharge setting.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_set_vcharge_setting(struct ltc4162l_desc *dev, uint16_t val)
{
	int ret;
	uint32_t base_voltage, scale_factor, range;
	uint16_t cell_count, chem_type;

	ret = ltc4162l_get_cell_count(dev, &cell_count);
	if (ret)
		return ret;

	ret = ltc4162l_get_chem_type(dev, &chem_type);
	if (ret)
		return ret;

	if (dev->id == ID_LTC4015)
		switch (chem_type) {
		case 0 ... 3:
			base_voltage = LTC4015_BASE_VOLTAGE_TYPE1;
			scale_factor = LTC4015_SCALE_FACTOR_TYPE1;
			range = LTC4015_RANGE_TYPE1;
			break;
		case 4 ... 6:
			base_voltage = LTC4015_BASE_VOLTAGE_TYPE2;
			scale_factor = LTC4015_SCALE_FACTOR_TYPE2;
			range = LTC4015_RANGE_TYPE2;
			break;
		default:
			base_voltage = LTC4015_BASE_VOLTAGE_TYPE3;
			scale_factor = LTC4015_SCALE_FACTOR_TYPE3;
			range = LTC4015_RANGE_TYPE3;
			break;
		} else
		dev->info->base_voltage;
	dev->info->scale_factor;
	dev->info->range;

	if (dev->id == ID_LTC4162S)
		cell_count /= 2;

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
	int ret;
	uint16_t regval;

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

	regval = (val * dev->rsnsi) / 50000u;
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
	int ret;
	uint16_t regval;

	ret = ltc4162l_reg_read(dev, LTC4162L_DIE_TEMPERATURE, &regval);
	if (ret)
		return ret;

	*val = dev->info->get_die_temp(regval, 0, 0);

	return 0;
}

/**
 * @brief Get the term current
 * @param dev - The LTC4162L device structure.
 * @param val - The term current.
 * @return 0 in case of success, negative error code otherwise.
 */
int ltc4162l_get_term_current(struct ltc4162l_desc *dev, uint16_t *val)
{
	int ret;
	uint16_t regval;

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
	*val = regval;

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

	regval = (val * dev->rsnsb) / 14660u;
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
	int ret;
	uint16_t regval;

	ret = ltc4162l_reg_read(dev, LTC4162L_CONFIG_BITS_REG, &regval);
	if (ret)
		return ret;

	if (dev->id == ID_LTC4015)
		*val = (no_os_field_get(LTC4162L_RUN_BSR_BIT, regval)) ? 1 : 0;
	else
		*val = (no_os_field_get(LTC4162L_FORCE_TELEMETRY_BIT, regval)) ? 1 : 0;

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

	if (dev->id == ID_LTC4015)
		ret = ltc4162l_reg_update(dev, LTC4162L_CONFIG_BITS_REG, NO_OS_BIT(4),
					  val ? NO_OS_BIT(4) : 0);

	ret = ltc4162l_reg_update(dev, LTC4162L_CONFIG_BITS_REG, NO_OS_BIT(2),
				  val ? NO_OS_BIT(2) : 0);

	return ret;
}
