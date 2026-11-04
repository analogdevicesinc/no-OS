/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data for the MAXM86161 project.
 *   @author Jan Carlo Roleda (jancarlo.roleda@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include "maxm86161.h"
#include "no_os_irq.h"
#include "parameters.h"

extern struct no_os_uart_init_param maxm86161_uart_ip;
extern struct maxm86161_init_param maxm86161_ip;
extern struct no_os_irq_init_param maxm86161_gpio_irq_ip;

#endif /* COMMON_DATA_H_ */
