/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by max11205pmb1 examples.
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
#include "max11205.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
extern struct no_os_uart_init_param max11205_uart_ip;

extern struct no_os_spi_init_param max11205_spi_ip;
extern struct no_os_gpio_init_param max11205_gpio_rdy_ip;
extern struct no_os_irq_init_param max11205_gpio_irq_ip;
extern struct max11205_init_param max11205_ip;

#endif /* __COMMON_DATA_H__ */
