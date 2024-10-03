/***************************************************************************//**
 *   @file   generic/generic_axi_io.c
 *   @brief  Implementation of generic AXI IO
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
