/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Basic example source file for lt7182s project.
 *   @author Cedric Justine Encarnacion (cedricjustine.encarnacion@analog.com)
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
#include "common_data.h"
#include "basic_example.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "lt7182s.h"

int basic_example_main()
{
	struct lt7182s_dev *dev;
	struct lt7182s_status status;
	int ret, chan, vals[4];

	pr_info("Running basic example.\n");

	ret = lt7182s_init(&dev, &lt7182s_ip);
	if (ret)
		goto exit;

	while(1) {
		for (chan = LT7182S_CHAN_0; chan <= LT7182S_CHAN_1; chan++) {
			ret = lt7182s_read_value(dev, chan,
						 LT7182S_VIN, &vals[0]);
			if (ret)
				goto exit;

			ret = lt7182s_read_value(dev, chan,
						 LT7182S_VOUT, &vals[1]);
			if (ret)
				goto exit;

			ret = lt7182s_read_value(dev, chan,
						 LT7182S_IOUT, &vals[2]);
			if (ret)
				goto exit;

			ret = lt7182s_read_value(dev, chan,
						 LT7182S_TEMP, &vals[3]);
			if (ret)
				goto exit;

			ret = lt7182s_read_status(dev, chan,
						  LT7182S_STATUS_ALL_TYPE,
						  &status);
			if (status.vout)
				pr_info("Status vout asserted.\n");
			if (status.iout)
				pr_info("Status iout asserted.\n");
			if (status.input)
				pr_info("Status input asserted.\n");
			if (status.temp)
				pr_info("Status temp asserted.\n");
			if (status.cml)
				pr_info("Status cml asserted.\n");
			if (status.mfr_specific)
				pr_info("Status mfr_specific asserted.\n");

			pr_info("Channel: %d: vin = %d mV | vout = %d mV | iout = %d mA | temp = %d C\n",
				chan, vals[0], vals[1], vals[2],
				vals[3] / 1000);
		}

		pr_info("\n");
		no_os_mdelay(500);
	}

exit:
	pr_err("Error code: %d.\n", ret);
	lt7182s_remove(dev);
	return ret;
}
