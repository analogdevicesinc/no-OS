/***************************************************************************//**
 *   @file   max31827.h
 *   @brief  Implementation of MAX31827 Driver.
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __MAX31827_H__
#define __MAX31827_H__

#include "no_os_i2c.h"
#include "no_os_util.h"

#define MAX31827_T_REG	0x0
#define MAX31827_CONF_REG	0x2
#define MAX31827_TH_REG	0x4
#define MAX31827_TL_REG 0x6
#define MAX31827_TH_HYST_REG	0x8
#define MAX31827_TL_HYST_REG	0xA

#define MAX31827_CONF_1SHOT_MASK	NO_OS_BIT(0)
#define MAX31827_CONF_CNV_RATE_MASK	NO_OS_GENMASK(3, 1)
#define MAX31827_CONF_PEC_EN_MASK	NO_OS_BIT(4)
#define MAX31827_CONF_TIMEOUT_MASK	NO_OS_BIT(5)
#define MAX31827_CONF_RESO_MASK	NO_OS_GENMASK(7, 6)
#define MAX31827_CONF_ALRM_POL_MASK	NO_OS_BIT(8)
#define MAX31827_CONF_COMP_INT_MASK	NO_OS_BIT(9)
#define MAX31827_CONF_FLT_Q_MASK	NO_OS_GENMASK(11, 10)
#define MAX31827_CONF_U_TEMP_STAT_MASK	NO_OS_BIT(14)
#define MAX31827_CONF_O_TEMP_STAT_MASK	NO_OS_BIT(15)

#define MAX31827_CONF_CNV_RATE(x) \
	no_os_field_get(MAX31827_CONF_CNV_RATE_MASK, x)
#define MAX31827_CONF_RESO(x) \
	no_os_field_get(MAX31827_CONF_RESO_MASK, x)
#define MAX31827_CONF_U_TEMP_STAT(x) \
	no_os_field_get(MAX31827_CONF_U_TEMP_STAT_MASK, x)
#define MAX31827_CONF_O_TEMP_STAT(x) \
	no_os_field_get(MAX31827_CONF_O_TEMP_STAT_MASK, x)

#define MAX31827_ALRM_POL_LOW	0x0
#define MAX31827_ALRM_POL_HIGH	0x1
#define MAX31827_FLT_Q_1	0x0
#define MAX31827_FLT_Q_4	0x2

#define MAX31827_M_DGR_TO_16_BIT(x)	(((x) << 4) / 1000)
#define MAX31827_DEVICE_ENABLE(x)	((x) ? 0xA : 0x0)

/**
 * @brief MAX31827 conversion rate
 */
enum max31827_cnv {
	MAX31827_CNV_1_DIV_64_HZ = 1,
	MAX31827_CNV_1_DIV_32_HZ,
	MAX31827_CNV_1_DIV_16_HZ,
	MAX31827_CNV_1_DIV_4_HZ,
	MAX31827_CNV_1_HZ,
	MAX31827_CNV_4_HZ,
	MAX31827_CNV_8_HZ,
};

/**
 * @brief MAX31827 temperature resolution
 */
enum max31827_resolution {
	MAX31827_RES_8_BIT = 0,
	MAX31827_RES_9_BIT,
	MAX31827_RES_10_BIT,
	MAX31827_RES_12_BIT,
};

/**
 * @brief MAX31827 conversion period in ms
 */
extern const uint16_t max31827_conversions[8];

/**
 * @brief MAX31827 temperature resolution
 */
extern const uint16_t max31827_resolutions[4];

/**
 * @brief MAX31827 temperature conversion times in us
 */
extern const int max31827_conv_times[4];

/**
 * @brief MAX31827 descriptor
 */
struct max31827_device {
	/** I2C Descriptor */
	struct no_os_i2c_desc *i2c_desc;
	/** Enables device */
	bool enable;
	/** Temperature resolution */
	unsigned int resolution;
	/** Conversion period */
	unsigned int update_interval;
};

/**
 * @brief MAX31827 init param
 */
struct max31827_init_param {
	/** Host processor I2C configuration */
	struct no_os_i2c_init_param i2c_init_param;
	/** Alarm mode */
	bool comp_int;
	/** Alarm active state */
	bool alarm_pol;
	/** Fault queue length */
	uint8_t fault_q;
	/** Enables timeout */
	bool timeout_enable;
};

/** Read a register value */
int max31827_reg_read(struct max31827_device *dev, uint8_t addr, uint16_t *val);

/** Write a register value */
int max31827_reg_write(struct max31827_device *dev, uint8_t addr, uint16_t val);

/** Read-modify-write operation */
int max31827_reg_update_bits(struct max31827_device *dev, uint8_t addr,
			     uint16_t mask, uint16_t val);

/** Device and comm init function */
int max31827_init(struct max31827_device **dev,
		  struct max31827_init_param *init_param);

/** Free resources allocated by the init function */
int max31827_remove(struct max31827_device *dev);

/** Initialize MAX31827 device setup */
int max31827_init_client(struct max31827_device *dev,
			 struct max31827_init_param *init_param);

/** Read a temperature from a register */
int max31827_read_temp(struct max31827_device *dev, uint8_t addr, int32_t *val);

/** Read input temperature */
int max31827_read_temp_input(struct max31827_device *dev, int32_t *val);

/** Shutdown then write register value */
int max31827_shutdown_write(struct max31827_device *dev, uint8_t reg,
			    uint16_t mask, uint16_t val);

/** Write the alarm value to the specified register */
int max31827_write_alarm_val(struct max31827_device *dev, unsigned int reg,
			     int32_t val);

#endif	/* __MAX31827_H__ */
