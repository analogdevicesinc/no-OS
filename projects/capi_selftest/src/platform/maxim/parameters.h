/***************************************************************************//**
 * @file parameters.h
 * @brief Definitions specific to Maxim platform used by capi_selftest project.
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *******************************************************************************/

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include "capi_uart.h"
#include "capi_gpio.h"
#include "capi_dma.h"
#include "capi_irq.h"
#include "maxim_capi_uart.h"
#include "maxim_capi_gpio.h"
#include "maxim_capi_irq.h"
#include "maxim_capi_dma.h"

/*
 * Maxim target + board selection.
 *
 * TARGET_NUM is pulled from uart.h → mxc_device.h → max<n>.h (MSDK macro).
 * BOARD_* (BOARD_APARD / BOARD_EVKIT_V1 / etc.) comes from CMakeLists.txt
 * (mirrored from the CMake BOARD cache var).
 */

/* ===== UART console routing ===== */

#if (TARGET_NUM == 32655)

	/* MAX32655FTHR: console = UART0, bridged to the onboard DAPLink CDC
	 * (no external adapter needed). Loopback uses LPUART (UART3) below, so
	 * the two must differ. */
	#define UART_IDENTIFIER		0U  /* UART0, DAPLink virtual COM */
	#define UART_CLK_FREQ_HZ	0U  /* 0 => APB/peripheral clock (UART0 is not LPUART) */
	#define UART_BAUDRATE		115200U

#elif (TARGET_NUM == 32690)

	#if defined(BOARD_APARD)
		/* AD-APARD32690-SL: console = UART0, programmer route via P50/P55/JP12/JP13 */
		#define UART_IDENTIFIER		0U
		#define UART_CLK_FREQ_HZ	IBRO_FREQ  /* BSP leaves console on IBRO */
		#define UART_BAUDRATE		115200U
	#elif defined(BOARD_EVKIT_V1)
		/* MAX32690EVKIT: console = UART2A on P1.9/P1.10, FT231X via CN2 */
		#define UART_IDENTIFIER		2U
		#define UART_CLK_FREQ_HZ	IBRO_FREQ
		#define UART_BAUDRATE		115200U
	#elif defined(BOARD_FTHR)
		/* MAX32690FTHR: BSP CONSOLE_UART is UART0 (Boards/MAX32690/FTHR). */
		#define UART_IDENTIFIER		0U
		#define UART_CLK_FREQ_HZ	IBRO_FREQ
		#define UART_BAUDRATE		115200U
	#else
		#error "MAX32690: BOARD_* not specified (expected BOARD_APARD, BOARD_EVKIT_V1 or BOARD_FTHR)"
	#endif

#elif (TARGET_NUM == 32650)

	/*
	 * MAX32650FTHR and AD-SWIOT1L-SL. CONSOLE_UART from the MSDK BSP
	 * (Boards/MAX32650/FTHR_APPS_A/Include/board.h) is UART0.
	 * Console only -- no loopback strap characterised on either board.
	 */
	#define UART_IDENTIFIER		0U
	#define UART_CLK_FREQ_HZ	0U
	#define UART_BAUDRATE		115200U

#elif (TARGET_NUM == 32660)

	/* MAX32660FTHR: BSP CONSOLE_UART is UART1 (Boards/MAX32660/EvKit_V1). */
	#define UART_IDENTIFIER		1U
	#define UART_CLK_FREQ_HZ	0U
	#define UART_BAUDRATE		115200U

#elif (TARGET_NUM == 32665)

	/* MAX32666FTHR (die MAX32665): BSP CONSOLE_UART is UART0 (FTHR2). */
	#define UART_IDENTIFIER		0U
	#define UART_CLK_FREQ_HZ	0U
	#define UART_BAUDRATE		115200U

#elif (TARGET_NUM == 32670)

	/* MAX32670EVKIT: BSP CONSOLE_UART is UART0 (EvKit_V1). */
	#define UART_IDENTIFIER		0U
	#define UART_CLK_FREQ_HZ	0U
	#define UART_BAUDRATE		115200U

#elif (TARGET_NUM == 78000)

	/* MAX78000FTHR: BSP CONSOLE_UART is UART0 (FTHR_RevA). */
	#define UART_IDENTIFIER		0U
	#define UART_CLK_FREQ_HZ	0U
	#define UART_BAUDRATE		115200U

