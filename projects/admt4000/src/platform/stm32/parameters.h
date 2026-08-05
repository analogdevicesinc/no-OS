/***************************************************************************//**
 *   @file   parameters.h
 *   @brief  Definitions specific to STM32 platform used by admt4000 project.
 *   @author Kister Genesis Jimenez (kister.jimenez@analog.com)
 *******************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "stm32_hal.h"
#include "stm32_irq.h"
#include "stm32_gpio_irq.h"
#include "stm32_i2c.h"
#include "stm32_spi.h"
#include "stm32_gpio.h"
#include "stm32_uart.h"
#include "stm32_uart_stdio.h"
#include "no_os_gpio.h"
#include "no_os_uart.h"

extern UART_HandleTypeDef huart5;

#define UART_DEVICE_ID			5
#define UART_BAUDRATE			230400
#define UART_EXTRA			&uart_extra_ip
#define UART_OPS			&stm32_uart_ops
#define UART_IRQ_ID			UART5_IRQn

/* SPI Pin mapping for Arduino interface */
#define SPI_DEVICE_ID			1
#define SPI_BAUDRATE			2000000
#define SPI_OPS				&stm32_spi_ops
#define SPI_CS_PORT			0
#define SPI_CS				15
#define SPI_EXTRA			&spi_extra_ip

#define GPIO_OPS			&stm32_gpio_ops
#define GPIO_EXTRA			&gpio_extra_ip
#define GPIO_PP_EXTRA			&gpio_extra_pp_ip
#define GPIO_IN_EXTRA			&gpio_in_extra_ip

#define GPIO_GPIO2_PORT			6
#define GPIO_GPIO2			7

#define GPIO_BUSY_PORT			3
#define GPIO_BUSY			12

#define GPIO_FAULT_PORT			6
#define GPIO_FAULT			11

#define GPIO_BOOTLOAD_PORT		0
#define GPIO_BOOTLOAD			2

#define GPIO_CNV_PORT			0
#define GPIO_CNV			11

#define GPIO_ACALC_PORT			0
#define GPIO_ACALC			10

#define GPIO_DIAG1_PORT			1
#define GPIO_DAIG1			15

#define GPIO_SHDN_N_PORT		0
#define GPIO_SHDN_N			4

#define GPIO_RSTB_PORT			0
#define GPIO_RSTB			6

#define GPIO_COIL_RS_PORT		6
#define GPIO_COIL_RS			9

#define GPIO_V_EN_PORT			2
#define GPIO_V_EN			1

#define GPIO_IRQ_ID			10
#define GPIO_IRQ_OPS			&stm32_gpio_irq_ops
#define GPIO_IRQ_EXTRA			&trigger_gpio_irq_ip
#define GPIO_IRQ_INTR_PRIORITY		EXTI15_10_IRQn

#define ADMT4000_GPIO_CB_HANDLE		NULL /* Not used in stm32 platform */
#define ADMT4000_GPIO_TRIG_IRQ_ID	0 /* Not used in stm32 platform */

#define I2C_DEVICE_ID			1
#define I2C_TIMING			0x00000E14 /* (Unused) */
#define I2C_OPS				&stm32_i2c_ops
#define I2C_EXTRA			&i2c_extra_ip

#define HW_CARRIER_NAME			"SDP_K1"
#ifndef FIRMWARE_VERSION
#define FIRMWARE_VERSION			"1.0.0"
#endif

#if (TMC)
#define TMC_CS_PORT			6
#define TMC_CS_PIN			10
#define SPI_TMC_EXTRA			&spi_tmc_extra_ip
#endif

extern struct stm32_uart_init_param uart_extra_ip;
extern struct stm32_spi_init_param spi_extra_ip;
extern struct stm32_spi_init_param spi_tmc_extra_ip;
extern struct stm32_i2c_init_param i2c_extra_ip;
extern struct stm32_gpio_init_param gpio_extra_ip;
extern struct stm32_gpio_init_param gpio_extra_pp_ip;
extern struct stm32_gpio_init_param gpio_in_extra_ip;
extern struct stm32_gpio_irq_init_param trigger_gpio_irq_ip;

#endif /* __PARAMETERS_H__ */
