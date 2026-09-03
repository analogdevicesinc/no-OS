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
#include "xilinx_capi_uart.h"
#include "xilinx_capi_gpio.h"
#include "xilinx_capi_spi.h"
#include "xilinx_capi_timer.h"
#include "xilinx_capi_i2c.h"
#include "xilinx_capi_irq.h"
#include "capi_timer.h"
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
#define I2C_SEL_PL
#define I2C_TARGET_SEL_PS
#define TIMER_SELECT		TIMER_SEL_AXI
#define UART_ASYNC_SEL_PS
/* #define UART_ASYNC_SEL_PL_LITE */

#define UART_IDENTIFIER		XPAR_XUARTPS_0_BASEADDR

#define UART_OPS		&capi_uart_xilinx_ps_ops
#define UART_EXTRA_TYPE		struct capi_uart_xilinx_config
#define UART_EXTRA_INIT		{ .use_irq = false }
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
 * Second UART, wired in EXTERNAL loopback (TX strapped to RX on the board) and
 * driven by test_uart.c. It is never the console mapped above: the speed case
 * reprograms the line rate mid-run, and doing that to the report transport
 * would silence the log. Two PL cores can carry it:
 *
 *   AXI UART 16550 (XUartNs550, capi_uart_xilinx_pl_ns550_ops):
 *     Full CAPI surface -- runtime line config (baud/parity/stop) and per-source
 *     interrupt masking. Its baud divider is computed from the synthesized
 *     input clock, so XPAR_XUARTNS550_0_CLOCK_FREQ must be passed through.
 *
 *   AXI UART Lite (XUartLite, capi_uart_xilinx_pl_lite_ops):
 *     Line format is fixed in the IP (set_line_config is always -ENOTSUP) and
 *     there is a single shared interrupt-enable bit rather than per-source
 *     masks, so ASYNC_SPEED and ASYNC_IRQ have nothing to exercise on it.
 *
 * Pick one with UART_ASYNC_SEL_PL_NS550 / UART_ASYNC_SEL_PL_LITE in the board
 * configuration at the top of this file -- which core the strap is actually
 * soldered to is a board fact the BSP cannot report, so it is stated by hand
 * like every other _SEL_ here. Left unset, NS550 is auto-picked when present
 * because it covers strictly more of the API, falling back to UART Lite. With
 * neither core in the BSP, UART_ASYNC_OPS stays undefined and the whole group
 * compiles out to a stub (test_uart.c gates on #ifdef UART_ASYNC_OPS).
 *
 * This block sits BELOW the IRQ topology above on purpose: the fabric line is
 * encoded as an AXI INTC input or a GIC id from IRQ_SEL_CASCADE, which that
 * section may set by default rather than by hand at the top of this file.
 */
#if !defined(UART_ASYNC_SEL_PL_NS550) && !defined(UART_ASYNC_SEL_PL_LITE) && \
    !defined(UART_ASYNC_SEL_PS)
#if defined(XPAR_XUARTNS550_NUM_INSTANCES) || defined(XPAR_XUARTNS550_0_BASEADDR)
#define UART_ASYNC_SEL_PL_NS550
#elif defined(XPAR_XUARTLITE_NUM_INSTANCES) || defined(XPAR_XUARTLITE_0_BASEADDR)
#define UART_ASYNC_SEL_PL_LITE
#endif
#endif

#if defined(UART_ASYNC_SEL_PL_NS550)

#define UART_ASYNC_IDENTIFIER	XPAR_XUARTNS550_0_BASEADDR
#define UART_ASYNC_OPS		&capi_uart_xilinx_pl_ns550_ops
#define UART_ASYNC_EXTRA_TYPE	struct capi_uart_xilinx_config
/*
 * The 16550's baud generator divides this clock; the BSP does not program it,
 * so the synthesized value has to be handed over or every rate comes out wrong.
 */
#define UART_ASYNC_CLK_FREQ_HZ	XPAR_XUARTNS550_0_CLOCK_FREQ
#if defined(XPAR_XUARTNS550_0_INTERRUPTS)
#define UART_ASYNC_IRQ_ID	(XGet_IntrId(XPAR_XUARTNS550_0_INTERRUPTS) + \
				 XGet_IntrOffset(XPAR_XUARTNS550_0_INTERRUPTS))
