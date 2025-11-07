/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  BASIC example header for eval-adis16607 project
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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

#include "common_data.h"
#include "adis16607.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_units.h"
#include "no_os_util.h"

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

/**
 * @brief Dummy example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 */
int example_main()
{
	struct adis_scale_fractional anglvel_scale;
	struct adis_scale_fractional accl_scale;
	struct adis_scale_fractional temp_scale;
	struct adis_burst_data burst_data;
	struct adis_dev *adis16607_desc;
	int temp_offset;
	int32_t val[7];
	int ret;

	ret = adis_init(&adis16607_desc, &adis16607_ip);
	if (ret)
		goto exit;

	ret = adis_get_accl_scale(adis16607_desc, &accl_scale);
	if (ret)
		goto exit;

	ret = adis_get_anglvel_scale(adis16607_desc, &anglvel_scale);
	if (ret)
		goto exit;

	ret = adis_get_temp_scale(adis16607_desc, &temp_scale);
	if (ret)
		goto exit;

	ret = adis_get_temp_offset(adis16607_desc, &temp_offset);
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

	while (1) {
		if (adis16607_desc->duplex_type == ADIS_SPI_HALF_DUPLEX) {
			ret = adis_write_burst32(adis16607_desc, 1);
			if (ret)
				return ret;

			ret = adis_read_burst_data(adis16607_desc, &burst_data, true, true, false,
						   true);
			if (ret)
				return ret;

			val[0] = no_os_sign_extend32(((burst_data.x_gyro_msb << 16) & 0xFF0000) |
						     (burst_data.x_gyro_lsb & 0xFFFF), 23);
			val[1] = no_os_sign_extend32(((burst_data.y_gyro_msb << 16) & 0xFF0000) |
						     (burst_data.y_gyro_lsb & 0xFFFF), 23);
			val[2] = no_os_sign_extend32(((burst_data.z_gyro_msb << 16) & 0xFF0000) |
						     (burst_data.z_gyro_lsb & 0xFFFF), 23);
			val[3] = no_os_sign_extend32(((burst_data.x_accel_msb << 16) & 0xFF0000) |
						     (burst_data.x_accel_lsb & 0xFFFF), 23);
			val[4] = no_os_sign_extend32(((burst_data.y_accel_msb << 16) & 0xFF0000) |
						     (burst_data.y_accel_lsb & 0xFFFF), 23);;
			val[5] = no_os_sign_extend32(((burst_data.z_accel_msb << 16) & 0xFF0000) |
						     (burst_data.z_accel_lsb & 0xFFFF), 23);;
			val[6] = burst_data.temp_lsb;
		} else {
			pr_info("while loop \n");
			no_os_mdelay(1000);
			ret = adis_read_x_gyro(adis16607_desc, &val[0]);
			if (ret)
				goto exit;
			ret = adis_read_y_gyro(adis16607_desc, &val[1]);
			if (ret)
				goto exit;
			ret = adis_read_z_gyro(adis16607_desc, &val[2]);
			if (ret)
				goto exit;
			ret = adis_read_x_accl(adis16607_desc, &val[3]);
			if (ret)
				goto exit;
			ret = adis_read_y_accl(adis16607_desc, &val[4]);
			if (ret)
				goto exit;
			ret = adis_read_z_accl(adis16607_desc, &val[5]);
			if (ret)
				goto exit;
			ret = adis_read_temp_out(adis16607_desc, &val[6]);
			if (ret)
				goto exit;
		}

		for (uint8_t i = 0; i < 7; i++) {
			if (i == 6) {
				pr_info("%s %.5f %s \n", output_data[i],
					(val[i] + temp_offset) * output_scale[i],
					output_unit[i]);
			} else {
				pr_info("%s %.5f %s \n", output_data[i], val[i] * output_scale[i],
					output_unit[i]);

			}
		}
	}
exit:
	adis_remove(adis16607_desc);
	if (ret)
		pr_info("Error!\n");
	return ret;
}
