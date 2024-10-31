/*******************************************************************************
 *   @file   ltc4162l.h
 *   @brief  Header file of the LTC4162L Driver
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

#ifndef __LTC4162L_H__
#define __LTC4162L_H__

#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_i2c.h"

#define LTC4162L_EN_LIMIT_ALERTS_REG                0x0D
#define LTC4162L_EN_CHARGER_STATE_ALERTS_REG        0x0E
#define LTC4162L_EN_CHARGE_STATUS_ALERTS_REG        0x0F
#define LTC4162L_CONFIG_BITS_REG                    0x14
#define LTC4162L_IIN_LIMIT_TARGET                   0x15
#define LTC4162L_ARM_SHIP_MODE                      0x19
#define LTC4162L_CHARGE_CURRENT_SETTING             0X1A
#define LTC4162L_VCHARGE_SETTING                    0X1B
#define LTC4162L_C_OVER_X_THRESHOLD                 0x1C
#define LTC4162L_MAX_CV_TIME                        0X1D
#define LTC4162L_MAX_CHARGE_TIME                    0X1E
#define LTC4162L_CHARGER_CONFIG                     0x29
#define LTC4162L_CHARGER_STATE                      0x34
#define LTC4162L_CHARGE_STATUS                      0x35
#define LTC4162L_LIMIT_ALERTS_REG                   0x36
#define LTC4162L_CHARGER_STATE_ALERTS_REG           0x37
#define LTC4162L_CHARGE_STATUS_ALERTS_REG           0x38
#define LTC4162L_SYSTEM_STATUS_REG                  0x39
#define LTC4162L_VBAT                               0x3A
#define LTC4162L_VIN                                0x3B
#define LTC4162L_VOUT                               0x3C
#define LTC4162L_IBAT                               0x3D
#define LTC4162L_IIN                                0x3E
#define LTC4162L_DIE_TEMPERATURE                    0x3F
#define LTC4162L_THERMISTOR_VOLTAGE                 0x40
#define LTC4162L_BSR                                0x41
#define LTC4162L_JEITA_REGION                       0x42
#define LTC4162L_CHEM_CELLS_REG                     0x43
#define LTC4162L_ICHARGE_DAC                        0x44
#define LTC4162L_VCHARGE_DAC                        0x45
#define LTC4162L_IIN_LIMIT_DAC                      0x46
#define LTC4162L_VBAT_FILT                          0x47
#define LTC4162L_INPUT_UNDERVOLTAGE_DAC             0x4B

#define LTC4162L_CELL_COUNT_MASK                    NO_OS_GENMASK(3, 0)
#define LTC4162L_CHEM_TYPE_MASK                     NO_OS_GENMASK(11, 8)
#define LTC4162L_CHEM_MASK                          NO_OS_GENMASK(11, 8)
#define LTC4162L_ONLINE_STATUS_BIT                  NO_OS_BIT(2)
#define LTC4162L_EN_C_OVER_X_TERM_BIT               NO_OS_BIT(2)
#define LTC4162L_IIN_LIMIT_DAC_MASK                 NO_OS_GENMASK(6, 0)
#define LTC4162L_CHARGER_CONFIG_BIT                 NO_OS_BIT(2)
#define LTC4162L_ICHARGE_DAC_MASK                   NO_OS_GENMASK(4, 0)
#define LTC4162L_VIN_GT_VBAT_BIT                    NO_OS_BIT(2)
#define LTC4162L_VCHARGE_SETTING_MASK               NO_OS_GENMASK(4, 0)
#define LTC4162L_RUN_BSR_BIT                        NO_OS_BIT(4)
#define LTC4162L_FORCE_TELEMETRY_BIT                NO_OS_BIT(2)

#define LTC4162L_ARM_SHIP_MODE_MAGIC		21325
#define LTC4162L_MULTIPLIER			14660
#define LTC4015_MULTIPLIER			146487
#define LTC4162L_VCHARGE_RANGE			38125
#define LTC4015_VCHARGE_RANGE			34125

#define LTC4162L_BASE_VOLTAGE			3812500
#define LTC4162L_SCALE_FACTOR			12500
#define LTC4162L_RANGE				31

#define LTC4162F_BASE_VOLTAGE			3412500
#define LTC4162F_SCALE_FACTOR			12500
#define LTC4162F_RANGE				31

#define LTC4162S_BASE_VOLTAGE			6000000
#define LTC4162S_SCALE_FACTOR			28571
#define LTC4162S_RANGE				31

#define LTC4015_BASE_VOLTAGE_TYPE1		3812500
#define LTC4015_SCALE_FACTOR_TYPE1		12500
#define LTC4015_RANGE_TYPE1			31
#define LTC4015_BASE_VOLTAGE_TYPE2		3412500
#define LTC4015_SCALE_FACTOR_TYPE2		12500
#define LTC4015_RANGE_TYPE2			31
#define LTC4015_BASE_VOLTAGE_TYPE3		6000000
#define LTC4015_SCALE_FACTOR_TYPE3		28571
#define LTC4015_RANGE_TYPE3			35

/**
 * @brief LTC4162L device ID
 */
enum ltc4162l_id {
	ID_LTC4162L,
	ID_LTC4162F,
	ID_LTC4162S,
	ID_LTC4015,
};

/**
 * @brief LTC4162L commands
 */
