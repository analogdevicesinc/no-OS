/***************************************************************************//**
 *   @file   dummy_example.c
 *   @brief  DUMMY example header for eval-adis project
 *   @author RBolboac (ramona.bolboaca@analog.com)
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

#include "dummy_example.h"
#include "common_data.h"
#include "adis16505.h"
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
	"°C"
};

static const float output_scale[] = {
	1.0/RAD_TO_DEGREE(40 << 16),
	1.0/RAD_TO_DEGREE(40 << 16),
	1.0/RAD_TO_DEGREE(40 << 16),
	78.0/ (32000 << 16),
	78.0/ (32000 << 16),
	78.0/ (32000 << 16),
	1.0/10,
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
int dummy_example_main()
{
	struct adis_dev *adis16505_desc;
	int ret;
	int val[7];

	adis16505_chip_info.ip = &adis16505_ip;

	ret = adis_init(&adis16505_desc, &adis16505_chip_info);
	if (ret)
		goto error;

	while(1) {
		pr_info ("while loop \n");
		no_os_mdelay(1000);
		ret = adis_read_x_gyro(adis16505_desc, &val[0]);
		if (ret)
			goto error;
		ret = adis_read_y_gyro(adis16505_desc, &val[1]);
		if (ret)
			goto error;
		ret = adis_read_z_gyro(adis16505_desc, &val[2]);
		if (ret)
			goto error;
		ret = adis_read_x_accl(adis16505_desc, &val[3]);
		if (ret)
			goto error;
		ret = adis_read_y_accl(adis16505_desc, &val[4]);
		if (ret)
			goto error;
		ret = adis_read_z_accl(adis16505_desc, &val[5]);
		if (ret)
			goto error;
		ret = adis_read_temp_out(adis16505_desc, &val[6]);
		if (ret)
			goto error;

		for (uint8_t i = 0; i < 7; i++)
			pr_info("%s %.5f %s \n", output_data[i], val[i]* output_scale[i],
				output_unit[i]);
	}
error:
	pr_info("Error!\n");
	return 0;
}
