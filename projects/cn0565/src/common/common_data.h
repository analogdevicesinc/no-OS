/***************************************************************************//**
 *   @file   common_data.h
 *   @brief  Defines common data to be used by cn0565 examples.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include <stdint.h>
#include "parameters.h"
#include "no_os_uart.h"
#include "no_os_spi.h"
#include "no_os_i2c.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "ad5940.h"

extern struct no_os_uart_init_param cn0565_uart_ip;
extern struct no_os_i2c_init_param cn0565_i2c_ip;
extern struct ad5940_init_param cn0565_ad5940_ip;

/* Interrupt flag shared with the AD5940 BIA measurement layer. */
extern volatile uint32_t ucInterrupted;
void ad5940_int_callback(void *ctx);
uint32_t GetMCUIntFlag(void);
uint32_t ClrMCUIntFlag(void);

#endif /* __COMMON_DATA_H__ */
