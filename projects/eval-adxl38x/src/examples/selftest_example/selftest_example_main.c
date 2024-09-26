/***************************************************************************//**
 *   @file   selftest_example_main.c
 *   @brief  Main program for self test example eval-adxl38x project
 *   @author BRajendran (balarupini.rajendran@analog.com)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "selftest_example_main.h"
#include "common_data.h"
#include "adxl38x.h"
#include "no_os_print_log.h"
#include <errno.h>
#include <stdlib.h>

/***************************************************************************//**
 * @brief Example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute the code and end at error statement.
*******************************************************************************/
int selftest_example_main()
{
	struct adxl38x_dev *adxl38x_desc;
	union adxl38x_sts_reg_flags device_flags;
	int ret;
	uint8_t device_range;
	uint8_t device_ID;
	uint8_t opmode;
	bool selftest_x, selftest_y, selftest_z;

	ret = adxl38x_init(&adxl38x_desc, adxl38x_ip);
	if (ret)
		goto error;
	pr_info("Initialization successful\n");

	ret = adxl38x_soft_reset(adxl38x_desc);
	if (ret == -EAGAIN)
		pr_info("Reset was not successful\n");
	else if (ret)
		goto error;

	ret = adxl38x_get_sts_reg(adxl38x_desc, &device_flags);
	if (ret)
		goto error;
	ret = adxl38x_set_range(adxl38x_desc, ADXL382_RANGE_60G);
	if (ret)
		goto error;
	ret = adxl38x_get_range(adxl38x_desc, &device_range);
	if (ret)
		goto error;

	ret = adxl38x_get_deviceID(adxl38x_desc, &device_ID);
	if (ret)
		goto error;

	if (adxl38x_desc->dev_type == ID_ADXL382) {
		pr_info(" Device Type = ADXL382\n");
	} else
		pr_info(" Device Type = ADXL380\n");

	pr_info(" Device range is %d\n", (int)device_range);
	pr_info(" Status value = 0x%08lx\n", device_flags.value);

	/* Set the accelerometer in high power mode for self test */
	opmode = ADXL38X_MODE_HP;

	/* Self test */
	ret = adxl38x_selftest(adxl38x_desc, opmode, &selftest_x, &selftest_y,
			       &selftest_z);
	pr_info(" X axis self test %s\n", selftest_x?"successful":"failed");
	pr_info(" Y axis self test %s\n", selftest_y?"successful":"failed");
	pr_info(" Z axis self test %s\n", selftest_z?"successful":"failed");

error:
	if(ret)
		pr_info("Error occurred!");
	else
		pr_info("The program has ended after successful execution\n");
	return 0;
}
