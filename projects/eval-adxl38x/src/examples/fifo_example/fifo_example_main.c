/***************************************************************************//**
 *   @file   fifo_example_main.c
 *   @brief  Main program for FIFO example eval-adxl38x project
 *   @author BRajendran (balarupini.rajendran@analog.com)
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "fifo_example_main.h"
#include "common_data.h"
#include "adxl38x.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
static char getaxis(uint8_t chID);

/***************************************************************************//**
 * @brief Example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute the code and end at error statement.
*******************************************************************************/
int fifo_example_main()
{
	struct adxl38x_dev *adxl38x_desc;
	int ret;
	uint8_t register_value;
	uint8_t status0;
	uint8_t fifo_status[2];
	uint8_t fifo_data[36];
	uint8_t set_fifo_entries = 0x0C;
	uint16_t fifo_entries;
	struct adxl38x_fractional_val data_frac[15];
	enum adxl38x_id devID;

	ret = adxl38x_init(&adxl38x_desc, adxl38x_ip);
	if (ret)
		goto error;
	pr_info("Initialization successful\n");

	ret = adxl38x_soft_reset(adxl38x_desc);
	if (ret == -EAGAIN)
		pr_info("Reset was not successful\n");
	else if (ret)
		goto error;
	ret = adxl38x_get_deviceID(adxl38x_desc, &devID);
	if (ret)
		goto error;
	ret = adxl38x_set_range(adxl38x_desc, ADXL382_RANGE_15G);
	if (ret)
		goto error;

	// FIFO sequence
	// Put the part in standby mode
	ret = adxl38x_set_op_mode(adxl38x_desc, ADXL38X_MODE_STDBY);
	if (ret)
		goto error;
	// Set FILTER register to 0x70 (Bypass EQ, LPF_MODE 0b11)
	register_value = 0x70;
	ret = adxl38x_write_device_data(adxl38x_desc, ADXL38X_FILTER, 1,
					&register_value);
	if (ret)
		goto error;

	// Set DIG_EN register to 0x78 (Enable XYZ axes and FIFO enable)
	register_value = 0x78;
	ret = adxl38x_write_device_data(adxl38x_desc, ADXL38X_DIG_EN, 1,
					&register_value);
	if (ret)
		goto error;

	// Set FIFO_CFG0 to 0x60 (Channel ID enable and FIFO stream mode)
	ret = adxl38x_accel_set_FIFO(adxl38x_desc, set_fifo_entries,
				     false, ADXL38X_FIFO_STREAM, true, false);

	// Set INT0_MAP0 to 0x08 (FIFO_WATERMARK_INT0)
	register_value = 0x08;
	ret = adxl38x_write_device_data(adxl38x_desc, ADXL38X_INT0_MAP0, 1,
					&register_value);
	if (ret)
		goto error;

	// Put the part in HP mode and read data when FIFO watermark pin is set
	ret = adxl38x_set_op_mode(adxl38x_desc, ADXL38X_MODE_HP);
	if (ret)
		goto error;

	pr_info("Starting watermark check\n");
	while(true) {
		// Read status to assert if FIFO_WATERMARK bit set
		ret = adxl38x_read_device_data(adxl38x_desc, ADXL38X_STATUS0, 1, &status0);
		if (ret)
			goto error;
		pr_info("Status 0: %d\n", status0 );
		ret = adxl38x_read_device_data(adxl38x_desc, ADXL38X_FIFO_STATUS0, 2,
					       fifo_status);
		if (ret)
			goto error;
		fifo_entries = no_os_get_unaligned_le16(fifo_status);
		fifo_entries = fifo_entries & 0x01ff;


		// Read FIFO status and data if FIFO_WATERMARK is set
		if(status0 & NO_OS_BIT(3)) {
			pr_info(" FIFO_WATERMARK is set. Total fifo entries =  %d\n", fifo_entries);
			if(fifo_entries < set_fifo_entries)
				goto unmatch_error;

			// Read data from FIFO (can read at least upto 12 samples * 3 bytes (chID, data))
			ret = adxl38x_read_device_data(adxl38x_desc, ADXL38X_FIFO_DATA, 36, fifo_data);
			if (ret)
				goto error;

			// Parse Data for fist five samples
			pr_info("First four entries (absolute values printed for magnitude between -1g & 1g):\n");
			for(int b = 0; b < 36; b += 3) {
				ret = adxl38x_data_raw_to_gees(adxl38x_desc, (fifo_data + b + 1), data_frac);
				if (ret)
					goto error;
				pr_info("%c : %lld.%07dg\n", getaxis(fifo_data[b]), data_frac->integer,
					abs(data_frac->fractional));
			}
		}
	}

error:
	if(ret)
		pr_info("Error occurred!");
	else
		pr_info("The program has ended after successful execution\n");
	return 0;
unmatch_error:
	pr_info("Number of entries in FIFO not matching the number set in FIFO config\n");
	return 0;
}

/***************************************************************************//**
 * @brief Assigns axis based on channel index
 *
 * @param chID         - Channel index
 *
 * @return ret         - Corresponding channel ID for channel index provided
*******************************************************************************/
static char getaxis(uint8_t chID)
{
	if(chID)
		return chID > 1 ? 'z' : 'y';
	return 'x';
}
