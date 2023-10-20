/***************************************************************************//**
 *   @file   iio_lwip_example.c
 *   @brief  Implementation of IIO LWIP example for eval-adxl355-pmdz project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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
#include "iio_lwip_example.h"
#include "iio_adxl355.h"
#include "common_data.h"
#include "iio_app.h"
#include "lwip_socket.h"
#include "lwip_adin1110.h"
#include "iio_ad7793.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#ifndef DATA_BUFFER_SIZE
#define DATA_BUFFER_SIZE 400
#endif

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/
uint8_t iio_data_buffer[DATA_BUFFER_SIZE*3*sizeof(int)];

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief IIO example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously function iio_app_run and will not return.
*******************************************************************************/

int iio_lwip_example_main()
{
	int32_t adc_data;
	uint32_t reg_data;
	int ret;
	struct adxl355_iio_dev *adxl355_iio_desc;
	struct adxl355_iio_dev_init_param adxl355_iio_ip;
	struct iio_app_desc *app;
	struct iio_data_buffer accel_buff = {
		.buff = (void *)iio_data_buffer,
		.size = DATA_BUFFER_SIZE*3*sizeof(int)
	};
	struct iio_app_init_param app_init_param = { 0 };

	struct ad7793_iio_desc *ad7793_desc;
	struct ad7793_iio_param iio_param = {
		.ad7793_ip = ad7799_ip,
	};
	ret = ad7793_iio_init(&ad7793_desc, &iio_param);
	if (ret)
		return ret;

	ret = ad7799_read(ad7793_desc->ad7793_desc, AD7799_REG_CONF, &reg_data);

	/* Enable excitation current */
	ret = ad7799_read(ad7793_desc->ad7793_desc, AD7799_REG_IO, &reg_data);
	reg_data |= NO_OS_BIT(3) | NO_OS_BIT(2) | NO_OS_BIT(1);
	ret = ad7799_write(ad7793_desc->ad7793_desc, AD7799_REG_IO, reg_data);
	ret = ad7799_set_mode(ad7793_desc->ad7793_desc, 0x3);

	ret = ad7799_read(ad7793_desc->ad7793_desc, AD7799_REG_MODE, &reg_data);
	reg_data |= NO_OS_GENMASK(3, 0);
	ret = ad7799_write(ad7793_desc->ad7793_desc, AD7799_REG_MODE, reg_data);

	struct iio_app_device iio_devices[] = {
		{
			.name = "ad7793",
			.dev = ad7793_desc,
			.dev_descriptor = ad7793_desc->iio_dev,
		}
	};

	memcpy(adin1110_ip.mac_address, adin1110_mac_address, NETIF_MAX_HWADDR_LEN);
	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adxl355_uart_ip;
	app_init_param.lwip_param.platform_ops = &adin1110_lwip_ops;
	app_init_param.lwip_param.mac_param = &adin1110_ip;
	app_init_param.lwip_param.extra = NULL;
	memcpy(app_init_param.lwip_param.hwaddr, adin1110_mac_address,
	       NETIF_MAX_HWADDR_LEN);

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
}
