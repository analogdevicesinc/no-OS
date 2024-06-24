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

/***************************************************************************//**
 * @brief Example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute the code and end at error statement.
*******************************************************************************/
int fifo_example_main()
{
	struct adxl38x_dev *adxl38x_desc;
	union adxl38x_sts_reg_flags device_flags;
	int ret;
	uint8_t opmode;
	uint8_t status0;
	uint8_t fifo_status[2];
	uint8_t fifo_data[36];
	uint8_t set_fifo_entries = 0x0C;
	uint16_t fifo_entries;
	char display_format_string[80];

	ret = adxl38x_init(&adxl38x_desc, adxl38x_ip);
	if (ret)
		goto error;
	pr_info("Initialization successful\n");

	ret = adxl38x_soft_reset(adxl38x_desc);
	if (ret == -EAGAIN)
		pr_info("Reset was not successful\n");
	else if (ret)
		goto error;

	// FIFO sequence
	// Put the part in standby mode
	ret = adxl38x_set_op_mode(adxl38x_desc, ADXL38X_MODE_STDBY);
	if (ret)
		goto error;
	// Set FILTER register to 0x70
	ret = adxl38x_write_device_data(adxl38x_desc, ADXL38X_FILTER, 1, 0x70);
	if (ret)
		goto error;
	// Set DIG_EN register to 0x78
	ret = adxl38x_write_device_data(adxl38x_desc, ADXL38X_DIG_EN, 1, 0x78);
	if (ret)
		goto error;
	// Set FIFO_CFG0 to 0x60
	ret = adxl38x_write_device_data(adxl38x_desc, ADXL38X_FIFO_CFG0, 1, 0x60);
	if (ret)
		goto error;
	// Set FIFO_CFG1 to 0x0C
	ret = adxl38x_write_device_data(adxl38x_desc, ADXL38X_FIFO_CFG1, 1, set_fifo_entries);
	if (ret)
		goto error;
	// Set INT0_MAP0 to 0x08
	ret = adxl38x_write_device_data(adxl38x_desc, ADXL38X_INT0_MAP0, 1, 0x08);
	if (ret)
		goto error;


	// Put the part in HP mode and read data when FIFO watermark pin is set
	ret = adxl38x_set_op_mode(adxl38x_desc, ADXL38X_MODE_HP);
	if (ret)
		goto error;

	pr_info("Starting watermark check\n");

	// Building display string for fifo data
	strcpy(display_format_string, "DATA: ");
	for(int i = 0; i < 12; i ++){
		strcat(display_format_string, "%d ");
	}

	while(true){
		// Read status to assert if FIFO_WATERMARK bit set
		ret = adxl38x_read_device_data(adxl38x_desc, ADXL38X_STATUS0, 1, &status0);
		if (ret)
			goto error;
		pr_info("Status 0: %d\n", status0 );
		ret = adxl38x_read_device_data(adxl38x_desc, ADXL38X_FIFO_STATUS0, 2, fifo_status);
		// Read FIFO status and data if FIFO_WATERMARK is set
		if(status0 & NO_OS_BIT(3)){
			pr_info(" --------- FIFO_WATERMARK is set ---------\n");
			ret = adxl38x_read_device_data(adxl38x_desc, ADXL38X_FIFO_STATUS0, 2, fifo_status);
			if (ret)
				goto error;
			fifo_entries = no_os_get_unaligned_le16(fifo_status);
			fifo_entries = fifo_entries & 0x01ff;
			pr_info("Fifo entries: %d\n", fifo_entries );
			if(fifo_entries < set_fifo_entries)
				goto unmatch_error;
			// Read data from FIFO
			ret = adxl38x_read_device_data(adxl38x_desc, ADXL38X_FIFO_DATA, 36, fifo_data);
			if (ret)
			goto error;
			// Display data
			pr_info("%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", fifo_data[0],fifo_data[1],fifo_data[2],
										fifo_data[3],fifo_data[4],fifo_data[5],
										fifo_data[6],fifo_data[7],fifo_data[8],
									 	fifo_data[9],fifo_data[10],fifo_data[11],
										fifo_data[12],fifo_data[13],fifo_data[14],
										fifo_data[15],fifo_data[16],fifo_data[17],
										fifo_data[18],fifo_data[19],fifo_data[20]);
										// fifo_data[21],fifo_data[22],fifo_data[23],
										// fifo_data[24],fifo_data[25],fifo_data[26],
										// fifo_data[27],fifo_data[28],fifo_data[29],
										// fifo_data[30],fifo_data[31],fifo_data[32],
										// fifo_data[33],fifo_data[34],fifo_data[35]);
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
