/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by aducm3029_flash_demo examples.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "no_os_flash.h"
#include "no_os_irq.h"
#include "no_os_uart.h"

extern struct no_os_uart_init_param flash_demo_uart_ip;
extern struct no_os_irq_init_param flash_demo_irq_ip;
extern struct no_os_flash_init_param flash_demo_flash_ip;

#endif /* __COMMON_DATA_H__ */
