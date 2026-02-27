/***************************************************************************//**
 *   @file   no_os_thermocouple.c
 *   @brief  Implementation of thermocouple temperature conversion routines.
 *   @author Mircea Vlasin (mircea.vlasin@analog.com)
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

#include <stddef.h>
#include "no_os_thermocouple.h"

/**
 * @brief Evaluate a polynomial using power accumulation.
 * @param coeffs - Array of coefficients (ascending powers of x).
 * @param n      - Number of coefficients.
 * @param x      - Input value.
 * @return Result of sum(coeffs[i] * x^i) for i = 0..n-1.
 */
static double eval_polynomial(const double *coeffs, size_t n, double x)
{
	double result = 0.0;
	double x_pow = 1.0;
	size_t i;

	for (i = 0; i < n; i++) {
		result += coeffs[i] * x_pow;
		x_pow *= x;
	}

	return result;
}

/**
 * @brief ITS-90 Type K thermocouple forward polynomial.
 *        Converts temperature (°C) to equivalent EMF voltage (mV).
 *        Valid for 0°C to 1372°C range.
 * @param temp_celsius - Temperature in degrees Celsius.
 * @return Equivalent thermocouple voltage in mV.
 */
double no_os_typek_temp_to_voltage(double temp_celsius)
{
	const double c[] = {
		 0.000000E+00,
		 3.945013E-02,
		 2.362237E-05,
		-3.285891E-07,
		-4.990483E-09,
		-6.750905E-11,
		-5.741032E-13,
		-3.108887E-15,
		-1.045160E-17,
		-1.988926E-20,
		-1.632269E-23
	};

	return eval_polynomial(c, sizeof(c) / sizeof(c[0]), temp_celsius);
}

/**
 * @brief ITS-90 Type K thermocouple inverse polynomial.
 *        Converts thermocouple voltage (mV) to temperature (°C).
 *        Uses separate polynomial ranges for negative and positive voltages.
 * @param voltage_mv - Thermocouple voltage in millivolts.
 * @return Temperature in degrees Celsius.
 */
double no_os_typek_voltage_to_temp(double voltage_mv)
{
	/* -5.891 mV to 0 mV (-200°C to 0°C) */
	const double d[] = {
		 0.0000000E+00,
		 2.5173462E+01,
		-1.1662878E+00,
		-1.0833638E+00,
		-8.9773540E-01,
		-3.7342377E-01,
		-8.6632643E-02,
		-1.0450598E-02,
		-5.1920577E-04
	};

	/* 0 mV to 20.644 mV (0°C to 500°C) */
	const double e[] = {
		 0.000000E+00,
		 2.508355E+01,
		 7.860106E-02,
		-2.503131E-01,
		 8.315270E-02,
		-1.228034E-02,
		 9.804036E-04,
		-4.413030E-05,
		 1.057734E-06,
		-1.052755E-08
	};

	if (voltage_mv < 0)
		return eval_polynomial(d, sizeof(d) / sizeof(d[0]), voltage_mv);

	return eval_polynomial(e, sizeof(e) / sizeof(e[0]), voltage_mv);
}
