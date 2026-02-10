/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  BASIC example source file for eval-adg1712 project
 *   @author GhisaA (Alexandru-nicolae.Ghisa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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
