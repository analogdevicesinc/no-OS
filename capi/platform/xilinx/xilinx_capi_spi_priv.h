/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx platform SPI private driver contract.
 */

#ifndef _XILINX_CAPI_SPI_PRIV_H_
#define _XILINX_CAPI_SPI_PRIV_H_

#include <xilinx_capi_spi.h>

#include "xparameters.h"
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

/**
 * @struct capi_spi_xilinx_async_transfer
 * @brief In-flight async state retained between the API call and the ISR.
 */
struct capi_spi_xilinx_async_transfer {
	/** Heap-allocated TX buffer kept alive until the ISR fires */
	uint8_t *tx_buffer;
	/** Heap-allocated RX buffer kept alive until the ISR fires */
	uint8_t *rx_buffer;
	/** Byte offset into rx_buffer where real data starts */
	uint16_t rx_offset;
	/** Device pointer held for GPIO CS deassert in the ISR or on abort */
	const struct capi_spi_device *gpio_cs_device;
	/** Copy of the caller's transfer descriptor for ISR-side completion */
	struct capi_spi_transfer transfer;
};

/**
 * @struct capi_spi_xilinx_handle
 * @brief Shared Xilinx SPI private controller state.
 */
struct capi_spi_xilinx_handle {
	/** Xilinx driver instance (XSpiPs* or XSpi*) */
	void *instance;
	/** Loopback mode enabled (PL only; used when rebuilding options per transfer) */
	bool loopback;
	/** User callback for async operations */
	capi_spi_callback_t callback;
	/** User callback argument */
	void *callback_arg;
	/** IRQ ID (only valid if use_irq is true) */
	uint32_t irq_id;
	/** True if IRQ was successfully connected during init */
	bool use_irq;
	/** True while an async transfer is in flight */
	volatile bool async_in_progress;
	/** Resources held between the async API call and ISR completion */
	struct capi_spi_xilinx_async_transfer async;
};

/**
 * @brief Declare a stack-allocated Xilinx SPI controller handle.
 *
 * Declares `name` (struct capi_spi_controller_handle) with embedded
 * private state. Pass &name to capi_spi_init().
 */
#define CAPI_SPI_HANDLE_XILINX_DEFINE(name)                            \
	struct capi_spi_controller_handle name = {                         \
		.ops = NULL,                                                   \
		.init_allocated = false,                                       \
		.priv = &(struct capi_spi_xilinx_handle){0}                    \
	}

/**
 * @brief Xilinx SPI operations tables. Select one via config->ops.
 */
extern struct capi_spi_ops capi_spi_xilinx_ps_ops;
extern struct capi_spi_ops capi_spi_xilinx_pl_ops;

#ifdef __cplusplus
}
#endif

#endif /* _XILINX_CAPI_SPI_PRIV_H_ */
