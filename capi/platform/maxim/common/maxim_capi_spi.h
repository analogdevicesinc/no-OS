/*******************************************************************************
 *   @file   maxim_capi_spi.h
 *   @brief  Header file for the MAX32650/MAX32690 CAPI SPI functions
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_SPI_H_
#define MAXIM_CAPI_SPI_H_

#include <stdint.h>
#include "capi_spi.h"
#include "maxim_capi_gpio.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @struct max_capi_spi_extra
 * @brief Platform specific SPI controller configuration (config->extra)
 */
struct max_capi_spi_extra {
	/** Voltage level of the SPI pins */
	enum max_capi_gpio_vssel vssel;
	/** Bitmask of the hardware target select pins to route, 0 for SS0 */
	uint8_t chip_select;
	/** Bitmask of the active high target selects */
	uint8_t polarity_mask;
	/** CAPI DMA channel IDs, used when config->dma_handle is set.
	 *  Both 0 selects channels 0 (RX) and 1 (TX). */
	uint32_t rx_dma_ch_id;
	uint32_t tx_dma_ch_id;
	/** Shortest synchronous transfer that uses DMA, 0 for the default.
	 *  Shorter ones use the register-level PIO loop. */
	uint32_t dma_min_len;
};

/**
 * @struct max_capi_spi_device_extra
 * @brief Platform specific SPI device configuration (device->extra)
 */
struct max_capi_spi_device_extra {
	/** Delay between the CS assert and the first SCLK edge, in ns */
	uint32_t cs_delay_first;
	/** Delay between the last SCLK edge and the CS deassert, in ns */
	uint32_t cs_delay_last;
};

extern const struct capi_spi_ops max_capi_spi_ops;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_SPI_H_ */
