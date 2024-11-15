/***************************************************************************//**
 *   @file   ad5592r.h
 *   @brief  Header file of AD5592R driver.
 *   @author Mircea Caprioru (mircea.caprioru@analog.com)
********************************************************************************
 * Copyright 2018, 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef AD5592R_H_
#define AD5592R_H_

#include "stdint.h"
#include "ad5592r-base.h"

#define AD5592R_GPIO_READBACK_EN	NO_OS_BIT(10)
#define AD5592R_LDAC_READBACK_EN	NO_OS_BIT(6)

#define swab16(x) \
	((((x) & 0x00ff) << 8) | \
	 (((x) & 0xff00) >> 8))

int32_t ad5592r_write_dac(struct ad5592r_dev *dev, uint8_t chan,
			  uint16_t value);
int32_t ad5592r_read_adc(struct ad5592r_dev *dev, uint8_t chan,
			 uint16_t *value);
int32_t ad5592r_multi_read_adc(struct ad5592r_dev *dev,
			       uint16_t chans, uint16_t *value);
int32_t ad5592r_reg_write(struct ad5592r_dev *dev, uint8_t reg,
			  uint16_t value);
int32_t ad5592r_reg_read(struct ad5592r_dev *dev, uint8_t reg,
			 uint16_t *value);
int32_t ad5592r_gpio_read(struct ad5592r_dev *dev, uint8_t *value);
int32_t ad5592r_init(struct ad5592r_dev *dev,
		     struct ad5592r_init_param *init_param);

#endif /* AD5592R_H_ */
