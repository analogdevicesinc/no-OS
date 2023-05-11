/***************************************************************************//**
 *   @file   ad413x/src/app/main.c
 *   @brief  Implementation of Main Function.
 *   @author Porumb Andrei (andrei.porumb@analog.com)
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
#ifdef IIO_SUPPORT
#include "iio.h"
#include "iio_ad413x.h"
#include "iio_app.h"
#include "xilinx_uart.h"
#endif
#include "no_os_irq.h"
#include "xilinx_irq.h"
#include "xilinx_gpio_irq.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "xilinx_gpio.h"
#include "xilinx_spi.h"
#include "ad413x.h"
#include "no_os_error.h"
#include "xparameters.h"
#include "parameters.h"
#include "no_os_print_log.h"
#include <stdint.h>
#include <inttypes.h>

#define DATA_BUFFER_SIZE 400

static uint8_t in_buff[DATA_BUFFER_SIZE*16*sizeof(int)];

#define ADC_DDR_BASEADDR	((uint32_t)in_buff)

/***************************************************************************//**
 * @brief main
*******************************************************************************/
int main()
{
	int ret;
	uint32_t buffer[20];
	int32_t i;

	/* SPI instance */
	struct xil_spi_init_param spi_extra = {
		.type = SPI_PS,
		.flags = 0U
	};
	struct no_os_spi_init_param spi_ip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 100000,
		.mode = NO_OS_SPI_MODE_3,
		.chip_select = 0U,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = &xil_spi_ops,
		.extra = &spi_extra
	};

	struct xil_gpio_init_param xil_gpio_param = {
		.type = GPIO_PS,
		.device_id = XPAR_PS7_GPIO_0_DEVICE_ID
	};

	/* GPIO_RDY instance desc */
	struct no_os_gpio_init_param  gpio_init = {
		.number = RDY_PIN,
		.platform_ops = &xil_gpio_ops,
		.extra = &xil_gpio_param
	};

	/* Device AD413X instance. */
	struct ad413x_dev *ad413x_dev;
	struct ad413x_init_param ad413x_dev_ip = {
		.spi_init = &spi_ip,
		.chip_id = AD4130_8,
		.mclk = AD413X_INT_76_8_KHZ_OUT_OFF,
		.rdy_pin_init = &gpio_init,
		.preset[0] = { AD413X_INT_REF, AD413X_GAIN_1, AD413X_SYNC4_STANDALONE, AD413X_32_MCLK },
		.preset[1] = { AD413X_REFOUT_AVSS, AD413X_GAIN_1, AD413X_SYNC3_PF1, AD413X_128_MCLK },
		.ch[0]  = { AD413X_PRESET_0, ENABLE,  AD413X_AIN0,  AD413X_DGND },
		.ch[1]  = { AD413X_PRESET_0, ENABLE,  AD413X_AIN1,  AD413X_DGND },
		.ch[2]  = { AD413X_PRESET_0, DISABLE, AD413X_AIN2,  AD413X_DGND },
		.ch[3]  = { AD413X_PRESET_0, DISABLE, AD413X_AIN3,  AD413X_DGND },
		.ch[4]  = { AD413X_PRESET_0, DISABLE, AD413X_AIN4,  AD413X_DGND },
		.ch[5]  = { AD413X_PRESET_0, DISABLE, AD413X_AIN5,  AD413X_DGND },
		.ch[6]  = { AD413X_PRESET_0, DISABLE, AD413X_AIN6,  AD413X_DGND },
		.ch[7]  = { AD413X_PRESET_0, DISABLE, AD413X_AIN7,  AD413X_DGND },
		.ch[8]  = { AD413X_PRESET_0, DISABLE, AD413X_AIN8,  AD413X_DGND },
		.ch[9]  = { AD413X_PRESET_0, DISABLE, AD413X_AIN9,  AD413X_DGND },
		.ch[10] = { AD413X_PRESET_0, DISABLE, AD413X_AIN10, AD413X_DGND },
		.ch[11] = { AD413X_PRESET_0, DISABLE, AD413X_AIN11, AD413X_DGND },
		.ch[12] = { AD413X_PRESET_0, DISABLE, AD413X_AIN12, AD413X_DGND },
		.ch[13] = { AD413X_PRESET_0, DISABLE, AD413X_AIN13, AD413X_DGND },
		.ch[14] = { AD413X_PRESET_0, DISABLE, AD413X_AIN14, AD413X_DGND },
		.ch[15] = { AD413X_PRESET_0, DISABLE, AD413X_AIN15, AD413X_DGND },
		.bipolar = DISABLE,
		.int_ref = AD413X_INTREF_2_5V,
		.data_stat = ENABLE,
		.spi_crc_en = ENABLE
	};

#ifndef IIO_SUPPORT
	ret = ad413x_init(&ad413x_dev, ad413x_dev_ip);
	if (ret)
		return -1;

	ret = ad413x_continuous_conv(ad413x_dev, buffer, 2, 10);
	if (ret)
		return -1;

	for(i = 0; i < 20; i++)
		printf("DATA[%"PRIi32"], chan %d = %f V \n", i, i % 3, buffer[i],
		       (float)buffer[i] / 0xFFFFFF * 2.5);

	return 0;
#else

	/* IIO device */
	struct iio_app_desc *app;
	struct ad413x_iio_dev *adciio = NULL;
	struct ad413x_iio_init_param adciio_init;
	struct iio_app_init_param app_init_param = { 0 };

	struct xil_uart_init_param platform_uart_init_par = {
		.type = UART_PS,
		.irq_id = UART_IRQ_ID
	};

	struct no_os_uart_init_param iio_uart_ip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = &platform_uart_init_par,
		.platform_ops = &xil_uart_ops,
	};

	struct iio_data_buffer iio_ad413x_read_buff = {
		.buff = (void *)in_buff,
		.size = DATA_BUFFER_SIZE*16*sizeof(int)
	};

	adciio_init.ad413x_ip = ad413x_dev_ip;

	ret = ad413x_iio_init(&adciio, adciio_init);
	if (ret < 0)
		goto error;

	struct iio_app_device iio_devices[] = {
		{
			.name = "ad4130",
			.dev = adciio,
			.dev_descriptor = adciio->iio_dev,
			.read_buff =  &iio_ad413x_read_buff,
			.write_buff = NULL,
		}
	};

	app_init_param.devices = iio_devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
	app_init_param.uart_init_params = iio_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
error:
	ad413x_iio_remove(adciio);
	return ret;
#endif
}
