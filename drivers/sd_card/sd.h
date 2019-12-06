/***************************************************************************//**
*   @file   sd.h
*   @brief  Header file for SD card interface over SPI.
*   This interface supports byte read and write operations for SD cards that
*   that meet the following conditions:
*   	- Version 2.00 or later
*   	- High capacity or extended capacity (SDHX or SDXC)
*   	- Supply voltage of 3.3V
*   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
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
*******************************************************************************/
#ifndef __SD_H__
#define __SD_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "spi.h"
#include "error.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/


#define BIT_CCS					(1u<<30)
#define BIT_APPLICATION_CMD		(1u<<7)

#define CMD(x)					((x) + 64)
#define ACMD(x)					(CMD(x) | BIT_APPLICATION_CMD)

#define CMD0_RETRY_NUMBER		(5u)

#define R1_READY_STATE			(0x00u)
#define R1_IDLE_STATE			(0x01u)
#define R1_ILLEGAL_COMMAND		(0x04u)

#define R1_LEN					(1u)
#define R2_LEN					(2u)
#define R3_LEN					(5u)
#define R7_LEN					(5u)
#define CSD_LEN					(18u)
#define MAX_RESPONSE_LEN		(18u)
#define CRC_LEN					(2u)
#define CMD_LEN					(8u)

#define STUFF_ARG				(0x00000000u)
#define CMD8_ARG				(0x000001AAu)
#define ACMD41_ARG				(0x40000000u)

#define DATA_BLOCK_BITS			(9u)
#define DATA_BLOCK_LEN			(1 << DATA_BLOCK_BITS)
#define MASK_ADDR_IN_BLOCK		(DATA_BLOCK_LEN - 1u)
#define MASK_BLOCK_NUMBER		(~(uint64_t)MASK_ADDR_IN_BLOCK)

#define START_1_BLOCK_TOKEN		(0xFEu)
#define START_N_BLOCK_TOKEN		(0xFCu)
#define STOP_TRANSMISSION_TOKEN	(0xFDu)
#define MASK_RESPONSE_TOKEN		(0x0Eu)
#define MASK_ERROR_TOKEN		(0xF0u)

#ifdef SD_DEBUG
#include <stdio.h>
#define DEBUG_MSG(X) printf((X))
#else
#define DEBUG_MSG(X)
#endif //SD_DEBUG

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * struct sd_init_param - Configuration structure sent in the function sd_init
 * @spi_desc		- Descriptor of an initialized SPI channel
 */
struct sd_init_param{
	struct spi_desc *spi_desc;
};

/**
 * struct sd_desc - Structure that stores data about the SD card configurations
 * @spi_channel		- Descriptor of an initialized SPI channel
 * @memory_size		- Memory size of the SD card in bytes
 * @high_capacity	- 1 if SD card is HC or XC, 0 otherwise
 */
struct sd_desc{
	struct spi_desc *spi_desc;
	uint64_t	memory_size;
	uint8_t		high_capacity;
	uint8_t		buff[18];
};
/**
 * struct cmd_desc - Contains the elements needed to build a command
 * @cmd				- Command code
 * @arg				- Argument for the command
 * @response 		- Response with the size response_len will be written here
 * @response_len	- Expected length for the response
 */
struct cmd_desc{
	uint8_t		cmd;
	uint32_t	arg;
	uint8_t		response[MAX_RESPONSE_LEN];
	uint32_t	response_len;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

uint32_t sd_init(struct sd_desc **sd_desc, const struct sd_init_param *init_param);
uint32_t sd_remove(struct sd_desc *desc);
uint32_t sd_read(struct sd_desc *desc,
						uint8_t *data,
						uint64_t address,
						uint64_t len);
uint32_t sd_write(struct sd_desc *desc,
						uint8_t *data,
						uint64_t address,
						uint64_t len);

#endif /* __SD_H__ */

