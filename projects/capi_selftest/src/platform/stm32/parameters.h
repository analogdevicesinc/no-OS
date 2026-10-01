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

#define PLATFORM_NAME		"STM32"

/* IRQ controller — NVIC, no base address needed. */
#define IRQ_CTRL_IDENTIFIER	0U

#if defined(STM32F767xx)

/*
 * ---------------------------------------------------------------------------
 * NUCLEO-F767ZI
 * ---------------------------------------------------------------------------
 */
#define BOARD_NAME		"NUCLEO-F767ZI"

extern UART_HandleTypeDef huart3;
extern SPI_HandleTypeDef hspi1;

/* Console: USART3 is routed to the ST-LINK virtual COM port. */
#define UART_IDENTIFIER		0U
#define UART_EXTRA_INIT		{ .huart = &huart3 }

/*
 * GPIO loopback pair:
 *   PE0 (output, GPIOE pin 0) wired to PC0 (input, GPIOC pin 0).
 * Each port is opened with num_pins=1 so bit 0 maps to physical pin 0, which
 * keeps the port-wide bitmask cases (they drive bits 0..num_pins-1) confined
 * to the wired pair.
 *
 * PC0 replaces PF0 because PF0 is NOT broken out on the Nucleo-144 header (it
 * is tied to the ST-LINK MCU for MCO/HSE-bypass). PC0 is exposed on the
 * Arduino header as A1 and is unused by any other peripheral in this design.
 *
 *   Jumper: PE0 (CN10/D34) <-> PC0 (CN9, Arduino A1)
 */
#define GPIO_OUTPUT_IDENTIFIER	((uint64_t)(uintptr_t)GPIOE)
#define GPIO_OUTPUT_NUM_PINS	1U
#define GPIO_OUTPUT_NAME	"PE0"
#define GPIO_INPUT_IDENTIFIER	((uint64_t)(uintptr_t)GPIOC)
#define GPIO_INPUT_NUM_PINS	1U
#define GPIO_INPUT_NAME		"PC0"

/*
 * Pin-level loopback: pin numbers are physical bit indices within the port
 * (PE0 = bit 0, PC0 = bit 0). The single wired pair is index 0 on each port.
 */
#define GPIO_HAS_PORT_LOOPBACK	1
#define GPIO_HAS_PIN_LOOPBACK	1
#define GPIO_OUTPUT_PIN_NUMBERS	{ 0U }
#define GPIO_INPUT_PIN_NUMBERS	{ 0U }

/*
 * The input pin is PC0, so its external interrupt arrives on EXTI line 0.
 *
 * GPIO_IRQ_CLK_ENABLE and GPIO_IRQ_EXTI_HANDLER complete the mapping so the
 * platform hook in main.c stays board-agnostic: the hook has to clock the port
 * the input pin lives on (CubeMX only clocks ports its .ioc uses), and the
 * vector name follows the EXTI line, not the board.
 */
#define GPIO_IRQ_PORT		GPIOC
#define GPIO_IRQ_PIN		GPIO_PIN_0
#define GPIO_IRQ_LINE		0U
#define GPIO_IRQ_IRQN		EXTI0_IRQn
#define GPIO_IRQ_CLK_ENABLE()	__HAL_RCC_GPIOC_CLK_ENABLE()
#define GPIO_IRQ_EXTI_HANDLER	EXTI0_IRQHandler

/*
 * SPI1:
 *   PA5 = SCK, PA6 = MISO, PA7 = MOSI
 *   External loopback requires PA7 physically wired to PA6.
 */
#define SPI_IDENTIFIER		((uint64_t)(uintptr_t)SPI1)
#define SPI_CLK_FREQ		96000000U

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
 * it is armed.
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
				  .dma_min_len = 1U }

/*
 * SPI DMA platform hook, implemented in spi_dma_platform_init.c. Defining this
 * pulls that file into the build and is what arms spi_extra.dma_handle; the SPI
 * suite invokes it from the ASYNC_DMA case.
 */
int spi_dma_platform_init(void);
#define SPI_DMA_PLATFORM_INIT()	spi_dma_platform_init()

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
				  .dma_min_len = 1U }

/*
 * TIM2: 32-bit general-purpose timer on APB1. The driver uses identifier=2 to
 * select TIM2 via get_timer_base_from_identifier() and auto-detects the APB1
 * clock.
 *
 * Output rate = this board's APB1 timer clock exactly (see TIMER_OUTPUT_FREQ_HZ
 * note in the shared block below for why it must divide the input evenly).
 * nucleo-f767zi.ioc: APB1Freq_Value=48 MHz, APB1CLKDivider=DIV2 => timer clock
 * 2x48 = 96 MHz. Keep in sync with the .ioc if the PLL is ever retuned.
 */
