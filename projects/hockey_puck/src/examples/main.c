/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for the apard32690 project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include "common_data.h"
#include "no_os_init.h"

#if defined(HP_TCP_ECHO_SERVER)
#include "tcp_echo_server_example.h"
#elif defined(HP_BASIC_EXAMPLE)
#include "hello_world.h"
#elif defined(HP_AD7124_APP)
#include "ad7124_app.h"
#elif defined(HP_MAX6613_APP)
#include "max6613_app.h"
#elif defined(HP_IIO_EXAMPLE)
#include "iio_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
#if defined(HP_BASIC_EXAMPLE)
	return hello_world_main();
#elif defined(HP_AD7124_APP)
	return ad7124_app_main();
#elif defined(HP_MAX6613_APP)
	return max6613_app_main();
#elif defined(HP_TCP_ECHO_SERVER)
	return tcp_echo_server_example_main();
#elif defined(HP_IIO_EXAMPLE)
	return iio_example_main();
#endif

	return 0;
}
