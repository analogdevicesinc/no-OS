/***************************************************************************//**
 *   @file   iio_ad4170.h
 *   @brief  Header file of the AD4170, AD4171, AD4171-8, AD4172, AD4190,
 *           AD4195 IIO driver.
 *   @author Janani Sunil (janani.sunil@analog.com)
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
*******************************************************************************/
#ifndef IIO_AD4170_H_
#define IIO_AD4170_H_

#include <stdint.h>
#include "ad4170.h"
#include "iio.h"

/**
 * @struct ad4170_iio_dev
 * @brief AD4170 IIO device descriptor.
 */
struct ad4170_iio_dev {
	/** AD4170 device descriptor. */
	struct ad4170_dev *ad4170_dev;
	/** IIO device descriptor. */
	struct iio_device *iio_dev;
	/** Scan type of every IIO channel. */
	struct scan_type *scan_types;
	/** Reference voltage in mV, ignored when REFOUT is selected. */
	uint32_t vref_mv;
	/** Master clock frequency in Hz the digital filter runs from. */
	uint32_t mclk_hz;
	/** Mask of the channels enabled by the current buffer. */
	uint32_t active_channels;
	/** Number of channels the mask above enables. */
	uint8_t num_active_channels;
	/** Channel_En to restore once the buffered read is done. */
	uint16_t prev_channel_en;
	/** Operating mode to restore once the buffered read is done. */
	enum ad4170_mode prev_mode;
	/**
	 * Data register selection to restore once the buffered read is done.
	 */
	bool prev_multi_data_reg_sel;
};

/**
 * @struct ad4170_iio_dev_init_param
 * @brief AD4170 IIO device initialization parameters.
 */
struct ad4170_iio_dev_init_param {
	/** AD4170 device initialization parameters. */
	struct ad4170_init_param *ad4170_init_param;
	/** Reference voltage in mV, ignored when REFOUT is selected. */
	uint32_t vref_mv;
	/**
	 * Frequency in Hz of the clock wired to XTAL1/XTAL2, ignored when the
	 * internal oscillator is selected.
	 */
	uint32_t mclk_hz;
};

int ad4170_iio_init(struct ad4170_iio_dev **device,
		    struct ad4170_iio_dev_init_param *init_param);
int ad4170_iio_remove(struct ad4170_iio_dev *device);

#endif /* IIO_AD4170_H_ */