#else
	#error "Unsupported Maxim TARGET_NUM for capi_selftest"
#endif

#define UART_OPS			&max_capi_uart_ops
#define UART_EXTRA_TYPE			struct max_capi_uart_extra
#define UART_EXTRA_INIT			{ .vssel = MXC_GPIO_VSSEL_VDDIO }
#define PLATFORM_NAME			"MAXIM"

/* ===== UART async loopback routing ===== */

#if (TARGET_NUM == 32655)

	/*
	 * MAX32655FTHR: loopback on LPUART (same as console).
	 * J9 pins 14↔15 = P2.6↔P2.7 (LPUART RX/TX). Strap them together on the header.
	 * HW-proven: full suite PASS=84 FAIL=0 on 2026-08-26.
	 */
	#define UART_ASYNC_MAX_IDENTIFIER	3U
	#define UART_ASYNC_CLK_FREQ_HZ		IBRO_FREQ

#elif (TARGET_NUM == 32690)

	#if defined(BOARD_APARD)
		/*
		 * APARD: loopback on UART1A.
		 * P6 pins 7↔8 = P2.14↔P2.16 (UART1A RX/TX). Strap on Arduino P6.
		 * HW-proven: full async suite PASS on 2026-08-26.
		 */
		#define UART_ASYNC_MAX_IDENTIFIER	1U
		#define UART_ASYNC_CLK_FREQ_HZ		IBRO_FREQ
	#elif defined(BOARD_EVKIT_V1)
		/*
		 * EVKIT: loopback on LPUART0B (alt route to SWD connector J3).
		 * P3.0/P3.1. Requires JH6 strap to enable.
		 */
		#define UART_ASYNC_MAX_IDENTIFIER	0U  /* LPUART0 */
		#define UART_ASYNC_CLK_FREQ_HZ		IBRO_FREQ
	#endif

#elif (TARGET_NUM == 32660)

	/*
	 * MAX32660-EVSYS: console is UART1A on P0.10/P0.11, hard-wired to the
	 * MAX32625PICO debug adapter through 1k series resistors, so the
	 * loopback must use the only other instance -- UART0.
	 *
	 * UART0A pins: P0.4 = TX (JH4-3), P0.5 = RX (JH3-3). Strap those two.
	 *
	 * No clock argument: MAX32660 is one of the parts where
	 * MAX_CAPI_UART_CLOCK_ARG is 0, so MXC_UART_Init() takes no clock
	 * selector and this value is ignored. Left 0 rather than IBRO_FREQ to
	 * avoid implying a choice that is not made.
	 */
	#define UART_ASYNC_MAX_IDENTIFIER	0U
	#define UART_ASYNC_CLK_FREQ_HZ		0U

#elif (TARGET_NUM == 78000)

	/*
	 * MAX78000FTHR: loopback on LPUART = UART3, pins P2.6 (RX) / P2.7 (TX),
	 * ALT2 -- confirmed from MSDK gpio_cfg_uart3 in pins_ai85.c. Strap the
	 * two header holes together.
	 *
	 * IBRO, not APB: LPUART is in the low-power domain and its clock enum
	 * (MXC_UART_IBRO_CLK) is what the backend resolves IBRO_FREQ to. Console
	 * stays on UART0, so the two instances differ as required.
	 */
	#define UART_ASYNC_MAX_IDENTIFIER	3U
	#define UART_ASYNC_CLK_FREQ_HZ		IBRO_FREQ

#endif

/*
 * The console is mandatory -- without it the run cannot report anything.
 *
 * The async loopback instance is NOT: it needs a physical strap between two
 * UARTs, and that pairing is only known for the boards characterised above. A
 * board without one still builds and runs every suite that needs no strap; the
 * UART async group reports NOT_CONFIGURED rather than failing. Bringing up a
 * new board therefore means adding UART_ASYNC_MAX_IDENTIFIER (and the GPIO
 * strap below) once its header wiring is known -- until then the async
 * coverage genuinely is absent, not merely quiet.
 */
#if !defined(UART_IDENTIFIER)
#error "UART_IDENTIFIER (console) must be defined for this target/board"
#endif

