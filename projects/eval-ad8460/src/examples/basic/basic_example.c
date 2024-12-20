/*******************************************************************************
 *   @file   basic_example.c
 *   @brief  Basic example code for ad8460 project
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
 ********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#include "basic_example.h"
#include "common_data.h"
#include "ad8460.h"
#include "no_os_print_log.h"

/*****************************************************************************
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/
int basic_example_main()
{
	struct ad8460_device *dev;
	int ret, i;
	uint16_t val;
	uint8_t flag;

	pr_info("\r\nRunning AD8460 Basic Example\r\n");

	ret = ad8460_init(&dev, &ad8460_ip);
	if (ret)
		goto error;

	ret = ad8460_reset(dev);
	if (ret)
		goto free_dev;

	/** Switch to APG mode */
	ret = ad8460_enable_apg_mode(dev, 1);
	if (ret)
		goto free_dev;

	/** Switch to AWG mode */
	ret = ad8460_enable_apg_mode(dev, 0);

	for (i = 0; i < 16; i++) {
		ret = ad8460_set_hvdac_word(dev, i, i);
		if (ret)
			goto free_dev;

		ret = ad8460_get_hvdac_word(dev, i, &val);
		if (ret)
			goto free_dev;

		pr_info("HVDAC[%d]: 0x%04X\r\n", i, val);
	}

	ret = ad8460_read_shutdown_flag(dev, &flag);
	if (ret)
		goto free_dev;

	pr_info("Shutdown flag: %d\r\n", flag);

	if (flag) {
		ret = ad8460_hv_reset(dev);
		if (ret)
			goto free_dev;
	}

	pr_info("AD8460 Basic Example Done\r\n");

	return 0;

free_dev:
	ad8460_remove(dev);
error:
	pr_info("Error!\r\n");
	return ret;
}
