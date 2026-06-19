/***************************************************************************//**
*   @file   mdio_spi.h
*   @brief  Header file for MDIO implementation.
*   @author Johnson Ralph Perez (Johnsonralph.Perez@analog.com)
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

#ifndef __MDIO_SPI_H__
#define __MDIO_SPI_H__

#include <stdint.h>
#include "no_os_spi.h"
#include "no_os_mdio.h"

/** MDIO SPI Preamble value (all 1s) */
#define MDIO_SPI_PREAMBLE		UINT32_MAX
/** Data field value for read operations */
#define MDIO_SPI_DATA_FIELD_READ	0xFFFF
/** Half buffer size in bytes */
#define MDIO_SPI_BUFF_SIZE_HALF_UINT8	4
/** Full buffer size in bytes */
#define MDIO_SPI_BUFF_SIZE_UINT8	8

/**
 * @struct mdio_spi_init_param
 * @brief Initialization parameter for the MDIO SPI interface.
 */
struct mdio_spi_init_param {
	/** SPI initialization parameters for the MDIO bus */
	struct no_os_spi_init_param mdio;
};

extern struct no_os_mdio_ops mdio_spi_ops;

#endif /* __MDIO_SPI_H__ */
