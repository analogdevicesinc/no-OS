/***************************************************************************//**
 *   @file   ad5593r.h
 *   @brief  Header file of AD5593R driver.
 *   @author Mircea Caprioru (mircea.caprioru@analog.com)
********************************************************************************
 * Copyright 2018, 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef AD5593R_H_
#define AD5593R_H_

#include "stdint.h"
#include "ad5592r-base.h"

int32_t ad5593r_write_dac(struct ad5592r_dev *dev, uint8_t chan,
			  uint16_t value);
int32_t ad5593r_read_adc(struct ad5592r_dev *dev, uint8_t chan,
			 uint16_t *value);
int32_t ad5593r_multi_read_adc(struct ad5592r_dev *dev,
			       uint16_t chans, uint16_t *value);
int32_t ad5593r_reg_write(struct ad5592r_dev *dev, uint8_t reg,
			  uint16_t value);
int32_t ad5593r_reg_read(struct ad5592r_dev *dev, uint8_t reg,
			 uint16_t *value);
int32_t ad5593r_gpio_read(struct ad5592r_dev *dev, uint8_t *value);
int32_t ad5593r_init(struct ad5592r_dev *dev,
		     struct ad5592r_init_param *init_param);

#endif /* AD5593R_H_ */
