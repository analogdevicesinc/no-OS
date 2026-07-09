/* SPDX-License-Identifier: BSD-3-Clause */

/**
 *   @file   maxim_gpio_spi.h
 *   @brief  Bit-banged (GPIO) SPI master for MAX78000, no_os_spi platform ops.
 *   @author Victor Pascu (victor.pascu@analog.com)
 */

#ifndef MAXIM_GPIO_SPI_H_
#define MAXIM_GPIO_SPI_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"

/**
 * @struct max_gpio_spi_init_param
 * @brief  Pin map and options for a bit-banged SPI bus, passed via
 *         no_os_spi_init_param.extra.
 *
 * All four signal pins live on a single GPIO port (the MAX78000 SPI-capable
 * pins broken out on the FTHR headers are all on port 0). SCLK/MOSI/MISO are the
 * shared bus; cs_pin is THIS device's chip-select, owned exclusively by the core
 * that instantiates the driver. To share one physical bus between two cores,
 * give each core its own descriptor with the same SCLK/MOSI/MISO and a distinct
 * cs_pin, and serialize transfers with an external lock (see @ref
 * max_gpio_spi_ops bus-ownership contract below).
 */
struct max_gpio_spi_init_param {
	/** GPIO port index for all pins (e.g. 0 for MXC_GPIO0). */
	uint8_t  port;
	/** Bit position (0..31) of the shared serial clock pin. */
	uint8_t  sclk_pin;
	/** Bit position of the shared master-out pin. */
	uint8_t  mosi_pin;
	/** Bit position of the shared master-in pin. */
	uint8_t  miso_pin;
	/** Bit position of this device's chip-select pin. */
	uint8_t  cs_pin;
	/**
	 * Drive/receive at VDDIOH (3.3 V) instead of the reset-default VDDIO
	 * (1.8 V). Required when the peripheral is powered from the 3.3 V rail,
	 * otherwise driven highs and sampled inputs use the wrong reference.
	 */
	bool     vddioh;
	/** Half-bit delay in NOP iterations; tunes the SCLK period. */
	uint32_t half_period_nops;
};

/**
 * @brief Bit-banged SPI platform ops (no_os_spi backend).
 *
 * Performance: pin registers and masks are resolved once at init and the bit
 * loop toggles the raw GPIO out_set/out_clr / reads `in` directly, so it runs at
 * the same speed as hand-written register bit-banging (no per-bit no_os_gpio
 * call overhead).
 *
 * Bus-ownership contract (single-driver discipline): CS is a persistent output
 * owned only by this descriptor. SCLK+MOSI are driven (push-pull) only for the
 * duration of a transfer and returned to high-Z inputs afterwards; MISO is
 * always an input. The transfer routine assumes the caller holds any cross-core
 * bus lock for the whole transfer -- it does not lock internally, keeping the
 * lock policy an application concern.
 *
 * Supported: SPI mode 3 (CPOL=1, CPHA=1), MSB-first. Other modes/orders are
 * rejected at init with -ENOTSUP.
 */
extern const struct no_os_spi_platform_ops max_gpio_spi_ops;

#endif /* MAXIM_GPIO_SPI_H_ */
