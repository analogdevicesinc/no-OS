/***************************************************************************//**
 *   @file   iio_tmc5240.h
 *   @brief  Header file of IIO TMC5240 driver.
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

#ifndef IIO_TMC5240_H
#define IIO_TMC5240_H

/**
 * @enum tmc5240_iio_attr
 * @brief TMC5240 IIO attribute identifiers
 */
enum tmc5240_iio_attr {
	/** Raw value attribute */
	TMC5240_CH_ATTR_RAW,
	/** Scale factor attribute */
	TMC5240_CH_ATTR_SCALE,
	/** Calibration scale attribute */
	TMC5240_CH_ATTR_CALIBSCALE,
	/** Position preset attribute. Writing sets RAMPMODE to HOLD. */
	TMC5240_CH_ATTR_PRESET,
	/** Shaft direction attribute */
	TMC5240_ATTR_DIRECTION,
	/** Motor stop attribute */
	TMC5240_ATTR_STOP,
	/** Maximum acceleration attribute */
	TMC5240_ATTR_AMAX,
	/** Maximum velocity attribute */
	TMC5240_ATTR_VMAX,
	/** Maximum deceleration attribute */
	TMC5240_ATTR_DMAX,
	/** Start velocity attribute */
	TMC5240_ATTR_VSTART,
	/** First acceleration phase attribute */
	TMC5240_ATTR_A1,
	/** First velocity threshold attribute */
	TMC5240_ATTR_V1,
	/** Second acceleration phase attribute */
	TMC5240_ATTR_A2,
	/** Second velocity threshold attribute */
	TMC5240_ATTR_V2,
	/** First deceleration phase attribute */
	TMC5240_ATTR_D1,
	/** Second deceleration phase attribute */
	TMC5240_ATTR_D2,
	/** Stop velocity attribute */
	TMC5240_ATTR_VSTOP,
};

/**
 * @struct tmc5240_iio_dev
 * @brief TMC5240 IIO device descriptor
 */
struct tmc5240_iio_dev {
	/** TMC5240 device descriptor */
	struct tmc5240_dev *tmc5240_dev;
	/** IIO device descriptor */
	struct iio_device *iio_dev;
};

/**
 * @struct tmc5240_iio_dev_init_param
 * @brief TMC5240 IIO initialization parameters
 */
struct tmc5240_iio_dev_init_param {
	/** TMC5240 initialization parameters */
	struct tmc5240_init_param *tmc5240_init_param;
};

/** Initialize the TMC5240 IIO driver */
int tmc5240_iio_init(struct tmc5240_iio_dev **iio_dev,
		     struct tmc5240_iio_dev_init_param *init_param);

/** Free resources allocated by tmc5240_iio_init() */
int tmc5240_iio_remove(struct tmc5240_iio_dev *desc);

#endif