#define TIMER_IDENTIFIER	2U
#define TIMER_IRQ_NUM		TIM2_IRQn
#define TIMER_OUTPUT_FREQ_HZ	96000000U	/* = APB1 timer clock */

/*
 * I2C initiator/target loopback:
 *   Initiator = I2C1, target = I2C2, wired PB6/PB9 (I2C1) <-> PB10/PB11 (I2C2).
 * CubeMX only sets up I2C1, so i2c_platform_init() brings up I2C2's clock,
 * pins and NVIC and installs the IRQ vectors that dispatch to capi_i2c_isr;
 * the test calls I2C_PLATFORM_SET_TARGET() after init so those vectors reach
 * the target handle.
 */
#define I2C_HAS_LOOPBACK	1
#define I2C_IDENTIFIER		1U
#define I2C_TARGET_IDENTIFIER	2U
#define I2C_TIMING		0x20303E5D

/*
 * DMA2 is the only controller that supports memory-to-memory transfers on this
 * part, and it is also the one behind SPI1, so a single controller is shared:
 * stream 0 channel 0 (id 0) serves the memory-to-memory suite, ids 1 and 2
 * serve SPI (streams 2 and 3). No collision: the three use distinct streams.
 */
#define DMA_NUM_CHANS		3U
#define DMA_MEM2MEM_STREAM	DMA2_Stream0
#define DMA_MEM2MEM_CHANNEL	DMA_CHANNEL_0
#define DMA_PLATFORM_INIT()	__HAL_RCC_DMA2_CLK_ENABLE()

#elif defined(STM32F469xx)
#define BOARD_NAME		"SDP-K1"

extern UART_HandleTypeDef huart5;
extern SPI_HandleTypeDef hspi1;

/* Console: UART5 (PC12/PD2) is routed to the on-board USB virtual COM port. */
#define UART_IDENTIFIER		0U
#define UART_EXTRA_INIT		{ .huart = &huart5 }

#define GPIO_OUTPUT_IDENTIFIER	((uint64_t)(uintptr_t)GPIOA)
#define GPIO_OUTPUT_NUM_PINS	12U
#define GPIO_OUTPUT_NAME	"PA11"
#define GPIO_INPUT_IDENTIFIER	((uint64_t)(uintptr_t)GPIOG)
#define GPIO_INPUT_NUM_PINS	8U
#define GPIO_INPUT_NAME		"PG7"

#define GPIO_HAS_PORT_LOOPBACK	0
#define GPIO_HAS_PIN_LOOPBACK	1
#define GPIO_OUTPUT_PIN_NUMBERS	{ 11U }
#define GPIO_INPUT_PIN_NUMBERS	{ 7U }

/*
 * The input pin is PG7, so its external interrupt arrives on EXTI line 7.
 * Lines 5..9 share one vector, hence EXTI9_5 rather than a dedicated one.
 *
 * GPIOG is NOT clocked by CubeMX here - sdp-ck1z.ioc maps no GPIOG pin - so
 * the hook must enable it explicitly. Without that the pin's input path is
 * dead: HAL_GPIO_Init() still programs SYSCFG and EXTI, so the line looks
 * armed, but no edge ever reaches it and the callback simply never fires.
 */
#define GPIO_IRQ_PORT		GPIOG
#define GPIO_IRQ_PIN		GPIO_PIN_7
#define GPIO_IRQ_LINE		7U
#define GPIO_IRQ_IRQN		EXTI9_5_IRQn
#define GPIO_IRQ_CLK_ENABLE()	__HAL_RCC_GPIOG_CLK_ENABLE()
#define GPIO_IRQ_EXTI_HANDLER	EXTI9_5_IRQHandler

/*
 * SPI1 on the Arduino DIGI1 header (P6):
 *   PB3 = SCK (D13, P6.6), PB4 = MISO (D12, P6.5), PA7 = MOSI (D11, P6.4)
 *   External loopback requires D11 physically wired to D12. They are adjacent
 *   pins on P6, so a single 2-pin shunt is enough; SCK stays open.
 *
 * SPI1 sits on APB2, which the .ioc clocks at 90 MHz.
 */
#define SPI_IDENTIFIER		((uint64_t)(uintptr_t)SPI1)
#define SPI_CLK_FREQ		90000000U

#define SPI_HAS_IRQ		0
#define SPI_HAS_DMA		1

