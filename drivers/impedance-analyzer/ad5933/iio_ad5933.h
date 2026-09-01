/***************************************************************************//**
 *   @file   iio_ad5933.h
 *   @brief  Header file of the AD5933 IIO driver.
 *   @author Mark John Lerry Casero (markjohnlerry.casero@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __IIO_AD5933_H__
#define __IIO_AD5933_H__

#include <stdint.h>
#include <iio_types.h>
#include "ad5933.h"

enum ad5933_measure_mode {AD5933_MEASURE_MODE_SINGLE, AD5933_MEASURE_MODE_SWEEP};

/**
 * @struct ad5933_iio_dev
 * @brief AD5933 IIO device descriptor.
 */
struct ad5933_iio_dev {
	/** Underlying device driver descriptor. */
	struct ad5933_dev *ad5933_dev;
	/** IIO device descriptor. */
	struct iio_device *iio_dev;
	/** Active channel mask (set on buffer pre-enable). */
	uint32_t active_channels;
	/** Number of active channels. */
	uint8_t no_of_active_channels;
	uint8_t state;
	uint32_t current_output_freq;
	int16_t channel_data[1024];
};

/**
 * @struct ad5933_iio_dev_init_param
 * @brief AD5933 IIO device initialization parameters.
 */
struct ad5933_iio_dev_init_param {
	/** Pointer to the underlying device init parameters. */
	struct ad5933_init_param *ad5933_dev_ip;
	/** Sweep start frequency (Hz). */
	uint32_t start_freq;
	/** Sweep frequency increment (Hz). */
	uint32_t freq_increment;
	/** Number of frequency increments (num_increments). */
	uint16_t freq_points;
};

/*! Initialize the AD5933 IIO driver. */
int ad5933_iio_init(struct ad5933_iio_dev **iio_dev,
		    struct ad5933_iio_dev_init_param *init_param);

/*! Free the resources allocated by ad5933_iio_init(). */
int ad5933_iio_remove(struct ad5933_iio_dev *desc);

#endif /* __IIO_AD5933_H__ */
