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
#include "xilinx_capi_spi.h"
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
#define SPI_SEL_PS
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

/*
 * SPI backend selection, mirroring the GPIO scheme:
 *
 *   PS SPI (XSpiPs, SPI0 EMIO routed to a PMOD):
 *     SCLK / MOSI / MISO / SS0 (CS0) on the PMOD. External loopback needs MOSI
 *     wired to MISO. 3 native CS: CS0, CS1, CS2. The PS SPI interrupt is a
 *     fixed PS SPI, always a GIC id.
 *
 *   PL SPI (XSpi, AXI Quad SPI): base at XPAR_XSPI_0_BASEADDR; its fabric line
 *     feeds the GIC (SPI) or the AXI INTC input depending on the build, chosen
 *     by the IRQ_SEL_CASCADE selection (INTC id under a cascade root, GIC id
 *     otherwise).
 *
 * PS is preferred when XSpiPs exists in the BSP; otherwise fall back to the PL
 * AXI SPI. Define SPI_SEL_PL / SPI_SEL_PS before this point to force one.
 */
#if !defined(SPI_SEL_PS) && !defined(SPI_SEL_PL)
#if defined(XPAR_XSPIPS_NUM_INSTANCES) || defined(XPAR_XSPIPS_0_BASEADDR)
#define SPI_SEL_PS
#elif defined(XPAR_XSPI_NUM_INSTANCES) || defined(XPAR_XSPI_0_BASEADDR)
#define SPI_SEL_PL
#endif
#endif

#if defined(SPI_SEL_PS)

#define SPI_IDENTIFIER		XPAR_XSPIPS_0_BASEADDR
#define SPI_OPS			&capi_spi_xilinx_ps_ops
#define SPI_EXTRA_TYPE		struct capi_spi_xilinx_config
#if defined(XPAR_XSPIPS_0_INTERRUPTS)
#define SPI_IRQ_ID		(XGet_IntrId(XPAR_XSPIPS_0_INTERRUPTS) + \
				 XGet_IntrOffset(XPAR_XSPIPS_0_INTERRUPTS))
#define SPI_EXTRA_INIT		{ .use_irq = true, \
				  .irq_id = CAPI_IRQ_XILINX_GIC(SPI_IRQ_ID) }
#else
/* No interrupt wired (polled build): sync transfers only. */
#define SPI_EXTRA_INIT		{ .use_irq = false }
#endif /* XPAR_XSPIPS_0_INTERRUPTS */

#elif defined(SPI_SEL_PL)

#define SPI_IDENTIFIER		XPAR_XSPI_0_BASEADDR
#define SPI_OPS			&capi_spi_xilinx_pl_ops
#define SPI_EXTRA_TYPE		struct capi_spi_xilinx_config
/*
 * An XSA built without fabric interrupts emits no XPAR_XSPI_0_INTERRUPTS at
 * all, so the presence of that macro decides whether an IRQ exists. When it is
 * present the fabric line goes to the AXI INTC (cascade root) or straight to
 * the GIC, chosen by the IRQ_SEL_CASCADE selection rather than by a per-node
 * INTERRUPT_PARENT check.
 */
#if defined(XPAR_XSPI_0_INTERRUPTS)
#if defined(IRQ_SEL_CASCADE)
/* Cascade root: the fabric line is an AXI INTC input (raw local number). */
#define SPI_IRQ_ID		XPAR_FABRIC_XSPI_0_INTR
#define SPI_EXTRA_INIT		{ .use_irq = true, \
				  .irq_id = CAPI_IRQ_XILINX_INTC(SPI_IRQ_ID) }
#else
/* GIC root: resolve the SDT-encoded fabric line to a GIC id. */
#define SPI_IRQ_ID		(XGet_IntrId(XPAR_XSPI_0_INTERRUPTS) + \
				 XGet_IntrOffset(XPAR_XSPI_0_INTERRUPTS))
#define SPI_EXTRA_INIT		{ .use_irq = true, \
				  .irq_id = CAPI_IRQ_XILINX_GIC(SPI_IRQ_ID) }
#endif
#else
/* No fabric interrupt wired (polled build): sync transfers only. */
#define SPI_EXTRA_INIT		{ .use_irq = false }
#endif /* XPAR_XSPI_0_INTERRUPTS */

#endif /* SPI_SEL_* */

/*
 * SPI async delivery mode (pinned with the GIC/INTC/none build axis). Derived
 * from the selected backend's interrupt macro, exactly as I2C_MASTER_HAS_IRQ is
 * below: an XSA built without fabric interrupts emits no XPAR_XSPI_0_INTERRUPTS,
 * so SPI_EXTRA_INIT above sets use_irq = false and the driver rejects every
 * async op with -ENOTSUP. SPI_HAS_IRQ must track that, or the async subtests run
 * against a polled controller and FAIL instead of SKIP.
 */
#if (defined(SPI_SEL_PS) && defined(XPAR_XSPIPS_0_INTERRUPTS)) || \
    (defined(SPI_SEL_PL) && defined(XPAR_XSPI_0_INTERRUPTS))
#define SPI_HAS_IRQ		1	/* async via interrupt available */
#else
#define SPI_HAS_IRQ		0
#endif

#define SPI_HAS_DMA		0	/* async via DMA available */

/*
 * clk_freq_hz is the controller REFERENCE clock, not the requested SCLK. Leave
 * it 0 so the driver keeps the BSP value (XPAR_XSPIPS_0_SPI_CLK_FREQ_HZ,
 * ~166.67 MHz); the requested bus rate is set via SPI_DEVICE_SPEED_HZ
 * (max_speed_hz) below. Passing the intended SCLK here instead would overwrite
 * InputClockHz and make the prescaler divide the wrong base, running SCLK far
 * too fast.
 */
#define SPI_CLK_FREQ		0U

#define SPI_DEVICE_NATIVE_CS	0x01U
#define SPI_DEVICE_MODE		CAPI_SPI_MODE_0
/*
 * max_speed_hz. PS SPI (XSpiPs) has a runtime prescaler and accepts a requested
 * rate. PL AXI Quad SPI has NO runtime divider — its SCLK ratio is fixed in HDL
 * (C_SCK_RATIO), so any non-zero max_speed_hz makes the driver return -ENOTSUP
 * (=134 in newlib baremetal). Request 0 for PL to keep the HDL-fixed rate.
 */
#if defined(SPI_SEL_PL)
#define SPI_DEVICE_SPEED_HZ	0U
#else
#define SPI_DEVICE_SPEED_HZ	1000000U
#endif

#endif /* __PARAMETERS_H__ */
