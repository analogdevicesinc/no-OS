/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example source file for ltc4296 project.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "common_data.h"
#include "basic_example.h"
#include "no_os_delay.h"
#include "no_os_spi.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "no_os_pwm.h"
#include "ltc4296.h"

int basic_example_main()
{
	int ret;

	struct ltc4296_dev *ltc4296_desc;

	ret = ltc4296_init(&ltc4296_desc, &ltc4296_ip);
	if (ret) {
		pr_info("Initialization error!\n");
		return ret;
	}

	ret = ltc4296_port_prebias(ltc4296_desc, LTC_PORT0, LTC_CFG_APL_MODE);
	if (ret)
		goto err;

	ret = ltc4296_port_en(ltc4296_desc, LTC_PORT0);
	if (ret)
		goto err;

	no_os_mdelay(100);

	ltc4296_port_prebias(ltc4296_desc, LTC_PORT1, LTC_CFG_APL_MODE);
	if (ret)
		goto err;

	ltc4296_port_en(ltc4296_desc, LTC_PORT1);
	if (ret)
		goto err;

	no_os_mdelay(100);

	ret = ltc4296_port_prebias(ltc4296_desc, LTC_PORT2, LTC_CFG_APL_MODE);
	if (ret)
		goto err;

	ret = ltc4296_port_en(ltc4296_desc, LTC_PORT2);
	if (ret)
		goto err;

	no_os_mdelay(100);

	ret = ltc4296_port_prebias(ltc4296_desc, LTC_PORT3, LTC_CFG_APL_MODE);
	if (ret)
		goto err;

	ret = ltc4296_port_en(ltc4296_desc, LTC_PORT3);
	if (ret)
		goto err;

	no_os_mdelay(100);

	return 0;

err:
	ltc4296_remove(ltc4296_desc);

	return ret;
}
