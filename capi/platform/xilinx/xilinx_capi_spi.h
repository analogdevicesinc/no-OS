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

#include <stdbool.h>
#include <stdint.h>
#include <capi_spi.h>

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

#ifdef __cplusplus
}
#endif

#endif /* _XILINX_CAPI_SPI_H_ */
