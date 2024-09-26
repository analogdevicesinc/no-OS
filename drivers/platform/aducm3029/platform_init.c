/***************************************************************************//**
 *   @file   platform_init.c
 *   @brief  ADuCM3029 platform initialization source.
 *   @author Andrei Drimbarean (Andrei.Drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#include <sys/platform.h>
#include "adi_initialize.h"
#include <drivers/pwr/adi_pwr.h>
#include "no_os_error.h"

/**
 * @brief Initialize the power controller and set the core and peripherals clock
 *        divider.
 * @return 0 in case of success or error code otherwise.
 */
int32_t platform_init(void)
{
	int32_t ret;

	ret = adi_pwr_Init();
	if(ret != 0)
		return -1;
	ret = adi_pwr_SetClockDivider(ADI_CLOCK_HCLK,1);
	if(ret != 0)
		return -1;
	ret = adi_pwr_SetClockDivider(ADI_CLOCK_PCLK,1);
	if(ret != 0)
		return ret;

	adi_initComponents();

	return 0;
}
