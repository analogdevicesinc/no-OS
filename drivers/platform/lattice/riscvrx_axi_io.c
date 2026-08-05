/***************************************************************************//**
 *   @file   riscvrx_axi_io.c
 *   @brief  Implementation of AXI IO for Lattice RISC-V RX.
 *   @author Vilmos-Csaba Jozsa (vilmoscsaba.jozsa@analog.com)
********************************************************************************
 * Copyright 2024-2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "reg_access.h"
#include "no_os_error.h"
#include "no_os_axi_io.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief AXI IO Lattice specific read function.
 * @param base - Base address
 * @param offset - Address offset
 * @param data - variable where returned data is stored
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_axi_io_read(uint32_t base, uint32_t offset, uint32_t *data)
{
	reg_32b_read(base + offset, data);
	return 0;
}

/**
 * @brief AXI IO Lattice specific write function.
 * @param base - Base address
 * @param offset - Address offset
 * @param data - data to be written.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_axi_io_write(uint32_t base, uint32_t offset, uint32_t data)
{
	reg_32b_write(base + offset, data);
	return 0;
}
