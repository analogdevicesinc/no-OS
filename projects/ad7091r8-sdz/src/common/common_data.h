/***************************************************************************//**
 *   @file   ad7091r8-sdz/src/common/common_data.h
 *   @brief  Defines common data to be used by ad7091r8-sdz examples.
 *   @author Marcelo Schmitt (marcelo.schmitt@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_includes.h"
#include "ad7091r8.h"
#ifdef IIO_SUPPORT
#include "iio_ad7091r8.h"
#endif
#if defined(IIO_TIMER_TRIGGER_EXAMPLE)
#include "iio_trigger.h"
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
extern struct no_os_uart_init_param ad7091r8_uart_ip;
extern struct no_os_spi_init_param ad7091r8_spi_ip;
extern struct ad7091r8_init_param ad7091r8_ip;

#ifdef IIO_TIMER_TRIGGER_EXAMPLE
#define AD7091R8_TIMER_TRIG_NAME "ad7091r8-timer-trig"
extern struct no_os_timer_init_param ad7091r8_timer_ip;
extern struct no_os_irq_init_param ad7091r8_timer_irq_ip;
extern struct iio_hw_trig_init_param ad7091r8_timer_trig_ip;
#endif

#endif /* __COMMON_DATA_H__ */
