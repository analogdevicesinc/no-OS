/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to ADuCM3029 platform used by iio_adpd1080
 *           project.
 *   @author DBogdan (dragos.bogdan@analog.com)
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "stdio.h"
#include "platform_init.h"
#include "aducm3029_gpio.h"
#include "aducm3029_gpio_irq.h"
#include "aducm3029_i2c.h"
#include "aducm3029_irq.h"
#include "aducm3029_timer.h"
#include "aducm3029_uart.h"

#define UART_DEVICE_ID	0
#define UART_IRQ_ID		ADUCM_UART_INT_ID
#define UART_BAUDRATE	115200
#define UART_EXTRA      NULL
#define UART_OPS        &aducm_uart_ops

#define I2C_SLAVE_ADDR  0x64
#define I2C_BAUDRATE    400000
#define I2C_OPS         &aducm_i2c_ops
#define I2C_EXTRA       NULL

#define GPIO_OPS        &aducm_gpio_ops
#define GPIO_EXTRA      NULL

#define GPIO_SYNC_NUM   0x0D

#define GPIO_IRQ_ID     ADUCM_GPIO_B_GROUP_SOFT_CTRL
#define GPIO_IRQ_OPS    &aducm_gpio_irq_ops
#define GPIO_IRQ_EXTRA  NULL

#define TIMER_DEVICE_ID 0
#define TIMER_FREQ_HZ   1
#define TIMER_TICKS     0
#define TIMER_OPS       &aducm_timer_ops
#define TIMER_EXTRA     NULL

#endif /* __PARAMETERS_H__ */
