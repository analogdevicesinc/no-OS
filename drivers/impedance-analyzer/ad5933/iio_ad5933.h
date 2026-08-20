/***************************************************************************//**
 *   @file   iio_ad5933.h
 *   @brief  Header file of the AD5933 IIO driver.
 *   @author Mark John Lerry Casero (markjohnlerry.casero@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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
	/** Calibration impedance value used. */
	uint32_t calibration_impedance;
};

/*! Initialize the AD5933 IIO driver. */
int ad5933_iio_init(struct ad5933_iio_dev **iio_dev,
		    struct ad5933_iio_dev_init_param *init_param);

/*! Free the resources allocated by ad5933_iio_init(). */
int ad5933_iio_remove(struct ad5933_iio_dev *desc);

#endif /* __IIO_AD5933_H__ */
