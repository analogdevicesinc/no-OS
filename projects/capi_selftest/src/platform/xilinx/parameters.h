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
#include "xilinx_capi_gpio.h"
#include "xilinx_capi_irq.h"
#include "xinterrupt_wrap.h"

/*
 * Cora Z7 configuration
 *
 *   mode      GPIO  SPI  I2C  I2C-tgt  Timer  UART-async
 *   GIC       PS    PL   PL   PS       TTC    NS550
 *     		   PL    PS   PS   PL       AXI    Lite
 *      	   					        SCU    PS
 *
 */
/* GIC only — no CASCADE, no NOIRQ */
#define GPIO_SEL_PS
#define PLATFORM_NAME		"XILINX"

/*
 * GPIO backend selection. Two loopback paths are supported:
 *
 *   PS GPIO (XGpioPs, EMIO routed to a PMOD):
 *     One EMIO pin drives, an adjacent one reads back — wired together on the
 *     board. Uses base_pin (the global EMIO pin index).
 *
 *   PL GPIO (XGpio, dual-channel AXI GPIO loopback core):
 *     Channel 1 (output) and channel 2 (input) wired together in HDL. Drive
 *     channel 1, read back on channel 2. Uses channel. Carries an interrupt
 *     (ip2intc_irpt) so the GPIO-IRQ test has a source.
 *
 * PS is preferred when XGpioPs exists in the BSP; otherwise fall back to the PL
 * AXI GPIO. Define GPIO_SEL_PL / GPIO_SEL_PS before this point to force one.
 * The PS base_pin values below are board-specific: set them to the wired EMIO
 * loopback pair for your board.
 */
#if !defined(GPIO_SEL_PS) && !defined(GPIO_SEL_PL)
#if defined(XPAR_XGPIOPS_NUM_INSTANCES) || defined(XPAR_XGPIOPS_0_BASEADDR)
#define GPIO_SEL_PS
#elif defined(XPAR_XGPIO_NUM_INSTANCES) || defined(XPAR_XGPIO_1_BASEADDR)
#define GPIO_SEL_PL
#endif
/* No GPIO instance in the BSP: leave GPIO_*_OPS undefined so the GPIO test
 * compiles out (common_data.h gates on #ifdef GPIO_OUTPUT_OPS). */
#endif

#if defined(GPIO_SEL_PS)

#define GPIO_OUTPUT_IDENTIFIER	XPAR_XGPIOPS_0_BASEADDR
#define GPIO_OUTPUT_NUM_PINS	1U
#define GPIO_OUTPUT_OPS		&capi_gpio_xilinx_ps_ops
#define GPIO_OUTPUT_EXTRA	struct capi_gpio_xilinx_ps_config
#define GPIO_OUTPUT_EXTRA_INIT	\
	{ .base_pin = 63U }

#define GPIO_INPUT_IDENTIFIER	XPAR_XGPIOPS_0_BASEADDR
#define GPIO_INPUT_NUM_PINS	1U
#define GPIO_INPUT_OPS		&capi_gpio_xilinx_ps_ops
#define GPIO_INPUT_EXTRA	struct capi_gpio_xilinx_ps_config
#define GPIO_INPUT_EXTRA_INIT	\
	{ .base_pin = 62U }

#elif defined(GPIO_SEL_PL)

/*
 * PL loopback: two separate single-channel AXI GPIO cores (output on GPIO_0,
 * input on GPIO_1), not a dual-channel IP. The new coraz7s_proj.xsa has
 * GPIO_0 and GPIO_1 as independent cores (IS_DUAL=0x0), so route output to
 * the first and input to the second. Both are 32-bit wide; use channel 1
 * (the only channel on single-channel cores).
 */
#define GPIO_OUTPUT_IDENTIFIER	XPAR_XGPIO_0_BASEADDR
#define GPIO_OUTPUT_NUM_PINS	1U
#define GPIO_OUTPUT_OPS		&capi_gpio_xilinx_pl_ops
#define GPIO_OUTPUT_EXTRA	struct capi_gpio_xilinx_pl_config
#define GPIO_OUTPUT_EXTRA_INIT	\
	{ .channel = 1U }

#define GPIO_INPUT_IDENTIFIER	XPAR_XGPIO_1_BASEADDR
#define GPIO_INPUT_NUM_PINS	1U
#define GPIO_INPUT_OPS		&capi_gpio_xilinx_pl_ops
#define GPIO_INPUT_EXTRA	struct capi_gpio_xilinx_pl_config
#define GPIO_INPUT_EXTRA_INIT	\
	{ .channel = 1U }

#endif /* GPIO_SEL_* */

/*
 * Pin-level loopback: pin numbers are port-local indices (0..num_pins-1); the
 * driver maps index i to physical pin base_pin+i (PS) or channel bit i (PL).
 * The single loopback pair is index 0 on each 1-pin port.
 */
#define GPIO_HAS_PIN_LOOPBACK	1
#define GPIO_OUTPUT_PIN_NUMBERS	{ 0U }
#define GPIO_INPUT_PIN_NUMBERS	{ 0U }

/*
 * Physical (global) input pin the GPIO-IRQ hooks configure directly through
 * XGpioPs. This is the EMIO line the test samples for its edge interrupt;
 * board-specific, match it to the wired input of the loopback pair.
 */
#define GPIO_INPUT_PIN		62U

/*
 * Interrupts one low->high pulse on the loopback pin actually raises, which is
 * a property of the input's detector and so differs per backend:
 *
 *   PS (XGpioPs): armed XGPIOPS_IRQ_TYPE_EDGE_RISING, a real single-edge
 *     detector -- the falling half of the pulse is ignored. One event.
 *   PL (AXI GPIO): channel-2 raises ip2intc_irpt on ANY change to the input
 *     register; the IP has no edge-type control at all. Both halves of the
 *     pulse are interrupt-worthy. Two events.
 *
 * The IRQ suite's exact-count case multiplies its pulse count by this, so a
 * change-triggered input is not mistaken for duplicate delivery. Cases that
 * only assert "something arrived" ignore it.
 */
#if defined(GPIO_SEL_PL)
#define GPIO_IRQ_EVENTS_PER_EDGE	2U
#else
#define GPIO_IRQ_EVENTS_PER_EDGE	1U
#endif

/* GIC-only IRQ topology */
#define IRQ_CTRL_IDENTIFIER	XPAR_XSCUGIC_0_BASEADDR
#define IRQ_CTRL_EXTRA		NULL

#endif /* __PARAMETERS_H__ */