#define UART_ASYNC_EXTRA_INIT	{ .use_irq = true, \
				  .irq_id = CAPI_IRQ_XILINX_GIC(UART_ASYNC_IRQ_ID) }
#else
#define UART_ASYNC_EXTRA_INIT	{ .use_irq = false }
#endif

/* The 16550 implements line config, per-source masks, and RX timeout events. */
#define UART_ASYNC_HAS_LINE_CONFIG	1
#define UART_ASYNC_HAS_IRQ_CTL		1
#define UART_ASYNC_HAS_RX_TIMEOUT	1

#elif defined(UART_ASYNC_SEL_PL_LITE)

#define UART_ASYNC_IDENTIFIER	XPAR_XUARTLITE_0_BASEADDR
#define UART_ASYNC_OPS		&capi_uart_xilinx_pl_lite_ops
#define UART_ASYNC_EXTRA_TYPE	struct capi_uart_xilinx_config
/* UART Lite's rate is fixed at synthesis; there is no divider to feed. */
#define UART_ASYNC_CLK_FREQ_HZ	0U
#if defined(XPAR_XUARTLITE_0_INTERRUPTS)
#define UART_ASYNC_IRQ_ID	(XGet_IntrId(XPAR_XUARTLITE_0_INTERRUPTS) + \
				 XGet_IntrOffset(XPAR_XUARTLITE_0_INTERRUPTS))
#define UART_ASYNC_EXTRA_INIT	{ .use_irq = true, \
				  .irq_id = CAPI_IRQ_XILINX_GIC(UART_ASYNC_IRQ_ID) }
#else
#define UART_ASYNC_EXTRA_INIT	{ .use_irq = false }
#endif

/*
 * The IP fixes the line format at synthesis (set_line_config is unconditionally
 * -ENOTSUP) and exposes one shared interrupt-enable bit instead of per-source
 * masks (set_irq_tx/rx/err are -ENOTSUP). It also has no receive-timeout
 * interrupt, so the timeout-continuation part of ASYNC_BASIC is inapplicable.
 */
#define UART_ASYNC_HAS_LINE_CONFIG	0
#define UART_ASYNC_HAS_IRQ_CTL		0
#define UART_ASYNC_HAS_RX_TIMEOUT	0

#elif defined(UART_ASYNC_SEL_PS)

/*
 * PS UART 1, NOT 0: UART_IDENTIFIER above maps UART 0 as the console, and the
 * loopback UART must be a different instance (test_uart reprograms the line
 * rate and strap-loops TX into RX -- doing that to the report transport kills
 * the log). main.c enforces the distinction with a #error, so mapping instance
 * 0 here fails the build rather than silently hanging the run. The Zynq PS has
 * two UARTs and the BSP reports both.
 */
#define UART_ASYNC_IDENTIFIER	XPAR_XUARTPS_1_BASEADDR
#define UART_ASYNC_OPS		&capi_uart_xilinx_ps_ops
#define UART_ASYNC_EXTRA_TYPE	struct capi_uart_xilinx_config
/* The PS UART's baud generator is programmed by the driver from the fixed
 * PS peripheral clock; nothing has to be handed over here. */
#define UART_ASYNC_CLK_FREQ_HZ	0U
/*
 * The PS UART interrupt is a fixed PS SPI line, so it is always a GIC id (never
 * an AXI INTC input) and is present regardless of whether any fabric interrupt
 * was wired. That is what lets the no-IRQ build still run async on it: the PL
 * cores lose their fabric lines, but XPAR_XUARTPS_1_INTERRUPTS stays. Note this
 * tracks instance 1, the loopback UART mapped above -- not the console's 0.
 */
#if defined(XPAR_XUARTPS_1_INTERRUPTS)
#define UART_ASYNC_IRQ_ID	(XGet_IntrId(XPAR_XUARTPS_1_INTERRUPTS) + \
				 XGet_IntrOffset(XPAR_XUARTPS_1_INTERRUPTS))
#define UART_ASYNC_EXTRA_INIT	{ .use_irq = true, \
				  .irq_id = CAPI_IRQ_XILINX_GIC(UART_ASYNC_IRQ_ID) }
#else
#define UART_ASYNC_EXTRA_INIT	{ .use_irq = false }
#endif /* XPAR_XUARTPS_1_INTERRUPTS */

/* The PS UART implements line config, per-source masks, and RX timeout events. */
#define UART_ASYNC_HAS_LINE_CONFIG	1
#define UART_ASYNC_HAS_IRQ_CTL		1
#define UART_ASYNC_HAS_RX_TIMEOUT	1

