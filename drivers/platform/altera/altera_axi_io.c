/***************************************************************************//**
 *   @file   altera/altera_axi_io.c
 *   @brief  Implementation of Altera AXI IO .
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <io.h>
#include "no_os_error.h"
#include "no_os_axi_io.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief AXI IO Altera specific read function.
 * @param base - Base address
 * @param offset - Address offset
 * @param data - variable where returned data is stored
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_axi_io_read(uint32_t base, uint32_t offset, uint32_t *data)
{
	*data = IORD_32DIRECT(base, offset);

	return 0;
}

/**
 * @brief AXI IO Altera specific write function.
 * @param base - Base address
 * @param offset - Address offset
 * @param data - data to be written
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_axi_io_write(uint32_t base, uint32_t offset, uint32_t data)
{
	IOWR_32DIRECT(base, offset, data);

	return 0;
}

