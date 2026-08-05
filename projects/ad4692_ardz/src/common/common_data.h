/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by ad469x examples.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "ad4692.h"

extern struct no_os_uart_init_param ad4692_uart_ip;
extern struct no_os_spi_init_param ad4692_spi_ip;
extern struct no_os_gpio_init_param ad4692_gpio0_ip;
extern struct no_os_gpio_init_param ad4692_reset_ip;
extern struct no_os_pwm_init_param ad4692_pwm_ip;
extern struct no_os_irq_init_param ad4692_gpio_irq_ip;

#define AD4692_GPIO_TRIG_NAME		"ad4692-dev0"

extern struct ad4692_init_param ad4692_ip;

#endif /* __COMMON_DATA_H__ */
