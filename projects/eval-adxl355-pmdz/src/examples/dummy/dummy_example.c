/***************************************************************************//**
 *   @file   dummy_example.c
 *   @brief  DUMMY example header for eval-adxl355-pmdz project
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#include "adxl355.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief Dummy example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int dummy_example_main()
{
	struct adxl355_dev *adxl355_desc;
	int ret;

	ret = adxl355_init(&adxl355_desc, adxl355_ip);
	if (ret)
		goto error;
	ret = adxl355_soft_reset(adxl355_desc);
	if (ret)
		goto error;
	ret = adxl355_set_odr_lpf(adxl355_desc, ADXL355_ODR_3_906HZ);
	if (ret)
		goto error;
	ret = adxl355_set_op_mode(adxl355_desc, ADXL355_MEAS_TEMP_ON_DRDY_OFF);
	if (ret)
		goto error;

	struct adxl355_frac_repr x[32] = {0};
	struct adxl355_frac_repr y[32] = {0};
	struct adxl355_frac_repr z[32] = {0};
	struct adxl355_frac_repr temp;
	union adxl355_sts_reg_flags status_flags = {0};
	uint8_t fifo_entries = 0;

	while(1) {

		pr_info("Single read \n");
		ret = adxl355_get_xyz(adxl355_desc,&x[0], &y[0], &z[0]);
		if (ret)
			goto error;
		pr_info(" x=%d"".%09u", (int)x[0].integer, (abs)(x[0].fractional));
		pr_info(" y=%d"".%09u", (int)y[0].integer, (abs)(y[0].fractional));
		pr_info(" z=%d"".%09u \n", (int)z[0].integer,(abs)(z[0].fractional));

		ret = adxl355_get_fifo_data(adxl355_desc,
					    &fifo_entries,
					    &x[0],
					    &y[0],
					    &z[0]);
		if (ret)
			goto error;
		pr_info("Number of read entries from the FIFO %d \n", fifo_entries);
		pr_info("Number of read data sets from the FIFO %d \n", fifo_entries/3);
		for (uint8_t idx = 0; idx < 32; idx ++) {
			if (idx < fifo_entries/3) {
				pr_info(" x=%d"".%09u m/s^2", (int)x[idx].integer, (abs)(x[idx].fractional));
				pr_info(" y=%d"".%09u m/s^2", (int)y[idx].integer, (abs)(y[idx].fractional));
				pr_info(" z=%d"".%09u m/s^2", (int)z[idx].integer, (abs)(z[idx].fractional));
				pr_info("\n");
			}
		}

		pr_info("==========================================================\n");
		ret = adxl355_get_sts_reg(adxl355_desc, &status_flags);
		if (ret)
			goto error;
		pr_info("Activity flag = %d \n", (uint8_t)(status_flags.fields.Activity));
		pr_info("DATA_RDY flag = %d \n", (uint8_t)(status_flags.fields.DATA_RDY));
		pr_info("FIFO_FULL flag = %d \n", (uint8_t)(status_flags.fields.FIFO_FULL));
		pr_info("FIFO_OVR flag = %d \n", (uint8_t)(status_flags.fields.FIFO_OVR));
		pr_info("NVM_BUSY flag = %d \n", (uint8_t)(status_flags.fields.NVM_BUSY));
		pr_info("===========================================================\n");

		ret = adxl355_get_temp(adxl355_desc, &temp);
		if (ret)
			goto error;
		pr_info(" Temp =%d"".%09u millidegress Celsius \n", (int)temp.integer,
			(abs)(temp.fractional));

		no_os_mdelay(1000);
	}

error:
	pr_info("Error!\n");
	return 0;
}
