/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Source file for basic example.
 *   @author Ciobanu Radu (Radu-rares.Ciobanu@analog.com)
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#include "basic_example.h"
#include "common_data.h"
#include "adgm3121.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

/**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 */
int basic_example_main(void)
{
	struct adgm3121_dev *dev;
	enum adgm3121_state state;
	uint8_t switch_mask;
	uint8_t error_status;
	int ret;
	int i;

	pr_info("ADGM3121 Basic Example\r\n");

	ret = adgm3121_init(&dev, &adgm3121_ip);
	if (ret) {
		pr_err("adgm3121_init failed with code %d\r\n", ret);
		goto exit;
	}

	pr_info("ADGM3121 initialized successfully in SPI mode\r\n");

	ret = adgm3121_check_internal_error(dev, &error_status);
	if (ret) {
		pr_err("Failed to check internal error: %d\r\n", ret);
		goto free_dev;
	}

	if (error_status)
		pr_info("WARNING: Internal error detected: 0x%02X\r\n", error_status);
	else
		pr_info("No internal errors detected\r\n");

	pr_info("Enabling switches one by one...\r\n");

	for (i = 0; i < 4; i++) {
		ret = adgm3121_set_switch_state(dev, (enum adgm3121_switch)i,
						ADGM3121_ENABLE);
		if (ret) {
			pr_err("Failed to enable SW%d: %d\r\n", i + 1, ret);
			goto free_dev;
		}
		pr_info("SW%d enabled\r\n", i + 1);
		no_os_mdelay(500);
	}

	ret = adgm3121_get_switches(dev, &switch_mask);
	if (ret) {
		pr_err("Failed to read switch states: %d\r\n", ret);
		goto free_dev;
	}
	pr_info("Switch states bitmask: 0x%02X\r\n", switch_mask);

	for (i = 0; i < 4; i++) {
		ret = adgm3121_get_switch_state(dev, (enum adgm3121_switch)i,
						&state);
		if (ret) {
			pr_err("Failed to read SW%d state: %d\r\n", i + 1, ret);
			goto free_dev;
		}
		pr_info("SW%d state: %s\r\n", i + 1,
			(state == ADGM3121_ENABLE) ? "ENABLED" : "DISABLED");
	}

	pr_info("Disabling all switches...\r\n");

	for (i = 0; i < 4; i++) {
		ret = adgm3121_set_switch_state(dev, (enum adgm3121_switch)i,
						ADGM3121_DISABLE);
		if (ret) {
			pr_err("Failed to disable SW%d: %d\r\n", i + 1, ret);
			goto free_dev;
		}
		pr_info("SW%d disabled\r\n", i + 1);
		no_os_mdelay(500);
	}

	pr_info("Setting switches using bitmask (SW1 and SW3 enabled)...\r\n");
	ret = adgm3121_set_switches(dev, ADGM3121_SW1_EN | ADGM3121_SW3_EN);
	if (ret) {
		pr_err("Failed to set switches: %d\r\n", ret);
		goto free_dev;
	}

	ret = adgm3121_get_switches(dev, &switch_mask);
	if (ret) {
		pr_err("Failed to read switch states: %d\r\n", ret);
		goto free_dev;
	}
	pr_info("Switch states bitmask: 0x%02X (expected 0x05)\r\n", switch_mask);

	pr_info("Resetting all switches...\r\n");
	ret = adgm3121_reset_switches(dev);
	if (ret) {
		pr_err("Failed to reset switches: %d\r\n", ret);
		goto free_dev;
	}

	pr_info("ADGM3121 basic example completed successfully!\r\n");

free_dev:
	adgm3121_remove(dev);
exit:
	if (ret)
		pr_err("ADGM3121 basic example failed!\r\n");

	return ret;
}
