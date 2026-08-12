/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  ADIOL100 project common data definitions.
 *   @author Liviu Stan (liviu.stan@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"

struct no_os_uart_init_param adiol100_uart_ip = {
	.device_id    = UART_DEVICE_ID,
	.baud_rate    = UART_BAUDRATE,
	.size         = NO_OS_UART_CS_8,
	.parity       = NO_OS_UART_PAR_NO,
	.stop         = NO_OS_UART_STOP_1_BIT,
	.extra        = UART_EXTRA,
	.platform_ops = UART_OPS,
};

struct no_os_spi_init_param adiol100_spi_ip = {
	.device_id    = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order    = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode         = NO_OS_SPI_MODE_0,
	.platform_ops = SPI_OPS,
	.chip_select  = SPI_CS,
	.extra        = SPI_EXTRA,
};

static struct no_os_gpio_init_param gpio_irqa_ip = {
	.port         = IRQA_PORT,
	.number       = IRQA_PIN,
	.pull         = NO_OS_PULL_UP,
	.platform_ops = GPIO_OPS,
	.extra        = GPIO_EXTRA,
};

struct no_os_irq_init_param gpio_irq_ip = {
	.irq_ctrl_id  = GPIO_IRQ_CTRL_ID,
	.platform_ops = GPIO_IRQ_OPS,
	.extra        = GPIO_IRQ_EXTRA,
};

struct no_os_irq_init_param nvic_ip = {
	.platform_ops = IRQ_OPS,
};

struct adiol100_init_param adiol100_ip = {
	.spi_ip      = &adiol100_spi_ip,
	.chip_addr   = 0x01,
	.clock_src   = ADIOL100_CLK_CRYSTAL,
	.clk_div     = 0,
	.gpio_irqa   = &gpio_irqa_ip,
};
