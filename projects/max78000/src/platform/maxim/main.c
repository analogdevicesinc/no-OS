/* SPDX-License-Identifier: LicenseRef-ADI-BSD */

/**
 *   @file   main.c
 *   @brief  Main file for the max78000 project.
 *   @author Victor Pascu (victor.pascu@analog.com)
 */

#include "common_data.h"

#if defined(CONFIG_MAX78000_BASIC_EXAMPLE)
#include "basic_example.h"
#endif

#if defined(CONFIG_MAX78000_DUAL_CORE_EXAMPLE)
#include "dual_core_example.h"
#endif





/***************************************************************************//**
 * @brief Main function execution.
 *
 * @return ret - Result of the enabled example execution.
*******************************************************************************/
int main()
{
#if defined(CONFIG_MAX78000_BASIC_EXAMPLE)
	return basic_example_main();
#elif defined(CONFIG_MAX78000_DUAL_CORE_EXAMPLE)
	return dual_core_example_main();
#endif

	return 0;
}
