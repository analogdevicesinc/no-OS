/*
 * Copyright (c) 2013, 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*! @file
    @ingroup LTC2315
    Header for LTC2315: 12/14-bit 1Msps ADC
*/

#ifndef LTC231X_H
#define LTC231X_H

/*! Define the SPI CS pin */
#ifndef LTC2315_CS
#define LTC2315_CS QUIKEVAL_CS
#endif

/*! @name LTC2315 Channel Address */
//! @{
// Channel Address
#define LTC2315_ADDRESS 0x00
//!@}

#define LTC2312_READ_BYTES_NUMBER 2
#define LTC2312_READ_VALUES_NUMBER 100

#include <stdint.h>
#include "no_os_delay.h"
#include "no_os_spi.h"

enum device_type {
	LTC2312_12,
	LTC2312_14
};

struct ltc2312_dev {
	/* Device characteristics */
	enum device_type type;
	/* SPI */
	struct no_os_spi_desc *spi_desc;
};

struct ltc2312_init_param {
	/* Device characteristics */
	enum device_type type;
	/* SPI */
	struct no_os_spi_init_param spi_init;
};

/* Initializes the ltc231x. */
int32_t ltc2312_setup(struct ltc2312_dev **device,
		      struct ltc2312_init_param *init_param);

/* Free the resources allocated by ltc231x_setup(). */
int32_t ltc2312_remove(struct ltc2312_dev *dev);

/* Reads the LTC2315 and returns 32-bit data in offset binary format. */
int32_t ltc2312_read(struct ltc2312_dev *dev, uint16_t *ptr_adc_code);

/* Calculates the LTC2315 input voltage given the binary data and LSB weight. */
void ltc2312_code_to_voltage(struct ltc2312_dev *dev, uint16_t adc_code,
			     float vref, float *voltage);

#endif  //  LTC2315_H
