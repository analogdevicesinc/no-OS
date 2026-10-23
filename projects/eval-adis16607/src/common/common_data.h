/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by eval-adis16607 examples.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "adis16607.h"
#ifdef IIO_TRIGGER_EXAMPLE
#include "iio_adis16607.h"
#include "iio_trigger.h"
#endif

extern struct no_os_uart_init_param adis16607_uart_ip;
extern struct no_os_spi_init_param adis16607_spi_ip;
extern struct no_os_gpio_init_param adis16607_reset_ip;
extern struct adis_init_param adis16607_ip;

#ifdef IIO_TRIGGER_EXAMPLE
#define ADIS16607_GPIO_TRIG_NAME "adis16607-dev0"

extern struct iio_hw_trig_init_param adis16607_gpio_trig_ip;
extern struct no_os_irq_init_param adis16607_gpio_irq_ip;
#endif

#endif /* __COMMON_DATA_H__ */
