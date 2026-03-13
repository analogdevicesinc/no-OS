/***************************************************************************//**
 *   @file   no_os_rtd.c
 *   @brief  Implementation of RTD temperature conversion routines.
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

#include <math.h>
#include "no_os_rtd.h"

double no_os_pt1000_resistance_to_temp(double resistance)
{
	const double R0 = 1000.0;
	const double A = 3.9083e-3;
	const double B = -5.775e-7;
	const double C = -4.183e-12;

	double ratio = resistance / R0;
	double c_coef = 1.0 - ratio;
	double discriminant = A * A - 4.0 * B * c_coef;
	double T;
	int i;

	if (discriminant < 0)
		return -999.0;

	T = (-A + sqrt(discriminant)) / (2.0 * B);

	if (T < 0.0) {
		for (i = 0; i < 5; i++) {
			double R_calc = R0 * (1.0 + A * T + B * T * T +
					      C * (T - 100.0) * T * T * T);
			double dR_dT = R0 * (A + 2.0 * B * T +
					     C * (4.0 * T * T * T -
						  300.0 * T * T));
			T = T - (R_calc - resistance) / dR_dT;
		}
	}

	return T;
}