/*
 * SPI1 DMA request mapping on this part: RX is DMA2 stream 2 channel 3, TX is
 * DMA2 stream 3 channel 3. Both carry an irq_num because the SPI DMA path
 * reports completion from the DMA interrupt - a channel armed without one
 * blocks in HAL_DMA_PollForTransfer() waiting for a request the peripheral
 * only enables after that call has returned.
 *
 * The channel IDs index the shared DMA controller configured further down; id
 * 0 is left to the memory-to-memory suite so the two never collide.
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
 * dma_handle is patched in at runtime by spi_dma_platform_init(): it cannot be
 * a static initializer because the handle is allocated by capi_dma_init().
 *
 * dma_min_len = 1 defeats the driver's length heuristic so that every
 * DMA-expressible transfer in the suite - including the short ones the async
 * case uses - really does go down the DMA path. A product build would leave
 * this at 0 and let the driver's default keep short transfers on PIO.
 *
 * Note the consequence for test coverage: at 1, the SYNCHRONOUS
 * capi_spi_transceive() also routes through DMA (stm32_capi_spi.c calls
 * use_dma() on the sync path too), so BASIC/MODES/DATA/LSB_FIRST exercise the
 * DMA engine as well - not just ASYNC_DMA. That is why the SPI suite's
 * DMA_SETUP case runs before every other case and not just before the async
 * ones: without it those cases would quietly fall back to PIO and still pass.
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
				  .dma_min_len = 1U }

/*
 * SPI DMA platform hook, implemented in spi_dma_platform_init.c.
 *
 * Defining this macro is what pulls that file into the build and tells the
 * portable SPI suite that the controller needs a DMA bring-up step before
 * capi_spi_init(). Boards delivering SPI completion by IRQ leave it undefined
 * and the suite's default no-op takes over.
 */
int spi_dma_platform_init(void);
#define SPI_DMA_PLATFORM_INIT()	spi_dma_platform_init()

/*
 * TIM2: 32-bit general-purpose timer on APB1, same driver identifier and
 * auto-detected clock as the Nucleo mapping.
 *
 * sdp-ck1z.ioc: APB1Freq_Value=45 MHz, APB1CLKDivider=DIV4 => timer clock
 * 2x45 = 90 MHz. Keep in sync with the .ioc if the PLL is ever retuned.
 */
#define TIMER_IDENTIFIER	2U
#define TIMER_IRQ_NUM		TIM2_IRQn
#define TIMER_OUTPUT_FREQ_HZ	90000000U	/* = APB1 timer clock */

/*
 * No I2C loopback on this board. I2C1 is on the Arduino DIGI1 header
 * (PB7 = SDA = P6.9, PB8 = SCL = P6.10, both pulled up 2.2k on-board), but
 * I2C2's pins (PB10/PB11) are taken by the USB-HS ULPI PHY and the only other
 * free controller (I2C3) is reachable through the SDP-120 connector alone, so
 * an initiator/target pair cannot be strapped from the Arduino header. Leaving
 * I2C_OPS undefined makes the I2C suite report NOT_CONFIGURED instead.
 */
#define I2C_HAS_LOOPBACK	0

/*
 * DMA2 is the only controller that supports memory-to-memory transfers on this
 * part, and it is also the one behind SPI1, so a single controller is shared:
 * stream 0 channel 0 (id 0) serves the memory-to-memory suite, ids 1 and 2
 * serve SPI.
 */
#define DMA_NUM_CHANS		3U
#define DMA_MEM2MEM_STREAM	DMA2_Stream0
#define DMA_MEM2MEM_CHANNEL	DMA_CHANNEL_0
#define DMA_PLATFORM_INIT()	__HAL_RCC_DMA2_CLK_ENABLE()

#else
#error "capi_selftest: no board mapping for this STM32 target"
#endif

/* Board-independent mapping */

#define UART_OPS		&stm32_capi_uart_ops
#define UART_BAUDRATE		115200U
#define UART_EXTRA_TYPE		struct stm32_uart_extra_config

#define GPIO_OUTPUT_OPS		&stm32_capi_gpio_ops
#define GPIO_OUTPUT_EXTRA	struct stm32_capi_gpio_port_config
#define GPIO_OUTPUT_EXTRA_INIT	{ .mode = GPIO_MODE_OUTPUT_PP, \
				  .speed = GPIO_SPEED_FREQ_LOW, \
				  .alternate = 0U, \
				  .pull = GPIO_NOPULL }

#define GPIO_INPUT_OPS		&stm32_capi_gpio_ops
#define GPIO_INPUT_EXTRA	struct stm32_capi_gpio_port_config
#define GPIO_INPUT_EXTRA_INIT	{ .mode = GPIO_MODE_INPUT, \
				  .speed = GPIO_SPEED_FREQ_LOW, \
				  .alternate = 0U, \
				  .pull = GPIO_NOPULL }

/*
 * The STM32 CAPI GPIO backend has no toggle op, so the toggle subtests are
 * skipped on this platform.
 */
#define GPIO_HAS_TOGGLE		0

#define SPI_OPS			&stm32_capi_spi_ops
#define SPI_EXTRA_TYPE		struct stm32_spi_extra_config

#define SPI_DEVICE_NATIVE_CS	0x01U
#define SPI_DEVICE_MODE		CAPI_SPI_MODE_0
#define SPI_DEVICE_SPEED_HZ	1000000U

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

