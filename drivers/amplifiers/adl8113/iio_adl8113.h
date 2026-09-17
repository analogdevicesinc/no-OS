/***************************************************************************//**
 * @file iio_adl8113.h
 * @brief Header file for ADL8113 IIO Driver.
 * @author Alexandru Vasile Popa (Alexandruvasile.Popa@analog.com)
 ********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#ifndef __IIO_ADL8113_H__
#define __IIO_ADL8113_H__

#include <stdint.h>
#include <stdbool.h>

#include "iio.h"
#include "iio_types.h"
#include "adl8113.h"

/**
 * @brief IIO-only gain configuration entry.
 *
 * Maps a user-visible hardwaregain value (in dB) to a concrete ADL8113 signal path
 * (mode). The core driver only switches modes (VA/VB GPIO truth table).
 */
struct adl8113_iio_gain_config {
	enum adl8113_mode path;
	int gain_db;
};

/**
 * @brief ADL8113 IIO device descriptor.
 */
struct adl8113_iio_dev {
	struct adl8113_dev *adl8113_dev;
	struct iio_device *iio_dev;
	struct adl8113_iio_gain_config *gain_configs;
	uint8_t num_gain_configs;
};

/**
 * @brief ADL8113 IIO initialization parameters.
 *
 * Internal amplifier (14 dB) and internal bypass (-2 dB) are always added by the
 * IIO driver. External bypass paths are optional and can be enabled here to keep
 * a Linux-like userspace contract (gain selects path).
 */
struct adl8113_iio_init_param {
	struct adl8113_init_param *adl8113_init_param;

	/* Optional external bypass gain configs (Linux DT equivalent). */
	bool has_external_bypass_a;
	int external_bypass_a_gain_db;

	bool has_external_bypass_b;
	int external_bypass_b_gain_db;
};

/**
 * @brief Initialize the ADL8113 IIO driver.
 * @param[out] iio_dev      - Pointer to the allocated IIO descriptor.
 * @param[in]  init_param   - Initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int adl8113_iio_init(struct adl8113_iio_dev **iio_dev,
		     struct adl8113_iio_init_param *init_param);

/**
 * @brief Remove the ADL8113 IIO driver.
 * @param[in] iio_dev - IIO descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int adl8113_iio_remove(struct adl8113_iio_dev *iio_dev);

#endif /* __IIO_ADL8113_H__ */