#endif /* UART_ASYNC_SEL_* */

/*
 * Async delivery for the loopback UART, pinned to the GIC/INTC/none build axis
 * exactly as SPI_HAS_IRQ and I2C_MASTER_HAS_IRQ are: it MUST be derived from the
 * same XPAR_*_INTERRUPTS macro that made UART_ASYNC_EXTRA_INIT set use_irq, or
 * the async subtests run against a polled controller and FAIL (-ENOTSUP) where
 * they should SKIP. Note it is defined AFTER the backend selection above, so it
 * tracks whichever core was actually mapped.
 */
#if (defined(UART_ASYNC_SEL_PL_NS550) && defined(XPAR_XUARTNS550_0_INTERRUPTS)) || \
    (defined(UART_ASYNC_SEL_PL_LITE) && defined(XPAR_XUARTLITE_0_INTERRUPTS)) || \
    (defined(UART_ASYNC_SEL_PS) && defined(XPAR_XUARTPS_1_INTERRUPTS))
#define UART_ASYNC_HAS_IRQ	1	/* async via interrupt available */
#else
#define UART_ASYNC_HAS_IRQ	0
#endif

/*
 * The two rates ASYNC_SPEED times against each other, and the payload it times.
 * 256 bytes is ~266 ms at 9600 and ~22 ms at 115200 -- both far above the
 * software floor, and a ~12x ratio leaves the 10% direction margin untroubled.
 */
#define UART_ASYNC_BAUDRATE	115200U
#define UART_ASYNC_BAUD_SLOW	9600U
#define UART_ASYNC_BAUD_FAST	115200U
#define UART_ASYNC_SPEED_LEN	256U

/*
 * TX buffer length for the async TX_BUSY case. It must be larger than the
 * deepest backend TX FIFO so the transfer cannot drain synchronously and drop
 * straight to done: the PS UART holds 64 bytes, so anything at or below that
 * completed in the fill and left TX_BUSY nothing to reject against. 128 clears
 * it with margin on every mapped core.
 */
#define UART_ASYNC_LEN		128U

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

/*
 * Timer selection. Xilinx exposes three timer flavours and the test drives one
 * at a time (a build maps a single TIMER_OPS). Which one is chosen two ways:
 *
 *   1. Override: define TIMER_SELECT (e.g. -DTIMER_SELECT=TIMER_SEL_AXI) to
 *      force a specific timer, provided its instances exist in the BSP.
 *   2. Auto: with no override, pick the first flavour present in the BSP,
 *      preferring the PS TTC — it is the validated path on this board (its
 *      overflow interrupt is a level-high GIC SPI, the same delivery route as
 *      the PS SPI/UART; the AXI timer's fabric IRQ_F2P never reached the GIC).
 *
 * Each selected block below emits the full mapping (TIMER_OPS, identifier,
 * clock, IRQ) plus the capability flags and counter shape test_timer.c reads,
 * so the type-agnostic test exercises exactly the paths that timer supports.
 */
#define TIMER_SEL_TTC	1
#define TIMER_SEL_AXI	2
#define TIMER_SEL_SCU	3

#ifndef TIMER_SELECT
#if defined(XPAR_XTTCPS_NUM_INSTANCES)
#define TIMER_SELECT	TIMER_SEL_TTC
#elif defined(XPAR_XTMRCTR_NUM_INSTANCES) || defined(XPAR_AXI_TIMER_NUM_INSTANCES)
#define TIMER_SELECT	TIMER_SEL_AXI
#elif defined(XPAR_XSCUTIMER_NUM_INSTANCES) || defined(XPAR_SCUTIMER_NUM_INSTANCES)
#define TIMER_SELECT	TIMER_SEL_SCU
#else
#error "No supported Xilinx timer (TTC/AXI/SCU) in the BSP; set TIMER_SELECT"
#endif
#endif /* TIMER_SELECT */

#if TIMER_SELECT == TIMER_SEL_TTC
/*
 * PS TTC (XTtcPs, triple timer counter) — 1 channel per instance,
 * capi_timer_xilinx_ps_ttc_ops. Overflow IRQ and output-compare match
 * registers, but no input capture. The Zynq-7000 (ARMA9) TTC counter is 16-bit
 * (XTTCPS_MAX_INTERVAL_COUNT == 0xFFFF); the driver's default /2 prescaler on
 * the ~111 MHz source rolls a 16-bit span over in ~1.2 ms, well inside the 1 s
 * IRQ timeout.
 */
