/***************************************************************************//**
 *   @file   iio_lwip_example.c
 *   @brief  Implementation of IIO LWIP example for adin1140 project.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#include <string.h>

#include "iio_adt7420.h"
#include "common_data.h"
#include "iio_app.h"
#include "lwip_socket.h"
#include "lwip_adin1140.h"
#include "adin1140.h"
#include "adt7420.h"

static uint8_t adin1140_mac_address[6] = {0x00, 0x18, 0x80, 0x03, 0x25, 0x80};

static const struct no_os_i2c_init_param adt7420_i2c_ip = {
	.device_id = I2C_DEVICE_ID,
	.max_speed_hz = I2C_CLK_SPEED,
	.slave_address = ADT7420_ADDRESS(0, 0),
	.platform_ops = I2C_OPS,
	.extra = I2C_EXTRA,
};

static struct adt7420_init_param adt7420_ip = {
	.interface_init.i2c_init = adt7420_i2c_ip,
	.active_device = ID_ADT7422,
	.resolution_setting = 0,
};

static struct adin1140_init_param adin1140_ip;

/***************************************************************************//**
 * @brief IIO LWIP example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the iio_app_run function and will not
 *               return.
*******************************************************************************/
int example_main()
{
	int ret;
	struct adt7420_iio_dev *adt7420_iio_desc;
	struct adt7420_iio_init_param adt7420_iio_ip;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	adin1140_ip.comm_param = adin1140_spi_ip;
	adin1140_ip.mac_cfg = (struct adin1140_mac_cfg) {
		.cps   = 0x6,
		.zarfe = true,
	};

	adt7420_iio_ip.adt7420_dev_init = &adt7420_ip;
	ret = adt7420_iio_init(&adt7420_iio_desc, &adt7420_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adt7420",
			.dev = adt7420_iio_desc,
			.dev_descriptor = adt7420_iio_desc->iio_dev,
		}
	};

	memcpy(adin1140_ip.mac_address, adin1140_mac_address, NETIF_MAX_HWADDR_LEN);

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = adin1140_uart_ip;
	app_init_param.lwip_param.platform_ops = &adin1140_lwip_ops;
	app_init_param.lwip_param.mac_param = &adin1140_ip;
	app_init_param.lwip_param.extra = NULL;
	memcpy(app_init_param.lwip_param.hwaddr, adin1140_mac_address,
	       NETIF_MAX_HWADDR_LEN);

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		goto adt7420_remove;

	return iio_app_run(app);

adt7420_remove:
	adt7420_iio_remove(adt7420_iio_desc);

	return ret;
}
