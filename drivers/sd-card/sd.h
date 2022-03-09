/***************************************************************************//**
*   @file   sd.h
*   @brief  Header file for SD card interface over SPI.
*   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
* @copyright
*
* Copyright 2019(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  - Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  - Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*  - Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*  - The use of this software may or may not infringe the patent rights
*    of one or more patent holders.  This license does not release you
*    from the requirement that you obtain separate licenses from these
*    patent holders to use this software.
*  - Use of the software either in source or binary form, must be run
*    on or directly connected to an Analog Devices Inc. component.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define DATA_BLOCK_LEN			(512u)
#define MAX_RESPONSE_LEN		(18u)

#ifdef SD_DEBUG
#include <stdio.h>
#define DEBUG_MSG(X)			printf((X))
#else
#define DEBUG_MSG(X)
#endif //SD_DEBUG

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

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

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

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

