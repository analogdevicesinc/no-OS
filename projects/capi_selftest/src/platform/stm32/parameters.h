/***************************************************************************//**
 * @file parameters.h
 * @brief Definitions specific to STM32 platform used by capi_selftest project.
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "stm32_hal.h"
#include "stm32_capi_gpio.h"
#include "stm32_capi_spi.h"
#include "stm32_capi_irq.h"
#include "stm32_capi_uart.h"
#include "stm32_capi_timer.h"
#include "stm32_capi_dma.h"
#include "capi_uart.h"

extern UART_HandleTypeDef huart3;
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim2;

#define UART_IDENTIFIER		0U
#define UART_OPS		&stm32_capi_uart_ops
#define UART_BAUDRATE		115200U
#define UART_EXTRA_TYPE		struct stm32_uart_extra_config
#define UART_EXTRA_INIT		{ .huart = &huart3 }
#define PLATFORM_NAME		"STM32"

/*
 * GPIO loopback pair on NUCLEO-F767ZI:
 *   PE0 (output, GPIOE pin 0) wired to PF0 (input, GPIOF pin 0).
 * Each port is opened with num_pins=1 so bit 0 maps to physical pin 0.
 */
#define GPIO_OUTPUT_IDENTIFIER		((uint64_t)(uintptr_t)GPIOE)
#define GPIO_OUTPUT_NUM_PINS		1U
#define GPIO_OUTPUT_OPS			&stm32_capi_gpio_ops
#define GPIO_OUTPUT_NAME		"PE0"
#define GPIO_OUTPUT_EXTRA		struct stm32_capi_gpio_port_config
#define GPIO_OUTPUT_EXTRA_INIT		{ .mode = GPIO_MODE_OUTPUT_PP, \
					  .speed = GPIO_SPEED_FREQ_LOW, \
					  .alternate = 0U, \
					  .pull = GPIO_NOPULL }

#define GPIO_INPUT_IDENTIFIER		((uint64_t)(uintptr_t)GPIOF)
#define GPIO_INPUT_NUM_PINS		1U
#define GPIO_INPUT_OPS			&stm32_capi_gpio_ops
#define GPIO_INPUT_NAME			"PF0"
#define GPIO_INPUT_EXTRA		struct stm32_capi_gpio_port_config
#define GPIO_INPUT_EXTRA_INIT		{ .mode = GPIO_MODE_INPUT, \
					  .speed = GPIO_SPEED_FREQ_LOW, \
					  .alternate = 0U, \
					  .pull = GPIO_NOPULL }

/* SPI async delivery mode selection. */
#define SPI_HAS_IRQ  1
#define SPI_HAS_DMA  0

/* IRQ controller — NVIC, no base address needed. */
#define IRQ_CTRL_IDENTIFIER		0U

/*
 * SPI1 on NUCLEO-F767ZI:
 *   PA5 = SCK, PA6 = MISO, PA7 = MOSI
 *   External loopback requires PA7 physically wired to PA6.
 */
#define SPI_IDENTIFIER		((uint64_t)(uintptr_t)SPI1)
#define SPI_OPS			&stm32_capi_spi_ops
#define SPI_EXTRA_TYPE		struct stm32_spi_extra_config
#define SPI_EXTRA_INIT		{ .hspi = &hspi1, \
				  .get_input_clock = NULL, \
				  .alternate = 0U, \
				  .dma_handle = NULL, \
				  .rxdma_ch_id = 0U, \
				  .txdma_ch_id = 0U, \
				  .irq_num = SPI1_IRQn }
#define SPI_CLK_FREQ		96000000U

#define SPI_DEVICE_NATIVE_CS	0x01U
#define SPI_DEVICE_MODE		CAPI_SPI_MODE_0
#define SPI_DEVICE_SPEED_HZ	1000000U

/*
 * TIM2 on NUCLEO-F767ZI: 32-bit general-purpose timer on APB1.
 * The driver uses identifier=2 to select TIM2 via get_timer_base_from_identifier()
 * and auto-detects the APB1 clock. output_freq_hz=1 MHz gives 1 us resolution.
 */
#define TIMER_IDENTIFIER	2U
#define TIMER_OPS		&stm32_capi_timer_ops
#define TIMER_EXTRA_TYPE	struct stm32_capi_timer_extra_config
#define TIMER_EXTRA_INIT	{ .htim = NULL, \
				  .get_input_clock = NULL, \
				  .irq_num = TIM2_IRQn }

#define TIMER_DIRECTION		CAPI_TIMER_COUNT_UP
#define TIMER_COUNTER_MAX	0xFFFFFFFFU
#define TIMER_COMPARE_VALUE	0x8000U
#define TIMER_RATE_WINDOW_US	10000U
#define TIMER_RATE_COUNTER_MASK	0xFFFFFFFFU
#define TIMER_RATE_TOLERANCE_PCT 5U
#define TIMER_HAS_IRQ		1
#define TIMER_HAS_CAPTURE	1

/*
 * DMA2 on NUCLEO-F767ZI: only DMA2 supports memory-to-memory transfers.
 * Stream 0, channel 0 is used (no peripheral trigger needed for mem-to-mem).
 * Polling mode (irq_num=0): the driver blocks in xfer_start until the
 * transfer completes — no interrupt infrastructure required.
 */
#define DMA_OPS			&stm32_capi_dma_ops
#define DMA_IDENTIFIER		0U
#define DMA_NUM_CHANS		1U
#define DMA_XFER_EXTRA_TYPE	struct stm32_dma_chan_extra_config
#define DMA_XFER_EXTRA_INIT	{ .hdma = &(DMA_HandleTypeDef){ \
					.Instance = DMA2_Stream0 }, \
				  .ch_num = DMA_CHANNEL_0, \
				  .mem_increment = true, \
				  .per_increment = true, \
				  .mem_data_alignment = CAPI_DMA_DATA_ALIGN_BYTE, \
				  .per_data_alignment = CAPI_DMA_DATA_ALIGN_BYTE, \
				  .dma_mode = CAPI_DMA_NORMAL_MODE, \
				  .trig = NULL }
#define DMA_PLATFORM_INIT()	__HAL_RCC_DMA2_CLK_ENABLE()
#define DMA_XFER_SIZE		64U

#endif /* __PARAMETERS_H__ */
