/*******************************************************************************
 *   @file   main.c
 *   @brief  C file containing main function
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#ifdef BASIC_EXAMPLE
#include "basic_example.h"
#endif

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Main function that executes on Maxim platform
 *
 * @return int - Result of the execution
 */
int main()
{
	int ret = -EINVAL;

#ifdef BASIC_EXAMPLE
	ret = basic_pqm_firmware();
#else
#error No example project has been selected.
#endif

	return ret;
}
