/***************************************************************************//**
 *   @file   net.h
 *   @brief  Network interface header for the wethlink project.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2023~2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "iio_adin1300.h"
#include "iio_max24287.h"
#include "mwc.h"

int net_init(struct adin1300_iio_desc **adin1300_iio,
	     struct max24287_iio_desc **max24287_iio,
	     unsigned int speed);