#define TIMER_IDENTIFIER	XPAR_XTTCPS_0_BASEADDR
#define TIMER_OPS		&capi_timer_xilinx_ps_ttc_ops
#define TIMER_INPUT_CLK_HZ	XPAR_XTTCPS_0_CLOCK_FREQ
#define TIMER_OUTPUT_FREQ_HZ	0U	/* TTC free-runs, no target frequency */
#define TIMER_EXTRA_TYPE	struct capi_timer_xilinx_config
#if defined(XPAR_XTTCPS_0_INTERRUPTS)
#define TIMER_IRQ_ID		(XGet_IntrId(XPAR_XTTCPS_0_INTERRUPTS) + \
				 XGet_IntrOffset(XPAR_XTTCPS_0_INTERRUPTS))
#define TIMER_EXTRA_INIT	{ .use_irq = true, \
				  .irq_id = CAPI_IRQ_XILINX_GIC(TIMER_IRQ_ID) }
#define TIMER_HAS_IRQ		1
#else
/* No interrupt wired (polled build): counter reads only. */
#define TIMER_EXTRA_INIT	{ .use_irq = false }
#define TIMER_HAS_IRQ		0
#endif /* XPAR_XTTCPS_0_INTERRUPTS */

#define TIMER_HAS_COMPARE	1

#define TIMER_DIRECTION		CAPI_TIMER_COUNT_UP
#define TIMER_COUNTER_MAX	0x0000FFFFU	/* 16-bit TTC, ~1.2 ms rollover */
#define TIMER_COUNTER_WIDTH	16U
#define TIMER_COMPARE_VALUE	0x00001000U

#define TIMER_RATE_WINDOW_US	100U
#define TIMER_RATE_COUNTER_MASK	0x0000FFFFU
#define TIMER_RATE_TOLERANCE_PCT 10U

/*
 * IRQ-count case: interrupt every TIMER_IRQ_PERIOD_US and count exactly
 * TIMER_IRQ_EXPECTED_COUNT over PERIOD_US*COUNT of run time. 1 ms (not 10 ms):
 * the 16-bit TTC at ~55 MHz holds only ~1.18 ms per interval, so 1 ms x 200 =>
 * 200 interrupts over 200 ms is the widest period this counter can carry.
 */
#define TIMER_IRQ_PERIOD_US	1000U
#define TIMER_IRQ_EXPECTED_COUNT 200U

#elif TIMER_SELECT == TIMER_SEL_AXI
/*
 * AXI Timer (XTmrCtr, PL fabric) — capi_timer_xilinx_pl_ops, 32-bit counter
 * with output-compare and input-capture channels. Its fabric line feeds the
 * GIC (SPI) or the AXI INTC input depending on the build, chosen the same way
 * as the PL SPI: INTC id under a cascade root, GIC id otherwise.
 */
#define TIMER_IDENTIFIER	XPAR_XTMRCTR_0_BASEADDR
#define TIMER_OPS		&capi_timer_xilinx_pl_ops
#define TIMER_INPUT_CLK_HZ	XPAR_XTMRCTR_0_CLOCK_FREQUENCY
#define TIMER_OUTPUT_FREQ_HZ	1000U
#define TIMER_EXTRA_TYPE	struct capi_timer_xilinx_config
/*
 * Like the PL SPI and PL I2C above, an XSA built without fabric interrupts
 * emits no XPAR_XTMRCTR_0_INTERRUPTS, so its presence decides whether this timer
 * has an IRQ at all. TIMER_HAS_IRQ tracks it so the IRQ subtests SKIP on a
 * polled build instead of running against a controller with use_irq = false.
 */
#if defined(XPAR_XTMRCTR_0_INTERRUPTS)
#if defined(IRQ_SEL_CASCADE)
/* Cascade root: the fabric line is an AXI INTC input (raw local number). */
#define TIMER_IRQ_ID		XPAR_FABRIC_XTMRCTR_0_INTR
#define TIMER_EXTRA_INIT	{ .use_irq = true, \
				  .irq_id = CAPI_IRQ_XILINX_INTC(TIMER_IRQ_ID) }
