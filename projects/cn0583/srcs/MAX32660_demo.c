/***************************************************************************//**
*   @file   ADuCM3029_demo_cn0537.c
*   @brief  Application main source.
*
*   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
* Copyright (c) 2020 Analog Devices, Inc. All Rights Reserved.
* This software is proprietary to Analog Devices, Inc. and its licensors.
*
*******************************************************************************/

#include <stdlib.h>
#include "adpd188.h"
#include "cn0537.h"
#include "no_os_error.h"
#include "no_os_init.h"

int main(int argc, char *argv[])
{
	int32_t ret;
	struct cn0537_init_param dut_init;
	struct cn0537_dev *dut_ptr;

	no_os_init();

	cn0537_get_config(&dut_init);
	ret = cn0537_init(&dut_ptr, &dut_init);
	if (ret != 0)
		return -1;

	while (1) {
		ret = cn0537_process(dut_ptr);
		if (ret != 0)
			return -1;
	}

	return 0;
}
