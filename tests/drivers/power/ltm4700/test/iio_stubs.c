/***************************************************************************//**
 *   @file   iio_stubs.c
 *   @brief  Stub implementation of iio_format_value for unit testing
 *   @author Carlos Jones Jr (carlosjr.jones@analog.com)
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
