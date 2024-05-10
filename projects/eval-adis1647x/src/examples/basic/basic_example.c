/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  BASIC example header for eval-adis1647x project
 *   @author RBolboac (ramona.gradinariu@analog.com)
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

#include "basic_example.h"
#include "common_data.h"
#include "adis1647x.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_units.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

static const char * const output_data[] = {
	"angular velocity x axis: ",
	"angular velocity y axis: ",
	"angular velocity z axis: ",
	"acceleration x axis    : ",
	"acceleration y axis    : ",
	"acceleration z axis    : ",
	"temperature            : "
};

static const char * const output_unit[] = {
	"rad/s",
	"rad/s",
	"rad/s",
	"m/s^2",
	"m/s^2",
	"m/s^2",
	"milli °C"
};

/******************************************************************************/
/************************* Functions Definitions ******************************/
/******************************************************************************/

/**
 * @brief Dummy example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 */
int basic_example_main()
{
	struct adis_dev *adis1647x_desc;
	int ret;
	int32_t val[7];
	struct adis_scale_fractional accl_scale;
	struct adis_scale_fractional anglvel_scale;
	struct adis_scale_fractional temp_scale;

	ret = adis_init(&adis1647x_desc, &adis1647x_ip);
	if (ret)
		goto exit;

	ret = adis_get_accl_scale(adis1647x_desc, &accl_scale);
	if (ret)
		goto exit;

	ret = adis_get_anglvel_scale(adis1647x_desc, &anglvel_scale);
	if (ret)
		goto exit;

	ret = adis_get_temp_scale(adis1647x_desc, &temp_scale);
	if (ret)
		goto exit;

	float output_scale[] = {
		(float)anglvel_scale.dividend / anglvel_scale.divisor,
		(float)anglvel_scale.dividend / anglvel_scale.divisor,
		(float)anglvel_scale.dividend / anglvel_scale.divisor,
		(float)accl_scale.dividend / accl_scale.divisor,
		(float)accl_scale.dividend / accl_scale.divisor,
		(float)accl_scale.dividend / accl_scale.divisor,
		(float)temp_scale.dividend / temp_scale.divisor,
	};

	while(1) {
		pr_info ("while loop \n");
		no_os_mdelay(1000);
		ret = adis_read_x_gyro(adis1647x_desc, &val[0]);
		if (ret)
			goto exit;
		ret = adis_read_y_gyro(adis1647x_desc, &val[1]);
		if (ret)
			goto exit;
		ret = adis_read_z_gyro(adis1647x_desc, &val[2]);
		if (ret)
			goto exit;
		ret = adis_read_x_accl(adis1647x_desc, &val[3]);
		if (ret)
			goto exit;
		ret = adis_read_y_accl(adis1647x_desc, &val[4]);
		if (ret)
			goto exit;
		ret = adis_read_z_accl(adis1647x_desc, &val[5]);
		if (ret)
			goto exit;
		ret = adis_read_temp_out(adis1647x_desc, &val[6]);
		if (ret)
			goto exit;

		for (uint8_t i = 0; i < 7; i++)
			pr_info("%s %.5f %s \n", output_data[i], val[i] * output_scale[i],
				output_unit[i]);
	}
exit:
	adis_remove(adis1647x_desc);
	if (ret)
		pr_info("Error!\n");
	return ret;
}
