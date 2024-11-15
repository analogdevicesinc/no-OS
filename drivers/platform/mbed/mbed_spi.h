/***************************************************************************//**
 *   @file   mbed_spi.h
 *   @brief  Header containing extra types required for SPI interface
********************************************************************************
 * Copyright (c) 2021-2022 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MBED_SPI_H_
#define MBED_SPI_H_

// Platform support needs to be C-compatible to work with other drivers
#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>
#include <stdbool.h>

/******************************************************************************/
/********************** Variables and User defined data types *****************/
/******************************************************************************/

/*
 * Note: The structure members are not strongly typed, as this file is included
 *       in application specific '.c' files. The mbed code structure does not
 *       allow inclusion of mbed driver files (e.g. mbed.h) into '.c' files.
 *       All the members are hence typecasted to mbed specific type during
 *       spi init and read/write operations.
 **/

/**
 * @struct mbed_spi_init_param
 * @brief Structure holding the SPI init parameters for mbed platform.
 */
struct mbed_spi_init_param {
	uint16_t spi_miso_pin;		// SPI MISO pin (PinName)
	uint16_t spi_mosi_pin;  	// SPI MOSI pin (PinName)
	uint16_t spi_clk_pin;  		// SPI CLK pin (PinName)
	bool use_sw_csb;			// Software/Hardware control of CSB pin
};

/**
 * @struct mbed_spi_desc
 * @brief SPI specific descriptor for the mbed platform.
 */
struct mbed_spi_desc {
	void *spi_port; 			// SPI port instance (mbed::SPI)
	void *csb_gpio;  			// SPI chip select gpio instance (DigitalOut)
	bool use_sw_csb; 			// Software/Hardware control of CSB pin
};

/**
* @brief Mbed platform specific SPI platform ops structure
*/
extern const struct no_os_spi_platform_ops mbed_spi_ops;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

#ifdef __cplusplus // Closing extern c
}
#endif

#endif /* MBED_SPI_H_ */
