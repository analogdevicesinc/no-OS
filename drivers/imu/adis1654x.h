/***************************************************************************//**
 *   @file   adis1654x.h
 *   @brief  Implementation of adis1654x.h
 *   @author RBolboac (ramona.gradinariu@analog.com)
 *******************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/

#ifndef __ADIS1654X_H__
#define __ADIS1654X_H__

#include "adis.h"

#define ADIS1654X_ID_NO_OFFSET(x) ((x) - ADIS16545_1)

extern const struct adis_chip_info adis1654x_chip_info;

#endif
