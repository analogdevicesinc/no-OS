/***************************************************************************//**
 *   @file   cn0531/src/main.c
 *   @brief  Implementation of Main Function.
 *   @author Drimbarean Andrei (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
#include "no_os_error.h"
#include "iio.h"
#include "no_os_irq.h"
#include "aducm3029_irq.h"
#include "no_os_uart.h"
#include "aducm3029_uart.h"
#include "ad5791.h"
#include "iio_ad5791.h"
#include "aducm3029_spi.h"
#include "iio_app.h"
#include "parameters.h"

#define MAX_SIZE_BASE_ADDR		1024

static uint8_t out_buff[MAX_SIZE_BASE_ADDR];

#define DAC_DDR_BASEADDR	((uint32_t)out_buff)
#define NUMBER_OF_DEVICES	1

/***************************************************************************//**
 * @brief main
*******************************************************************************/
int main(void)
{
	int32_t status, val;
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };

	struct no_os_uart_init_param uart_ip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.asynchronous_rx = true,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = NULL,
		.platform_ops = &aducm_uart_ops,
	};

	status = platform_init();
	if (NO_OS_IS_ERR_VALUE(status))
		return status;

	struct ad5791_dev *ad5791_device;
	struct aducm_spi_init_param aducm_spi_ini = {
		.continuous_mode = true,
		.dma = false,
		.half_duplex = false,
		.master_mode = MASTER
	};
	struct ad5791_init_param ad5791_initial = {
		.act_device = ID_AD5791,
		.gpio_clr.number = 0x10,
		.gpio_clr.extra = NULL,
		.gpio_ldac.number = 0x0E,
		.gpio_ldac.extra = NULL,
		.gpio_reset.number = 0x0C,
		.gpio_reset.extra = NULL,
		.spi_init.device_id = 1,
		.spi_init.chip_select = 0x00,
		.spi_init.extra = &aducm_spi_ini,
		.spi_init.max_speed_hz = 5000000,
		.spi_init.mode = NO_OS_SPI_MODE_3
	};
	struct iio_data_buffer iio_ad5791_read_buff = {
		.buff = DAC_DDR_BASEADDR,
		.size = MAX_SIZE_BASE_ADDR,
	};
	struct ad5791_iio_desc *ad5791_iio_handle;
	struct ad5791_iio_init_param ad5791_iio_ini = {
		.ad5791_initial = &ad5791_initial,
		.vref_mv = 5000,
		.vref_neg_mv = 5000
	};

	status = ad5791_iio_init(&ad5791_iio_handle, &ad5791_iio_ini);
	if (status != 0)
		return status;

	status = ad5791_set_dac_value(ad5791_iio_handle->ad5791_handle, 0);
	if (status != 0)
		return status;

	status = ad5791_get_register_value(ad5791_iio_handle->ad5791_handle,
					   AD5791_REG_CTRL, &val);
	if (status < 0)
		return status;
	val &= ~(AD5791_CTRL_OPGND | AD5791_CTRL_RBUF_MASK);
	status = ad5791_set_register_value(ad5791_iio_handle->ad5791_handle,
					   AD5791_REG_CTRL, (uint32_t)val);
	if (status != 0)
		return status;

	struct iio_app_device devices[] = {
		IIO_APP_DEVICE("ad5791", ad5791_iio_handle,
			       ad5791_iio_handle->ad5791_iio_dev,
			       &iio_ad5791_read_buff, NULL, NULL)
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = uart_ip;

	status = iio_app_init(&app, app_init_param);
	if (status)
		return status;

	return iio_app_run(app);
}

