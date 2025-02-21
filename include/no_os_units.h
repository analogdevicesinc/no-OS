/***************************************************************************//**
 *   @file   no_os_units.h
 *   @brief  Header file of Units
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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
#ifndef _NO_OS_UNITS_H_
#define _NO_OS_UNITS_H_

#define PETA	1000000000000000ULL
#define TERA	1000000000000ULL
#define GIGA	1000000000UL
#define MEGA	1000000UL
#define KILO	1000UL
#define HECTO	100UL
#define DECA	10UL
#define DECI	10UL
#define CENTI	100UL
#define MILLI	1000UL
#define MICRO	1000000UL
#define NANO	1000000000UL
#define PICO	1000000000000ULL
#define FEMTO	1000000000000000ULL

#define HZ_PER_KHZ		1000UL
#define KHZ_PER_MHZ		1000UL
#define HZ_PER_MHZ		1000000UL

#define MILLIVOLT_PER_VOLT	1000UL
#define MICROVOLT_PER_VOLT	1000000UL
#define NANOVOLT_PER_VOLT	1000000000ULL

#define MILLIAMPER_PER_AMPER	1000UL
#define MICROAMPER_PER_MILLIAMPER  1000UL
#define MICROAMPER_PER_AMPER	1000000UL
#define NANOAMPER_PER_AMPER	1000000000ULL

#define MILLIWATT_PER_WATT	1000UL
#define MICROWATT_PER_MILLIWATT	1000UL
#define MICROWATT_PER_WATT	1000000UL

#define MILLIDEGREE_PER_DEGREE	1000UL

/* Returns the given value converted from degree to rad */
#define DEGREE_TO_RAD(deg) (((deg) * 314159ULL + 9000000ULL) / 18000000ULL)

/* Returns the given value converted from rad to degree */
#define RAD_TO_DEGREE(rad) \
	(((rad) * 18000000ULL + 314159ULL / 2) / 314159ULL)

/* Returns the given value converted from g to meter / second**2 */
#define G_TO_M_S_2(g) ((g) * 980665ULL / 100000ULL)

/* Returns the given value converted from meter / second**2 to g */
#define M_S_2_TO_G(ms2) (((ms2) * 100000ULL + 980665ULL / 2) / 980665ULL)

#endif // _NO_OS_UNITS_H_
