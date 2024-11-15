/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Source file for basic example.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "basic_example.h"
#include "common_data.h"
#include "max14919.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

int basic_example_main()
{
	struct max14919_desc *desc;
	enum max14919_out_state state[4] = {
		MAX14919_OUT_OFF,
		MAX14919_OUT_ON,
		MAX14919_OUT_OFF,
		MAX14919_OUT_OFF
	};
	int ret;
	uint8_t gpio_state;

	ret = max14919_init(&desc, &max14919_ip);
	if (ret)
		goto exit;

	ret = max14919_set_out(desc, state);
	if (ret)
		goto free_max14919;

	pr_info("MAX14919 output channel 2 is ON.\n");

	ret = max14919_get_fault(desc, &gpio_state);
	if (ret)
		goto free_max14919;

	switch (gpio_state) {
	case 0:
		pr_info("MAX14919 detected faults.\n");
		break;
	case 1:
		pr_info("MAX14919 detected no faults.\n");
		break;
	default:
		goto free_max14919;
	}

	ret = max14919_detect_rev(desc, &gpio_state);
	if (ret)
		goto free_max14919;

	switch (gpio_state) {
	case 0:
		pr_info("Current polarity is normal.\n");
		break;
	case 1:
		pr_info("Current polarity is reversed.\n");
		break;
	default:
		goto free_max14919;
	}

	ret = max14919_set_climit(desc, true);
	if (ret)
		goto free_max14919;

	pr_info("2x Current Limiting has been activated.");

free_max14919:
	max14919_remove(desc);
exit:
	if (ret)
		pr_info("Error!\n");

	return ret;
}
