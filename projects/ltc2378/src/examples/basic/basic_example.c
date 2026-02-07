/*******************************************************************************
 *   @file   basic_example.c
 *   @brief  Basic example code for ltc2378 project
 *   @author Cherrence Sarip (cherrence.sarip@analog.com)
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

#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_spi.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "common_data.h"
#include "ltc2378.h"
#include "parameters.h"
#include <stdlib.h>

/*****************************************************************************
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/

int example_main()
{
	struct ltc2378_dev *dev;
	int ret;

	pr_info("Enter basic example \n");

	ret = ltc2378_init(&dev, &ltc2378_ip);
	if (ret) {
		pr_info("Init failed: %d\n", ret);
		return ret;
	}

	pr_info("VREF: %lu uV, Mode: %s\n",
		dev->vref_uv,
		(dev->input_mode == LTC2378_UNIPOLAR) ? "Unipolar" : "Bipolar");

	while (1) {
		uint32_t raw;
		int32_t voltage_uv;

		ret = ltc2378_read_raw(dev, &raw);
		if (ret) {
			pr_info("Read failed: %d\n", ret);
			continue;
		}

		ret = ltc2378_raw_to_uv(dev, raw, &voltage_uv);
		if (ret) {
			pr_info("Convert failed: %d\n", ret);
			continue;
		}

		pr_info("Raw: %lu, Voltage: %ld uV\n", raw, voltage_uv);

		no_os_mdelay(BASIC_EXAMPLE_DELAY_MS);
	}

	return 0;
}
