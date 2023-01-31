/***************************************************************************//**
 *   @file   main.c
 *   @brief  EIT main.
 *   @author Darius Berghe (darius.berghe@analog.com)
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
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "parameters.h"
#include "no_os_uart.h"
#include "no_os_gpio.h"
#include "no_os_i2c.h"
#include "no_os_spi.h"
#include "no_os_irq.h"
#include "no_os_delay.h"
#include "no_os_util.h"

#if defined(STM32_PLATFORM)
#include "stm32_spi.h"
#include "stm32_gpio.h"
#include "stm32_uart.h"
#include "stm32_i2c.h"
#include "stm32_irq.h"
#include "stm32_gpio_irq.h"
#include "stm32_uart_stdio.h"
#elif defined(ADUCM_PLATFORM)
#include "platform_init.h"
#include "aducm3029_spi.h"
#include "aducm3029_gpio.h"
#include "aducm3029_uart.h"
#include "aducm3029_i2c.h"
#include "aducm3029_irq.h"
#include "aducm3029_uart_stdio.h"
#endif

#include "app.h"
#include "ad5940.h"

#ifdef IIO_SUPPORT
#include "iio_ad5940.h"
#include "iio_adg2128.h"
#include "iio_app.h"
#endif

struct no_os_spi_desc *spi;
struct no_os_i2c_desc *i2c;
struct no_os_uart_desc *uart;

#if defined(STM32_PLATFORM)
extern UART_HandleTypeDef huart5;
#endif

void ad5940_int_callback(void *ctx)
{
	ucInterrupted = 1;
}

int main(void)
{
	int ret;

#if defined(STM32_PLATFORM)
	stm32_init();
#elif defined(ADUCM_PLATFORM)
	ret = platform_init();
	if (ret)
		return ret;
#endif

#ifndef IIO_SUPPORT
#if defined(STM32_PLATFORM)
	struct stm32_uart_init_param xuip = {
		.huart = &huart5,
		.timeout = 10,
	};
#endif
	struct no_os_uart_init_param uip = {
		.device_id = UART_DEVICE_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
#if defined(STM32_PLATFORM)
		.extra = &xuip,
#endif
		.asynchronous_rx = true,
		.irq_id = UART_IRQ_ID,
#if defined(STM32_PLATFORM)
		.platform_ops = &stm32_uart_ops,
#elif defined(ADUCM_PLATFORM)
		.platform_ops = &aducm_uart_ops,
#endif
	};

	ret = no_os_uart_init(&uart, &uip);
	if (ret < 0)
		return ret;

	no_os_uart_stdio(uart);

	printf("Hello!\n");
#endif
	struct no_os_i2c_init_param i2cip = {
		.device_id = I2C_DEVICE_ID,
		.max_speed_hz = I2C_BAUDRATE,
		.slave_address = 0x70,
#if defined(STM32_PLATFORM)
		.platform_ops = &stm32_i2c_ops,
#elif defined(ADUCM_PLATFORM)
		.platform_ops = &aducm_i2c_ops,
#endif
	};
	ret = no_os_i2c_init(&i2c, &i2cip);
	if (ret)
		goto error;

#if defined(STM32_PLATFORM)
	struct stm32_spi_init_param xsip  = {
		.chip_select_port = SPI_CS_PORT,
		.get_input_clock = HAL_RCC_GetPCLK1Freq,
	};
#elif defined(ADUCM_PLATFORM)
	struct aducm_spi_init_param xsip = {
		.continuous_mode = true,
		.dma = false,
		.half_duplex = false,
		.master_mode = MASTER
	};
#endif
	struct no_os_spi_init_param sip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 3000000,
		.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.mode = NO_OS_SPI_MODE_0,
		.extra = &xsip,
#if defined(STM32_PLATFORM)
		.platform_ops = &stm32_spi_ops,
#elif defined(ADUCM_PLATFORM)
		.platform_ops = &aducm_spi_ops,
#endif
		.chip_select = SPI_CS,
	};

#if defined(STM32_PLATFORM)
	struct stm32_gpio_init_param reset_xgip = {
		.mode = GPIO_MODE_OUTPUT_PP,
		.speed = GPIO_SPEED_FREQ_VERY_HIGH,
	};
#endif
	struct no_os_gpio_init_param reset_gip = {
		.port = 3,
		.number = RESET_PIN,
		.pull = NO_OS_PULL_NONE,
#if defined(STM32_PLATFORM)
		.platform_ops = &stm32_gpio_ops,
		.extra = &reset_xgip,
#elif defined(ADUCM_PLATFORM)
		.platform_ops = &aducm_gpio_ops,
#endif
	};

#if defined(STM32_PLATFORM)
	struct stm32_gpio_init_param gp0_xgip = {
		.mode = GPIO_MODE_INPUT,
		.speed = GPIO_SPEED_FREQ_VERY_HIGH,
	};
#endif

	struct no_os_gpio_init_param gp0_gip = {
		.port = 6,
		.number = GP0_PIN,
		.pull = NO_OS_PULL_NONE,
#if defined(STM32_PLATFORM)
		.platform_ops = &stm32_gpio_ops,
		.extra = &gp0_xgip,
#elif defined(ADUCM_PLATFORM)
		.platform_ops = &aducm_gpio_ops,
#endif
	};

	// aducm3029 hack, need to initialize gpio before irq, otherwise it doesn't work
#if defined(ADUCM_PLATFORM)
	struct no_os_gpio_desc *gp0;
	no_os_gpio_get(&gp0, &gp0_gip);
#endif

	struct ad5940_init_param ad5940_ip = {
		.spi_init = sip,
		.reset_gpio_init = reset_gip,
		.gp0_gpio_init = gp0_gip,
	};
#ifndef IIO_SUPPORT
	/* interrupt controller  */
	struct no_os_irq_init_param nvic_ip = {
		.irq_ctrl_id = INTC_DEVICE_ID,
		.platform_ops = IRQ_OPS,
	};
	struct no_os_irq_ctrl_desc *nvic;
	ret = no_os_irq_ctrl_init(&nvic, &nvic_ip);
	if (ret < 0)
		return ret;

	/* gpio interrupt controller  */
