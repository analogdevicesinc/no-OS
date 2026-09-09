/***************************************************************************//**
 * @file common_data.h
 * @brief Common data header file for capi_selftest project.
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

#include <stdbool.h>
#include <stdint.h>
#include "capi_uart.h"
#include "parameters.h"

struct test_framework_config;

/**
 * @brief Console UART configuration used by the test framework output path.
 */
extern const struct capi_uart_config uart_config;
/**
 * @brief Short platform label printed in the test run header.
 */
extern const char platform_name[];

#ifdef GPIO_OUTPUT_OPS
#include "capi_gpio.h"

/* If it's not declared, assume the port loopback test is on by default. */
#ifndef GPIO_HAS_PORT_LOOPBACK
#define GPIO_HAS_PORT_LOOPBACK 1
#endif /* GPIO_HAS_PORT_LOOPBACK */

/* If it's not declared, assume the pin loopback test is off by default. */
#ifndef GPIO_HAS_PIN_LOOPBACK
#define GPIO_HAS_PIN_LOOPBACK 0
#endif /* GPIO_HAS_PIN_LOOPBACK */

/* Toggle is an optional CAPI op; assume supported unless a platform opts out. */
#ifndef GPIO_HAS_TOGGLE
#define GPIO_HAS_TOGGLE 1
#endif /* GPIO_HAS_TOGGLE */

/**
 * @brief CAPI GPIO config for the output/readback test port.
 */
extern const struct capi_gpio_port_config gpio_output_config;
/**
 * @brief CAPI GPIO config for the input sampling test port.
 */
extern const struct capi_gpio_port_config gpio_input_config;

#if GPIO_HAS_PIN_LOOPBACK
/**
 * @brief Pin number mappings for loopback testing.
 */
extern const uint32_t gpio_output_pin_numbers[];
extern const uint32_t gpio_input_pin_numbers[];
extern const uint32_t gpio_num_output_pins;
extern const uint32_t gpio_num_input_pins;
#endif /* GPIO_HAS_PIN_LOOPBACK */

/*
 * Platform hooks for the GPIO-driven IRQ integration test (test_irq.c). The
 * loopback input pin doubles as an interrupt source: arm() routes that pin to
 * an IRQ line and returns the CAPI IRQ number, ack() clears the pin's latched
 * source from inside the ISR, disarm() masks it again. Implemented per platform
 * in a later commit; arm() returns -ENOTSUP where the board has no GPIO-IRQ
 * path, causing the IRQ test to skip.
 */
/**
 * @brief Route the loopback input pin to an interrupt line.
 * @param irq_line - Out: CAPI IRQ number to connect/enable.
 * @return 0 on success, -ENOTSUP if the board has no GPIO interrupt path,
 *         negative error code otherwise.
 */
int platform_gpio_irq_arm(uint32_t *irq_line);
/**
 * @brief Clear the loopback input pin as the interrupt source.
 * @return true if the input pin was the source and was cleared.
 */
bool platform_gpio_irq_ack(void);
/**
 * @brief Mask the loopback input pin's interrupt again.
 */
void platform_gpio_irq_disarm(void);

/*
 * Interrupts one low->high pulse on the loopback pin raises. A platform whose
 * input is a true single-edge detector leaves this alone; one whose input is
 * change-triggered (interrupting on both halves of the pulse) publishes 2 from
 * its parameters.h. The IRQ suite's exact-count case scales by it.
 */
#ifndef GPIO_IRQ_EVENTS_PER_EDGE
#define GPIO_IRQ_EVENTS_PER_EDGE	1U
#endif /* GPIO_IRQ_EVENTS_PER_EDGE */
#endif /* GPIO_OUTPUT_OPS */

#ifdef SPI_OPS
#include "capi_spi.h"
#include "capi_irq.h"

/**
 * @brief CAPI SPI config for external loopback.
 */
extern const struct capi_spi_config spi_controller_config;
/**
 * @brief CAPI SPI device descriptor for the external loopback test.
 */
extern struct capi_spi_device spi_dev;
#endif /* SPI_OPS */

