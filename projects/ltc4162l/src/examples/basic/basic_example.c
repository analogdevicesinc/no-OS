/*******************************************************************************
 *   @file   basic_example.c
 *   @brief  Basic example code for ltc4162l project
 *   @author Marc Paolo Sosa <marcpaolo.sosa@analog.com>
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
#include "ltc4162l.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_units.h"

/*****************************************************************************
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/

//create example code for ltc4162l project
int basic_example_main()
{
	struct ltc4162l_desc *dev;
	uint16_t val;
	int ret;

	pr_info("LTC4162L Basic Example\n");

	ret = ltc4162l_init(&dev, &ltc4162l_ip);
	if (ret)
		goto error;

	while (1) {

		ret = ltc4162l_get_cell_count(dev, &val);
		if (ret)
			goto free_dev;

		pr_info("Cell count: %d\n", val);

		ret = ltc4162l_get_chem_type(dev, &val);
		if (ret)
			goto free_dev;

		pr_info("Chem type: %d\n", val);

		ret = ltc4162l_read_vbat(dev, &val);
		if (ret)
			goto free_dev;

		pr_info("VBAT: %d\n", val);

		ret = ltc4162l_read_avg_vbat(dev, &val);
		if (ret)
			goto free_dev;

		pr_info("Average VBAT: %d\n", val);

		ret = ltc4162l_read_ibat(dev, &val);
		if (ret)
			goto free_dev;

		pr_info("IBAT: %d\n", val);
	}

	pr_info("\n");
	no_os_mdelay(500);

free_dev:
	ltc4162l_remove(dev);

error:
	pr_err("Error code: %d.\n", ret);
	return ret;

}