#if defined(STM32_PLATFORM)
	struct stm32_gpio_irq_init_param gic_xip = {
		.port_nb = 6, /* port G */
	};
#endif
	struct no_os_irq_init_param gic_ip = {
#if defined(STM32_PLATFORM)
		.irq_ctrl_id = GP0_PIN,
#elif defined(ADUCM_PLATFORM)
		.irq_ctrl_id = INTC_DEVICE_ID,
#endif
		.platform_ops = GPIO_IRQ_OPS,
#if defined(STM32_PLATFORM)
		.extra = &gic_xip,
#endif
	};
	struct no_os_irq_ctrl_desc *gic;
	ret = no_os_irq_ctrl_init(&gic, &gic_ip);
	if (ret < 0)
		return ret;

	/* callback */
	struct no_os_callback_desc int_cb = {
		.callback = ad5940_int_callback,
		.event = NO_OS_EVT_GPIO,
		.peripheral = NO_OS_GPIO_IRQ,
	};

	ret = no_os_irq_register_callback(gic, INT_IRQn, &int_cb);
	if (ret < 0)
		return ret;

	ret = no_os_irq_trigger_level_set(gic, INT_IRQn, NO_OS_IRQ_EDGE_FALLING);
	if (ret < 0)
		return ret;

	ret = no_os_irq_enable(gic, INT_IRQn);
	if (ret < 0)
		return ret;

	ret = app_main(i2c, &ad5940_ip);
	if (ret < 0)
		goto error;
#else
#if defined(STM32_PLATFORM)
	struct stm32_uart_init_param uart_extra_ip = {
		.huart = &huart5,
	};
#endif

	struct no_os_uart_init_param iio_uart_ip = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.baud_rate = UART_BAUDRATE,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
#if defined(STM32_PLATFORM)
		.extra = &uart_extra_ip,
#else
		.extra = NULL,
#endif
		.platform_ops = UART_OPS
	};
	struct iio_app_desc *app;
	struct iio_app_init_param app_init_param = { 0 };
	struct ad5940_iio_dev *ad5940_iio = NULL;
	struct ad5940_iio_init_param ad5940_iio_ip = {
		.ad5940_init = &ad5940_ip,
	};
	ret = ad5940_iio_init(&ad5940_iio, &ad5940_iio_ip);
	if (ret < 0)
		goto error;

	struct adg2128_iio_dev *adg2128_iio = NULL;
	ret = adg2128_iio_init(&adg2128_iio, i2c);
	if (ret < 0)
		goto error;

	struct iio_app_device devices[] = {
		{
			.name = "ad5940",
			.dev = ad5940_iio,
			.dev_descriptor = ad5940_iio->iio,
			.read_buff = NULL,
			.write_buff = NULL
		},
		{
			.name = "adg2128",
			.dev = adg2128_iio,
			.dev_descriptor = adg2128_iio->iio_dev,
		},
	};

	app_init_param.devices = devices;
	app_init_param.nb_devices = NO_OS_ARRAY_SIZE(devices);
	app_init_param.uart_init_params = iio_uart_ip;

	ret = iio_app_init(&app, app_init_param);
	if (ret)
		return ret;

	return iio_app_run(app);
#endif

	printf("Bye!\n");
	return 0;
error:
	printf("Bye! (%d)\n", ret);
	return ret;
}

