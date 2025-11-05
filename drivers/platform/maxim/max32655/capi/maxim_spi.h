/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file maxim_spi.h
 * @brief MAX32655 SPI CAPI implementation header
 * @author Claude Code (noreply@anthropic.com)
 */

#ifndef MAXIM_CAPI_SPI_H
#define MAXIM_CAPI_SPI_H

#include "capi_spi.h"
#include "capi_pinctrl.h"
#include "spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MAX32655 SPI CAPI private handle structure
 */
struct maxim_capi_spi_handle {
	/** SPI instance identifier (0-2) */
	uint32_t instance;
	/** SPI registers pointer */
	mxc_spi_regs_t *regs;
	/** SPI request structure */
	mxc_spi_req_t req;
	/** Clock frequency in Hz */
	uint32_t clk_freq_hz;
	/** Three-pin mode enabled */
	bool three_pin_mode;
	/** Loopback mode enabled */
	bool loopback;
	/** Callback function */
	capi_spi_callback_t callback;
	/** Callback argument */
	void *callback_arg;
	/** Current transfer in progress */
	bool transfer_active;
	/** Pinctrl handle for SPI pins */
	struct capi_pinctrl_handle *pinctrl_handle;
};

/**
 * @brief SPI pin configuration structure
 *
 * This structure can be passed via the 'extra' field of capi_spi_device
 * to automatically configure SPI pins during device operations.
 */
struct maxim_spi_pinctrl_config {
	/** Array of pin descriptors for SPI pins */
	const struct capi_pinctrl_descriptor *pins;
	/** Number of pins in the array */
	uint32_t num_pins;
	/** Pinctrl configuration */
	struct capi_pinctrl_config pinctrl_config;
};

/**
 * @brief Static allocation macro for MAX32655 SPI CAPI handle
 *
 * Usage:
 *   struct capi_spi_controller_handle *handle = CAPI_SPI_HANDLE;
 */
#define CAPI_SPI_HANDLE &(struct capi_spi_controller_handle){ \
	.priv = &(struct maxim_capi_spi_handle){} \
}

/**
 * @brief Create SPI pin configuration for standard SPI pins
 *
 * @param port GPIO port number (0-3)
 * @param clk_pin Clock pin number
 * @param mosi_pin MOSI pin number
 * @param miso_pin MISO pin number
 * @param cs_pin Chip select pin number
 * @param func Function selection (MAXIM_PIN_FUNC_ALTx)
 */
#define MAXIM_SPI_PIN_CONFIG(port, clk_pin, mosi_pin, miso_pin, cs_pin, func) \
	{ \
		MAXIM_PIN_DESCRIPTOR(port, clk_pin, func, MAXIM_PIN_PAD_NONE, \
				     MAXIM_PIN_VSSEL_VDDIO, MAXIM_PIN_DRVSTR_0), \
		MAXIM_PIN_DESCRIPTOR(port, mosi_pin, func, MAXIM_PIN_PAD_NONE, \
				     MAXIM_PIN_VSSEL_VDDIO, MAXIM_PIN_DRVSTR_0), \
		MAXIM_PIN_DESCRIPTOR(port, miso_pin, func, MAXIM_PIN_PAD_NONE, \
				     MAXIM_PIN_VSSEL_VDDIO, MAXIM_PIN_DRVSTR_0), \
		MAXIM_PIN_DESCRIPTOR(port, cs_pin, func, MAXIM_PIN_PAD_NONE, \
				     MAXIM_PIN_VSSEL_VDDIO, MAXIM_PIN_DRVSTR_0) \
	}

/**
 * @brief Create SPI pin configuration for 3-wire SPI (no MISO)
 *
 * @param port GPIO port number (0-3)
 * @param clk_pin Clock pin number
 * @param mosi_pin MOSI pin number
 * @param cs_pin Chip select pin number
 * @param func Function selection (MAXIM_PIN_FUNC_ALTx)
 */
#define MAXIM_SPI_3WIRE_PIN_CONFIG(port, clk_pin, mosi_pin, cs_pin, func) \
	{ \
		MAXIM_PIN_DESCRIPTOR(port, clk_pin, func, MAXIM_PIN_PAD_NONE, \
				     MAXIM_PIN_VSSEL_VDDIO, MAXIM_PIN_DRVSTR_0), \
		MAXIM_PIN_DESCRIPTOR(port, mosi_pin, func, MAXIM_PIN_PAD_NONE, \
				     MAXIM_PIN_VSSEL_VDDIO, MAXIM_PIN_DRVSTR_0), \
		MAXIM_PIN_DESCRIPTOR(port, cs_pin, func, MAXIM_PIN_PAD_NONE, \
				     MAXIM_PIN_VSSEL_VDDIO, MAXIM_PIN_DRVSTR_0) \
	}

/**
 * @brief MAX32655 SPI CAPI operations structure
 */
extern const struct capi_spi_ops maxim_capi_spi_ops;

#ifdef __cplusplus
}
#endif

#endif /* MAXIM_CAPI_SPI_H */