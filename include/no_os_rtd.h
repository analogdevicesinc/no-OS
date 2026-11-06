/***************************************************************************//**
 *   @file   no_os_rtd.h
 *   @brief  Header file of RTD temperature conversion routines.
 *   @author Mircea Vlasin (mircea.vlasin@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef NO_OS_RTD_H_
#define NO_OS_RTD_H_

/**
 * @brief Callendar-Van Dusen equation for Pt1000 RTD (IEC 60751).
 *        Converts RTD resistance (Ohms) to temperature (°C). For T < 0°C a
 *        Newton-Raphson refinement loop is applied to account for the cubic C term.
 * @param resistance - Measured RTD resistance in Ohms.
 * @return Temperature in degrees Celsius, or -999.0 on domain error.
 */
double no_os_pt1000_resistance_to_temp(double resistance);

#endif /* NO_OS_RTD_H_ */
