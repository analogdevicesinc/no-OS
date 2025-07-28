/*******************************************************************************
 *   @file   basic_example.c
 *   @brief  Basic example code for eval-ad7490sdz project
 *   @author Radu Sabau (radu.sabau@analog.com)
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
#include "ad7490.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

#define AD7490_VREF		2.5
#define AD7490_FULLSCALE	4096

/*****************************************************************************
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/
int example_main()
{
	struct ad7490_desc *ad7490;
	int ret, i;
	int16_t val;

	ret = ad7490_init(&ad7490, &ad7490_ip);
	if (ret)
		goto error;

	ret = ad7490_config(ad7490, AD7490_DOUT_TRISTATE, AD7490_RANGE_REFIN,
			    AD7490_CODING_BINARY);
	if (ret)
		goto free_desc;

	while (1) {
		for (i = 0; i < AD7490_CHAN_ADDR_VIN15; i++) {
			ret = ad7490_read_ch(ad7490, (enum ad7490_address)i, &val);
			if (ret)
				goto free_desc;

			pr_info("Channel %d RAW Value : %d\n", i, val);

			pr_info("Channel %d Scaled : %0.2f\n", i,
				((float)val * AD7490_VREF) / AD7490_FULLSCALE);
		}

		no_os_mdelay(1000);
	}

	return 0;

free_desc:
	ad7490_remove(ad7490);
error:
	pr_info("Error!\r\n");
	return ret;
}
