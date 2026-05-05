/*******************************************************************************
 *   @file   adt7604_basic_example.c
 *   @brief  Basic example for the EVAL-ADT7604-AZ evaluation board
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#include "adt7604_basic_example.h"
#include "common_data.h"
#include "ltc2983.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/*****************************************************************************
 * @brief EVAL-ADT7604-AZ basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/
int adt7604_basic_example_main()
{
	struct ltc2983_desc *dev;
	uint32_t raw_cov;
	uint8_t type;
	int val;
	int ret;
	int i;

	pr_info("\r\nRunning EVAL-ADT7604-AZ Basic Example\r\n");

	ret = ltc2983_init(&dev, &ltc2983_ip);
	if (ret)
		goto error;

	while (1) {
		pr_info("---\r\n");

		for (i = 0; i < dev->max_channels_nr; i++) {
			if (!dev->sensors[i] ||
			    dev->sensors[i]->type == LTC2983_RSENSE)
				continue;

			type = dev->sensors[i]->type;

			if (type == LTC2983_RTD_CUSTOM) {
				/*
				 * IIO convention: 1/1024 mOhm per LSB.
				 * Divide by 1024 to get mOhm; dividing by
				 * 1024000 to get Ohm loses precision in integer
				 * arithmetic.
				 */
				ret = ltc2983_chan_read_resistance(dev, i + 1,
								   &raw_cov);
				if (ret)
					goto free_dev;
				pr_info("CH%02d copper trace:   %u mOhm\r\n",
					i + 1, raw_cov / 1024);
			} else if (type == LTC2983_THERMISTOR_CUSTOM) {
				/*
				 * IIO convention: 1/1024 Ohm per LSB.
				 * Divide by 1024 to get Ohm.
				 */
				ret = ltc2983_chan_read_resistance(dev, i + 1,
								   &raw_cov);
				if (ret)
					goto free_dev;
				pr_info("CH%02d resistance:     %u Ohm\r\n",
					i + 1, raw_cov / 1024);
				if (to_thermistor(dev->sensors[i])->custom) {
					ret = ltc2983_chan_read(dev, i + 1, &val);
					if (!ret)
						pr_info("CH%02d coverage:       %d m%%\r\n",
							i + 1, val);
					ret = 0;
				}
			} else {
				ret = ltc2983_chan_read(dev, i + 1, &val);
				if (ret)
					goto free_dev;
				pr_info("CH%02d temperature:    %d mC\r\n",
					i + 1, val);
			}
		}

		no_os_mdelay(1000);
	}

free_dev:
	ltc2983_remove(dev);
error:
	pr_info("Error!\r\n");
	return ret;
}
