/***************************************************************************//**
*   @file   sd.h
*   @brief  Header file for SD card interface over SPI.
*   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
* @copyright
*
* Copyright 2019(c) Analog Devices, Inc.
*
* SPDX-License-Identifier: BSD-3-Clause
********************************************************************************
*
* @section details Library description
* This interface supports byte read and write operations for SD cards that
* that meet the following conditions:
*   	- Version 2.00 or later
*   	- High capacity or extended capacity (SDHX or SDXC)
*   	- Supply voltage of 3.3V
*
*******************************************************************************/

#ifndef __SD_H__
#define __SD_H__

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "no_os_spi.h"

#define DATA_BLOCK_LEN			(512u)
#define MAX_RESPONSE_LEN		(18u)

#ifdef SD_DEBUG
#include <stdio.h>
#define DEBUG_MSG(X)			printf((X))
#else
#define DEBUG_MSG(X)
#endif //SD_DEBUG

/**
 * @struct sd_init_param
 * @brief Configuration structure sent in the function sd_init
 */
struct sd_init_param {
	/** Descriptor of an initialized SPI channel */
	struct no_os_spi_desc *spi_desc;
};

/**
 * @struct sd_desc
 * @brief Structure that stores data about the SD card configurations
 */
struct sd_desc {
	/** Descriptor of an initialized SPI channel */
	struct no_os_spi_desc	*spi_desc;
	/** Memory size of the SD card in bytes */
	uint64_t	memory_size;
	/** 1 if SD card is HC or XC, 0 otherwise */
	uint8_t		high_capacity;
	/** Buffer used for the driver implementation */
	uint8_t		buff[18];
};

/**
 * @struct cmd_desc
 * @brief Contains the elements needed to build a command
 */
struct cmd_desc {
	/** Command code */
	uint8_t		cmd;
	/** Argument for the command */
	uint32_t	arg;
	/** Response with the size response_len will be written here */
	uint8_t		response[MAX_RESPONSE_LEN];
	/** Expected length for the response */
	uint32_t	response_len;
};

int32_t sd_init(struct sd_desc **sd_desc,
		const struct sd_init_param *init_param);
int32_t sd_remove(struct sd_desc *desc);
int32_t sd_read(struct sd_desc *desc,
		uint8_t *data,
		uint64_t address,
		uint64_t len);
int32_t sd_write(struct sd_desc *desc,
		 uint8_t *data,
		 uint64_t address,
		 uint64_t len);

#endif /* __SD_H__ */

