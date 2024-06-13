/***************************************************************************//**
 *   @file   st_example_main.c
 *   @brief  Main program for self test example eval-adxl38x project
 *   @author BRajendran (balarupini.rajendran@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "st_example_main.h"
#include "common_data.h"
#include "adxl38x.h"
#include "no_os_print_log.h"
#include <errno.h>
#include <stdlib.h>


/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute the code and end at error statement.
*******************************************************************************/
int st_example_main()
{
	struct adxl38x_dev *adxl38x_desc;
	union adxl38x_sts_reg_flags device_flags;
	int ret;
	uint8_t device_range;
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
	ret |= adxl38x_set_range(adxl38x_desc, ADXL382_RANGE_60G);
	ret |= adxl38x_get_range(adxl38x_desc, &device_range);
	if (ret)
		goto error;

	if (adxl38x_desc->dev_type == ID_ADXL382) {
		pr_info(" Device Type = ADXL382\n");
	} else
		pr_info(" Device Type = ADXL380\n");

	pr_info(" Device range is %d\n", (int)device_range);
	pr_info(" Status value = 0x%08x\n", device_flags.value);

	/* Set the accelerometer in high power mode for self test */
	opmode = ADXL38x_MODE_HP;

	/* Self test */
	ret = adxl38x_selftest(adxl38x_desc, opmode, &selftest_x, &selftest_y,
			       &selftest_z);
	pr_info(" X axis self test %s\n", selftest_x?"successful":"failed");
	pr_info(" Y axis self test %s\n", selftest_y?"successful":"failed");
	pr_info(" Z axis self test %s\n", selftest_z?"successful":"failed");

error:
	pr_info("The program ended or an Error occurred!\n");
	return 0;
}
