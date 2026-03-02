/***************************************************************************//**
 * @file iio_adl8113.h
 * @brief Header file for ADL8113 IIO Driver.
 * @author Alexandru Vasile Popa (Alexandruvasile.Popa@analog.com)
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
