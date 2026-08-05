/***************************************************************************//**
 *   @file   adis1657x.h
 *   @brief  Implementation of adis1657x.h
 *   @author RBolboac (ramona.bolboaca@analog.com)
 *******************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/

#ifndef __ADIS1657X_H__
#define __ADIS1657X_H__

#include "adis.h"

#define ADIS1657X_ID_NO_OFFSET(x)		((x) - ADIS16575_2)

extern const struct adis_chip_info adis1657x_chip_info;

#endif