#ifdef UART_ASYNC_MAX_IDENTIFIER
#if (UART_IDENTIFIER == UART_ASYNC_MAX_IDENTIFIER)
#error "Console and loopback UART must differ (self-test would silence the log)"
#endif
#endif

/*
 * common_data.c builds uart_async_config from UART_ASYNC_IDENTIFIER / _OPS /
 * _EXTRA_TYPE / _EXTRA_INIT, and test_uart.c compiles the whole suite out with
 * "NOT_CONFIGURED" unless UART_ASYNC_OPS is defined. The per-board blocks above
 * name the instance UART_ASYNC_MAX_IDENTIFIER ("MAX" = Maxim), so alias it onto
 * the contract name rather than duplicating the per-board selection.
 */
#ifdef UART_ASYNC_MAX_IDENTIFIER
#define UART_ASYNC_IDENTIFIER		UART_ASYNC_MAX_IDENTIFIER
#define UART_ASYNC_OPS			&max_capi_uart_ops
#define UART_ASYNC_EXTRA_TYPE		struct max_capi_uart_extra
/*
 * Same shape as the console UART_EXTRA_INIT: VDDIO pads, no DMA attached. The
 * async suite drives the UART through its interrupt path, not through DMA, so
 * .dma_config stays NULL here even on targets where a DMA backend exists.
 */
#define UART_ASYNC_EXTRA_INIT		{ .vssel = MXC_GPIO_VSSEL_VDDIO, \
					  .dma_config = NULL }
#endif /* UART_ASYNC_MAX_IDENTIFIER */

/* ===== GPIO loopback ===== */

#if (TARGET_NUM == 32655)

	/*
	 * MAX32655FTHR: port 1 is isolated (port 0 = console, port 2 = LPUART loopback).
	 * J7 pins 9↔10 = P1.8↔P1.9 (MXC_GPIO1). Strap them together on the header.
	 * HW-proven: GPIO PASS=84 (including loopback + IRQ) on 2026-08-26.
	 */
	#define GPIO_OUTPUT_IDENTIFIER		1U
	#define GPIO_OUTPUT_NUM_PINS		10U  /* P1.0..P1.9; strap is P1.9 */
	#define GPIO_OUTPUT_PIN			9U
	#define GPIO_OUTPUT_OPS			&max_capi_gpio_ops
	#define GPIO_OUTPUT_EXTRA		struct max_capi_gpio_extra_config
	#define GPIO_OUTPUT_EXTRA_INIT		{ .vssel = MXC_GPIO_VSSEL_VDDIO }

	#define GPIO_INPUT_IDENTIFIER		1U
	#define GPIO_INPUT_NUM_PINS		10U
	#define GPIO_INPUT_PIN			8U
	#define GPIO_INPUT_OPS			&max_capi_gpio_ops
	#define GPIO_INPUT_EXTRA		struct max_capi_gpio_extra_config
	#define GPIO_INPUT_EXTRA_INIT		{ .vssel = MXC_GPIO_VSSEL_VDDIO }

