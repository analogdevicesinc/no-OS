/***************************************************************************//**
 *   @file   lt8491.h
 *   @brief  Implementation of LT8491 Driver.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __LT8491_H__
#define __LT8491_H__

#include "no_os_i2c.h"
#include "no_os_util.h"

#define LT8491_TELE_TBAT_REG 0x0
#define LT8491_TELE_POUT_REG 0x2
#define LT8491_TELE_PIN_REG  0x4
#define LT8491_TELE_EFF_REG  0x6
#define LT8491_TELE_IOUT_REG 0x8
#define LT8491_TELE_IIN_REG  0xA
#define LT8491_TELE_VBAT_REG 0xC
#define LT8491_TELE_VIN_REG  0xE
#define LT8491_TELE_VINR_REG 0x10

#define LT8491_STAT_CHARGER_REG 0x12
#define LT8491_STAT_SUPPLY_REG 0x14

#define LT8491_CTRL_CHRG_EN_REG 0x23
#define LT8491_CTRL_RESTART_CHIP_REG 0x24
#define LT8491_CTRL_UPDATE_TELEM 0x26

#define LT8491_CFG_RSENSE1_REG 0x28
#define LT8491_CFG_RIMON_OUT_REG 0x2A
#define LT8491_CFG_RSENSE2_REG 0x2C
#define LT8491_CFG_RDACO_REG 0x2E
#define LT8491_CFG_RFBOUT1_REG 0x30
#define LT8491_CFG_RFBOUT2_REG 0x32
#define LT8491_CFG_RDACI_REG 0x34
#define LT8491_CFG_RFBIN2_REG 0x36
#define LT8491_CFG_RFBIN1_REG 0x38
#define LT8491_CFG_TBAT_MIN_REG 0x40
#define LT8491_CFG_TBAT_MAX_REG 0x41
#define LT8491_CFG_CHRG_MISC_REG 0x4D

#define LT8491_CFG_USER_CODE 0x5A
#define LT8491_MFR_DATA1_REG 0x5C
#define LT8491_MFR_DATA2_REG 0x5E
#define LT8491_MFR_DATA3_REG 0x60

#define LT8491_CHRG_STAGE_MASK NO_OS_GENMASK(5, 3)
#define LT8491_CHARGING_MASK NO_OS_BIT(2)

#define LT8491_PS_OR_SOLAR_MASK NO_OS_BIT(3)

/**
 * @brief LT8491 charging stage
 */
enum lt8491_charging_stage {
	LT8491_STAGE0,
	LT8491_STAGE1,
	LT8491_STAGE2,
	LT8491_STAGE3,
	LT8491_DONE_CHARGING,
};

/**
 * @brief LT8491 descriptor
 */
struct lt8491_desc {
	/** I2C Descriptor */
	struct no_os_i2c_desc *i2c_desc;
};

/**
 * @brief LT8491 init param
 */
struct lt8491_init_param {
	/** Host processor I2C configuration */
	struct no_os_i2c_init_param i2c_init_param;
	/** Rsense1 in micro ohms */
	unsigned int rsense1_micro_ohms;
	/** Rimon out in ohms */
	unsigned int rimon_out_ohms;
	/** Rsense2 in micro ohms */
	unsigned int rsense2_micro_ohms;
	/** Rdaco in ohms */
	unsigned int rdaco_ohms;
	/** Rfbout1 in ohms */
	unsigned int rfbout1_ohms;
	/** Rfbout2 in ohms */
	unsigned int rfbout2_ohms;
	/** Rdaci in ohms */
	unsigned int rdaci_ohms;
	/** Rfbin2 in ohms */
	unsigned int rfbin2_ohms;
	/** Rfbin1 in ohms */
	unsigned int rfbin1_ohms;
};

/** Read a register value */
int lt8491_reg_read(struct lt8491_desc *dev, uint8_t addr, uint16_t *val,
		    bool is_word);

/** Write a register value */
int lt8491_reg_write(struct lt8491_desc *dev, uint8_t addr, uint16_t val,
		     bool is_word);

/** Device and comm init function */
int lt8491_init(struct lt8491_desc **dev,
		struct lt8491_init_param *init_param);

/** Free resources allocated by the init function */
int lt8491_remove(struct lt8491_desc *dev);

/** Device and comm init function */
int lt8491_configure_telemetry(struct lt8491_desc *dev,
			       struct lt8491_init_param *init_param);

/** Read the telemetry battery temperature */
int lt8491_read_tbat(struct lt8491_desc *dev, int32_t *val);

/** Read the telemetry battery output power */
int lt8491_read_pout(struct lt8491_desc *dev, uint32_t *val);

/** Read the telemetry battery input power */
int lt8491_read_pin(struct lt8491_desc *dev, uint32_t *val);

/** Read the telemetry battery efficiency */
int lt8491_read_efficiency(struct lt8491_desc *dev, uint32_t *val);

/** Read the telemetry battery output current */
int lt8491_read_iout(struct lt8491_desc *dev, uint32_t *val);

/** Read the telemetry battery input current */
int lt8491_read_iin(struct lt8491_desc *dev, uint32_t *val);

/** Read the telemetry battery voltage */
int lt8491_read_vbat(struct lt8491_desc *dev, uint32_t *val);

/** Read the telemetry input voltage */
int lt8491_read_vin(struct lt8491_desc *dev, uint32_t *val);

/** Read the telemetry battery charging stage */
int lt8491_read_charging_stage(struct lt8491_desc *dev, uint32_t *uval);

/** Read the telemetry battery charging status */
int lt8491_read_charging_status(struct lt8491_desc *dev, uint32_t *charging);

/** Read the telemetry battery charging enable */
int lt8491_read_enable(struct lt8491_desc *dev, uint32_t *enable);

/** Write the telemetry battery charging enable */
int lt8491_write_enable(struct lt8491_desc *dev, uint32_t enable);

/** Restart the chip */
int lt8491_restart_chip(struct lt8491_desc *dev);

/** Read the scratch register */
int lt8491_read_scratch(struct lt8491_desc *dev, uint32_t *val);

/** Write the scratch register */
int lt8491_write_scratch(struct lt8491_desc *dev, uint32_t val);

/** Read the serial ID */
int lt8491_read_serial_id(struct lt8491_desc *dev, uint32_t *val);

#endif	/* __LT8491_H__ */
