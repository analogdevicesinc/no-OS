/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for XILINX platform of eval-adxl367z project.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#include "platform_includes.h"
#include "common_data.h"

#ifdef IIO_EXAMPLE
#include "iio_example.h"
#endif

#ifdef DUMMY_EXAMPLE
#include "dummy_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution for XILINX platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret;

	/* Enable the instruction cache. */
	Xil_ICacheEnable();
	/* Enable the data cache. */
	Xil_DCacheEnable();

#ifdef IIO_EXAMPLE
	ret = iio_example_main();
	if (ret < 0)
		goto error;
#endif

#ifdef DUMMY_EXAMPLE
	ret = dummy_example_main();
	if (ret < 0)
		goto error;
#endif

	/* Disable the instruction cache. */
	Xil_DCacheDisable();
	/* Disable the data cache. */
	Xil_ICacheDisable();

#if (IIO_EXAMPLE+DUMMY_EXAMPLE != 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and re-build the project.
#endif

error:
	return 0;
}