#elif (TARGET_NUM == 32690)

	#if defined(BOARD_APARD)
		/*
		 * APARD GPIO strap -- selectable, because which two pins are
		 * jumpered is a property of the bench, not of the board.
		 *
		 * Pick a strap with -DGPIO_STRAP=<n>, or override the pin/port
		 * macros individually from the build for a pairing not listed
		 * here. Every macro below is #ifndef-guarded, so a -D on the
		 * command line always wins.
		 *
		 *   GPIO_STRAP 7 (default) -- P7-1 <-> P7-2 = P2.7 <-> P2.8
		 *       Arduino "GPIO & CAN" header, nets GPIO_2 / GPIO_3.
		 *       Dedicated GPIO: the schematic shows no other load on
		 *       either net, and neither pin carries an alternate
		 *       function used on this board. Preferred.
		 *
		 *   GPIO_STRAP 5 -- P5-7 <-> P5-6 = P1.29 <-> P1.28
		 *       Arduino "SPI & I2C" header, nets SPI1A_MOSI /
		 *       SPI1A_MISO. Electrically fine -- each net appears
		 *       exactly twice in the schematic (MCU pin, header pin),
		 *       so nothing contends -- but the pins double as SPI1A,
		 *       so a future SPI test on this bus would collide.
		 *
		 * Both straps sit on a single port, so the two handles alias
		 * the same registers and the port-wide cases stay off (see the
		 * GPIO_HAS_PORT_LOOPBACK note below). Enabling those needs two
		 * pins on different ports; the only non-port-2 header pin
		 * found is P4-5 (P0.22), shared with the RISC-V TDO, so it is
		 * deliberately not offered.
		 */
		#ifndef GPIO_STRAP
		#define GPIO_STRAP			7
		#endif

		#if (GPIO_STRAP == 7)
			#define GPIO_STRAP_PORT		2U
			#define GPIO_STRAP_OUT_PIN	7U   /* P2.7, P7-1 */
			#define GPIO_STRAP_IN_PIN	8U   /* P2.8, P7-2 */
			#define GPIO_STRAP_NUM_PINS	11U  /* P2.0..P2.10 */
		#elif (GPIO_STRAP == 5)
			#define GPIO_STRAP_PORT		1U
			#define GPIO_STRAP_OUT_PIN	29U  /* P1.29, P5-7 */
			#define GPIO_STRAP_IN_PIN	28U  /* P1.28, P5-6 */
			#define GPIO_STRAP_NUM_PINS	31U  /* P1.0..P1.30 */
		#else
			#error "APARD: unknown GPIO_STRAP (expected 5 or 7); or override GPIO_{OUTPUT,INPUT}_{IDENTIFIER,PIN} directly"
		#endif

		#ifndef GPIO_OUTPUT_IDENTIFIER
		#define GPIO_OUTPUT_IDENTIFIER		GPIO_STRAP_PORT
		#endif
		#ifndef GPIO_OUTPUT_NUM_PINS
		#define GPIO_OUTPUT_NUM_PINS		GPIO_STRAP_NUM_PINS
		#endif
		#ifndef GPIO_OUTPUT_PIN
		#define GPIO_OUTPUT_PIN			GPIO_STRAP_OUT_PIN
		#endif
		#define GPIO_OUTPUT_OPS			&max_capi_gpio_ops
		#define GPIO_OUTPUT_EXTRA		struct max_capi_gpio_extra_config
		#define GPIO_OUTPUT_EXTRA_INIT		{ .vssel = MXC_GPIO_VSSEL_VDDIO }

		#ifndef GPIO_INPUT_IDENTIFIER
		#define GPIO_INPUT_IDENTIFIER		GPIO_STRAP_PORT
		#endif
		#ifndef GPIO_INPUT_NUM_PINS
		#define GPIO_INPUT_NUM_PINS		GPIO_STRAP_NUM_PINS
		#endif
		#ifndef GPIO_INPUT_PIN
		#define GPIO_INPUT_PIN			GPIO_STRAP_IN_PIN
		#endif
		#define GPIO_INPUT_OPS			&max_capi_gpio_ops
		#define GPIO_INPUT_EXTRA		struct max_capi_gpio_extra_config
		#define GPIO_INPUT_EXTRA_INIT		{ .vssel = MXC_GPIO_VSSEL_VDDIO }

		#if (GPIO_OUTPUT_PIN == GPIO_INPUT_PIN) && \
		    (GPIO_OUTPUT_IDENTIFIER == GPIO_INPUT_IDENTIFIER)
		#error "GPIO strap output and input are the same pin"
		#endif

	#elif defined(BOARD_EVKIT_V1)
		/*
		 * EVKIT: GPIO loopback not yet wired on this board.
		 * Leave undefined so GPIO test compiles out.
		 */
	#endif

