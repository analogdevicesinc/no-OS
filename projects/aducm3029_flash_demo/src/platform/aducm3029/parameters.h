/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to ADuCM3029 platform used by
 *           aducm3029_flash_demo project.
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "stdio.h"
#include "platform_init.h"
#include "aducm3029_irq.h"
#include "aducm3029_uart.h"
#include "aducm3029_uart_stdio.h"

#define UART_DEVICE_ID	0
#define UART_BAUDRATE	115200
#define UART_EXTRA      NULL
#define UART_OPS        &aducm_uart_ops

#define IRQ_CTRL_ID     0
#define IRQ_OPS         &aducm_irq_ops
#define IRQ_EXTRA       NULL

#define FLASH_DEVICE_ID 0

#endif /* __PARAMETERS_H__ */
