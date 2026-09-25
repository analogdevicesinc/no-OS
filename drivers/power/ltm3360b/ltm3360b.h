/***************************************************************************//**
 *   @file   ltm3360b.h
 *   @brief  Header file of the LTM3360B driver
 *   @author Janani Sunil (janani.sunil@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __LTM3360B_H__
#define __LTM3360B_H__

#include <stdint.h>
#include <stdbool.h>
#include "no_os_util.h"
#include "no_os_i2c.h"

#define LTM3360B_I2C_ADDR			0x69

#define LTM3360B_EN_IMON_CTRL			0x01
#define LTM3360B_VOUT_SETTING			0x03
#define LTM3360B_ADC_SELECT			0x04
#define LTM3360B_ADC_CTRL			0x05
#define LTM3360B_ADC_OUTPUT			0x06

#define LTM3360B_IGNORE_EN_IN_BIT		NO_OS_BIT(0)
#define LTM3360B_EN_I2C_BIT			NO_OS_BIT(1)
#define LTM3360B_OPT_IMON_TH_MSK		NO_OS_GENMASK(3, 2)
#define LTM3360B_ADC_SEL_IN(x)			((x) & NO_OS_GENMASK(2, 0))
#define LTM3360B_OPT_IMON_TH(x)			(((x) << 2) & LTM3360B_OPT_IMON_TH_MSK)
#define LTM3360B_VOUT_LSB_MV			4
#define LTM3360B_VOUT_MV_TO_CODE(mv)	((mv) / LTM3360B_VOUT_LSB_MV)
#define LTM3360B_VOUT_CODE_TO_MV(code)	((code) * LTM3360B_VOUT_LSB_MV)
#define LTM3360B_EN_ADC_BIT			NO_OS_BIT(0)

/* LTM3360B Constants */
#define LTM3360B_ADC_CONV_TIME_MS	4
#define LTM3360B_VOUT_MIN_MV		297
#define LTM3360B_VOUT_MAX_MV		1000

/**
 * @enum ltm3360b_imon_threshold
 * @brief IMON Threshold settings
 */
enum ltm3360b_imon_threshold {
	LTM3360B_IMON_TH_99_PERCENT,
	LTM3360B_IMON_TH_89_PERCENT,
	LTM3360B_IMON_TH_79_PERCENT,
	LTM3360B_IMON_TH_69_PERCENT
};

/**
 * @enum ltm3360b_adc_channel
 * @brief ADC Input selection
 */
enum ltm3360b_adc_channel {
	LTM3360B_ADC_IMON,
	LTM3360B_ADC_TEMP,
	LTM3360B_ADC_VOUT_P,
	LTM3360B_ADC_VOUT_N,
	LTM3360B_ADC_VIN,
};

/**
 * @struct ltm3360b_dev
 * @brief LTM3360B Device Descriptor
 */
struct ltm3360b_dev {
	struct no_os_i2c_desc *i2c_desc;
	bool auto_enable;
};

/**
 * @struct ltm3360b_init_param
 * @brief LTM3360B Initialization Parameters
 */
struct ltm3360b_init_param {
	struct no_os_i2c_init_param *i2c_init;
	bool auto_enable;
	uint16_t vout_mv;
	enum ltm3360b_imon_threshold imon_threshold;
};

int ltm3360b_init(struct ltm3360b_dev **device,
		  struct ltm3360b_init_param *init_param);

int ltm3360b_remove(struct ltm3360b_dev *dev);

int ltm3360b_reg_read(struct ltm3360b_dev *dev, uint8_t reg, uint8_t *data);

int ltm3360b_reg_write(struct ltm3360b_dev *dev, uint8_t reg, uint8_t data);

int ltm3360b_reg_update(struct ltm3360b_dev *dev, uint8_t reg,
			uint8_t mask, uint8_t val);

int ltm3360b_enable_device(struct ltm3360b_dev *dev, bool enable);

int ltm3360b_set_vout_mv(struct ltm3360b_dev *dev, uint16_t vout_mv);

int ltm3360b_get_vout_mv(struct ltm3360b_dev *dev, uint16_t *vout_mv);

int ltm3360b_set_imon_threshold(struct ltm3360b_dev *dev,
				enum ltm3360b_imon_threshold threshold);

int ltm3360b_adc_enable(struct ltm3360b_dev *dev, bool enable);

int ltm3360b_adc_read_raw(struct ltm3360b_dev *dev,
			  enum ltm3360b_adc_channel channel, uint8_t *result);

#endif /* __LTM3360B_H__ */
