/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx platform SPI driver for CAPI
 *
 * Backends (select via config.ops):
 *   capi_spi_xilinx_ps_ops  - XSpiPs  (Zynq PS SPI)
 *   capi_spi_xilinx_pl_ops  - XSpi    (AXI Quad SPI)
 *
 * Both backends operate in master mode only.
 */

#ifndef _XILINX_CAPI_SPI_H_
#define _XILINX_CAPI_SPI_H_

#include "xparameters.h"
#include <stdbool.h>
#include <stdint.h>
#include <capi_spi.h>

#ifdef XPAR_XSPIPS_NUM_INSTANCES
#include "xspips.h"
#include "xspips_hw.h"
#endif

#ifdef XPAR_XSPI_NUM_INSTANCES
#include "xspi.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Pass config->extra = NULL for polled mode. To use interrupts, pass
 * struct capi_spi_xilinx_config with use_irq=true and the desired irq_id.
 * IRQ ID 0 is valid for INTC input 0. XSpiPs clock freq comes from
 * config->clk_freq_hz; XSpi loopback comes from config->loopback.
 */

/**
 * @struct capi_spi_xilinx_config
 * @brief Optional Xilinx platform-specific SPI configuration.
 *
 * Passed via config->extra during init. use_irq decides whether the driver
 * connects irq_id through the CAPI IRQ singleton. IRQ ID 0 is valid for an
 * INTC input, so it is passed through unchanged.
 */
struct capi_spi_xilinx_config {
	/** Enable CAPI IRQ connection during init */
	bool use_irq;
	/** Normalized CAPI IRQ ID. Zero is valid for INTC input 0. */
	uint32_t irq_id;
};

/**
 * @struct capi_spi_xilinx_handle
 * @brief Shared Xilinx SPI controller handle.
 *
 * Base handle MUST be first for container_of pattern.
 * Used by both PS and PL backends.
 */
struct capi_spi_xilinx_handle {
	/** Base handle - MUST be first */
	struct capi_spi_controller_handle handle;
	/** Xilinx driver instance (XSpiPs* or XSpi*) */
	void *instance;
	/** Loopback mode enabled (PL only — used when rebuilding options per transfer) */
	bool loopback;
	/** User callback for async operations */
	capi_spi_callback_t callback;
	/** User callback argument */
	void *callback_arg;
	/** IRQ ID (only valid if use_irq is true) */
	uint32_t irq_id;
	/** True if IRQ was successfully connected during init */
	bool use_irq;
	/** Async transfer in progress flag */
	volatile bool async_in_progress;
	/** Temp TX buffer for read_command async */
	uint8_t *async_tx_temp;
	/** Temp RX buffer for read_command async */
	uint8_t *async_rx_temp;
	/** User's RX buffer pointer for async */
	uint8_t *async_user_rx_buf;
	/** Offset of user RX data in the temporary async RX buffer */
	uint16_t async_tx_size;
	/** Original RX size for read_command */
	uint16_t async_rx_size;
	/** Device pointer held during async GPIO CS transfer - for ISR/abort deassert */
	const struct capi_spi_device *async_device;
};

/**
 * @brief Xilinx SPI operations tables. Select one via config->ops.
 */
extern struct capi_spi_ops capi_spi_xilinx_ps_ops;  /**< XSpiPs (Zynq PS SPI) */
extern struct capi_spi_ops capi_spi_xilinx_pl_ops;  /**< XSpi (AXI Quad SPI) */

#ifdef __cplusplus
}
#endif

#endif /* _XILINX_CAPI_SPI_H_ */
