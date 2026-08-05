/***************************************************************************//**
 *   @file   eval-adis1646x/src/common/common_data.h
 *   @brief  Defines common data to be used by eval-adis1646x examples.
 *   @author RBolboac (ramona.gradinariu@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "adis1646x.h"

extern struct no_os_uart_init_param adis1646x_uart_ip;
extern struct no_os_spi_init_param adis1646x_spi_ip;
extern struct no_os_gpio_init_param adis1646x_gpio_reset_ip;
extern struct adis_init_param adis1646x_ip;

#endif /* __COMMON_DATA_H__ */
