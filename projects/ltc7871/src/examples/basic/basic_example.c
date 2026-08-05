/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example source file for ltc7871 project.
 *   @author Aldrin Abacan (aldrin.abacan@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_spi.h"
#include "no_os_util.h"
#include "common_data.h"
#include "ltc7871.h"

int example_main()
{
	int ret;
	uint8_t mode;
	int8_t idac;
	struct ltc7871_dev *ltc7871_dev;

	struct no_os_uart_desc *uart_desc;

	ret = no_os_uart_init(&uart_desc, &ltc7871_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);


	pr_info("\nBasic example\n");

	ret = ltc7871_init(&ltc7871_dev, &ltc7871_ip);
	if (ret)
		goto exit;

	ret = ltc7871_get_mfr_config2_setting(ltc7871_dev,
					      LTC7871_BUCK_BOOST_MASK, &mode);
	if (ret)
		goto exit;

	if (mode) {
		pr_info("Device on Buck mode\n");
		ltc7871_set_mfr_idac_vlow(ltc7871_dev, 0);
		ltc7871_get_mfr_idac_vlow(ltc7871_dev, &idac);
		pr_info("IDAC_Vlow: %duA\n", idac);
	} else {
		pr_info("Device on Boost mode\n");
		ltc7871_set_mfr_idac_vhigh(ltc7871_dev, 0);
		ltc7871_get_mfr_idac_vhigh(ltc7871_dev, &idac);
		pr_info("IDAC_Vhigh: %duA\n", idac);
	}

	ret = ltc7871_get_mfr_config2_setting(ltc7871_dev,
					      LTC7871_BURST_MASK, &mode);
	if (ret)
		goto exit;

	if (mode)
		pr_info("Device on Burst mode\n");

	ret = ltc7871_get_mfr_config2_setting(ltc7871_dev,
					      LTC7871_DCM_MASK, &mode);
	if (ret)
		goto exit;

	if (mode)
		pr_info("Device on DCM\n");

	ret = ltc7871_get_mfr_config2_setting(ltc7871_dev,
					      LTC7871_HIZ_MASK, &mode);
	if (ret)
		goto exit;

	if (mode)
		pr_info("Device on HIZ mode\n");

	ret = ltc7871_get_mfr_config2_setting(ltc7871_dev,
					      LTC7871_SPRD_MASK, &mode);
	if (ret)
		goto exit;

	if (mode)
		pr_info("Device on spread spectrum mode\n");

	return 0;

exit:
	if (ret)
		pr_err("Error!\n");

	ltc7871_remove(ltc7871_dev);

	return ret;

}