#define TIMER_OPS		&stm32_capi_timer_ops
#define TIMER_INPUT_CLK_HZ	0U		/* auto-detected from APB1 */
#define TIMER_EXTRA_TYPE	struct stm32_capi_timer_extra_config
#define TIMER_EXTRA_INIT	{ .htim = NULL, \
				  .get_input_clock = NULL, \
				  .irq_num = TIMER_IRQ_NUM }

#define TIMER_DIRECTION		CAPI_TIMER_COUNT_UP
/*
 * Counter wrap point. Although TIM2 is 32 bits wide, the rollover period must
 * sit between two test windows: wider than the BASIC rate window (10 ms) so a
 * rate sample never straddles more than one wrap, yet narrower than the
 * ASYNC_IRQ overflow timeout (1 s) so the counter-overflow interrupt actually
 * fires within it. A full 0xFFFFFFFF span rolls over only every ~48 s at
 * 90 MHz, so ASYNC_IRQ would never see an overflow.
 *
 * !! TIMER_COUNTER_MAX AND TIMER_RATE_COUNTER_MASK MUST BE EQUAL !!
 *
 * BASIC recovers its tick delta as (second - first) & mask. That identity is
 * only valid when the mask is exactly the counter's modulus, i.e. mask ==
 * max and max+1 is a power of two. If they disagree, the masked value is
 * meaningless the moment the counter wraps: with max=0x1FFFF (131072 ticks,
 * ~1.5 ms at 90 MHz) the 10 ms window wraps ~7 times and a mask of 0x7FFFFF
 * reported 8371048 ticks against an expected 900000 - not a rate error, an
 * arithmetic one.
 *
 * Both values scale with TIMER_OUTPUT_FREQ_HZ, which is now the APB1 timer
 * clock rather than 1 MHz. 0x7FFFFF gives ~93 ms at 90 MHz and ~87 ms at the
 * Nucleo's 96 MHz: ~9x the rate window and ~10x under the IRQ timeout, which
 * preserves the margins the original 1 MHz / 0x1FFFF pairing had.
 */
#define TIMER_COUNTER_MAX	0x7FFFFFU
#define TIMER_COMPARE_VALUE	0x8000U
#define TIMER_RATE_WINDOW_US	10000U
#define TIMER_RATE_COUNTER_MASK	0x7FFFFFU
#define TIMER_RATE_TOLERANCE_PCT 5U
#define TIMER_HAS_IRQ		1
#define TIMER_HAS_COMPARE	1

#if I2C_HAS_LOOPBACK
#define I2C_OPS			&stm32_capi_i2c_ops
#define I2C_EXTRA_TYPE		struct stm32_i2c_extra_config
#define I2C_EXTRA_INIT		{ .hi2c = NULL, .i2c_timing = I2C_TIMING }
#define I2C_TARGET_ADDR		0x42U
#define I2C_HAS_IRQ		0

#define I2C_TARGET_OPS		&stm32_capi_i2c_ops
#define I2C_TARGET_EXTRA_TYPE	struct stm32_i2c_extra_config
#define I2C_TARGET_EXTRA_INIT	{ .hi2c = NULL, .i2c_timing = I2C_TIMING }

struct capi_i2c_controller_handle;
int i2c_platform_init(void);
void i2c_platform_deinit(void);
void i2c_platform_set_target_handle(struct capi_i2c_controller_handle *handle);
#define I2C_PLATFORM_INIT()		i2c_platform_init()
#define I2C_PLATFORM_DEINIT()		i2c_platform_deinit()
#define I2C_PLATFORM_SET_TARGET(h)	i2c_platform_set_target_handle(h)
#endif /* I2C_HAS_LOOPBACK */

/*
 * Memory-to-memory DMA channel. Polling mode (irq_num left at 0): the driver
 * blocks in xfer_start until the transfer completes, so this suite needs no
 * interrupt infrastructure.
 */
#define DMA_OPS			&stm32_capi_dma_ops
#define DMA_IDENTIFIER		0U
#define DMA_XFER_EXTRA_TYPE	struct stm32_dma_chan_extra_config
#define DMA_XFER_EXTRA_INIT	{ .hdma = &(DMA_HandleTypeDef){ \
					.Instance = DMA_MEM2MEM_STREAM }, \
				  .ch_num = DMA_MEM2MEM_CHANNEL, \
				  .mem_increment = true, \
				  .per_increment = true, \
				  .mem_data_alignment = CAPI_DMA_DATA_ALIGN_BYTE, \
				  .per_data_alignment = CAPI_DMA_DATA_ALIGN_BYTE, \
				  .dma_mode = CAPI_DMA_NORMAL_MODE, \
				  .trig = NULL }
#define DMA_XFER_SIZE		64U

#endif /* __PARAMETERS_H__ */