#else
/* GIC root: resolve the SDT-encoded fabric line to a GIC id. */
#define TIMER_IRQ_ID		(XGet_IntrId(XPAR_XTMRCTR_0_INTERRUPTS) + \
				 XGet_IntrOffset(XPAR_XTMRCTR_0_INTERRUPTS))
#define TIMER_EXTRA_INIT	{ .use_irq = true, \
				  .irq_id = CAPI_IRQ_XILINX_GIC(TIMER_IRQ_ID) }
#endif
#define TIMER_HAS_IRQ		1
#else
/* No fabric interrupt wired (polled build): counter reads only. */
#define TIMER_EXTRA_INIT	{ .use_irq = false }
#define TIMER_HAS_IRQ		0
#endif /* XPAR_XTMRCTR_0_INTERRUPTS */

#define TIMER_HAS_CAPTURE	1
#define TIMER_HAS_COMPARE	1

#define TIMER_DIRECTION		CAPI_TIMER_COUNT_UP
/*
 * Counter span the free-running cases request, NOT the counter's physical
 * width (that is TIMER_COUNTER_WIDTH below, still a full 32 bits). The driver
 * turns `max` into an up-counter period by preloading TLR = 2^32 - max, so
 * this value is what sets the overflow rate -- and ASYNC_IRQ waits only
 * TIMER_IRQ_TIMEOUT_US (1 s) for one overflow to arrive.
 *
 * At the synthesized 100 MHz a full 0xFFFFFFFF span is ~42.9 s per overflow,
 * so the interrupt cannot possibly land inside that 1 s window and ASYNC_IRQ
 * fails with zero deliveries. The narrow PS TTC only passes here by accident
 * of being 16 bits (~1.2 ms per wrap). Ask for 0x01000000 ticks instead:
 * ~168 ms at 100 MHz, comfortably inside the timeout while still exercising a
 * genuinely wide 32-bit rollover.
 *
 * It also has to stay well ABOVE the ~10 us that TIMER_RATE_WINDOW_US samples
 * in BASIC, or the counter would wrap inside the rate window and spoil the
 * tick-delta cross-check. 168 ms clears that by four orders of magnitude.
 */
#define TIMER_COUNTER_MAX	0x01000000U	/* ~168 ms at 100 MHz */
#define TIMER_COUNTER_WIDTH	32U
#define TIMER_COMPARE_VALUE	0x00010000U

#define TIMER_RATE_WINDOW_US	100U
#define TIMER_RATE_COUNTER_MASK	0xFFFFFFFFU
#define TIMER_RATE_TOLERANCE_PCT 10U

/*
 * IRQ-count case sizing (compiled even though TIMER_HAS_IRQ gates the subtest
 * off here). The 32-bit AXI counter can hold the full 10 ms period, so this
 * matches the literal "every 10 ms, 20 interrupts over 200 ms" once the fabric
 * IRQ is routed and TIMER_HAS_IRQ is set.
 */
#define TIMER_IRQ_PERIOD_US	10000U
#define TIMER_IRQ_EXPECTED_COUNT 20U

#elif TIMER_SELECT == TIMER_SEL_SCU
/*
 * PS SCU private timer (XScuTimer) — capi_timer_xilinx_ps_scu_ops, a 32-bit
 * down-counter with auto-reload and an overflow IRQ. It supports compare mode
 * but has no input capture. Runs at half the CPU (3x3) clock behind the GIC.
 */
#define TIMER_IDENTIFIER	XPAR_XSCUTIMER_0_BASEADDR
#define TIMER_OPS		&capi_timer_xilinx_ps_scu_ops
/*
 * The SDT BSP emits no XPAR_XSCUTIMER_0_CLOCK_FREQ (Vitis 2025.x describes the
 * SCU timer with only base address and interrupt), so derive the rate from the
 * architectural relationship instead: the A9 private timer is clocked at half
 * the CPU clock, which the BSP does publish. Prefer a BSP-supplied value if a
 * future toolchain starts emitting one.
 */