enum ltc4162l_command {
	LTC4162L_CMD_GET_VBAT,
	LTC4162L_CMD_GET_VCHARGE,
	LTC4162L_CMD_GET_DIE_TEMP,
};

/**
 * @brief LTC4162L descriptor
 */
struct ltc4162l_desc {
	/** I2C Descriptor */
	struct no_os_i2c_desc *i2c_desc;
	struct ltc4162l_chip_info *info;
	enum ltc4162l_id id;
	uint32_t rsnsb;
	uint32_t rsnsi;
};

/**
 * @brief LTC4162L init param
 */
struct ltc4162l_init_param {
	/** Host processor I2C configuration */
	struct no_os_i2c_init_param i2c_init_param;
	/** Rsense1 in micro ohms */
	uint32_t rsnsb;
	uint32_t rsnsi;
	enum ltc4162l_id id;
};

/**
 * @brief LTC4162L chip info
 */
struct ltc4162l_chip_info {
	uint32_t base_voltage;
	uint32_t scale_factor;
	uint32_t range;
	uint16_t (*get_vcharge_setting)(uint16_t reg, uint16_t cell_count,
					uint16_t chem_type);
	uint16_t (*get_vbat)(uint16_t reg, uint16_t cell_count, uint16_t chem_type);
	uint16_t (*get_avg_vbat)(uint16_t reg, uint16_t cell_count, uint16_t chem_type);
	uint16_t (*get_die_temp)(uint16_t reg, uint16_t cell_count, uint16_t chem_type);
	uint16_t (*read_ibat)(struct ltc4162l_desc *dev, uint16_t signed_regval);
	uint16_t (*read_input_current)(struct ltc4162l_desc *dev,
				       uint16_t signed_regval);
};

/* Write register value*/
int ltc4162l_reg_write(struct ltc4162l_desc *dev, uint8_t addr, uint16_t val);

/* Read register value*/
int ltc4162l_reg_read(struct ltc4162l_desc *dev, uint8_t addr, uint16_t *val);

/* Update register value*/
int ltc4162l_reg_update(struct ltc4162l_desc *dev, uint8_t addr, uint16_t mask,
			uint16_t val);

/* Initialize the LTC4162L device */
int ltc4162l_init(struct ltc4162l_desc **dev,
		  struct ltc4162l_init_param *init_param);

/* Free the resources allocated by ltc4162l_init() */
int ltc4162l_remove(struct ltc4162l_desc *dev);

/* Get the number of cells */
int ltc4162l_get_cell_count(struct ltc4162l_desc *dev, uint16_t *val);

/* Get the chemistry type */
int ltc4162l_get_chem_type(struct ltc4162l_desc *dev, uint16_t *val);

/* Get the charger state */
int ltc4162l_get_charger_state(struct ltc4162l_desc *dev, uint16_t *val);

/* Get the charge status */
int ltc4162l_get_charge_status(struct ltc4162l_desc *dev, uint16_t *val);

/* Get the online status */
int ltc4162l_get_online_status(struct ltc4162l_desc *dev, uint16_t *val);

/* Get the battery voltage */
int ltc4162l_read_vbat(struct ltc4162l_desc *dev, uint16_t *val);

/* Get the average battery voltage */
int ltc4162l_read_avg_vbat(struct ltc4162l_desc *dev, uint16_t *val);

/* Get the battery current */
int ltc4162l_read_ibat(struct ltc4162l_desc *dev, uint16_t *val);

/* Get the input voltage */
int ltc4162l_read_input_voltage(struct ltc4162l_desc *dev, uint16_t *val);

/* Get the input current */
int ltc4162l_read_input_current(struct ltc4162l_desc *dev, uint16_t *val);

/* Get icharge setting*/
int ltc4162l_get_icharge_setting(struct ltc4162l_desc *dev, uint16_t *val);

/* Set icharge setting*/
int ltc4162l_set_icharge_setting(struct ltc4162l_desc *dev, uint16_t val);

/* Get icharge*/
int ltc4162l_get_icharge(struct ltc4162l_desc *dev, uint16_t *val);

/* Get vcharge setting*/
int ltc4162l_get_vcharge_setting(struct ltc4162l_desc *dev, uint16_t *val);

/* Set vcharge setting*/
int ltc4162l_set_vcharge_setting(struct ltc4162l_desc *dev, uint16_t val);

/* Get iin vcharge*/
int ltc4162l_get_vcharge(struct ltc4162l_desc *dev, uint16_t *val);

/* Get iin limit*/
int ltc4162l_get_iin_limit(struct ltc4162l_desc *dev, uint16_t *val);

/* Set iin limit*/
int ltc4162l_set_iin_limit(struct ltc4162l_desc *dev, uint16_t val);

/* Get die temperature*/
int ltc4162l_get_die_temperature(struct ltc4162l_desc *dev, uint16_t *val);

/* Get term current*/
int ltc4162l_get_term_current(struct ltc4162l_desc *dev, uint16_t *val);

/* Set term current*/
int ltc4162l_set_term_current(struct ltc4162l_desc *dev, uint16_t val);

/* Get telemetry*/
int ltc4162l_get_telemetry(struct ltc4162l_desc *dev, uint16_t *val);

/* Set telemetry*/
int ltc4162l_set_telemetry(struct ltc4162l_desc *dev, uint16_t val);

#endif //__LTC4162L_H__
