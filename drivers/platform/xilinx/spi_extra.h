/*******************************************************************************
 *   @file   xilinx/spi_extra.h
 *   @brief  Header containing extra types used in the spi driver.
 *   @author scuciurean (sergiu.cuciurean@analog.com)
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
#ifndef SPI_EXTRA_H_
#define SPI_EXTRA_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include "spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define SPI_CS_DECODE			0x01
#define SPI_DEASSERT_CURRENT_SS	0x0F

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

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
} xil_spi_type;

/**
 * @struct xil_spi_init_param
 * @brief Structure holding the initialization parameters for Xilinx platform
 * specific SPI parameters when using xil_platform_ops.
 */
typedef struct xil_spi_init_param {
	/** Xilinx architecture */
	enum xil_spi_type	type;
	/** SPI flags */
	uint32_t		flags;
} xil_spi_init_param;

/**
 * @struct xil_spi_desc
 * @brief Xilinx platform specific SPI descriptor
 */
typedef struct xil_spi_desc {
	/** Xilinx architecture */
	enum xil_spi_type	type;
	/** SPI flags */
	uint32_t		flags;
	/** SPI configuration */
	void			*config;
	/** SPI instance */
	void			*instance;
} xil_spi_desc;

/**
 * @brief SPI engine platform ops structure
 */
extern const struct spi_platform_ops spi_eng_platform_ops;

/**
 * @brief Xilinx specific SPI platform ops structure
 */
extern const struct spi_platform_ops xil_platform_ops;

/**
 * @brief Xilinx SPI PS register functions. No extra param needed
 */
extern const struct spi_platform_ops xil_spi_reg_ops_pl;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initialize the SPI communication peripheral. */
int32_t xil_spi_init(struct spi_desc **desc,
		     const struct spi_init_param *param);

/* Free the resources allocated by spi_init(). */
int32_t xil_spi_remove(struct spi_desc *desc);

/* Write and read data to/from SPI. */
int32_t xil_spi_write_and_read(struct spi_desc *desc, uint8_t *data,
			       uint16_t bytes_number);

#endif // SPI_EXTRA_H_
