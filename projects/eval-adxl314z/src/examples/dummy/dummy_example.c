/*******************************************************************************
 *   @file   dummy_example.c
 *   @brief  DUMMY example header for eval-adxl314z project
 *   @author GMois (george.mois@analog.com)
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
#include "adxl314.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_error.h"

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
	struct adxl314_dev *adxl314;
	int ret = -EINVAL;

	/* Clear the screen. */
	pr_info("%c",27);
	pr_info("%c",'[');
	pr_info("%c",'2');
	pr_info("%c",'J');

	/* Initialize component. */
	ret = adxl314_init(&adxl314, adxl314_user_init);
	if (ret)
		goto error;

	pr_info("ADXL314 successfully initialized.\n");

	/* Perform device self-test. */
	ret = adxl314_self_test(adxl314);
	if (ret)
		goto error;

	/* Set Standby mode, required for performing setup. */
	ret = adxl314_set_op_mode(adxl314, ADXL314_STDBY);
	if (ret)
		goto error;

	/* Set output data rate. */
	ret = adxl314_set_odr(adxl314, ADXL314_ODR_800HZ);
	if (ret)
		goto error;

	/* Set offset for Z axis. */
	ret = adxl314_set_offset(adxl314, 6, ADXL314_Z_AXIS);
	if (ret)
		goto error;

	/* Set FIFO mode to FIFO. */
	ret = adxl314_set_fifo_mode(adxl314, ADXL314_FIFO_MODE);
	if (ret)
		goto error;

	/* Put device in Measure mode after setup. */
	ret = adxl314_set_op_mode(adxl314, ADXL314_MEAS);
	if (ret)
		goto error;

	/* Wait for 2 seconds. */
	no_os_mdelay(2000);

	struct adxl314_frac_repr x[ADXL314_MAX_FIFO_ENTRIES] = {0};
	struct adxl314_frac_repr y[ADXL314_MAX_FIFO_ENTRIES] = {0};
	struct adxl314_frac_repr z[ADXL314_MAX_FIFO_ENTRIES] = {0};
	union adxl314_int_src_reg_flags int_status_flags = {0};
	uint8_t fifo_entries = 0;
	int16_t x_raw = 0, y_raw = 0, z_raw = 0;

	while(1) {
		/* Clear the screen. */
		pr_info("%c",27);
		pr_info("%c",'[');
		pr_info("%c",'2');
		pr_info("%c",'J');

		pr_info("Single read raw data: \n");
		ret = adxl314_get_raw_xyz(adxl314,&x_raw, &y_raw, &z_raw);
		if (ret)
			goto error;
		pr_info(" x=%d", x_raw);
		pr_info(" y=%d", y_raw);
		pr_info(" z=%d\n", z_raw);

		pr_info("Single read: \n");
		ret = adxl314_get_xyz(adxl314,&x[0], &y[0], &z[0]);
		if (ret)
			goto error;
		pr_info(" x=%d"".%09u", (int)x[0].integer, (abs)(x[0].fractional));
		pr_info(" y=%d"".%09u", (int)y[0].integer, (abs)(y[0].fractional));
		pr_info(" z=%d"".%09u \n", (int)z[0].integer,(abs)(z[0].fractional));

		/* Read FIFO data. */
		ret = adxl314_get_fifo_data(adxl314, &fifo_entries, x, y, z);
		if (ret)
			goto error;
		pr_info("Number of read entries from the FIFO %d. \n", fifo_entries);
		for (uint8_t idx = 0; idx < fifo_entries; idx++) {
			pr_info(" x=%d"".%09u m/s^2", (int)x[idx].integer, (abs)(x[idx].fractional));
			pr_info(" y=%d"".%09u m/s^2", (int)y[idx].integer, (abs)(y[idx].fractional));
			pr_info(" z=%d"".%09u m/s^2\n", (int)z[idx].integer, (abs)(z[idx].fractional));
		}

		pr_info("==========================================================\n");
		pr_info("Interrupt sources: \n");
		ret = adxl314_get_int_source_reg(adxl314, &int_status_flags);
		if (ret)
			goto error;
		pr_info("DATA_READY event flag = %d. \n",
			(uint8_t)(int_status_flags.fields.DATA_READY));
		pr_info("ACTIVITY event flag = %d. \n",
			(uint8_t)(int_status_flags.fields.ACTIVITY));
		pr_info("INACTIVITY event flag = %d. \n",
			(uint8_t)(int_status_flags.fields.INACTIVITY));
		pr_info("WATERMARK event flag = %d. \n",
			(uint8_t)(int_status_flags.fields.WATERMARK));
		pr_info("OVERRUN event flag = %d. \n",
			(uint8_t)(int_status_flags.fields.OVERRUN));
		pr_info("===========================================================\n");

		/* Wait for one second. */
		no_os_mdelay(1000);
	}

error:
	pr_info("Error! (code %d)\n",ret);
	return ret;
}
