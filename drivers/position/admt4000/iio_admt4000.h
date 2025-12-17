/***************************************************************************//**
 *   @file   iio_admt4000.h
 *   @brief  Header file of IIO ADMT4000 driver.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
 *******************************************************************************
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
 ******************************************************************************/

#ifndef IIO_ADMT4000_H
#define IIO_ADMT4000_H

#include "iio.h"
#include "no_os_gpio.h"

/** @enum admt4000_iio_chan_type
 *  @brief ADMT4000 IIO channels enumeration
 */
enum admt4000_iio_chan_type {
	ADMT4000_TURNS,
	ADMT4000_ANGLE,
	ADMT4000_TEMP,
	ADMT4000_COSINE,
	ADMT4000_SINE,
	ADMT4000_RADIUS,
	ADMT4000_NUM_CHAN,
};


/**
 * @brief ADMT4000 IIO device descriptor
 * @details Runtime structure for the ADMT4000 IIO (Industrial I/O) driver.
 *          This structure bridges the ADMT4000 hardware driver with the IIO
 *          framework, managing channel configuration, data buffering, and
 *          hardware triggers for data acquisition. It is allocated by
 *          admt4000_iio_init() and used by all IIO operations.
 */
struct admt4000_iio_dev {
	/** Pointer to the underlying ADMT4000 hardware device descriptor */
	struct admt4000_dev *admt4000_desc;
	/** IIO device descriptor for framework registration and management */
	struct iio_device *iio_dev;
	/** Data buffer for storing channel samples (max 6 channels) */
	int16_t data[ADMT4000_NUM_CHAN];
};

/**
 * @brief ADMT4000 IIO initialization parameters
 * @details Configuration structure for initializing the ADMT4000 IIO driver.
 *          Contains the hardware initialization parameters required to set up
 *          both the underlying ADMT4000 device and its IIO framework integration.
 *          This structure is passed to admt4000_iio_init() to configure the
 *          IIO device instance.
 */
struct admt4000_iio_dev_init_param {
	/** Hardware initialization parameters for the ADMT4000 device */
	struct admt4000_init_param *admt4000_init_param;
};

int admt4000_iio_init(struct admt4000_iio_dev **,
		      struct admt4000_iio_dev_init_param *);
int admt4000_iio_remove(struct admt4000_iio_dev *);

extern struct iio_trigger admt4000_iio_trig_desc;

#endif