#if defined(XPAR_XSCUTIMER_0_CLOCK_FREQ)
#define TIMER_INPUT_CLK_HZ	XPAR_XSCUTIMER_0_CLOCK_FREQ
#else
#define TIMER_INPUT_CLK_HZ	(XPAR_CPU_CORE_CLOCK_FREQ_HZ / 2U)
#endif
/*
 * Free-run at the source clock, as the TTC block does -- do NOT ask for a
 * target tick rate here. The SCU prescaler is 8 bits (divide by N+1, N <= 255),
 * so the ~325 MHz private-timer clock cannot be divided anywhere near 1 kHz:
 * the driver computes a prescaler of ~325000, clamps it to 255, and silently
 * runs at ~1.27 MHz instead. Worse, the resulting 788 ns tick is longer than
 * the gap between two back-to-back counter reads, so BASIC's COUNTER_MOVED
 * check sees the same value twice and fails. Unprescaled, one tick is ~3 ns
 * and both reads differ.
 */
#define TIMER_OUTPUT_FREQ_HZ	0U
#define TIMER_EXTRA_TYPE	struct capi_timer_xilinx_config
#define TIMER_IRQ_ID		CAPI_IRQ_XILINX_GIC(XPS_SCU_TMR_INT_ID)
#define TIMER_EXTRA_INIT	{ .use_irq = true, \
				  .irq_id = TIMER_IRQ_ID }

#define TIMER_HAS_IRQ		1
#define TIMER_HAS_COMPARE	1

#define TIMER_DIRECTION		CAPI_TIMER_COUNT_DOWN
/*
 * Counter span the free-running cases request, not the counter's physical
 * width (that is TIMER_COUNTER_WIDTH below, a full 32 bits). For this
 * down-counter `max` is the reload value, so it sets the overflow period --
 * and ASYNC_IRQ waits only TIMER_IRQ_TIMEOUT_US (1 s) for one overflow.
 *
 * At the unprescaled ~325 MHz a full 0xFFFFFFFF span is ~13.2 s per overflow,
 * so the interrupt cannot land inside that window and ASYNC_IRQ sees zero
 * deliveries. 0x01000000 ticks is ~52 ms: well inside the timeout, and still
 * far above the ~32.5k ticks TIMER_RATE_WINDOW_US samples in BASIC, so the
 * counter cannot wrap mid-measurement and spoil the rate cross-check.
 */
#define TIMER_COUNTER_MAX	0x01000000U	/* ~52 ms at ~325 MHz */
#define TIMER_COUNTER_WIDTH	32U
#define TIMER_COMPARE_VALUE	0x00010000U

#define TIMER_RATE_WINDOW_US	100U
#define TIMER_RATE_COUNTER_MASK	0xFFFFFFFFU
#define TIMER_RATE_TOLERANCE_PCT 10U

/*
 * IRQ-count case: the 32-bit SCU down-counter easily holds a 10 ms period, so
 * this is the literal "every 10 ms, 20 interrupts over 200 ms" — each period the
 * counter reloads and reaches zero once, firing one expiry interrupt.
 */
#define TIMER_IRQ_PERIOD_US	10000U
#define TIMER_IRQ_EXPECTED_COUNT 20U

#else
#error "TIMER_SELECT must be TIMER_SEL_TTC, TIMER_SEL_AXI or TIMER_SEL_SCU"
#endif /* TIMER_SELECT */

/*
 * I2C initiator/target loopback. Each role picks its backend independently
 * (mirroring the GPIO/SPI schemes), so all four wirings are expressible:
 * PS/PL initiator x PS/PL target.
 *
 *   PS I2C (XIicPs, EMIO): behind the GIC, its interrupt is always a GIC id.
 *   PL AXI IIC (XIic, fabric): its IRQ_F2P line feeds the GIC (SPI) or the AXI
 *     INTC input depending on the build, chosen the same way as the PL SPI:
 *     INTC id under a cascade root (IRQ_SEL_CASCADE), GIC id otherwise. A polled
 *     ("noirq") build has the core but no wired interrupt (no
 *     XPAR_XIIC_0_INTERRUPTS), so use_irq falls back to false.
 *
 * Selection per role (define before this point to force):
 *   Initiator: I2C_SEL_PS / I2C_SEL_PL
 *   Target:    I2C_TARGET_SEL_PS / I2C_TARGET_SEL_PL
 * With no override each role auto-detects the form present in the BSP,
 * preferring the one used now: PL initiator, PS target. A role whose backend is
 * absent from the BSP leaves its macros undefined so the build still compiles.
 * Wire the two buses together (SCL<->SCL, SDA<->SDA) with pull-ups; the target
 * answers I2C_TARGET_ADDR, the initiator addresses that same address.
 */
#define I2C_TARGET_ADDR		0x42U