#ifdef IRQ_CTRL_IDENTIFIER
/**
 * @brief CAPI IRQ controller config used before IRQ-backed async tests.
 */
extern struct capi_irq_config irq_config;
#endif /* IRQ_CTRL_IDENTIFIER */

#ifdef TIMER_OPS
#include "capi_timer.h"

/*
 * Xilinx has three timer flavours (AXI/PL, PS TTC, PS SCU) and each supports a
 * different subset of the CAPI timer surface. The test never branches on the
 * timer type; instead each platform declares what its mapped timer can do and
 * the test exercises exactly those hardware paths, skipping the rest. Defaults
 * below assume a fully featured timer; a platform opts out in parameters.h.
 */

/* Counting direction the free-running counter advances in. */
#ifndef TIMER_DIRECTION
#define TIMER_DIRECTION		CAPI_TIMER_COUNT_UP
#endif /* TIMER_DIRECTION */

/* Counter-overflow interrupt path is wired (async_irq case). */
#ifndef TIMER_HAS_IRQ
#define TIMER_HAS_IRQ		1
#endif /* TIMER_HAS_IRQ */

/* Output-compare channel mode is supported (compare case). */
#ifndef TIMER_HAS_COMPARE
#define TIMER_HAS_COMPARE	1
#endif /* TIMER_HAS_COMPARE */

/*
 * Free-running counter reload/rollover value: one overflow per this many ticks.
 * Default is the 16-bit max (the ARMA9 TTC interval width); a wider timer can
 * raise it in parameters.h. Sized with the clock so a rate window sees tens of
 * overflows (e.g. ~55 MHz TTC / 0xFFFF ~= 1.2 ms period).
 */
#ifndef TIMER_COUNTER_MAX
#define TIMER_COUNTER_MAX	0xFFFFU
#endif /* TIMER_COUNTER_MAX */

/* Output-compare match threshold used by the compare case. */
#ifndef TIMER_COMPARE_VALUE
#define TIMER_COMPARE_VALUE	0x8000U
#endif /* TIMER_COMPARE_VALUE */

/* Rate-check window length (us), timed by the independent uptime clock. */
#ifndef TIMER_RATE_WINDOW_US
#define TIMER_RATE_WINDOW_US	100000U
#endif /* TIMER_RATE_WINDOW_US */

/*
 * IRQ-count case (distinct from the BASIC rate check): fire the overflow
 * interrupt every TIMER_IRQ_PERIOD_US and count exactly TIMER_IRQ_EXPECTED_COUNT
 * of them over PERIOD_US*COUNT of run time (e.g. 1 ms x 200 => 200 interrupts).
 * The period is real time, converted to ticks at runtime, so it is clock
 * agnostic; a platform only needs a period its counter width can hold (the
 * 16-bit TTC tops out near 1.18 ms, hence a 1 ms default). Override per timer in
 * parameters.h.
 */
#ifndef TIMER_IRQ_PERIOD_US
#define TIMER_IRQ_PERIOD_US	1000U
#endif /* TIMER_IRQ_PERIOD_US */

#ifndef TIMER_IRQ_EXPECTED_COUNT
#define TIMER_IRQ_EXPECTED_COUNT	200U
#endif /* TIMER_IRQ_EXPECTED_COUNT */

/* Allowed deviation (%) of measured rate/count from the predicted value. */
#ifndef TIMER_RATE_TOLERANCE_PCT
#define TIMER_RATE_TOLERANCE_PCT	10U
#endif /* TIMER_RATE_TOLERANCE_PCT */

/* Mask applied to the counter delta so it works for any counter width. */
#ifndef TIMER_RATE_COUNTER_MASK
#define TIMER_RATE_COUNTER_MASK	0xFFFFFFFFU
#endif /* TIMER_RATE_COUNTER_MASK */

/**
 * @brief CAPI timer config for the counter/compare/IRQ tests.
 */
extern const struct capi_timer_config timer_config;
#endif /* TIMER_OPS */

#ifdef I2C_OPS
#include "capi_i2c.h"

