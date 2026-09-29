/***************************************************************************//**
 *   @file   iio_adar300x.h
 *   @brief  Header file of the ADAR300X IIO driver.
 *   @author Stefan Popa (stefan.popa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef IIO_ADAR300X_H_
#define IIO_ADAR300X_H_

#include <stdint.h>
#include "iio.h"
#include "adar300x.h"

#define ADAR300X_IIO_BEAMS			4

/**
 * @enum adar300x_iio_mode
 * @brief Beam mode as seen through IIO.
 *
 *	  The first four match enum adar300x_beam_mode. Reset and mute are not
 *	  modes of the device but commands, applied by the next update.
 */
enum adar300x_iio_mode {
	ADAR300X_IIO_DIRECT,
	ADAR300X_IIO_MEMORY,
	ADAR300X_IIO_FIFO,
	ADAR300X_IIO_INST_DIRECT,
	ADAR300X_IIO_RESET,
	ADAR300X_IIO_MUTE,
};

/**
 * @struct adar300x_iio_dev
 * @brief ADAR300X IIO device descriptor.
 */
struct adar300x_iio_dev {
	/** Core driver descriptor */
	struct adar300x_dev *adar300x_dev;
	/** IIO device descriptor */
	struct iio_device *iio_dev;
	/** Last mode written per beam; reset and mute take effect on update */
	enum adar300x_iio_mode mode[ADAR300X_IIO_BEAMS];
	/** Bank the beamstate channels and the buffer target, per beam */
	enum adar300x_iio_mode load_mode[ADAR300X_IIO_BEAMS];
	/** RAM beamstate the channels address and the buffer starts at */
	uint8_t ram_index[ADAR300X_IIO_BEAMS];
	/** FIFO beamstate being assembled, loaded when its last value lands */
	uint8_t fifo_state[ADAR300X_IIO_BEAMS][ADAR300X_UNPACKED_BEAMSTATE_LEN];
};

/**
 * @struct adar300x_iio_dev_init_param
 * @brief ADAR300X IIO device initialization parameters.
 */
struct adar300x_iio_dev_init_param {
	/** Core driver initialization parameters */
	struct adar300x_init_param *adar300x_dev_init;
};

/** Initialize the ADAR300X IIO device. */
int adar300x_iio_init(struct adar300x_iio_dev **iio_dev,
		      struct adar300x_iio_dev_init_param *init_param);

/** Free the resources allocated by adar300x_iio_init(). */
int adar300x_iio_remove(struct adar300x_iio_dev *desc);

#endif /* IIO_ADAR300X_H_ */
