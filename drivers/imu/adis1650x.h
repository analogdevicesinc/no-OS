/***************************************************************************//**
 *   @file   adis1650x.h
 *   @brief  Implementation of adis1650x.h
 *   @author RBolboac (ramona.bolboaca@analog.com)
 *******************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/

#ifndef __ADIS1650X_H__
#define __ADIS1650X_H__

#include "adis.h"

#define ADIS1650X_ID_NO_OFFSET(x) ((x) - ADIS16500)

extern const struct adis_chip_info adis1650x_chip_info;

#endif
