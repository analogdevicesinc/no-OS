/*******************************************************************************
 *   @file   lattice/lattice_spi.h
 *   @brief  Header containing extra types used in the spi driver.
 *   @author Villyam (Vilmoscsab.Jozsa@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#ifndef LATTICE_SPI_H_
#define LATTICE_SPI_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum latt_spi_type
 * @brief Lattice platform architecture sections
 */
enum latt_spi_type {
	/** Programmable Logic */
	SPI_PL,
	/** Processing System */
	SPI_PS,
	/** SPI Engine */
	SPI_ENGINE
};

/**
 * @struct latt_spi_init_param
 * @brief Structure holding the initialization parameters for Lattice platform
 * specific SPI parameters.
 */
struct latt_spi_init_param {
	/** Lattice architecture */
	enum latt_spi_type	type;
	/** Lattice Specific SPI parameters */

	/** The width of Chip Select BUS */
	uint8_t slave_count;

	/** Input clock frequency in Hz */
	uint32_t input_clock;

	/** Base address of the SPI IP */
	uint32_t base_addr;
};

/**
 * @struct latt_spi_desc
 * @brief Lattice platform specific SPI descriptor
 */
struct latt_spi_desc {
	/** Lattice architecture */
	enum latt_spi_type	type;
	/** SPI instance */
	void				*instance;
};

/**
 * @brief SPI engine platform ops structure
 */
extern const struct no_os_spi_platform_ops spi_eng_platform_ops;

/**
 * @brief Lattice specific SPI platform ops structure
 */
extern const struct no_os_spi_platform_ops latt_spi_ops;

/**
 * @brief Lattice SPI PS register functions. No extra param needed
 */
extern const struct no_os_spi_platform_ops latt_spi_pl_ops;

#endif // LATTICE_SPI_H_