#elif (TARGET_NUM == 32660)

	/*
	 * MAX32660-EVSYS: strap P0.6 <-> P0.7 = JH3-4 <-> JH3-5, adjacent holes
	 * on the same header.
	 *
	 * MAX32660 has a single GPIO port with 14 pins (P0.0..P0.13), so every
	 * choice is same-port and the port-wide cases stay off. Pins avoided
	 * and why: P0.0/P0.1 are SWDIO/SWDCLK (driving them would cut the
	 * debugger off mid-run), P0.10/P0.11 are the console, P0.4/P0.5 are the
	 * UART0 loopback above, P0.12 is the pushbutton and P0.13 the LED, and
	 * P0.2/P0.3 run to the PICO's P1_6/P1_7 on this board. P0.6/P0.7 are
	 * brought out to the header and to nothing else; their alternates
	 * (UART0 CTS/RTS, SPI0A, UART1C) are all unused here since no flow
	 * control is configured.
	 *
	 * The port-wide cases are the hazard here: pin_mask defaults to the
	 * contiguous window (1 << num_pins) - 1, which on a single-port part
	 * spans SWD and the console, so a port-wide direction or config write
	 * re-muxes both -- the debugger drops and the log goes silent while the
	 * run continues invisibly to the end. Pin the mask to the two strap pins
	 * so port-wide ops touch nothing else. Every MSDK me11 pin config uses
	 * VDDIO, so match it rather than VDDIOH.
	 */
	#define GPIO_STRAP_MASK			((1U << 6) | (1U << 7))
	#ifndef GPIO_OUTPUT_IDENTIFIER
	#define GPIO_OUTPUT_IDENTIFIER		0U
	#endif
	#ifndef GPIO_OUTPUT_NUM_PINS
	#define GPIO_OUTPUT_NUM_PINS		14U  /* P0.0..P0.13 */
	#endif
	#ifndef GPIO_OUTPUT_PIN
	#define GPIO_OUTPUT_PIN			6U   /* P0.6, JH3-4 */
	#endif
	#define GPIO_OUTPUT_OPS			&max_capi_gpio_ops
	#define GPIO_OUTPUT_EXTRA		struct max_capi_gpio_extra_config
	#define GPIO_OUTPUT_EXTRA_INIT		{ .vssel = MXC_GPIO_VSSEL_VDDIO, \
					  .pin_mask = GPIO_STRAP_MASK }

	#ifndef GPIO_INPUT_IDENTIFIER
	#define GPIO_INPUT_IDENTIFIER		0U
	#endif
	#ifndef GPIO_INPUT_NUM_PINS
	#define GPIO_INPUT_NUM_PINS		14U
	#endif
	#ifndef GPIO_INPUT_PIN
	#define GPIO_INPUT_PIN			7U   /* P0.7, JH3-5 */
	#endif
	#define GPIO_INPUT_OPS			&max_capi_gpio_ops
	#define GPIO_INPUT_EXTRA		struct max_capi_gpio_extra_config
	#define GPIO_INPUT_EXTRA_INIT		{ .vssel = MXC_GPIO_VSSEL_VDDIO, \
					  .pin_mask = GPIO_STRAP_MASK }

#elif (TARGET_NUM == 78000)

	/*
	 * MAX78000FTHR: strap P1.6 <-> P0.19 across the unpopulated header
	 * holes. Free on this BSP -- board.c claims only P0.2 and P1.7
	 * (buttons) and P2.0..P2.2 (LEDs), and the console is P0.0/P0.1.
	 *
	 * The two sides are on DIFFERENT ports, which would normally allow the
	 * port-wide cases. They stay off anyway: the input side is port 0,
	 * which also carries the console UART, and a port-wide test drives or
	 * re-muxes every pin of the port -- that would take P0.0/P0.1 away from
	 * UART0 mid-run and kill the log. Pin-level masks a single bit
	 * (1U << pin), so it leaves the console alone.
	 */
	#ifndef GPIO_OUTPUT_IDENTIFIER
	#define GPIO_OUTPUT_IDENTIFIER		1U
	#endif
	#ifndef GPIO_OUTPUT_NUM_PINS
	#define GPIO_OUTPUT_NUM_PINS		10U  /* P1.0..P1.9 */
	#endif
	#ifndef GPIO_OUTPUT_PIN
	#define GPIO_OUTPUT_PIN			6U   /* P1.6 */
	#endif
	#define GPIO_OUTPUT_OPS			&max_capi_gpio_ops
	#define GPIO_OUTPUT_EXTRA		struct max_capi_gpio_extra_config
	#define GPIO_OUTPUT_EXTRA_INIT		{ .vssel = MXC_GPIO_VSSEL_VDDIOH }

	#ifndef GPIO_INPUT_IDENTIFIER
	#define GPIO_INPUT_IDENTIFIER		0U
	#endif
	#ifndef GPIO_INPUT_NUM_PINS
	#define GPIO_INPUT_NUM_PINS		32U  /* P0.0..P0.31 */
	#endif
	#ifndef GPIO_INPUT_PIN
	#define GPIO_INPUT_PIN			19U  /* P0.19 */
	#endif
	#define GPIO_INPUT_OPS			&max_capi_gpio_ops
	#define GPIO_INPUT_EXTRA		struct max_capi_gpio_extra_config
	#define GPIO_INPUT_EXTRA_INIT		{ .vssel = MXC_GPIO_VSSEL_VDDIOH }

