/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  Implementation of no-OS driver for eval-adxl37x project.
 *   @author Raul Georgian (Raul.Georgian@analog.com)
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#include "no_os_print_log.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "adxl37x.h"
#include "common_data.h"

int example_main()
{
	//adxl37x init and start SPI
	struct adxl37x_dev *dev;
	struct adxl37x_init_param dev_init = adxl37x_init_param;
	int32_t ret;
	struct no_os_uart_desc *uart;
	struct adxl37x_xyz_accel_data recorded_data[MAX_SAMPLES];
	int count = 0;
	uint16_t numberSamples = EXTRACTED_SAMPLES;
	uint8_t pwr_ctl;
	int16_t x, y, z;

	ret = no_os_uart_init(&uart, &adxl37x_uart_ip);
	if (ret)
		return ret;

	no_os_uart_stdio(uart);

	ret = adxl37x_init(&dev, dev_init);
	if (ret != 0)
		goto error;

	adxl37x_read_reg(dev, ADXL37X_POWER_CTL, &pwr_ctl);
	pr_info("pwr_ctl register after initialization of adxl37x: 0x%02X\n\r",
		pwr_ctl);

	ret = adxl37x_set_internal_sync(dev);
	if (ret != 0) {
		printf("internal sync unsuccesful!\n\r");
		goto error;
	}

	while (1) {
		ret = adxl37x_get_fifo_xyz_data_workaround(dev, recorded_data, numberSamples);
		if (ret != 0)
			goto error;

		for (int i = 0; i < numberSamples; i++) {
			x = (int16_t)(recorded_data[i].x << 4) >> 4; //sign extension
			y = (int16_t)(recorded_data[i].y << 4) >> 4;
			z = (int16_t)(recorded_data[i].z << 4) >> 4;
			pr_info("Accelerometer data for sample number %d is: x=%d,y=%d,z=%d\r\n", i, x,
				y, z);
		}
	}

	no_os_free(dev);
error:
	pr_info("Error!\n");
	return ret;

}
