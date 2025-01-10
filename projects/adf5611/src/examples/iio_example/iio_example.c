/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Implementation of IIO example for adf5611 project.
 *   @author Josemene (jude.osemene@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "iio_adf5611.h"
#include "common_data.h"
#include "no_os_print_log.h"
#include "iio_app.h"

#define BOARD_NAME_SUPPORT 1

/**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
 */
int example_main()
{
	struct adf5611_iio_dev *adf5611_iio_dev;
	struct adf5611_iio_dev_init_param adf5611_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	struct iio_ctx_attr *context_attributes;
	int ret;


	adf5611_iio_ip.adf5611_dev_init = &adf5611_ip;
	ret = adf5611_iio_init(&adf5611_iio_dev, &adf5611_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adf5611",
			.dev = adf5611_iio_dev,
			.dev_descriptor = adf5611_iio_dev->iio_dev,
		}
	};
#if BOARD_NAME_SUPPORT
	struct no_os_eeprom_desc *eeprom_dev;
	ret = eeprom_24xx32a_ops.init(&eeprom_dev, &adf5611_eeprom_ip);
	if (ret)
		return ret;
	char board_name[17] = {0};
	char hw_mezz[25];
	uint32_t eeprom_address = 0x1B;

	ret = eeprom_24xx32a_ops.read(eeprom_dev, eeprom_address,
				      (uint8_t *)board_name, sizeof(board_name));
	if (ret)
		return ret;
	int i, index = 0;
	for (i = 0; board_name[i] != '\0'; i++) {
		if (isalnum((unsigned char)board_name[i])
		    || board_name[i] == '-') {
			hw_mezz[index++] = board_name[i];
		}
	}
	hw_mezz[index] = '\0';

	ret = eeprom_24xx32a_ops.remove(eeprom_dev);
	if (ret)
		return ret;

#endif
	context_attributes = (struct iio_ctx_attr *)calloc(NUM_CTX_ATTR,
			     sizeof(*context_attributes));
	context_attributes[0].name = "hw_mezzanine";
	context_attributes[0].value = hw_mezz;
	context_attributes[1].name = "hw_carrier";
	context_attributes[1].value = HW_CARRIER;
	context_attributes[2].name = "hw_name";
	context_attributes[2].value = ACTIVE_DEVICE_NAME;
	context_attributes[3].name = "hw_vendor";
	context_attributes[3].value = ACTIVE_DEVICE_VENDOR;

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adf5611_uart_ip;
	app_init_param.ctx_attrs = context_attributes;
	app_init_param.nb_ctx_attr = NUM_CTX_ATTR;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto exit;
	iio_app_run(app);

	iio_app_remove(app);
exit:
	adf5611_iio_remove(adf5611_iio_dev);

	if (ret)
		pr_info("Error!\n");
	return ret;
}