#endif

/* ===== GPIO loopback topology =====
 *
 * Every Maxim target here maps the GPIO output and input onto the SAME
 * physical port (a Cortex-M part exposes one GPIO instance per port, and only
 * one port is free of console/LPUART on these boards). The port-wide loopback
 * tests drive the whole output port and read the whole input port back, but on
 * a single shared port the two handles alias the same registers:
 * set_direction(in, all-input) flips the port that set_direction(out, ...) just
 * made outputs, so the drive never reaches a pad. So port-wide loopback is off;
 * the pin-level path drives one strapped pin and reads its wired partner, which
 * masks bits independently (1U << pin->number) and does not alias.
 *
 * This also fixes the IRQ edge: test_irq's IRQ_EDGE_OUT_BIT is
 * (1 << gpio_output_pin_numbers[0]) only when pin loopback is defined; without
 * it, it falls back to bit 0 and drives the wrong pin, so no edge reaches the
 * armed input.
 */
#ifdef GPIO_OUTPUT_PIN
	#define GPIO_HAS_PORT_LOOPBACK	0
	#define GPIO_HAS_PIN_LOOPBACK	1
	#define GPIO_OUTPUT_PIN_NUMBERS	{ GPIO_OUTPUT_PIN }
	#define GPIO_INPUT_PIN_NUMBERS	{ GPIO_INPUT_PIN }
#endif

/* ===== DMA ===== */

/*
 * test_dma.c gates on DMA_OPS and common_data.c builds dma_config from
 * DMA_IDENTIFIER / DMA_NUM_CHANS / DMA_OPS plus the per-transfer
 * DMA_XFER_EXTRA_TYPE / _INIT. (The previous DMA_CONFIG macro here matched no
 * consumer -- nothing in the tree referenced it -- so the DMA suite reported
 * "no runner provided" on every Maxim target.)
 *
 * maxim_capi_dma.c builds for every supported part, so this is not gated on
 * TARGET_NUM. MXC_DMA_CHANNELS comes from the part's max<n>.h and differs per
 * die (4 on MAX32655, 16 on MAX32690), so take it from the header rather than
 * hardcoding a count.
 */
#define DMA_OPS				&max_capi_dma_ops
#define DMA_IDENTIFIER			0U
#define DMA_NUM_CHANS			MXC_DMA_CHANNELS
#define DMA_XFER_EXTRA_TYPE		struct max_capi_dma_xfer_extra
/* Memory-to-memory: no peripheral handshake line. */
#define DMA_XFER_EXTRA_INIT		{ .reqsel = MXC_DMA_REQUEST_MEMTOMEM }
#define DMA_XFER_SIZE			64U

/* ===== Async capability gates ===== */

/*
 * Flag which async features are available on this target.
 * These are gated in capi_selftest/src/common_data.h.
 */
#if (TARGET_NUM == 32655) || (TARGET_NUM == 32690)
	#define UART_ASYNC_HAS_IRQ		1
	#define UART_ASYNC_HAS_LINE_CONFIG	1
	#define UART_ASYNC_HAS_IRQ_CTL		1
	#define UART_ASYNC_HAS_RX_TIMEOUT	0  /* Not yet supported */
#else
	#define UART_ASYNC_HAS_IRQ		0
	#define UART_ASYNC_HAS_LINE_CONFIG	0
	#define UART_ASYNC_HAS_IRQ_CTL		0
	#define UART_ASYNC_HAS_RX_TIMEOUT	0
#endif

/* ===== IRQ control ===== */

/*
 * NVIC (Cortex-M NVIC). One flat controller, id 0, no base address.
 * capi_irq_init() MUST run once in main.c before example_main().
 */
#define IRQ_CTRL_IDENTIFIER		0U

#endif /* __PARAMETERS_H__ */