/*
 * Test-only I2C constants. Defaults let the tests build before I2C is mapped;
 * a platform overrides any of them in parameters.h if its bus needs different
 * values (e.g. a taken address, or an extra config the speed switch requires).
 */

/* Second address the target listens on for the readdress case. */
#ifndef I2C_TARGET_ALT_ADDR
#define I2C_TARGET_ALT_ADDR	0x55U
#endif /* I2C_TARGET_ALT_ADDR */

/* Alternate bus speed the bus-speed case switches to and proves still works. */
#ifndef I2C_SPEED_ALT
#define I2C_SPEED_ALT		CAPI_I2C_SPEED_FAST
#endif /* I2C_SPEED_ALT */

/* SCLK duty cycle (percent) passed to configure_bus_speed in the speed case. */
#ifndef I2C_DUTY_CYCLE
#define I2C_DUTY_CYCLE		50U
#endif /* I2C_DUTY_CYCLE */

/*
 * Floor (microseconds) below which a bus-speed transfer time is dominated by
 * the async poll granularity + ISR latency + timer resolution (the software
 * floor) rather than on-wire time, so a fast-vs-slow comparison cannot resolve
 * the SCL rate change. Probes shorter than this skip the direction assert.
 * Override in parameters.h to match a platform's measured floor.
 */
#ifndef I2C_SPEED_MIN_RESOLVABLE_US
#define I2C_SPEED_MIN_RESOLVABLE_US	20000U
#endif /* I2C_SPEED_MIN_RESOLVABLE_US */

/*
 * Whether the initiator's controller can honor a non-50% SCL duty ratio. The
 * duty ratio is a property of the clock-GENERATING (master) side; a target that
 * never drives SCL cannot honor it, so it is only ever exercised on the
 * initiator. All Xilinx masters (XIic, XIicPs) lack an asymmetric-duty API, so
 * this defaults off and the duty sub-block compiles out. Set to 1 in
 * parameters.h on a platform whose master honors duty_cycle; the sub-block then
 * runs and asserts the request is accepted and the bus still carries data.
 */
#ifndef I2C_DUTY_CYCLE_SUPPORTED
#define I2C_DUTY_CYCLE_SUPPORTED	0
#endif /* I2C_DUTY_CYCLE_SUPPORTED */

/*
 * Two axes gate the I2C suite, kept separate so a new platform sets each on its
 * own terms:
 *
 *   I2C_PAIR_TARGET_ASYNC - both roles are mapped AND the target can arm a
 *                  background (async, IRQ-backed) listen. Every case runs the
 *                  target's half async while the initiator blocks; on one core
 *                  that listen must be armed before the blocking initiator call
 *                  and complete during it, so a target with no IRQ cannot back
 *                  the loopback at all. Hence this folds in I2C_TARGET_HAS_IRQ,
 *                  not just presence.
 *   I2C_MASTER_ASYNC - the INITIATOR can run async too (its IRQ path is live).
 *                  Only the MASTER_ASYNC case needs it; the plain cases keep the
 *                  initiator blocking. That case ANDs it with the pair flag
 *                  above, so this carries just the initiator half.
 *
 * A platform overrides the per-role IRQ flags in its parameters.h; the defaults
 * below assume a fully-mapped, fully-IRQ-capable board.
 */
#ifndef I2C_MASTER_HAS_IRQ
#define I2C_MASTER_HAS_IRQ	1
#endif /* I2C_MASTER_HAS_IRQ */

#ifndef I2C_TARGET_HAS_IRQ
#define I2C_TARGET_HAS_IRQ	1
#endif /* I2C_TARGET_HAS_IRQ */

#ifndef I2C_PAIR_TARGET_ASYNC
#if defined(I2C_OPS) && defined(I2C_TARGET_OPS) && I2C_TARGET_HAS_IRQ
#define I2C_PAIR_TARGET_ASYNC	1
#else
#define I2C_PAIR_TARGET_ASYNC	0
#endif
#endif /* I2C_PAIR_TARGET_ASYNC */

#define I2C_MASTER_ASYNC	I2C_MASTER_HAS_IRQ

/**
 * @brief CAPI I2C initiator configuration for the loopback tests.
 */
