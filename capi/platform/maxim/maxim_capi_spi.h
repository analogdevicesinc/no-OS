/*
 * Copyright (c) 2026 Analog Devices, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file maxim_capi_spi.h
 * @brief Common Maxim (MSDK) SPI backend, one source for all MSDK parts.
 *
 * The MSDK ships two SPI driver generations that differ only in the
 * MXC_SPI_Init() signature and one extra request field (txDummyValue):
 *
 *   - "v2"   (MAX32572 / MAX32690 / MAX78002):
 *            MXC_SPI_Init(spi, type, if_mode, numTargets, ts_pol, freq, pins)
 *   - "reva" (everything else, incl. MAX32655 / MAX32657):
 *            MXC_SPI_Init(spi, masterMode, quadModeUsed, numSlaves, ssPol, hz, pins)
 *
 * Everything else (mxc_spi_req_t layout, MXC_SPI_MasterTransaction[Async],
 * MXC_SPI_SetMode/SetFrequency, the completion callback type) is identical, so
 * the transfer engine is written once and the Init difference is isolated to a
 * single signature shim gated on MAX_CAPI_SPI_V2.
 */

#ifndef MAXIM_CAPI_SPI_H_
#define MAXIM_CAPI_SPI_H_

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include "capi_spi.h"
#include "maxim_capi_gpio.h"

/**
 * MAX_CAPI_SPI_V2 - selects the SPI v2 MXC_SPI_Init() signature family.
 * The v2 driver ships on MAX32572, MAX32690 and MAX78002; every other MSDK
 * part uses the older "reva" signature.
 */
#if (TARGET_NUM == 32572) || (TARGET_NUM == 32690) || (TARGET_NUM == 78002)
#define MAX_CAPI_SPI_V2 1
#else
#define MAX_CAPI_SPI_V2 0
#endif

/**
 * MAX_CAPI_SPI_HAS_PINS - whether MXC_SPI_Init() takes a trailing pins argument.
 * The v2 driver and the "reva-with-pins" driver (MAX32655 / MAX32657) both take
 * an mxc_spi_pins_t. The older me11-class parts (MAX32660) ship a pinless 6-arg
 * MXC_SPI_Init() that muxes its own pins internally and define no
 * mxc_spi_pins_t type at all.
 */
#if (TARGET_NUM == 32660)
#define MAX_CAPI_SPI_HAS_PINS 0
#else
#define MAX_CAPI_SPI_HAS_PINS 1
#endif

/**
 * @enum max_capi_spi_device_role
 * @brief SPI device role selection.
 */
enum max_capi_spi_device_role {
	/** Target (peripheral) mode. */
	MAX_CAPI_SPI_DEVICE_ROLE_TARGET,
	/** Controller (host) mode. */
	MAX_CAPI_SPI_DEVICE_ROLE_CONTROLLER,
};

/**
 * @enum max_capi_spi_bus_width
 * @brief SPI bus width selection.
 */
enum max_capi_spi_bus_width {
	/** 1 data line, half duplex. */
	MAX_CAPI_SPI_BUS_WIDTH_3WIRE,
	/** CITO/COTI, full duplex. */
	MAX_CAPI_SPI_BUS_WIDTH_STANDARD,
	/** 2 data lines, half duplex. */
	MAX_CAPI_SPI_BUS_WIDTH_DUAL,
	/** 4 data lines, half duplex. */
	MAX_CAPI_SPI_BUS_WIDTH_QUAD,
};

/**
 * @enum max_capi_spi_chip_select
 * @brief SPI chip-select selection bitmask.
 */
enum max_capi_spi_chip_select {
	MAX_CAPI_SPI_CS0 = (1 << 0),
	MAX_CAPI_SPI_CS1 = (1 << 1),
	MAX_CAPI_SPI_CS2 = (1 << 2),
};

/**
 * @struct max_capi_spi_extra
 * @brief Maxim platform-specific extra configuration for SPI.
 */
struct max_capi_spi_extra {
	/** Controller or target. */
	enum max_capi_spi_device_role device_role;
	/** Bus width. */
	enum max_capi_spi_bus_width bus_width;
	/** Number of targets (chip-select lines to claim). */
	uint32_t num_targets;
	/** Bitmask for chip-select active polarity. */
	uint8_t polarity_mask;
	/** Which chip-select pin(s) to enable (max_capi_spi_chip_select bitmask). */
	uint8_t chip_select;
	/** Voltage level of the SPI pads. */
	enum max_capi_gpio_vssel vssel;
	/** OPTIONAL - DMA config parameters (reserved; DMA path is a follow-up). */
	struct capi_dma_config *dma_config;
};

extern const struct capi_spi_ops max_capi_spi_ops;

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* MAXIM_CAPI_SPI_H_ */
