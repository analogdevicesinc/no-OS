/***************************************************************************//**
 *   @file   eval-adxl355-pmdz/src/common/common_data.h
 *   @brief  Defines common data to be used by eval-adxl355-pmdz examples.
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
#include "adxl355.h"
#ifdef IIO_SUPPORT
#include "iio_adxl355.h"
#ifdef IIO_TRIGGER_EXAMPLE
#include "iio_trigger.h"
#endif
#ifdef IIO_LWIP_EXAMPLE
#include "adin1110.h"
#endif
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
extern struct no_os_uart_init_param adxl355_uart_ip;
extern struct no_os_spi_init_param adxl355_spi_ip;
extern struct adxl355_init_param adxl355_ip;

#ifdef IIO_TRIGGER_EXAMPLE
#define ADXL355_GPIO_TRIG_NAME "adxl355-dev0"

extern struct iio_hw_trig_init_param adxl355_gpio_trig_ip;
extern struct no_os_irq_init_param adxl355_gpio_irq_ip;
#endif

#ifdef IIO_LWIP_EXAMPLE
extern uint8_t adin1110_mac_address[6];
extern struct adin1110_init_param adin1110_ip;
#endif

#endif /* __COMMON_DATA_H__ */
