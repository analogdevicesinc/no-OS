/***************************************************************************//**
 *   @file   no_os_thermocouple.h
 *   @brief  Header file of thermocouple temperature conversion routines.
 *   @author Mircea Vlasin (mircea.vlasin@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __NO_OS_THERMOCOUPLE_H__
#define __NO_OS_THERMOCOUPLE_H__

/**
 * @brief ITS-90 Type K thermocouple forward polynomial.
 *        Converts temperature (°C) to equivalent EMF voltage (mV).
 *        Valid for 0°C to 1372°C range.
 * @param temp_celsius - Temperature in degrees Celsius.
 * @return Equivalent thermocouple voltage in mV.
 */
double no_os_typek_temp_to_voltage(double temp_celsius);

/**
 * @brief ITS-90 Type K thermocouple inverse polynomial.
 *        Converts thermocouple voltage (mV) to temperature (°C), -200°C to 500°C.
 *        Uses separate polynomial ranges for negative and positive voltages.
 * @param voltage_mv - Thermocouple voltage in millivolts.
 * @return Temperature in degrees Celsius.
 */
double no_os_typek_voltage_to_temp(double voltage_mv);

#endif /* __NO_OS_THERMOCOUPLE_H__ */
