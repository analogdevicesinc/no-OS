/***************************************************************************//**
 *   @file   ad5592r.h
 *   @brief  Header file of AD5592R driver.
 *   @author Mircea Caprioru (mircea.caprioru@analog.com)
 *   @author Niel Acuna (niel.acuna@analog.com)
********************************************************************************
 * Copyright 2018, 2020, 2025, 2026 (c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef AD5592R_H_
#define AD5592R_H_

#include "stdint.h"
#include "ad5592r-base.h"

#define AD5592R_GPIO_READBACK_EN	NO_OS_BIT(10)
#define AD5592R_LDAC_READBACK_EN	NO_OS_BIT(6)

/*
 * ad5592r datasheet pg 8:
 * Temperature conversion takes 5us when ADC buffer enabled
 * and 20us when buffer disabled. But real world tests show they are much
 * higher. We pad additional 100us to account for the the variance in tracking
 * time.
 */
#define AD5592R_TRACING_VARIANCE_PAD 	100
#define AD5592R_TEMPERATURE_TRACK_TIME_BUFFERED 	(5 + AD5592R_TRACING_VARIANCE_PAD)
#define AD5592R_TEMPERATURE_TRACK_TIME_UNBUFFERED 	(20 + AD5592R_TRACING_VARIANCE_PAD)

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
int32_t ad5592r_init(struct ad5592r_dev **dev,
		     struct ad5592r_init_param *init_param);
int32_t ad5592r_remove(struct ad5592r_dev *dev);
int32_t ad5592r_enable_busy(struct ad5592r_dev *dev, bool enable);
int32_t ad5592r_spi_wnop_r16(struct ad5592r_dev *dev, uint16_t *buf);

#endif /* AD5592R_H_ */
