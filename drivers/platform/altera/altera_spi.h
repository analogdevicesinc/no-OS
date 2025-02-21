/***************************************************************************//**
 *   @file   altera/altera_spi.h
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#ifndef ALTERA_SPI_H_
#define ALTERA_SPI_H_

#include "no_os_spi.h"

/**
 * @enum spi_type
 * @brief Altera platform architecture types
 */
enum spi_type {
	/** Nios II architecture */
	NIOS_II_SPI
};

/**
 * @struct altera_spi_init_param
 * @brief Structure holding the initialization parameters for Altera platform
 * specific SPI parameters.
 */
struct altera_spi_init_param {
	/** Altera architecture type */
	enum spi_type	type;
	/** SPI base address */
	uint32_t	base_address;
};

/**
 * @struct altera_spi_desc
 * @brief Altera platform specific SPI descriptor
 */
struct altera_spi_desc {
	/** Altera architecture type */
	enum spi_type	type;
	/** SPI base address */
	uint32_t	base_address;
};

/**
 * @brief Altera specific SPI platform ops structure
 */
extern const struct no_os_spi_platform_ops altera_spi_ops;

#endif /* ALTERA_SPI_H_ */
