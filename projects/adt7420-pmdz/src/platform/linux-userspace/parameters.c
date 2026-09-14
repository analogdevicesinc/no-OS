/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of Linux platform data used by adt7420-pmdz project.
 *   @author Jamila Macagba (Jamila.Macagba@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "parameters.h"

struct linux_i2c_init_param adt7420_i2c_extra = {
	.device_id = I2C_DEVICE_ID
};
