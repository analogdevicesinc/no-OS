/*******************************************************************************
 *   @file   maxim_capi_spi.h
 *   @brief  Header file for SPI functions
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef MAXIM_CAPI_SPI_H_
#define MAXIM_CAPI_SPI_H_

#include "capi_spi.h"
#include "maxim_capi_gpio.h"

#define MAX_DELAY_SCLK 255

enum max_capi_spi_device_role {
	MAX_CAPI_SPI_DEVICE_ROLE_TARGET,
	MAX_CAPI_SPI_DEVICE_ROLE_CONTROLLER,
};

enum max_capi_spi_bus_width {
	/** 1 Data line, half duplex */
	MAX_CAPI_SPI_BUS_WIDTH_3WIRE,
	/** CITO/COTI, full duplex */
	MAX_CAPI_SPI_BUS_WIDTH_STANDARD,
	/** 2 data lines, half duplex */
	MAX_CAPI_SPI_BUS_WIDTH_DUAL,
	/** 4 data lines, half duplex */
	MAX_CAPI_SPI_BUS_WIDTH_QUAD,
};

enum max_capi_spi_clock_phase {
	MAX_CAPI_SPI_CLOCK_PHASE_0,
	MAX_CAPI_SPI_CLOCK_PHASE_1,
};

enum max_capi_spi_clock_polarity {
	MAX_CAPI_SPI_CLOCK_POLARITY_0,
	MAX_CAPI_SPI_CLOCK_POLARITY_1,
};

enum max_capi_spi_chip_select {
	MAX_CAPI_SPI_CS0 = (1 << 0),
	MAX_CAPI_SPI_CS1 = (1 << 1),
	MAX_CAPI_SPI_CS2 = (1 << 2),
};

struct max_capi_spi_fifo_async {
	/* TX buffer */
	const uint8_t *tx_buf;
	/* RX buffer */
	uint8_t *rx_buf;
	/* TX bytes to send */
	uint32_t tx_size;
	/* RX bytes to receive */
	uint32_t rx_size;
	/* max(tx_effective, rx_effective) */
	uint32_t clk_len;
	/* chars queued to TX FIFO so far */
	uint32_t tx_cnt;
	/* chars drained from RX FIFO so far */
	uint32_t rx_cnt;
	/* RX bytes to discard before rx_buf */
	uint32_t rx_skip;
	/* FIFO async in progress */
	bool active;
};

/**
 * @struct max_spi_delays
 * @brief Delays in nanoseconds
 */
struct max_capi_spi_delays {
	/** Delay between the CS assert and first SCLK edge */
	uint32_t cs_delay_first;
	/** Delay between the last SCLK edge and the CS deassert */
	uint32_t cs_delay_last;
};

struct max_capi_spi_extra {
	/** Controller or Peripheral */
	enum max_capi_spi_device_role device_role;
	/** Bus width */
	enum max_capi_spi_bus_width bus_width;
	/** Number of targets */
	uint32_t num_targets;
	/** Bitmask for chip select pins polarity */
	uint8_t polarity_mask;
	/** Choose which chip select(s) gets enabled */
	uint8_t chip_select;
	/** Voltage level of the SPI peripheral */
	enum max_capi_gpio_vssel vssel;
	/** Clock phase; used for setting SPI mode */
	enum max_capi_spi_clock_phase clock_phase;
	/** Clock polarity; used for setting SPI mode */
	enum max_capi_spi_clock_polarity clock_polarity;
	/** SPI delays */
	struct max_capi_spi_delays platform_delays;
	/** OPTIONAL - DMA config parameters */
	struct capi_dma_config *dma_config;
};

struct max_capi_spi_priv {
	/** SPI controller handle */
	struct capi_spi_controller_handle *spi_handle;
	/** SPI device ID; only 0 is valid for MAX32657 */
	uint32_t identifier;
	/** DMA controller handle */
	struct capi_dma_handle *dma_handle;
	/** For storing the platform-specific config */
	struct max_capi_spi_extra extra;
	/** Clock frequency */
	uint32_t clock_freq;
	/** DMA callback function */
	capi_spi_callback_t callback;
	/** DMA callback arg */
	void *callback_arg;
	/** FIFO async */
	struct max_capi_spi_fifo_async fifo_async;
};

extern const struct capi_spi_ops max_capi_spi_ops;

#endif /* MAXIM_CAPI_SPI_H_ */
