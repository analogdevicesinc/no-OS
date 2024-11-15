/***************************************************************************//**
 *   @file   iio_ad5791.h
 *   @brief  Header of AD5791 IIO Driver.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef IIO_AD5791_H_
#define IIO_AD5791_H_

#include "iio.h"
#include "no_os_util.h"
#include "ad5791.h"

#define AD5791_CH_NO 1

/**
 * @enum ad5791_iio_powerdown_modes
 * @brief AD5791 channel power down modes.
 */
enum ad5791_iio_powerdown_modes {
	AD5791_6kOHMS_TO_GND,
	AD5791_THREE_STATE
};

/**
 * @struct ad5791_iio_desc
 * @brief D5791 IIO driver handler.
 */
struct ad5791_iio_desc {
	struct ad5791_dev *ad5791_handle;
	struct iio_device *ad5791_iio_dev;
	enum ad5791_iio_powerdown_modes curr_mode;
	uint32_t vref_mv;
	uint32_t vref_neg_mv;
};

/**
 * @struct ad5791_iio_init_param
 * @brief AD5791 IIO driver initialization structure.
 */
struct ad5791_iio_init_param {
	struct ad5791_init_param *ad5791_initial;
	uint32_t vref_mv;
	uint32_t vref_neg_mv;
};

/* Initialize the AD5791 IIO driver. */
int32_t ad5791_iio_init(struct ad5791_iio_desc **iio_dev,
			struct ad5791_iio_init_param *init_param);

/* Free memory allocated by ad5791_iio_init(). */
int32_t ad5791_iio_remove(struct ad5791_iio_desc *desc);

/*****************************************************************************/
/***************************** Constant definition ***************************/
/*****************************************************************************/
extern struct iio_device const iio_ad5791_device;

#endif /* IIO_AD5791_H_ */
