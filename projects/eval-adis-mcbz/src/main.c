/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main file for eval-adis-mcbz project.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
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
#include <stdio.h>

#include "no_os_util.h"
#include "no_os_i2c.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_delay.h"

#include "stm32_hal.h"
#include "stm32_i2c.h"
#include "stm32_gpio.h"
#include "stm32_spi.h"
#include "stm32_irq.h"
#include "stm32_uart.h"
#include "stm32_usb_uart.h"

#include "adxl38x.h"
#include "iio_adxl38x.h"
#include "iio_app.h"

/***************************************************************************//**
 * @brief Main function execution for the eval-adis-mcbz project.
 *
 * @return ret - Return non-zero exit code in case of error.
*******************************************************************************/
int main(void)
{
	struct no_os_uart_init_param uart_init = {
		.device_id = 5,
		.irq_id = UART5_IRQn,
		.asynchronous_rx = true,
		.baud_rate = 115200,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = NULL,
		.platform_ops = &stm32_usb_uart_ops,
	};

	struct stm32_i2c_init_param i2c_extra = {
		.i2c_timing = 0x0040486C,
	};

	const struct no_os_i2c_init_param i2c_init = {
		.device_id = 1,
		.max_speed_hz = 100000,
		.slave_address = 0x1D,
		.platform_ops = &stm32_i2c_ops,
		.extra = &i2c_extra,
	};

	struct stm32_spi_init_param spi_extra = {
		.chip_select_port = 0,
		.get_input_clock = HAL_RCC_GetPCLK2Freq,
	};

	const struct no_os_spi_init_param spi_init = {
		.device_id = 1,
		.max_speed_hz = 1000000,
		.chip_select = 15,
		.mode = NO_OS_SPI_MODE_0,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = &stm32_spi_ops,
		.extra = &spi_extra,
	};

#if 0
	struct adxl38x_init_param adxl38x_init = {
		.comm_type = ADXL38X_I2C_COMM,
		.dev_type = ID_ADXL380,
		.comm_init.i2c_init = i2c_init,
	};
#else
	struct adxl38x_init_param adxl38x_init = {
		.comm_type = ADXL38X_SPI_COMM,
		.dev_type = ID_ADXL382,
		.comm_init.spi_init = spi_init,
	};
#endif

	struct stm32_gpio_init_param extra_gpio_init = {
		.mode = GPIO_MODE_OUTPUT_PP,
		.speed = GPIO_SPEED_FREQ_VERY_HIGH,
	};

	struct no_os_gpio_init_param gpio_init = {
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &stm32_gpio_ops,
		.extra = &extra_gpio_init,
	};

	struct adxl38x_iio_dev *adxl38x_iio_desc;
	struct adxl38x_iio_dev_init_param adxl38x_iio_ip;
	struct iio_app_init_param app_init_param = { 0 };
	struct iio_app_desc *app;
	int ret;

	stm32_init();

	struct no_os_gpio_desc *rgb1_r_gpio;	// LD6 blue
	struct no_os_gpio_desc *rgb1_g_gpio;	// LD6 red
	struct no_os_gpio_desc *rgb1_b_gpio;	// LD6 green

	gpio_init.port = 2;
	gpio_init.number = 0;
	ret = no_os_gpio_get(&rgb1_r_gpio, &gpio_init);
	if (ret)
		return ret;
	gpio_init.port = 0;
	gpio_init.number = 7;
	ret = no_os_gpio_get(&rgb1_g_gpio, &gpio_init);
	if (ret)
		return ret;
	gpio_init.port = 1;
	gpio_init.number = 1;
	ret = no_os_gpio_get(&rgb1_b_gpio, &gpio_init);
	if (ret)
		return ret;

	no_os_gpio_set_value(rgb1_r_gpio, 1);
	no_os_gpio_set_value(rgb1_g_gpio, 1);
	no_os_gpio_set_value(rgb1_b_gpio, 1);
	
	struct no_os_gpio_desc *rgb2_r_gpio;	// LD5 blue
	struct no_os_gpio_desc *rgb2_g_gpio;	// LD5 red
	struct no_os_gpio_desc *rgb2_b_gpio;	// LD5 green

	gpio_init.port = 3;
	gpio_init.number = 8;
	ret = no_os_gpio_get(&rgb2_r_gpio, &gpio_init);
	if (ret)
		return ret;
	gpio_init.port = 3;
	gpio_init.number = 12;
	ret = no_os_gpio_get(&rgb2_g_gpio, &gpio_init);
	if (ret)
		return ret;
	gpio_init.port = 3;
	gpio_init.number = 13;
	ret = no_os_gpio_get(&rgb2_b_gpio, &gpio_init);
	if (ret)
		return ret;

	no_os_gpio_set_value(rgb2_r_gpio, 1);
	no_os_gpio_set_value(rgb2_g_gpio, 1);
	no_os_gpio_set_value(rgb2_b_gpio, 1);

	struct no_os_gpio_desc *oeh_m_gpio;
	gpio_init.port = 4;
	gpio_init.number = 2;
	ret = no_os_gpio_get(&oeh_m_gpio, &gpio_init);
	if (ret)
		return ret;
	no_os_gpio_set_value(oeh_m_gpio, 1);

	struct no_os_gpio_desc *oel_m_gpio;
	gpio_init.port = 1;
	gpio_init.number = 7;
	ret = no_os_gpio_get(&oel_m_gpio, &gpio_init);
	if (ret)
		return ret;
	no_os_gpio_set_value(oel_m_gpio, 1);

	adxl38x_iio_ip.adxl38x_dev_init = &adxl38x_init;
	ret = adxl38x_iio_init(&adxl38x_iio_desc, &adxl38x_iio_ip);
	if (ret)
		return ret;

	struct iio_app_device iio_devices[] = {
		{
			.name = "adxl38x",
			.dev = adxl38x_iio_desc,
			.dev_descriptor = adxl38x_iio_desc->iio_dev,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = uart_init;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
}
