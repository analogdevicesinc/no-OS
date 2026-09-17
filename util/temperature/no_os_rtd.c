/***************************************************************************//**
 *   @file   no_os_rtd.c
 *   @brief  Implementation of RTD temperature conversion routines.
 *   @author Mircea Vlasin (mircea.vlasin@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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
