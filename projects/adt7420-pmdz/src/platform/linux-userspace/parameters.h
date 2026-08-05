/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to Linux platform used by adt7420-pmdz
 *           project.
 *   @author Jamila Macagba (Jamila.Macagba@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "linux_i2c.h"

#define I2C_DEVICE_ID   1
#define I2C_EXTRA	&adt7420_i2c_extra
#define I2C_OPS		&linux_i2c_ops

extern struct linux_i2c_init_param adt7420_i2c_extra;

#endif /* __PARAMETERS_H__ */
