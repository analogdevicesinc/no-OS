/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  BASIC example source file for eval-adg1712 project
 *   @author GhisaA (Alexandru-nicolae.Ghisa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "common_data.h"
#include "adg1712.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/***************************************************************************//**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int example_main()
{
	struct adg1712_dev *dev;
	struct no_os_uart_desc *uart_desc;
	enum adg1712_switch sw;
	enum adg1712_switch i;
	bool enabled;
	int ret;

	ret = no_os_uart_init(&uart_desc, &adg1712_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart_desc);

	ret = adg1712_init(&dev, &adg1712_init_param);
	if (ret) {
		pr_err("Failed to initialize ADG1712: %d\r\n", ret);
		goto exit_uart;
	}

	pr_info("ADG1712 initialized successfully\r\n");

	while (1) {
		for (sw = ADG1712_SW1; sw <= ADG1712_SW4; sw++) {

			ret = adg1712_set_switch_state(dev, sw, true);
			if (ret) {
				pr_err("Failed to enable switch %d: %d\r\n", sw + 1, ret);
				goto exit;
			}

			for (i = ADG1712_SW1; i <= ADG1712_SW4; i++) {
				ret = adg1712_get_switch_state(dev, i, &enabled);
				if (ret) {
					pr_err("Failed to get switch %d state: %d\r\n", i + 1, ret);
					goto exit;
				}
				pr_info("SW%d:%d ", i + 1, enabled);
			}
			pr_info("\r\n");

			no_os_mdelay(1000);

			ret = adg1712_set_switch_state(dev, sw, false);
			if (ret) {
				pr_err("Failed to disable switch %d: %d\r\n", sw + 1, ret);
				goto exit;
			}

		}
	}

exit:
	adg1712_remove(dev);
exit_uart:
	no_os_uart_remove(uart_desc);
	if (ret)
		pr_err("Example terminated with error: %d\r\n", ret);
	return ret;
}
