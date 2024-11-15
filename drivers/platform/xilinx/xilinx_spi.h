/*******************************************************************************
 *   @file   xilinx/xilinx_spi.h
 *   @brief  Header containing extra types used in the spi driver.
 *   @author scuciurean (sergiu.cuciurean@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef XILINX_SPI_H_
#define XILINX_SPI_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include "no_os_spi.h"

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
