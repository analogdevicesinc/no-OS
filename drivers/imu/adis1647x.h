/***************************************************************************//**
 *   @file   adis1647x.h
 *   @brief  Implementation of adis1647x.h
 *   @author RBolboac (ramona.gradinariu@analog.com)
 *******************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/

#ifndef __ADIS1647X_H__
#define __ADIS1647X_H__

#include "adis.h"

#define ADIS1647X_ID_NO_OFFSET(x) ((x) - ADIS16477_1)

extern const struct adis_chip_info adis1647x_chip_info;

#endif
