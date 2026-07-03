/*****************************************************************************
 * main.c
 * Main file for ADuCM3029 platform of aducm_blinky_example project.
 *****************************************************************************/

#include "parameters.h"
#include "common_data.h"

extern int example_main();

/*****************************************************************************
 * @brief Main function execution for ADuCM3029 platform.
 *
 * @return ret - Result of the enabled examples execution.
 *****************************************************************************/
int main()
{
	int ret;

	ret = platform_init();
	if (ret)
		return ret;

	return example_main();
}
