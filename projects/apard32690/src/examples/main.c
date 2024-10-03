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

#if defined(APARD32690_ECHO_SERVER_EXAMPLE)
#include "tcp_echo_server_example.h"
#elif defined(APARD32690_BASIC_EXAMPLE)
#include "basic_example.h"
#elif defined(APARD32690_ADIN1110_STANDALONE_EXAMPLE)
#include "adin1110_standalone_example.h"
#endif

/***************************************************************************//**
 * @brief Main function execution.
 *
 * @return ret - Result of the enabled examples execution.
*******************************************************************************/
int main()
{
#if defined(APARD32690_ECHO_SERVER_EXAMPLE)
	return tcp_echo_server_example_main();
#elif defined(APARD32690_BASIC_EXAMPLE)
	return basic_example_main();
#elif defined(APARD32690_ADIN1110_STANDALONE_EXAMPLE)
	return adin1110_standalone_example_main();
#endif

	return 0;
}
