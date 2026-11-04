/*******************************************************************************
 *   @file   common_data.h
 *   @brief  Defines common data to be used by max31855 examples.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
 ********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include "parameters.h"
#include "max31855.h"
#ifdef IIO_SUPPORT
#include "iio_max31855.h"
#endif

extern struct no_os_uart_init_param uip;

extern const struct no_os_spi_init_param max31855_spi_ip;
extern struct max31855_init_param max31855_ip;

#endif /* COMMON_DATA_H_ */
