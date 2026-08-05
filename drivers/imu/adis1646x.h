/***************************************************************************//**
 *   @file   adis1646x.h
 *   @brief  Implementation of adis1646x.h
 *   @author RBolboac (ramona.gradinariu@analog.com)
 *******************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/

#ifndef __ADIS1646X_H__
#define __ADIS1646X_H__

#include "adis.h"

#define ADIS1646X_ID_NO_OFFSET(x) ((x) - ADIS16465_1)

extern const struct adis_chip_info adis1646x_chip_info;

#endif