extern const struct capi_i2c_config i2c_master_config;
/**
 * @brief CAPI I2C device descriptor used by the initiator to address the target.
 */
extern struct capi_i2c_device i2c_dev;
#endif /* I2C_OPS */

#ifdef I2C_TARGET_OPS
/**
 * @brief CAPI I2C target configuration for the loopback tests.
 */
extern const struct capi_i2c_config i2c_target_config;
/**
 * @brief CAPI I2C device descriptor for the target side.
 */
extern struct capi_i2c_device i2c_target_dev;
#endif /* I2C_TARGET_OPS */

#ifdef UART_ASYNC_OPS
/*
 * Second UART instance, wired in EXTERNAL loopback (TX strapped to RX on the
 * board) and used only by test_uart.c. It is deliberately NOT the console UART
 * above: the framework's report transport must stay untouched, since the speed
 * case reprograms the line rate mid-run and a console reconfigured underneath
 * the log would silence it.
 *
 * Internal/local loopback is never used -- uart_async_line_config.loopback is
 * false -- so every case moves bytes over the real wire.
 */

/* Line rate the loopback UART is brought up at and restored to. */
#ifndef UART_ASYNC_BAUDRATE
#define UART_ASYNC_BAUDRATE	115200U
#endif /* UART_ASYNC_BAUDRATE */

/*
 * Reference clock feeding the UART's baud generator. 0 keeps whatever the BSP
 * configured; a core whose clock the BSP does not publish (a PL UART) sets its
 * synthesized frequency here, since the baud divider is computed from it.
 */
#ifndef UART_ASYNC_CLK_FREQ_HZ
#define UART_ASYNC_CLK_FREQ_HZ	0U
#endif /* UART_ASYNC_CLK_FREQ_HZ */

/*
 * The two rates the speed case times against each other. They must be far
 * enough apart that the ratio survives the software floor; ~12x here.
 */
#ifndef UART_ASYNC_BAUD_SLOW
#define UART_ASYNC_BAUD_SLOW	9600U
#endif /* UART_ASYNC_BAUD_SLOW */

#ifndef UART_ASYNC_BAUD_FAST
#define UART_ASYNC_BAUD_FAST	115200U
#endif /* UART_ASYNC_BAUD_FAST */

/*
 * Speed-case payload. At 9600 baud, 256 bytes is ~266 ms on the wire, well
 * clear of the floor below; at 115200 it is ~22 ms, still clear.
 */
#ifndef UART_ASYNC_SPEED_LEN
#define UART_ASYNC_SPEED_LEN	256U
#endif /* UART_ASYNC_SPEED_LEN */

/*
 * TX buffer length for the TX_BUSY case. It must exceed the deepest backend TX
 * FIFO or the transfer drains during the fill and drops straight to done,
 * leaving TX_BUSY nothing to reject against. A platform whose FIFO is deeper
 * than this raises it in parameters.h.
 */
#ifndef UART_ASYNC_LEN
#define UART_ASYNC_LEN		128U
#endif /* UART_ASYNC_LEN */

/*
 * Floor (microseconds) below which a measured transfer time is dominated by the
 * async poll granularity + ISR latency + timer resolution rather than on-wire
 * time, so a fast-vs-slow comparison cannot resolve the baud change. Legs
 * shorter than this skip the direction assert. Override in parameters.h to
 * match a platform's measured floor.
 */
#ifndef UART_ASYNC_SPEED_MIN_RESOLVABLE_US
#define UART_ASYNC_SPEED_MIN_RESOLVABLE_US	5000U
#endif /* UART_ASYNC_SPEED_MIN_RESOLVABLE_US */

