/***************************************************************************//**
 *   @file   iio_stubs.c
 *   @brief  Stub implementation of iio_format_value for unit testing
 *   @author Carlos Jones Jr (carlosjr.jones@analog.com)
 *******************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/

#include "iio.h"
#include <stdio.h>
#include <stdarg.h>

/**
 * @brief Stub implementation of iio_format_value for unit testing.
 *
 * This stub formats values in a simple way to allow IIO driver functions to be tested
 * without requiring the full IIO library implementation.
 *
 * @param buf - Output buffer
 * @param len - Buffer length
 * @param type - Value type (IIO_VAL_INT, IIO_VAL_INT_PLUS_MICRO, etc.)
 * @param size - Number of values
 * @param vals - Array of values
 * @return Number of characters written
 */
int iio_format_value(char *buf, uint32_t len, enum iio_val type,
		     int32_t size, int32_t *vals)
{
	int ret;

	switch (type) {
	case IIO_VAL_INT:
		/* For LTM4700, most values are in milli-units (mV, mA, mW)
		 * Format as decimal with 3 places: 12345 -> "12.345"
		 * For large values like frequency (Hz), format as integer.
		 * Use threshold of 100000 to distinguish.
		 */
		if (vals[0] < 100000 && vals[0] > -100000) {
			int integer_part = vals[0] / 1000;
			int fractional_part = vals[0] % 1000;
			if (fractional_part < 0)
				fractional_part = -fractional_part;
			ret = snprintf(buf, len, "%d.%03d", integer_part, fractional_part);
		} else {
			ret = snprintf(buf, len, "%d", vals[0]);
		}
		break;
	case IIO_VAL_INT_PLUS_MICRO:
		if (vals[1] < 0)
			ret = snprintf(buf, len, "-%d.%06d", vals[0], -vals[1]);
		else
			ret = snprintf(buf, len, "%d.%06d", vals[0], vals[1]);
		break;
	case IIO_VAL_INT_PLUS_NANO:
		if (vals[1] < 0)
			ret = snprintf(buf, len, "-%d.%09d", vals[0], -vals[1]);
		else
			ret = snprintf(buf, len, "%d.%09d", vals[0], vals[1]);
		break;
	case IIO_VAL_FRACTIONAL:
		ret = snprintf(buf, len, "%d/%d", vals[0], vals[1]);
		break;
	default:
		ret = snprintf(buf, len, "%d", vals[0]);
		break;
	}

	return ret;
}
