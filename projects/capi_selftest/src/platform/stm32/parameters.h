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
#include "stm32_capi_i2c.h"
#include "stm32_capi_dma.h"
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

/*
 * Run BOTH async delivery modes in a single image. SPI_HAS_IRQ and SPI_HAS_DMA
 * are independent skip flags, so enabling both un-gates the full subtest table,
 * whose IRQ cases (ASYNC_IRQ/MANUAL_ISR/ABORT_IRQ) are listed before the DMA
 * cases (ASYNC_DMA/ABORT_DMA). The delivery mode is decided at runtime by
 * stm32_capi_spi_use_dma(), which only engages DMA once spi_extra.dma_handle is
 * non-NULL: it boots NULL (below) and is armed by spi_dma_platform_init(), which
 * fires inside ASYNC_DMA via SPI_DMA_PLATFORM_INIT(). So every IRQ case runs on
 * the IT path first, then DMA is armed and the DMA cases run - "IRQ first, then
 * DMA" in one flash. This deliberately deviates from the project's usual
 * one-mode-per-build convention (arming is one-way, so a single run is assumed).
 */
#define SPI_HAS_IRQ		1
#define SPI_HAS_DMA		1

/* IRQ controller — NVIC, no base address and no extra config needed. */
#define IRQ_CTRL_IDENTIFIER		0U
#define IRQ_CTRL_EXTRA			NULL

/*
 * SPI1 on NUCLEO-F767ZI:
 *   PA5 = SCK, PA6 = MISO, PA7 = MOSI
 *   External loopback requires PA7 physically wired to PA6.
 */
#define SPI_IDENTIFIER		((uint64_t)(uintptr_t)SPI1)
#define SPI_OPS			&stm32_capi_spi_ops
#define SPI_EXTRA_TYPE		struct stm32_spi_extra_config

/*
 * SPI1 DMA request mapping on this part (RM0410): RX is DMA2 stream 2 channel 3,
 * TX is DMA2 stream 3 channel 3 - the same streams the SDP-K1 (F469) mapping
 * uses. Both carry an irq_num because the SPI DMA path reports completion from
 * the DMA interrupt. The channel IDs index the shared DMA controller configured
 * further down; id 0 is left to the memory-to-memory suite so they never collide.
 */
#define SPI_RXDMA_CH_ID		1U
#define SPI_TXDMA_CH_ID		2U
#define SPI_RXDMA_IRQN		DMA2_Stream2_IRQn
#define SPI_TXDMA_IRQN		DMA2_Stream3_IRQn
#define SPI_RXDMA_IRQ_HANDLER	DMA2_Stream2_IRQHandler
#define SPI_TXDMA_IRQ_HANDLER	DMA2_Stream3_IRQHandler
#define SPI_RXDMA_EXTRA_INIT	{ .hdma = &(DMA_HandleTypeDef){ \
					.Instance = DMA2_Stream2 }, \
				  .ch_num = DMA_CHANNEL_3, \
				  .irq_num = SPI_RXDMA_IRQN, \
				  .mem_increment = true, \
				  .per_increment = false, \
				  .mem_data_alignment = CAPI_DMA_DATA_ALIGN_BYTE, \
				  .per_data_alignment = CAPI_DMA_DATA_ALIGN_BYTE, \
				  .dma_mode = CAPI_DMA_NORMAL_MODE, \
				  .trig = NULL }
#define SPI_TXDMA_EXTRA_INIT	{ .hdma = &(DMA_HandleTypeDef){ \
					.Instance = DMA2_Stream3 }, \
				  .ch_num = DMA_CHANNEL_3, \
				  .irq_num = SPI_TXDMA_IRQN, \
				  .mem_increment = true, \
				  .per_increment = false, \
				  .mem_data_alignment = CAPI_DMA_DATA_ALIGN_BYTE, \
				  .per_data_alignment = CAPI_DMA_DATA_ALIGN_BYTE, \
				  .dma_mode = CAPI_DMA_NORMAL_MODE, \
				  .trig = NULL }

/*
 * dma_handle stays NULL at boot and is patched in at runtime by
 * spi_dma_platform_init(); until then use_dma() returns false and the async
 * cases take the IT path. dma_min_len = 1 defeats the driver's length heuristic
 * so the short transfers the DMA cases use really do go down the DMA path once
 * it is armed. irq_num = SPI1_IRQn keeps the IT path's completion vector wired
 * for the IRQ cases that run first.
 */
#define SPI_EXTRA_INIT		{ .hspi = &hspi1, \
				  .get_input_clock = NULL, \
				  .alternate = 0U, \
				  .dma_handle = NULL, \
				  .rxdma_ch_id = SPI_RXDMA_CH_ID, \
				  .txdma_ch_id = SPI_TXDMA_CH_ID, \
				  .rxdma_extra = &(struct stm32_dma_chan_extra_config) \
						 SPI_RXDMA_EXTRA_INIT, \
				  .txdma_extra = &(struct stm32_dma_chan_extra_config) \
						 SPI_TXDMA_EXTRA_INIT, \
				  .dma_min_len = 1U, \
				  .irq_num = SPI1_IRQn }
#define SPI_CLK_FREQ		96000000U

/*
 * SPI DMA platform hook, implemented in spi_dma_platform_init.c. Defining this
 * pulls that file into the build and is what arms spi_extra.dma_handle; the SPI
 * suite invokes it from the ASYNC_DMA case.
 */
int spi_dma_platform_init(void);
#define SPI_DMA_PLATFORM_INIT()	spi_dma_platform_init()

#define SPI_DEVICE_NATIVE_CS	0x01U
#define SPI_DEVICE_MODE		CAPI_SPI_MODE_0
#define SPI_DEVICE_SPEED_HZ	1000000U

