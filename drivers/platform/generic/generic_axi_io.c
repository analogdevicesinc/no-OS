/***************************************************************************//**
 *   @file   generic/generic_axi_io.c
 *   @brief  Implementation of generic AXI IO
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include "no_os_error.h"
#include "no_os_util.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief AXI IO generic read function.
 * @param base - Base address
 * @param offset - Address offset
 * @param data - variable where returned data is stored
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_axi_io_read(uint32_t base, uint32_t offset, uint32_t *data)
{
	NO_OS_UNUSED_PARAM(base);
	NO_OS_UNUSED_PARAM(offset);
	NO_OS_UNUSED_PARAM(data);

	return 0;
}

/**
 * @brief AXI IO generic write function.
 * @param base - Base address
 * @param offset - Address offset
 * @param data - data to be written.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_axi_io_write(uint32_t base, uint32_t offset, uint32_t data)
{
	NO_OS_UNUSED_PARAM(base);
	NO_OS_UNUSED_PARAM(offset);
	NO_OS_UNUSED_PARAM(data);

	return 0;
}
