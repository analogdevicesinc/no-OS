/***************************************************************************//**
 *   @file   ad7124-8pmdz/src/main.c
 *   @brief  Implementation of Main Function.
 *   @author Drimbarean Andrei (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#include "app_config.h"
#include "error.h"
#include "iio.h"
#include "irq.h"
#include "irq_extra.h"
#include "uart.h"
#include "uart_extra.h"
#include "iio_adpd188.h"
#include "i2c.h"
#include "iio_app.h"

#define MAX_SIZE_BASE_ADDR		1024

static uint8_t in_buff[MAX_SIZE_BASE_ADDR];

#define ADC_DDR_BASEADDR	((uint32_t)in_buff)

/***************************************************************************//**
 * @brief main
*******************************************************************************/
int main(void)
{
	int32_t status;

	status = platform_init();
	if (IS_ERR_VALUE(status))
		return status;

	struct adpd188_iio_desc *adpd1080_iio_device;
	struct adpd188_iio_init_param adpd1080_iio_inital = {
		.drv_init_param.device = APDP1080,
		.drv_init_param.phy_opt = ADPD188_I2C,
		.drv_init_param.phy_init.i2c_phy =
		{
			.slave_address = 0x64,
			.max_speed_hz = 400000,
			.platform_ops = NULL,
			.extra = NULL
		},
		.drv_init_param.gpio0_init =
		{
			.number = 0,
			.platform_ops = NULL,
			.extra = 0
		},
		.drv_init_param.gpio1_init =
		{
			.number = 0,
			.platform_ops = NULL,
			.extra = 0
		}
	};
	struct iio_data_buffer iio_adpd1080_read_buff = {
		.buff = ADC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR,
	};

	status = adpd188_iio_init(&adpd1080_iio_device, &adpd1080_iio_inital);
	if (status < 0)
		return status;

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("adpd1080", adpd1080_iio_device, &iio_adpd188_device,
			       &iio_adpd1080_read_buff, NULL)
	};

	return iio_app_run(devices, ARRAY_SIZE(devices));
}

