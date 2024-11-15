/***************************************************************************//**
 *   @file   eval-adis1650x/src/common/common_data.h
 *   @brief  Defines common data to be used by eval-adis1650x examples.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "platform_includes.h"
#include "adis1650x.h"
#ifdef IIO_SUPPORT
#include "iio_adis1650x.h"
#ifdef IIO_TRIGGER_EXAMPLE
#include "iio_trigger.h"
#endif
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

extern struct no_os_uart_init_param adis1650x_uart_ip;
extern struct no_os_spi_init_param adis1650x_spi_ip;
extern struct no_os_gpio_init_param adis1650x_gpio_reset_ip;
extern struct adis_init_param adis1650x_ip;

#ifdef IIO_TRIGGER_EXAMPLE
#define ADIS1650X_GPIO_TRIG_NAME "adis16505-2-dev0"

extern struct iio_hw_trig_init_param adis1650x_gpio_trig_ip;
extern struct no_os_irq_init_param adis1650x_gpio_irq_ip;
#endif

#endif /* __COMMON_DATA_H__ */