/*
 * SPI initiator/target loopback:
 *   Initiator = SPI1, target = SPI5. SPI5 is chosen because its SCK/MISO/MOSI
 *   (PF7/PF8/PF9) are all broken out on the populated Zio connector CN9, so the
 *   whole test wires up with plain jumpers -- no soldering of the morpho header
 *   (SPI3's SCK PC10 is morpho-only). The slave uses software NSS and is
 *   permanently selected, so it needs no NSS pin or wire. CubeMX maps only
 *   SPI1, so spi_platform_init() brings up SPI5's clock, pins (PF7/8/9) and
 *   NVIC and installs SPI5_IRQHandler -> capi_spi_isr. The test calls
 *   SPI_PLATFORM_SET_TARGET() after init so that vector reaches the target.
 * Wiring (RX direction): PA5(D13)->PF7/CN9.24 (SCK), PA7(D11)->PF9/CN9.26
 *   (MOSI); PF8/CN9.22 (MISO) left open; keep the PA7<->PA6 (D11<->D12) strap.
 *   PA7 fans out to both PA6 and PF9 -- join them on a breadboard row or a
 *   Y-splitter (still solderless).
 */
#define SPI_HAS_TARGET		1
#define SPI_TARGET_IDENTIFIER	((uint64_t)(uintptr_t)SPI5)
extern SPI_HandleTypeDef hspi5;
#define SPI_TARGET_EXTRA_INIT	{ .hspi = &hspi5, \
				  .get_input_clock = NULL, \
				  .alternate = 0U, \
				  .dma_handle = NULL, \
				  .dma_min_len = 1U, \
				  .irq_num = SPI5_IRQn }

/*
 * SPI target (slave) ops and platform glue. Defined only on boards that declare
 * a target (SPI_TARGET_IDENTIFIER). spi_platform_init() lives in
 * spi_platform_init.c (gated on SPI_TARGET_OPS) and brings up the second SPI
 * controller; SPI_PLATFORM_SET_TARGET() routes its IRQ vector to the handle.
 */
#ifdef SPI_TARGET_IDENTIFIER
#define SPI_TARGET_OPS		&stm32_capi_spi_ops
#define SPI_TARGET_EXTRA_TYPE	struct stm32_spi_extra_config

struct capi_spi_controller_handle;
int spi_platform_init(void);
void spi_platform_deinit(void);
void spi_platform_set_target_handle(struct capi_spi_controller_handle *handle);
#define SPI_PLATFORM_INIT()		spi_platform_init()
#define SPI_PLATFORM_DEINIT()		spi_platform_deinit()
#define SPI_PLATFORM_SET_TARGET(h)	spi_platform_set_target_handle(h)
#endif /* SPI_TARGET_IDENTIFIER */

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

/*
 * I2C initiator/target loopback on NUCLEO-F767ZI:
 *   Initiator = I2C1, target = I2C2, wired PB6/PB9 (I2C1) <-> PB10/PB11 (I2C2).
 * CubeMX only sets up I2C1, so i2c_platform_init() brings up I2C2's clock,
 * pins and NVIC and installs the IRQ vectors that dispatch to capi_i2c_isr;
 * the test calls I2C_PLATFORM_SET_TARGET() after init so those vectors reach
 * the target handle.
 */
#define I2C_IDENTIFIER		1U
#define I2C_OPS			&stm32_capi_i2c_ops
#define I2C_EXTRA_TYPE		struct stm32_i2c_extra_config
#define I2C_EXTRA_INIT		{ .hi2c = NULL, .i2c_timing = 0x20303E5D }
#define I2C_TARGET_ADDR		0x42U
#define I2C_HAS_IRQ		0

#define I2C_TARGET_IDENTIFIER	2U
#define I2C_TARGET_OPS		&stm32_capi_i2c_ops
#define I2C_TARGET_EXTRA_TYPE	struct stm32_i2c_extra_config
#define I2C_TARGET_EXTRA_INIT	{ .hi2c = NULL, .i2c_timing = 0x20303E5D }

struct capi_i2c_controller_handle;
int i2c_platform_init(void);
void i2c_platform_deinit(void);
void i2c_platform_set_target_handle(struct capi_i2c_controller_handle *handle);
#define I2C_PLATFORM_INIT()		i2c_platform_init()
#define I2C_PLATFORM_DEINIT()		i2c_platform_deinit()
#define I2C_PLATFORM_SET_TARGET(h)	i2c_platform_set_target_handle(h)

/*
 * DMA2 on NUCLEO-F767ZI: only DMA2 supports memory-to-memory transfers, and it
 * is also the controller behind SPI1, so a single controller is shared:
 * stream 0 channel 0 (id 0) serves the memory-to-memory suite, ids 1 and 2
 * serve SPI (streams 2 and 3). No collision: the three use distinct streams.
 * Polling mode (irq_num=0) for the mem-to-mem channel: the driver blocks in
 * xfer_start until the transfer completes — no interrupt infrastructure needed.
 */
#define DMA_OPS			&stm32_capi_dma_ops
#define DMA_IDENTIFIER		0U
#define DMA_NUM_CHANS		3U
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

/*
 * The STM32 CAPI DMA backend is polling-only: it implements no
 * register_complete_callback op, so the CAPI wrapper returns -EINVAL and no
 * completion interrupt can be delivered. xfer_start() blocks until the transfer
 * finishes. The DMA ASYNC case is therefore skipped on this platform.
 */
#define DMA_HAS_IRQ		0

/* Largest buffer the DMA sizes sweep / increment cases allocate (bytes). */
#define DMA_MAX_XFER_SIZE	256U

#endif /* __PARAMETERS_H__ */
