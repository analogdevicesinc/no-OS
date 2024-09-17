/*******************************************************************************
 *   @file   basic_example.c
 *   @brief  Basic example code for ltc2983 project
 *   @author John Erasmus Mari Geronimo (johnerasmusmari.geronimo@analog.com)
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

#include "basic_example.h"
#include "common_data.h"
#include "ltc2983.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/*****************************************************************************
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/
int basic_example_main()
{
	struct ltc2983_desc *dev;
	int ret, i;
	int val;

	if (ltc2983_ip.dev_type == ID_LTC2983)
		pr_info("\r\nRunning LTC2983 Basic Example\r\n");
	else if (ltc2983_ip.dev_type == ID_LTC2984)
		pr_info("\r\nRunning LTC2984 Basic Example\r\n");
	else if (ltc2983_ip.dev_type == ID_LTC2986)
		pr_info("\r\nRunning LTC2986 Basic Example\r\n");

	ret = ltc2983_init(&dev, &ltc2983_ip);
	if (ret)
		goto error;

	while (1) {
		for (i = 0; i < dev->max_channels_nr; i++) {
			if (!dev->sensors[i] ||
			    dev->sensors[i]->type == LTC2983_RSENSE)
				continue;
			ret = ltc2983_chan_read(dev, i + 1, &val);
			if (ret)
				goto free_dev;
			pr_info("Channel %d: temperature: %d mC\r\n", i + 1,
				val);
		}
		no_os_mdelay(1000);
	}

free_dev:
	ltc2983_remove(dev);
error:
	pr_info("Error!\r\n");
	return 0;
}
