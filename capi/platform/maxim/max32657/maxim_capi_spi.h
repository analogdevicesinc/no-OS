/*******************************************************************************
 *   @file   maxim_capi_spi.h
 *   @brief  Header file for SPI functions
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CAPI_SPI_H_
#define MAXIM_CAPI_SPI_H_

#include "capi_spi.h"
#include "maxim_capi_gpio.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @enum max_capi_spi_device_role
 * @brief SPI device role selection
 */
enum max_capi_spi_device_role {
	/** Target mode */
	MAX_CAPI_SPI_DEVICE_ROLE_TARGET,
	/** Controller mode */
	MAX_CAPI_SPI_DEVICE_ROLE_CONTROLLER,
};

/**
 * @enum max_capi_spi_bus_width
 * @brief SPI bus width selection
 */
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

/**
 * @enum max_capi_spi_clock_phase
 * @brief SPI clock phase selection
 */
enum max_capi_spi_clock_phase {
	/** Clock is sampled on the leading edge of SCLK */
	MAX_CAPI_SPI_CLOCK_PHASE_0,
	/** Clock is sampled on the trailing edge of SCLK */
	MAX_CAPI_SPI_CLOCK_PHASE_1,
};

/**
 * @enum max_capi_spi_clock_polarity
 * @brief SPI clock polarity selection
 */
enum max_capi_spi_clock_polarity {
	/** Clock's leading edge is a rising edge */
	MAX_CAPI_SPI_CLOCK_POLARITY_0,
	/** Clock's leading edge is a falling edge */
	MAX_CAPI_SPI_CLOCK_POLARITY_1,
};

/**
 * @enum max_capi_spi_chip_select
 * @brief SPI chip selection
 */
enum max_capi_spi_chip_select {
	MAX_CAPI_SPI_CS0 = (1 << 0),
	MAX_CAPI_SPI_CS1 = (1 << 1),
	MAX_CAPI_SPI_CS2 = (1 << 2),
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

/**
 * @struct max_capi_spi_extra
 * @brief MAX32657 platform-specific extra coniguration
 */
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
	/** Whether to enable IRQ connection during init */
	bool use_irq;
};

extern const struct capi_spi_ops max_capi_spi_ops;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_SPI_H_ */
