/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for the apard32690 project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
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
