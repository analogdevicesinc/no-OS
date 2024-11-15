/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Common data used by the MAX25603 project.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "platform_includes.h"
#include "max25603.h"

extern struct no_os_uart_init_param max25603_uart_ip;
extern struct no_os_gpio_init_param max25603_shunt_ip;
extern struct no_os_gpio_init_param max25603_flt_ip;
extern struct no_os_pwm_init_param max25603_en1_ip;
extern struct no_os_pwm_init_param max25603_en2_ip;
extern struct no_os_pwm_init_param max25603_hb_ip;
extern struct no_os_pwm_init_param max25603_lb_ip;
extern struct max25603_init_param max25603_ip;

#endif /* __COMMON_DATA_H__ */
