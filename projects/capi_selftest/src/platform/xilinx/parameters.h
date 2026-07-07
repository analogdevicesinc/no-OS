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
#include "xilinx_capi_timer.h"
#include "xilinx_capi_i2c.h"
#include "xilinx_capi_irq.h"
#include "capi_timer.h"
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

/*
 * PS TTC (XTtcPs, triple timer counter) — 1 channel per instance, driven by
 * capi_timer_xilinx_ps_ttc_ops. TTC0 is a PS peripheral behind the GIC, so its
 * overflow interrupt is a level-high SPI (same delivery path as the PS SPI/UART
 * that already work) rather than a fabric edge — the AXI timer's fabric IRQ_F2P
 * pulse never reached the GIC on this board. Base, clock and interrupt come from
 * the SDT xparameters macros.
 */
#define TIMER_IDENTIFIER	XPAR_XTTCPS_0_BASEADDR
#define TIMER_OPS		&capi_timer_xilinx_ps_ttc_ops
#define TIMER_INPUT_CLK_HZ	XPAR_XTTCPS_0_CLOCK_FREQ
#define TIMER_OUTPUT_FREQ_HZ	0U	/* TTC free-runs, no target frequency */
#define TIMER_EXTRA_TYPE	struct capi_timer_xilinx_config
#define TIMER_IRQ_ID		(XGet_IntrId(XPAR_XTTCPS_0_INTERRUPTS) + \
				 XGet_IntrOffset(XPAR_XTTCPS_0_INTERRUPTS))
#define TIMER_EXTRA_INIT	{ .use_irq = true, \
				  .irq_id = CAPI_IRQ_XILINX_GIC(TIMER_IRQ_ID) }

/*
 * Timer test capability flags and counter shape (consumed by test_timer.c).
 *
 * The TTC has an overflow interrupt, so IRQ-backed tests run. It has no input
 * capture, so capture is gated off here rather than failing.
 *
 * The Zynq-7000 (ARMA9) TTC counter is 16-bit: XTTCPS_MAX_INTERVAL_COUNT is
 * 0xFFFF, so max, the rate mask and the counter width are all 16-bit here. The
 * driver's default prescaler divides the ~111 MHz source by 2, so a 16-bit span
 * rolls over in ~1.2 ms — well inside the test's 1 s IRQ timeout.
 */
#define TIMER_HAS_IRQ		1
#define TIMER_HAS_CAPTURE	0

#define TIMER_DIRECTION		CAPI_TIMER_COUNT_UP
#define TIMER_COUNTER_MAX	0x0000FFFFU	/* 16-bit TTC, ~1.2 ms rollover */
#define TIMER_COUNTER_WIDTH	16U
#define TIMER_COMPARE_VALUE	0x00001000U

/*
 * Rate check window: short enough that the counter delta measured over it never
 * straddles a rollover (window << rollover period), and the mask covers the full
 * 16-bit counter width. Tolerance allows for measurement/quantization slack.
 */
#define TIMER_RATE_WINDOW_US	100U
#define TIMER_RATE_COUNTER_MASK	0x0000FFFFU
#define TIMER_RATE_TOLERANCE_PCT 10U

/*
 * I2C initiator/target loopback on ZedBoard:
 *   Initiator = PL AXI IIC (XIic) in the fabric, IRQ_F2P GIC id 36.
 *   Target    = PS I2C0 (XIicPs) on EMIO, behind the GIC (level-high SPI).
 * Wire the two buses together (SCL<->SCL, SDA<->SDA) with pull-ups. The
 * target answers I2C_TARGET_ADDR; the initiator addresses that same address.
 */
#define I2C_TARGET_ADDR		0x42U

#define I2C_IDENTIFIER		XPAR_XIIC_0_BASEADDR
#define I2C_OPS			&capi_i2c_xilinx_pl_ops
#define I2C_EXTRA_TYPE		struct capi_i2c_xilinx_config
#define I2C_EXTRA_INIT		{ .use_irq = true, \
				  .irq_id = CAPI_IRQ_XILINX_GIC(XPAR_FABRIC_AXI_IIC_0_INTR) }

#define I2C_TARGET_IDENTIFIER	XPAR_XIICPS_0_BASEADDR
#define I2C_TARGET_OPS		&capi_i2c_xilinx_ps_ops
#define I2C_TARGET_EXTRA_TYPE	struct capi_i2c_xilinx_config
#define I2C_TARGET_PS_IRQ_ID	(XGet_IntrId(XPAR_XIICPS_0_INTERRUPTS) + \
				 XGet_IntrOffset(XPAR_XIICPS_0_INTERRUPTS))
#define I2C_TARGET_EXTRA_INIT	{ .use_irq = true, \
				  .irq_id = CAPI_IRQ_XILINX_GIC(I2C_TARGET_PS_IRQ_ID) }

/*
 * On Xilinx the CAPI GIC IRQ singleton routes each controller's interrupt to
 * the ISR connected at capi_i2c_init(), and the BSP handles clocking/pinmux.
 * So the test's platform hooks reduce to no-ops (unlike STM32, where I2C2's
 * clock, pins and IRQ vectors must be brought up by hand).
 */
#define I2C_PLATFORM_INIT()		0
#define I2C_PLATFORM_DEINIT()		((void)0)
#define I2C_PLATFORM_SET_TARGET(h)	((void)(h))

#endif /* __PARAMETERS_H__ */
