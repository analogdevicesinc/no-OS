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
#include "stm32_capi_uart.h"
#include "stm32_capi_gpio.h"
#include "stm32_capi_spi.h"
#include "stm32_capi_irq.h"
#include "stm32_capi_timer.h"
#include "capi_uart.h"

extern UART_HandleTypeDef huart3;
extern SPI_HandleTypeDef hspi1;

#define UART_IDENTIFIER		0U
#define UART_OPS		&stm32_capi_uart_ops
#define UART_BAUDRATE		115200U
#define UART_EXTRA_TYPE		struct stm32_uart_extra_config
#define UART_EXTRA_INIT		{ .huart = &huart3 }
#define PLATFORM_NAME		"STM32"

/*
 * GPIO loopback pair on NUCLEO-F767ZI:
 *   PE0 (output, GPIOE pin 0) wired to PC0 (input, GPIOC pin 0).
 * Each port is opened with num_pins=1 so bit 0 maps to physical pin 0.
 *
 * Both endpoints are physical pin 0: the port tests configure only bit 0
 * (num_pins=1) and the pin-loopback arrays are {0}; the input pin's line 0
 * routes to EXTI0 for the IRQ suite. PC0 replaces PF0 because PF0 is NOT
 * broken out on the Nucleo-144 header (it is tied to the ST-LINK MCU for
 * MCO/HSE-bypass). PC0 is exposed on the Arduino header as A1 and is unused
 * by any other peripheral in this design.
 *
 *   Jumper: PE0 (CN10/D34) <-> PC0 (CN9, Arduino A1)
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

#define GPIO_INPUT_IDENTIFIER		((uint64_t)(uintptr_t)GPIOC)
#define GPIO_INPUT_NUM_PINS		1U
#define GPIO_INPUT_OPS			&stm32_capi_gpio_ops
#define GPIO_INPUT_NAME			"PC0"
#define GPIO_INPUT_EXTRA		struct stm32_capi_gpio_port_config
#define GPIO_INPUT_EXTRA_INIT		{ .mode = GPIO_MODE_INPUT, \
					  .speed = GPIO_SPEED_FREQ_LOW, \
					  .alternate = 0U, \
					  .pull = GPIO_NOPULL }

/*
 * The STM32 CAPI GPIO backend has no toggle op, so the toggle subtests are
 * skipped on this platform.
 */
#define GPIO_HAS_TOGGLE		0

/*
 * Pin-level loopback: pin numbers are physical bit indices within the port
 * (PE0 = bit 0, PC0 = bit 0). The single wired pair is index 0 on each port.
 */
#define GPIO_HAS_PIN_LOOPBACK	1
#define GPIO_OUTPUT_PIN_NUMBERS	{ 0U }
#define GPIO_INPUT_PIN_NUMBERS	{ 0U }

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
#define TIMER_INPUT_CLK_HZ	0U		/* auto-detected from APB1 */
#define TIMER_OUTPUT_FREQ_HZ	1000000U	/* 1 MHz -> 1 us resolution */
#define TIMER_EXTRA_TYPE	struct stm32_capi_timer_extra_config
#define TIMER_EXTRA_INIT	{ .htim = NULL, \
				  .get_input_clock = NULL, \
				  .irq_num = TIM2_IRQn }

#define TIMER_DIRECTION		CAPI_TIMER_COUNT_UP
/*
 * Counter wrap point. Although TIM2 is 32 bits wide, the rollover period must
 * sit between two test windows: wider than the BASIC rate window (10 ms) so a
 * rate sample never straddles more than one wrap, yet narrower than the
 * ASYNC_IRQ overflow timeout (1 s) so the counter-overflow interrupt actually
 * fires within it. At the 1 MHz output rate a full 0xFFFFFFFF span rolls over
 * only every ~71 min, so ASYNC_IRQ never sees an overflow; 0x1FFFF gives a
 * ~131 ms period (13x the rate window, ~7x under the IRQ timeout).
 *
 * TIMER_RATE_COUNTER_MASK must equal TIMER_COUNTER_MAX so the BASIC delta
 * (second - first) & mask stays correct across a wrap; that requires max+1 to
 * be a power of two, which 0x1FFFF satisfies.
 */
#define TIMER_COUNTER_MAX	0x1FFFFU
#define TIMER_COMPARE_VALUE	0x8000U
#define TIMER_RATE_WINDOW_US	10000U
#define TIMER_RATE_COUNTER_MASK	0x1FFFFU
#define TIMER_RATE_TOLERANCE_PCT 5U
#define TIMER_HAS_IRQ		1
#define TIMER_HAS_COMPARE	1

#endif /* __PARAMETERS_H__ */
