/***************************************************************************//**
 *   @file   eval-adxl367z/src/common/common_data.h
 *   @brief  Defines common data to be used by eval-adxl367z examples.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "adxl367.h"

extern const struct no_os_spi_init_param spi_ip;

extern struct adxl367_init_param init_param;

#ifdef IIO_SUPPORT
extern struct no_os_uart_init_param adxl367_uart_ip;
#endif

#endif /* __COMMON_DATA_H__ */
