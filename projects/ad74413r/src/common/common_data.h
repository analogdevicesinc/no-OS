/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ad74413r examples.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "ad74413r.h"
#ifdef IIO_TRIGGER_EXAMPLE
#include "iio_trigger.h"
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
extern struct no_os_uart_init_param ad74413r_uart_ip;

extern struct no_os_spi_init_param ad74413r_spi_ip;
extern struct ad74413r_init_param ad74413r_ip;

#ifdef IIO_TRIGGER_EXAMPLE
#define AD74413R_GPIO_TRIG_NAME "ad74413r-dev0"

extern struct iio_trigger ad74413r_iio_trig_desc;
extern struct iio_hw_trig_init_param ad74413r_gpio_trig_ip;
extern struct no_os_irq_init_param ad74413r_gpio_irq_ip;
extern struct iio_trigger ad74413r_iio_trig_desc;
#endif

#endif /* __COMMON_DATA_H__ */
