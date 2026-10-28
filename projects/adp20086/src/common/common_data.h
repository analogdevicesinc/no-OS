/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Header file of common data for the adp20086 examples.
 *   @author Mark John Lerry Casero (markjohnlerry.casero@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include "parameters.h"
#include "no_os_i2c.h"
#include "no_os_irq.h"
#include "adp20086.h"

extern struct no_os_uart_init_param adp20086_uart_ip;
extern struct no_os_i2c_init_param adp20086_i2c_ip;
extern struct no_os_gpio_init_param adp20086_en_ip;
extern struct no_os_gpio_init_param adp20086_intb_ip;
extern struct no_os_irq_init_param adp20086_gpio_irq_ip;
extern struct adp20086_init_param adp20086_ip;

#endif /* __COMMON_DATA_H__ */
