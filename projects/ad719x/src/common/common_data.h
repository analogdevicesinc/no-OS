/***************************************************************************//**
 *   @file   ad719x/src/common/common_data.h
 *   @brief  Defines common data to be used by ad719x examples.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include "platform_includes.h"
#include "ad719x.h"

extern const struct no_os_spi_init_param spi_ip;
extern const struct no_os_gpio_init_param miso_ip;
extern const struct no_os_gpio_init_param sync_ip;

extern struct ad719x_init_param ad7190_dev_ip;

#endif /* COMMON_DATA_H_ */
