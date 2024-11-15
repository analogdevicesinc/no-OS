/***************************************************************************//**
 *   @file   adxrs290-pmdz/src/common/common_data.h
 *   @brief  Defines common data to be used by eval-adxrs290-pmdz examples.
 *   @author RBolboac (ramona.bolboaca@analog.com)
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
#include "adxrs290.h"
#ifdef IIO_SUPPORT
#include "iio_adxrs290.h"
#if defined(IIO_TRIGGER_EXAMPLE) || defined(IIO_TIMER_TRIGGER_EXAMPLE)
#include "iio_trigger.h"
#endif
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
extern struct no_os_uart_init_param adxrs290_uart_ip;
extern struct no_os_spi_init_param adxrs290_spi_ip;
extern struct no_os_gpio_init_param adxrs290_gpio_sync_ip;
extern struct adxrs290_init_param adxrs290_ip;

#ifdef IIO_TRIGGER_EXAMPLE
#define ADXRS290_GPIO_TRIG_NAME "adxrs290-dev0"

extern struct iio_hw_trig_init_param adxrs290_gpio_trig_ip;
extern struct no_os_irq_init_param adxrs290_gpio_irq_ip;
#endif

#ifdef IIO_TIMER_TRIGGER_EXAMPLE
#define ADXRS290_TIMER_TRIG_NAME "adxrs290_timer_trig"
extern struct no_os_timer_init_param adxrs290_tip;
extern struct no_os_irq_init_param adxrs290_timer_irq_ip;
extern struct iio_hw_trig_init_param adxrs290_timer_trig_ip;
#endif

#endif /* __COMMON_DATA_H__ */
