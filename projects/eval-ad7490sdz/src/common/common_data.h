/*******************************************************************************
 *   @file   common_data.h
 *   @brief  Common data used within the eval-ad7490sdz project.
 *   @author Radu Sabau (radu.sabau@analog.com)
 ********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "ad7490.h"

#ifdef IIO_SUPPORT
#include "iio_ad7490.h"
#endif

extern struct no_os_uart_init_param uart_ip;
extern struct no_os_spi_init_param ad7490_spi_ip;
extern struct ad7490_init_param ad7490_ip;


#endif /* __COMMON_DATA_H__ */