/*
 * input_clock_hz (PL only) enables the AXI IIC runtime SCL timing writes
 * (THIGH/TLOW). The core is synthesized at C_S_AXI_ACLK_FREQ_HZ = 100 MHz;
 * feeding it lets configure_bus_speed reprogram the bus rate, at the driver
 * default register offsets (0x13C/0x140) and 50% duty.
 */
#define I2C_PL_INPUT_CLK_HZ	100000000U

/* --- Initiator role: PL AXI IIC preferred, else PS I2C. --- */
#if !defined(I2C_SEL_PS) && !defined(I2C_SEL_PL)
#if defined(XPAR_XIIC_NUM_INSTANCES) || defined(XPAR_XIIC_0_BASEADDR)
#define I2C_SEL_PL
#elif defined(XPAR_XIICPS_NUM_INSTANCES) || defined(XPAR_XIICPS_0_BASEADDR)
#define I2C_SEL_PS
#endif
#endif

#if defined(I2C_SEL_PL)

#define I2C_IDENTIFIER		XPAR_XIIC_0_BASEADDR
#define I2C_OPS			&capi_i2c_xilinx_pl_ops
#define I2C_EXTRA_TYPE		struct capi_i2c_xilinx_config
#if defined(XPAR_XIIC_0_INTERRUPTS)
#if defined(IRQ_SEL_CASCADE)
/* Cascade root: the fabric line is an AXI INTC input (raw local number). */
#define I2C_IRQ_ID		XPAR_FABRIC_XIIC_0_INTR
#define I2C_EXTRA_INIT		{ .use_irq = true, \
				  .irq_id = CAPI_IRQ_XILINX_INTC(I2C_IRQ_ID), \
				  .input_clock_hz = I2C_PL_INPUT_CLK_HZ }
#else
/*
 * GIC root. CAPI_IRQ_XILINX_GIC() wants the fully-resolved GIC id in its low
 * half; the GIC backend passes it straight to XScuGic_Connect() with no offset
 * added. XPAR_FABRIC_XIIC_0_INTR is the raw fabric input (36), NOT a GIC id, so
 * resolve it as XGet_IntrId() + the SPI base from XGet_IntrOffset() (+32) =>
 * real GIC id 68.
 */
#define I2C_IRQ_ID		(XGet_IntrId(XPAR_XIIC_0_INTERRUPTS) + \
				 XGet_IntrOffset(XPAR_XIIC_0_INTERRUPTS))
#define I2C_EXTRA_INIT		{ .use_irq = true, \
				  .irq_id = CAPI_IRQ_XILINX_GIC(I2C_IRQ_ID), \
				  .input_clock_hz = I2C_PL_INPUT_CLK_HZ }
#endif
#else
/* No fabric interrupt wired (polled build): initiator runs sync only. */
#define I2C_EXTRA_INIT		{ .use_irq = false, \
				  .input_clock_hz = I2C_PL_INPUT_CLK_HZ }
#endif /* XPAR_XIIC_0_INTERRUPTS */

#elif defined(I2C_SEL_PS)

#define I2C_IDENTIFIER		XPAR_XIICPS_0_BASEADDR
#define I2C_OPS			&capi_i2c_xilinx_ps_ops
#define I2C_EXTRA_TYPE		struct capi_i2c_xilinx_config
#if defined(XPAR_XIICPS_0_INTERRUPTS)
#define I2C_IRQ_ID		(XGet_IntrId(XPAR_XIICPS_0_INTERRUPTS) + \
				 XGet_IntrOffset(XPAR_XIICPS_0_INTERRUPTS))
#define I2C_EXTRA_INIT		{ .use_irq = true, \
				  .irq_id = CAPI_IRQ_XILINX_GIC(I2C_IRQ_ID) }
#else
#define I2C_EXTRA_INIT		{ .use_irq = false }
#endif /* XPAR_XIICPS_0_INTERRUPTS */

#endif /* I2C_SEL_* */

/* --- Target role: PS I2C preferred, else PL AXI IIC. --- */
#if !defined(I2C_TARGET_SEL_PS) && !defined(I2C_TARGET_SEL_PL)
#if defined(XPAR_XIICPS_NUM_INSTANCES) || defined(XPAR_XIICPS_0_BASEADDR)
#define I2C_TARGET_SEL_PS
#elif defined(XPAR_XIIC_NUM_INSTANCES) || defined(XPAR_XIIC_0_BASEADDR)
#define I2C_TARGET_SEL_PL
#endif
#endif

