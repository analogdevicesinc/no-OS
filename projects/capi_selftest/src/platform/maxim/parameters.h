/***************************************************************************//**
 * @file parameters.h
 * @brief Definitions specific to MAX32657 platform used by capi_selftest project.
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "maxim_capi_uart.h"
#include "maxim_capi_gpio.h"
#include "maxim_capi_spi.h"
#include "maxim_capi_irq.h"
#include "maxim_capi_timer.h"
#include "maxim_capi_dma.h"
#include "maxim_capi_i2c.h"
#include "capi_uart.h"

#define UART_IDENTIFIER		0U
#define UART_OPS		&max_capi_uart_ops
#define UART_BAUDRATE		115200
#define UART_EXTRA_TYPE		struct max_capi_uart_extra
#define UART_EXTRA_INIT		{ .use_irq = true }
#define PLATFORM_NAME		"MAX32657"

/**
 * GPIO loopback pair on MAX32657EVKIT:
 * 	P0.7 (output) wired to P0.8 (input)
 * Requires the on-board jumpers on JP15 to be removed.
 */
#define GPIO_OUTPUT_IDENTIFIER	0U
#define GPIO_OUTPUT_NUM_PINS	9U
#define GPIO_OUTPUT_OPS		&max_capi_gpio_ops
#define GPIO_OUTPUT_NAME	"P0.7"
#define GPIO_OUTPUT_EXTRA	struct max_capi_gpio_extra_config
#define GPIO_OUTPUT_EXTRA_INIT	{ .func = MAX_CAPI_GPIO_FUNC_OUT }

#define GPIO_INPUT_IDENTIFIER	0U
#define GPIO_INPUT_NUM_PINS	9U
#define GPIO_INPUT_OPS		&max_capi_gpio_ops
#define GPIO_INPUT_NAME		"P0.8"
#define GPIO_INPUT_EXTRA	struct max_capi_gpio_extra_config
#define GPIO_INPUT_EXTRA_INIT	{ .func = MAX_CAPI_GPIO_FUNC_IN }

#define GPIO_HAS_PORT_LOOPBACK	0
#define GPIO_HAS_PIN_LOOPBACK	1

#define GPIO_OUTPUT_PIN_NUMBERS	{ 7 }
#define GPIO_INPUT_PIN_NUMBERS	{ 8 }

/*
 * SPI async delivery mode. SPI_HAS_IRQ and SPI_HAS_DMA are mutually exclusive:
 * the loopback test configures exactly one delivery path per build. MAX32657
 * uses IRQ/FIFO delivery here; DMA delivery would need IRQ-driven DMA
 * completion, which the shared dma_config (extra = NULL) does not wire.
 */
#define SPI_HAS_IRQ	1
#define SPI_HAS_DMA	0

/* IRQ controller - NVIC, no base address needed. */
#define IRQ_CTRL_IDENTIFIER	0U

/**
 * SPI0 on MAX32657EVKIT:
 * 	P0.6 = SCLK, P0.4 = MISO, P0.2 = MOSI
 * 	External loopback requires P0.4 physically wired to P0.2.
 */
#define SPI_IDENTIFIER		0U
#define SPI_OPS			&max_capi_spi_ops
#define SPI_EXTRA_TYPE		struct max_capi_spi_extra
#define SPI_EXTRA_INIT		{ .device_role = MAX_CAPI_SPI_DEVICE_ROLE_CONTROLLER, \
				  .bus_width = MAX_CAPI_SPI_BUS_WIDTH_STANDARD, \
				  .num_targets = 1, \
				  .polarity_mask = 0b111, \
				  .chip_select = MAX_CAPI_SPI_CS0, \
				  .clock_phase = MAX_CAPI_SPI_CLOCK_PHASE_0, \
				  .clock_polarity = MAX_CAPI_SPI_CLOCK_POLARITY_0, \
				  .use_irq = true }
#define SPI_CLK_FREQ		25000000

#define SPI_DEVICE_NATIVE_CS	MAX_CAPI_SPI_CS0
#define SPI_DEVICE_MODE		CAPI_SPI_MODE_0
#define SPI_DEVICE_SPEED_HZ	100000

/**
 * TIM0 on MAX32657EVKIT: 32-bit general purpose timer
 */
#define TIMER_IDENTIFIER		0U
#define TIMER_OPS			&max_capi_timer_ops
#define TIMER_INPUT_CLK_HZ		0
#define TIMER_OUTPUT_FREQ_HZ		1000000U
#define TIMER_EXTRA_TYPE		struct max_capi_timer_extra
#define TIMER_EXTRA_INIT		{ .use_irq = true }

#define TIMER_DIRECTION			CAPI_TIMER_COUNT_UP
#define TIMER_COUNTER_MAX		0x40000U
#define TIMER_COMPARE_VALUE		0x8000U
#define TIMER_RATE_WINDOW_US		10000U
#define TIMER_RATE_COUNTER_MASK		0xFFFFFFFFU
#define TIMER_RATE_TOLERANCE_PCT	5U
#define TIMER_HAS_IRQ			1
#define TIMER_HAS_COMPARE		1

/**
 * I2C on MAX32657EVKIT
 */
#define I2C_IDENTIFIER		0U
#define I2C_OPS			&max_capi_i2c_ops
#define I2C_EXTRA_TYPE		struct max_capi_i2c_extra
#define I2C_EXTRA_INIT		{ .use_irq = true }
#define I2C_TARGET_ADDR		0x42
#define I2C_HAS_IRQ		1

/**
 * DMA1_S on MAX32657EVKIT:
 */
#define DMA_OPS			&max_capi_dma_ops
#define DMA_IDENTIFIER		0U
#define DMA_NUM_CHANS		4U
#define DMA_XFER_EXTRA_TYPE	struct max_capi_dma_xfer_extra
#define DMA_XFER_EXTRA_INIT	{ .reqsel = MAX_CAPI_DMA_REQUEST_MEMTOMEM }
#define DMA_PLATFORM_INIT()
#define DMA_XFER_SIZE		64U

/** IRQ */
#define IRQ_CTRL_EXTRA		&(struct max_capi_irq_extra_config) {}

#endif /* __PARAMETERS_H__ */
