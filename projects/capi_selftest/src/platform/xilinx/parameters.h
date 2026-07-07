/***************************************************************************//**
 * @file parameters.h
 * @brief Definitions specific to Xilinx platform used by capi_selftest project.
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include <xparameters.h>
#include "capi_uart.h"
#include "xilinx_capi_gpio.h"
#include "xilinx_capi_spi.h"
#include "xilinx_capi_irq.h"
#include "xinterrupt_wrap.h"

extern struct capi_uart_ops capi_uart_xilinx_ps_ops;

#define UART_IDENTIFIER		XPAR_XUARTPS_0_BASEADDR

#define UART_OPS		&capi_uart_xilinx_ps_ops
#define UART_BAUDRATE		115200U
#define PLATFORM_NAME		"XILINX"

/*
 * PS GPIO EMIO on ZedBoard JA PMOD:
 *   EMIO 0 = JA1 (input), EMIO 1 = JA2 (output) — wired together
 *   EMIO 2 = JA7 (input), EMIO 3 = JA8 (output) — wired together
 * Use the JA1/JA2 pair: drive JA2 (global pin 55), read back on JA1 (pin 54).
 */
#define GPIO_OUTPUT_IDENTIFIER	XPAR_XGPIOPS_0_BASEADDR
#define GPIO_OUTPUT_NUM_PINS	1U
#define GPIO_OUTPUT_OPS		&capi_gpio_xilinx_ps_ops
#define GPIO_OUTPUT_EXTRA	struct capi_gpio_xilinx_ps_config
#define GPIO_OUTPUT_EXTRA_INIT	{ .base_pin = 55U }

#define GPIO_INPUT_IDENTIFIER	XPAR_XGPIOPS_0_BASEADDR
#define GPIO_INPUT_NUM_PINS	1U
#define GPIO_INPUT_OPS		&capi_gpio_xilinx_ps_ops
#define GPIO_INPUT_EXTRA	struct capi_gpio_xilinx_ps_config
#define GPIO_INPUT_EXTRA_INIT	{ .base_pin = 54U }

/* IRQ controller: PS GIC. Its identifier is the controller the IRQ test and
 * every IRQ-backed async peripheral initialize through capi_irq_init(). */
#define IRQ_CTRL_IDENTIFIER	XPAR_XSCUGIC_0_BASEADDR

/* SPI async delivery mode selection (pinned with GIC/INTC/none build axis). */
#define SPI_HAS_IRQ  1   /* async via interrupt available */
#define SPI_HAS_DMA  0   /* async via DMA available */

/*
 * PS SPI0 (EMIO) on ZedBoard JC PMOD:
 *   JC1 = SCLK, JC2 = MOSI, JC3 = MISO, JC4 = SS0 (CS0)
 *   External loopback requires MOSI (JC2) physically wired to MISO (JC3).
 * 3 native chip-selects are available: CS0, CS1, CS2.
 */
#define SPI_IDENTIFIER		XPAR_XSPIPS_0_BASEADDR
#define SPI_OPS			&capi_spi_xilinx_ps_ops
#define SPI_EXTRA_TYPE		struct capi_spi_xilinx_config
#define SPI_IRQ_ID		(XGet_IntrId(XPAR_XSPIPS_0_INTERRUPTS) + \
				 XGet_IntrOffset(XPAR_XSPIPS_0_INTERRUPTS))
#define SPI_EXTRA_INIT		{ .use_irq = true, \
				  .irq_id = CAPI_IRQ_XILINX_GIC(SPI_IRQ_ID) }
#define SPI_CLK_FREQ		1000000U

#define SPI_DEVICE_NATIVE_CS	0x01U
#define SPI_DEVICE_MODE		CAPI_SPI_MODE_0
#define SPI_DEVICE_SPEED_HZ	1000000U

#endif /* __PARAMETERS_H__ */
