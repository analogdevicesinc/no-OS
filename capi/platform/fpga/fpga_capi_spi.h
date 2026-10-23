/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief CAPI SPI backend for the ADI SPI Engine (axi_spi_engine,
 *        spi_engine_execution, spi_engine_offload).
 *
 * Runs SPI transfers on an ADI SPI Engine using only CAPI services;
 * registers are accessed through CAPI_PLATFORM_IO_*. The cores are packaged
 * for both Xilinx and Intel fabric.
 *
 * Configuration (struct capi_spi_config):
 *   - ops   : &fpga_capi_spi_ops
 *   - extra : struct fpga_capi_spi_config * (required)
 *
 * The engine's neighbours in the block design are not owned here. They are
 * peer CAPI handles that the application initializes and passes in:
 *   - clkgen           : SCLK clock (capi_clk), enabled at init; NULL if the
 *                        SCLK is a running fixed clock
 *   - trigger          : offload trigger, a capi_timer PWM channel
 *   - offload_dma_chan : IRQ-driven DEV_TO_MEM capi_dma channel that captures
 *                        the offload SDI samples
 *
 * transceive runs through the command/SDO/SDI FIFOs and needs only the SCLK.
 * transceive_async uses offload mode and needs trigger and offload_dma_chan:
 * the transfer must fit the offload memories, and completion comes from the
 * DMA callback as CAPI_SPI_EVENT_XFR_DONE.
 */

#ifndef _FPGA_CAPI_SPI_H_
#define _FPGA_CAPI_SPI_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <capi_spi.h>
#include <capi_clk.h>
#include <capi_timer.h>
#include <capi_dma.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct fpga_capi_spi_config
 * @brief Platform-specific SPI Engine configuration, passed via config->extra.
 *
 * Only the SPI Engine base is a raw address; the clock, trigger and offload DMA
 * are composed as references to peer CAPI handles the application owns.
 */
struct fpga_capi_spi_config {
	/** SPI Engine AXI register-map base address. */
	uint32_t regmap_baseaddr;
	/**
	 * SCLK clock generator handle. The SPI Engine's SCLK domain MMCM powers up
	 * held in reset; init enables this clock so TRANSFER instructions can
	 * retire. NULL means the SCLK is already running (no bring-up needed).
	 */
	struct capi_clk_handle *clkgen;
	/**
	 * Offload trigger timer handle (PWM mode). In offload mode the engine
	 * executes its command program only when pulsed on its 'trigger' input;
	 * the async path enables channel trigger_chan per transfer and disables
	 * it on completion. The channel must already be initialized and
	 * configured in CAPI_TIMER_PWM_MODE, with a period longer than one
	 * program run plus the completion latency (each rising edge starts a
	 * run). NULL disables the async path.
	 */
	struct capi_timer_handle *trigger;
	/** Timer channel wired to the engine's trigger input. */
	uint32_t trigger_chan;
	/**
	 * Offload sample-capture S2MM DMA channel. The async path captures the SDI
	 * samples into the backend's sample buffer through this channel and takes
	 * completion from its callback, so the channel must be IRQ-driven (a
	 * polled channel would block in xfer_start before the trigger is on and
	 * time out). NULL disables the async (offload) path.
	 */
	struct capi_dma_chan *offload_dma_chan;
	/** SPI Engine module reference clock in Hz (drives the SCLK divider). */
	uint32_t ref_clk_hz;
	/** Word width in bits (8/16/24/32). Byte buffers are framed into words. */
	uint8_t data_width;
	/** Chip-select assert delay (0..3) for the ASSERT command (cs_delay). */
	uint8_t cs_delay;
	/** SDO idle line state (0 or 1) programmed into the engine CONFIG reg. */
	uint8_t sdo_idle_state;
};

/**
 * @brief SPI Engine operations table. Select via config->ops.
 */
extern const struct capi_spi_ops fpga_capi_spi_ops;

#ifdef __cplusplus
}
#endif

#endif /* _FPGA_CAPI_SPI_H_ */
