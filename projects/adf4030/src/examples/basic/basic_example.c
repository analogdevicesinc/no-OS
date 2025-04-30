/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example EVAL-ADF4030 project
 *   @author Sirac Kucukarabacioglu (sirac.kucukarabacioglu@analog.com)
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
#include "no_os_delay.h"
#include "no_os_print_log.h"

/**
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 */
int basic_example_main()
{
	struct adf4030_dev *dev;
	int ret;

	pr_info("Enter basic example \n");

	ret = adf4030_init(&dev, &adf4030_ip);
	if (ret)
		goto remove_adf4030;

	// Set CH 1 as TX
	ret = adf4030_set_channel_direction(dev, 1, true);
	if (ret)
		goto remove_adf4030;

	// Set CH 2 as TX
	ret = adf4030_set_channel_direction(dev, 2, true);
	if (ret)
		goto remove_adf4030;

	ret = adf4030_set_tdc_source(dev, 1);
	if (ret)
		goto remove_adf4030;

	ret = adf4030_set_channel_delay(dev, 2, 200000);
	if (ret)
		goto remove_adf4030;

	ret = adf4030_set_single_ch_alignment(dev, 2);
	if (ret)
		goto remove_adf4030;

	ret = adf4030_set_tdc_measurement(dev, 2);
	if (ret)
		goto remove_adf4030;

	int64_t tdc_res;
	ret = adf4030_get_tdc_measurement(dev, &tdc_res);
	if (ret)
		goto remove_adf4030;

	pr_info("tdc_res : %lld\n", tdc_res);

remove_adf4030:
	adf4030_remove(dev);

	if (ret)
		pr_info("Error!\n");
	return ret;
}
