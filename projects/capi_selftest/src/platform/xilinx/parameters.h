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
#include "xilinx_capi_irq.h"

extern struct capi_uart_ops capi_uart_xilinx_ps_ops;

#define UART_IDENTIFIER		XPAR_XUARTPS_0_BASEADDR

#define UART_OPS		&capi_uart_xilinx_ps_ops
#define UART_BAUDRATE		115200U
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
	{ .base_pin = 55U }

#define GPIO_INPUT_IDENTIFIER	XPAR_XGPIOPS_0_BASEADDR
#define GPIO_INPUT_NUM_PINS	1U
#define GPIO_INPUT_OPS		&capi_gpio_xilinx_ps_ops
#define GPIO_INPUT_EXTRA	struct capi_gpio_xilinx_ps_config
#define GPIO_INPUT_EXTRA_INIT	\
	{ .base_pin = 54U }

#elif defined(GPIO_SEL_PL)

/*
 * PL loopback core is a single dual-channel AXI GPIO: both ports share one
 * base address, distinguished by channel (1 = output, 2 = input).
 */
#define GPIO_OUTPUT_IDENTIFIER	XPAR_XGPIO_1_BASEADDR
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
	{ .channel = 2U }

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
 * XGpioPs. This is the JA1 EMIO line the test samples for its edge interrupt.
 */
#define GPIO_INPUT_PIN		54U

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

/*
 * IRQ controller topology, selected from the BSP. IRQ_CTRL_IDENTIFIER is the
 * root controller capi_irq_init() brings up; the IRQ test and every IRQ-backed
 * async peripheral initialize through it. IRQ_CTRL_EXTRA is config->extra.
 *
 *   1. Cascade (GIC + AXI INTC) - GIC root, INTC sub-controller. extra = &cascade
 *   2. PS GIC only              - GIC is root, no PL INTC.        extra = NULL
 *   3. AXI INTC only            - MicroBlaze root, no GIC.        extra = NULL
 *      !! NOT DESIGNED YET, NEEDS SPECIAL MAILBOX INTERMEDIATE
 *
 * Note the AXI INTC's presence macro is XPAR_XINTC_NUM_INSTANCES (the canonical
 * XIntc name the SDT BSP emits), NOT XPAR_AXI_INTC_NUM_INSTANCES.
 *
 * When both controllers exist the hardware is a cascade (the AXI INTC's output
 * is a GIC SPI), so cascade is the default. Define IRQ_SEL_GIC before this
 * point to force GIC-only regardless (the INTC is then left unused by the API).
 */
#if !defined(IRQ_SEL_GIC) && !defined(IRQ_SEL_CASCADE)
#if defined(XPAR_XSCUGIC_NUM_INSTANCES) && defined(XPAR_XINTC_NUM_INSTANCES)
#define IRQ_SEL_CASCADE
#endif
#endif

#if defined(IRQ_SEL_CASCADE)
/* Cascade: the GIC owns the API as root, the AXI INTC hangs off it. */
#define IRQ_CTRL_IDENTIFIER	XPAR_XSCUGIC_0_BASEADDR
#define IRQ_CTRL_EXTRA		(&(struct capi_irq_xilinx_extra) { \
			.subctrl = &xilinx_capi_irq_intc_subctrl, \
			.subctrl_ctrl_id = XPAR_AXI_INTC_0_BASEADDR, \
			.cascade_gic_irq = CAPI_IRQ_XILINX_CASCADE_AUTO })
#elif defined(XPAR_XSCUGIC_NUM_INSTANCES)
/* PS GIC only. On Zynq the GIC is always present, so a polled ("noirq") build
 * still lands here; the GPIO-IRQ test skips at runtime when no fabric IRQ is
 * wired (main.c returns -ENOTSUP), not by leaving the root undefined. */
#define IRQ_CTRL_IDENTIFIER	XPAR_XSCUGIC_0_BASEADDR
#define IRQ_CTRL_EXTRA		NULL
#elif defined(XPAR_XINTC_NUM_INSTANCES)
/* AXI INTC only (MicroBlaze root). */
#define IRQ_CTRL_IDENTIFIER	XPAR_AXI_INTC_0_BASEADDR
#define IRQ_CTRL_EXTRA		NULL
#else
/* No interrupt controller in the BSP: leave IRQ_CTRL_IDENTIFIER undefined so
 * the IRQ test and every IRQ-backed async path compile out. */
#endif

#endif /* __PARAMETERS_H__ */