/*
 * Capability axes, one per gated case. A platform sets these in parameters.h
 * from its selected backend; the defaults below assume a fully-featured,
 * IRQ-wired UART.
 *
 *   UART_ASYNC_HAS_IRQ - an interrupt is wired to the mapped UART, so use_irq
 *                  is true and the driver accepts the async ops. A polled build
 *                  clears it and every async case skips instead of failing on
 *                  -ENOTSUP. MUST be derived from the same XPAR_*_INTERRUPTS
 *                  macro that decides use_irq, or the two disagree.
 *   UART_ASYNC_HAS_LINE_CONFIG - the line format can be reprogrammed at runtime.
 *                  UART Lite fixes it in the IP, so its set_line_config is
 *                  unconditionally -ENOTSUP and the speed case has nothing to
 *                  measure.
 *   UART_ASYNC_HAS_IRQ_CTL - the backend exposes per-source interrupt masking
 *                  (set_irq_tx / set_irq_rx / set_irq_err). UART Lite has a
 *                  single shared enable bit and STM32 lacks the calls entirely.
 *   UART_ASYNC_HAS_RX_TIMEOUT - an incomplete receive can raise a non-terminal
 *                  RX_TIMEOUT event. UART Lite has no receive-timeout source.
 */
#ifndef UART_ASYNC_HAS_IRQ
#define UART_ASYNC_HAS_IRQ	1
#endif /* UART_ASYNC_HAS_IRQ */

#ifndef UART_ASYNC_HAS_LINE_CONFIG
#define UART_ASYNC_HAS_LINE_CONFIG	1
#endif /* UART_ASYNC_HAS_LINE_CONFIG */

#ifndef UART_ASYNC_HAS_IRQ_CTL
#define UART_ASYNC_HAS_IRQ_CTL	1
#endif /* UART_ASYNC_HAS_IRQ_CTL */

#ifndef UART_ASYNC_HAS_RX_TIMEOUT
#define UART_ASYNC_HAS_RX_TIMEOUT	1
#endif /* UART_ASYNC_HAS_RX_TIMEOUT */

/**
 * @brief CAPI UART configuration for the external-loopback tests.
 */
extern const struct capi_uart_config uart_async_config;
#endif /* UART_ASYNC_OPS */

#ifdef DMA_OPS
#include "capi_dma.h"

/*
 * DMA test knobs and capability axes. Defaults assume a fully-featured
 * mem-to-mem DMA controller; a platform overrides any of them in parameters.h.
 *
 *   DMA_CHAN_ID       - channel id the tests open (init_chan). Streams/channels
 *                       that carry mem-to-mem differ per part; the platform
 *                       selects a valid one.
 *   DMA_XFER_SIZE     - nominal transfer length (bytes) for the fixed-size cases.
 *   DMA_MAX_XFER_SIZE - largest buffer the sizes sweep and the increment cases
 *                       allocate; must be >= the biggest entry the sweep uses.
 *   DMA_HAS_IRQ       - the controller can deliver completion through an IRQ and
 *                       the driver wires a per-channel irq_num + a
 *                       register_complete_callback op. When 0 the ASYNC case
 *                       skips and every transfer is verified in polling mode
 *                       (xfer_start blocks until the copy is done). The STM32
 *                       CAPI DMA backend runs mem-to-mem in polling mode and
 *                       implements no callback op, so it sets this to 0.
 */
#ifndef DMA_CHAN_ID
#define DMA_CHAN_ID		0U
#endif /* DMA_CHAN_ID */

#ifndef DMA_XFER_SIZE
#define DMA_XFER_SIZE		64U
#endif /* DMA_XFER_SIZE */

#ifndef DMA_MAX_XFER_SIZE
#define DMA_MAX_XFER_SIZE	256U
#endif /* DMA_MAX_XFER_SIZE */

#ifndef DMA_HAS_IRQ
#define DMA_HAS_IRQ		1
#endif /* DMA_HAS_IRQ */

/**
 * @brief CAPI DMA configuration for the memory-to-memory tests.
 */
extern const struct capi_dma_config dma_config;
/**
 * @brief Platform-specific DMA transfer extra config.
 */
extern DMA_XFER_EXTRA_TYPE dma_xfer_extra;
#endif /* DMA_OPS */

/**
 * @brief Fill a test framework configuration for the selected platform.
 * @param config - Destination framework configuration.
 * @param uart - UART handle used as the framework write context.
 */
void get_test_framework_config(struct test_framework_config *config,
			       struct capi_uart_handle *uart);

#endif /* __COMMON_DATA_H__ */
