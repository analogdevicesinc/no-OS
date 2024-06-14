/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for Maxim platform of ad9545-sdz project.
 *   @author Marcelo Schmitt (marcelo.schmitt@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "no_os_delay.h"
#include "no_os_uart.h"
#include "no_os_error.h"
#include "common_data.h"
#include "platform_includes.h"

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution for Maxim platform.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
	int ret;

#ifdef BASIC_EXAMPLE
	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &ad9545_uart_ip);
	if (NO_OS_IS_ERR_VALUE(ret))
		return ret;

	no_os_uart_stdio(uart_desc);
	ret = basic_example_main();
	no_os_uart_remove(uart_desc);
#endif

#if (BASIC_EXAMPLE == 0)
#error At least one example has to be selected using y value in Makefile.
#elif (BASIC_EXAMPLE > 1)
#error Selected example projects cannot be enabled at the same time. \
Please enable only one example and re-build the project.
#endif

	return ret;
}
