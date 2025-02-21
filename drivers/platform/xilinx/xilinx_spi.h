/*******************************************************************************
 *   @file   xilinx/xilinx_spi.h
 *   @brief  Header containing extra types used in the spi driver.
 *   @author scuciurean (sergiu.cuciurean@analog.com)
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
#ifndef XILINX_SPI_H_
#define XILINX_SPI_H_

#include <stdint.h>
#include "no_os_spi.h"

#define SPI_CS_DECODE			0x01
#define SPI_DEASSERT_CURRENT_SS	0x0F

/**
 * @enum xil_spi_type
 * @brief Xilinx platform architecture sections
 */
enum xil_spi_type {
	/** Programmable Logic */
	SPI_PL,
	/** Processing System */
	SPI_PS,
	/** SPI Engine */
	SPI_ENGINE
};

/**
 * @struct xil_spi_init_param
 * @brief Structure holding the initialization parameters for Xilinx platform
 * specific SPI parameters when using xil_spi_ops.
 */
struct xil_spi_init_param {
	/** Xilinx architecture */
	enum xil_spi_type	type;
	/** SPI flags */
	uint32_t		flags;
};

/**
 * @struct xil_spi_desc
 * @brief Xilinx platform specific SPI descriptor
 */
struct xil_spi_desc {
	/** Xilinx architecture */
	enum xil_spi_type	type;
	/** SPI flags */
	uint32_t		flags;
	/** SPI configuration */
	void			*config;
	/** SPI instance */
	void			*instance;
};

/**
 * @brief SPI engine platform ops structure
 */
extern const struct no_os_spi_platform_ops spi_eng_platform_ops;

/**
 * @brief Xilinx specific SPI platform ops structure
 */
extern const struct no_os_spi_platform_ops xil_spi_ops;

/**
 * @brief Xilinx SPI PS register functions. No extra param needed
 */
extern const struct no_os_spi_platform_ops xil_spi_pl_ops;

#endif // XILINX_SPI_H_