#if defined(I2C_TARGET_SEL_PS)

#define I2C_TARGET_IDENTIFIER	XPAR_XIICPS_0_BASEADDR
#define I2C_TARGET_OPS		&capi_i2c_xilinx_ps_ops
#define I2C_TARGET_EXTRA_TYPE	struct capi_i2c_xilinx_config
#if defined(XPAR_XIICPS_0_INTERRUPTS)
#define I2C_TARGET_PS_IRQ_ID	(XGet_IntrId(XPAR_XIICPS_0_INTERRUPTS) + \
				 XGet_IntrOffset(XPAR_XIICPS_0_INTERRUPTS))
#define I2C_TARGET_EXTRA_INIT	{ .use_irq = true, \
				  .irq_id = CAPI_IRQ_XILINX_GIC(I2C_TARGET_PS_IRQ_ID) }
#else
#define I2C_TARGET_EXTRA_INIT	{ .use_irq = false }
#endif /* XPAR_XIICPS_0_INTERRUPTS */

#elif defined(I2C_TARGET_SEL_PL)

#define I2C_TARGET_IDENTIFIER	XPAR_XIIC_0_BASEADDR
#define I2C_TARGET_OPS		&capi_i2c_xilinx_pl_ops
#define I2C_TARGET_EXTRA_TYPE	struct capi_i2c_xilinx_config
#if defined(XPAR_XIIC_0_INTERRUPTS)
#if defined(IRQ_SEL_CASCADE)
/* Cascade root: the fabric line is an AXI INTC input (raw local number). */
#define I2C_TARGET_IRQ_ID	XPAR_FABRIC_XIIC_0_INTR
#define I2C_TARGET_EXTRA_INIT	{ .use_irq = true, \
				  .irq_id = CAPI_IRQ_XILINX_INTC(I2C_TARGET_IRQ_ID), \
				  .input_clock_hz = I2C_PL_INPUT_CLK_HZ }
#else
/* GIC root: resolve the SDT-encoded fabric line to a GIC id. */
#define I2C_TARGET_IRQ_ID	(XGet_IntrId(XPAR_XIIC_0_INTERRUPTS) + \
				 XGet_IntrOffset(XPAR_XIIC_0_INTERRUPTS))
#define I2C_TARGET_EXTRA_INIT	{ .use_irq = true, \
				  .irq_id = CAPI_IRQ_XILINX_GIC(I2C_TARGET_IRQ_ID), \
				  .input_clock_hz = I2C_PL_INPUT_CLK_HZ }
#endif
#else
#define I2C_TARGET_EXTRA_INIT	{ .use_irq = false, \
				  .input_clock_hz = I2C_PL_INPUT_CLK_HZ }
#endif /* XPAR_XIIC_0_INTERRUPTS */

#endif /* I2C_TARGET_SEL_* */

/*
 * Per-role IRQ capability: a role's async/listen ops work only when the
 * selected backend's interrupt macro exists. Set them here per selection; a
 * polled build (no XPAR_XIIC_0_INTERRUPTS) clears the relevant one. common_data.h
 * derives I2C_PAIR_TARGET_ASYNC and I2C_MASTER_ASYNC from these plus presence.
 */
#if (defined(I2C_SEL_PL) && defined(XPAR_XIIC_0_INTERRUPTS)) || \
    (defined(I2C_SEL_PS) && defined(XPAR_XIICPS_0_INTERRUPTS))
#define I2C_MASTER_HAS_IRQ	1
#else
#define I2C_MASTER_HAS_IRQ	0
#endif

#if (defined(I2C_TARGET_SEL_PL) && defined(XPAR_XIIC_0_INTERRUPTS)) || \
    (defined(I2C_TARGET_SEL_PS) && defined(XPAR_XIICPS_0_INTERRUPTS))
#define I2C_TARGET_HAS_IRQ	1
#else
#define I2C_TARGET_HAS_IRQ	0
#endif

/*
 * The CAPI IRQ singleton and BSP handle IRQ routing and clocking/pinmux, so the
 * test's platform hooks are no-ops (unlike STM32, which brings these up by hand).
 */
#define I2C_PLATFORM_INIT()		0
#define I2C_PLATFORM_DEINIT()		((void)0)
#define I2C_PLATFORM_SET_TARGET(h)	((void)(h))

#endif /* __PARAMETERS_H__ */
