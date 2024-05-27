/***************************************************************************//**
 *   @file   example_main.c
 *   @brief  Main program for example eval-adxl38x project
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
#include "example_main.h"
#include "common_data.h"
#include "adxl38x.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include <errno.h>

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief Example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int example_main()
{
	struct adxl38x_dev *adxl38x_desc;
	union adxl38x_sts_reg_flags device_flags;
	int ret;
	uint8_t reg_value[10] = {0};
	uint8_t device_range;
	uint8_t *opmode;
	uint16_t x, y, z, t;
	struct adxl38x_fractional_val xf,yf,zf,tf;

	/* Initializing the device*/
	ret = adxl38x_init(&adxl38x_desc, adxl38x_ip);
	ret = adxl38x_soft_reset(adxl38x_desc);
	if (ret == -EAGAIN)
		pr_info("Reset was not successful\n");
	else if (ret)
		goto error;
	ret = adxl38x_get_sts_reg(adxl38x_desc, &device_flags);
	if (ret)
		goto error;
	pr_info(" status value = 0x%08x\n", device_flags.value);

	ret = adxl38x_init(&adxl38x_desc, adxl38x_ip);
	ret |= adxl38x_set_op_mode(adxl38x_desc, ADXL38x_MODE_LP);
	ret |= adxl38x_set_range(adxl38x_desc, ADXL382_RANGE_15G);
	ret |= adxl38x_get_range(adxl38x_desc, &device_range);
	if (ret)
		goto error;
	if (adxl38x_desc->dev_type == ID_ADXL382)
		pr_info(" Device Type = ADXL382\n");
	else
		pr_info(" Device Type = ADXL380\n");
	no_os_mdelay(500);

	while(1) {
		ret = adxl38x_get_sts_reg(adxl38x_desc, &device_flags);
		ret |= adxl38x_set_op_mode(adxl38x_desc, ADXL38x_MODE_HP);
		ret |= adxl38x_get_op_mode(adxl38x_desc, opmode);
		ret |= adxl38x_read_device_data(adxl38x_desc, ADXL38x_DEVID_AD,
						2, &reg_value);
		if (ret)
			goto error;
		pr_info("Status value = 0x%08x\n", device_flags.value);
		pr_info("OP_MODE = %d\n", *opmode);
		pr_info("DEVICE_AD = 0x%02X\n", (int)reg_value[0]);
		pr_info("Device type = %d\n", (int)adxl38x_desc->dev_type);
		pr_info("Device range setting = %d\n", (int)adxl38x_desc->range);


		pr_info("RAW DATA in Gees (magnitude less than 1 and negative is displayed as absolute value)\n");
		pr_info("----------------\n");
		ret = adxl38x_get_xyz_gees(adxl38x_desc, ADXL38x_CH_EN_XYZT,
					   &xf, &yf, &zf);
		pr_info(" x = %lld.%07dg\n", xf.integer, abs(xf.fractional));
		pr_info(" y = %lld.%07dg\n", yf.integer, abs(yf.fractional));
		pr_info(" z = %lld.%07dg\n", zf.integer, abs(zf.fractional));
		pr_info(" Debug = %d\n", ret);
		if (ret)
			goto error;
		no_os_mdelay(1000);

		ret = adxl38x_get_op_mode(adxl38x_desc, opmode);
		pr_info("OP_MODE = %d\n", *opmode);
		pr_info("RAW DATA in LSBs\n");
		pr_info("----------------\n");
		ret = adxl38x_get_raw_data(adxl38x_desc, ADXL38x_CH_EN_XYZT,  &x, &y, &z, &t);
		if (ret)
			goto error;
		pr_info(" x = %d\n", (int)x);
		pr_info(" y = %d\n", (int)y);
		pr_info(" z = %d\n", (int)z);
		pr_info(" t = %d\n", (int)t);
		no_os_mdelay(1000);
	}

error:
	pr_info("Error!\n");
	return 0;
}
