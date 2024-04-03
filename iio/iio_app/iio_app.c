/***************************************************************************//**
 *   @file   iio_app.c
 *   @brief  C file of iio_app
 *   @author Mihail Chindris (mihail.chindris@analog.com)
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

#ifdef IIO_SUPPORT

#include <stdlib.h>
#include <stdio.h>
#include "iio_app.h"
#include "parameters.h"
#include "no_os_alloc.h"

#if defined(ADUCM_PLATFORM)
#include "aducm3029_uart.h"
#include "aducm3029_irq.h"
#endif
#if defined(XILINX_PLATFORM)
#include "xilinx_uart.h"
#include "xilinx_irq.h"
#endif
#if defined(STM32_PLATFORM)
#include <errno.h>
#include "stm32_uart.h"
#include "stm32_irq.h"
#endif
#if defined(MAXIM_PLATFORM)
#include "maxim_irq.h"
#include "maxim_uart.h"
#endif

#ifdef NO_OS_NETWORKING
/* Fix: Use static buffers instead of calloc for new connections */
#warning "iio may not work with WIFI on aducm3029."
#ifdef ADUCM_PLATFORM
#include "wifi.h"
#endif
#include "tcp_socket.h"
#endif

#ifdef LINUX_PLATFORM
#include "linux_socket.h"
#include "tcp_socket.h"
#endif

#ifdef NO_OS_LWIP_NETWORKING
#include "lwip_socket.h"
#endif

// The default baudrate iio_app will use to print messages to console.
#define UART_BAUDRATE_DEFAULT	115200
#define UART_STOPBITS_DEFAULT	NO_OS_UART_STOP_1_BIT

static inline uint32_t _calc_uart_xfer_time(uint32_t len, uint32_t baudrate)
{
	uint32_t ms = 1000ul * len * 8 / UART_BAUDRATE_DEFAULT;
	ms += ms / 10; // overhead
	return ms;
}

#if !defined(LINUX_PLATFORM) && !defined(NO_OS_NETWORKING) && !defined(NO_OS_USB_UART)
static int32_t iio_print_uart_info_message(struct no_os_uart_desc **uart_desc,
		struct no_os_uart_init_param *user_uart_params,
		char *message, int32_t msglen)
{
	int32_t status;
	uint32_t delay_ms;

	status = no_os_uart_write(*uart_desc, (uint8_t *)message, msglen);
	if (status < 0)
		return status;

	delay_ms = _calc_uart_xfer_time(msglen, UART_BAUDRATE_DEFAULT);
	no_os_mdelay(delay_ms);

	/** Reinitialize UART with parameters required by the IIO application */
	no_os_uart_remove(*uart_desc);
	return no_os_uart_init(uart_desc, user_uart_params);
}
#endif

static int32_t print_uart_hello_message(struct no_os_uart_desc **uart_desc,
					struct no_os_uart_init_param *user_uart_params)
{
#if defined(LINUX_PLATFORM) || defined(NO_OS_NETWORKING) || defined(NO_OS_LWIP_NETWORKING) || defined(NO_OS_USB_UART)
	return 0;
#else
	const char *uart_data_size[] = { "5", "6", "7", "8", "9" };
	const char *uart_parity[] = { "none", "mark", "space", "odd", "even" };
	const char *uart_stop[] = { "1", "2" };
	char message[512];
	uint32_t msglen = sprintf(message,
				  "Running IIOD server...\n"
				  "If successful, you may connect an IIO client application by:\n"
				  "1. Disconnecting the serial terminal you use to view this message.\n"
				  "2. Connecting the IIO client application using the serial backend configured as shown:\n"
				  "\tBaudrate: %lu\n"
				  "\tData size: %s bits\n"
				  "\tParity: %s\n"
				  "\tStop bits: %s\n"
				  "\tFlow control: none\n",
				  user_uart_params->baud_rate,
				  uart_data_size[user_uart_params->size],
				  uart_parity[user_uart_params->parity],
				  uart_stop[user_uart_params->stop]);

	return iio_print_uart_info_message(uart_desc, user_uart_params,
					   message, msglen);
#endif
}

static int32_t print_uart_error_message(struct no_os_uart_desc **uart_desc,
					struct no_os_uart_init_param *user_uart_params,
					int32_t status)
{
	char message[512];
	uint32_t msglen = sprintf(message,
				  "IIOD server failed with code %d.\n",
				  (int)status);
#if defined(LINUX_PLATFORM) || defined(NO_OS_NETWORKING) || defined(NO_OS_LWIP_NETWORKING) || defined(NO_OS_USB_UART)
	(void)msglen;
	printf("%s", message);
	return 0;
#else
	return iio_print_uart_info_message(uart_desc, user_uart_params,
					   message, msglen);
#endif
}

#if defined(NO_OS_LWIP_NETWORKING)
static int32_t lwip_network_setup(struct iio_app_desc *app,
				  struct iio_app_init_param param,
				  struct iio_init_param *iio_init_param)
{
	static struct tcp_socket_init_param socket_param;
	static struct lwip_network_desc lwip_desc;
	static bool is_initialized = false;
	int ret;

	if (NO_OS_LWIP_INIT_ONETIME && is_initialized) {
		socket_param.net = &lwip_desc.no_os_net;
	} else {
		ret = no_os_lwip_init(&app->lwip_desc, &param.lwip_param);
		if (ret)
			return ret;

		socket_param.net = &app->lwip_desc->no_os_net;
		memcpy(&lwip_desc, app->lwip_desc, sizeof(lwip_desc));
	}

	is_initialized = true;
	socket_param.max_buff_size = 0;

	iio_init_param->phy_type = USE_NETWORK;
	iio_init_param->tcp_socket_init_param = &socket_param;

	return 0;
}
#endif

#if defined(NO_OS_NETWORKING) || defined(LINUX_PLATFORM)
static int32_t network_setup(struct iio_init_param *iio_init_param,
			     struct no_os_uart_desc *uart_desc,
			     void *irq_desc)
{
	static struct tcp_socket_init_param socket_param;

#ifdef LINUX_PLATFORM
	socket_param.net = &linux_net;
#endif
#ifdef ADUCM_PLATFORM
	int32_t status;
	static struct wifi_desc *wifi;
	struct wifi_init_param wifi_param = {
		.irq_desc = irq_desc,
		.uart_desc = uart_desc,
		.uart_irq_conf = uart_desc,
		.uart_irq_id = UART_IRQ_ID,
		.sw_reset_en = true
	};
	status = wifi_init(&wifi, &wifi_param);
	if (status < 0)
		return status;

	status = wifi_connect(wifi, WIFI_SSID, WIFI_PWD);
	if (status < 0)
		return status;

	char buff[100];
	wifi_get_ip(wifi, buff, 100);
	printf("iiod ip is: %s\n", buff);

	wifi_get_network_interface(wifi, &socket_param.net);
#endif

	socket_param.max_buff_size = 0;
	iio_init_param->phy_type = USE_NETWORK;
	iio_init_param->tcp_socket_init_param = &socket_param;

	return 0;
}
#endif

static int32_t uart_setup(struct no_os_uart_desc **uart_desc,
			  struct no_os_uart_init_param *uart_init_par)
{
#if defined(LINUX_PLATFORM) || defined(NO_OS_LWIP_NETWORKING)
	*uart_desc = NULL;
	return 0;
#endif
	struct no_os_uart_init_param luart_par = {
		.device_id = uart_init_par->device_id,
		/* TODO: remove this ifdef when asynchrounous rx is implemented on every platform. */
#if defined(STM32_PLATFORM) || defined(MAXIM_PLATFORM) || defined(ADUCM_PLATFORM) || defined(PICO_PLATFORM)
		.irq_id = uart_init_par->irq_id,
		.asynchronous_rx = true,
#endif
		.baud_rate = UART_BAUDRATE_DEFAULT,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = uart_init_par->platform_ops,
#ifndef ADUCM_PLATFORM
		.extra = uart_init_par->extra
#endif
	};

	return no_os_uart_init(uart_desc, &luart_par);
}

#if defined(ADUCM_PLATFORM) || (defined(STM32_PLATFORM)) || defined(MAXIM_PLATFORM)
static int32_t irq_setup(struct no_os_irq_ctrl_desc **irq_desc)
{
	int32_t status;
#if defined(ADUCM_PLATFORM)
	void *platform_irq_init_par = NULL;
	const struct no_os_irq_platform_ops *platform_irq_ops = &aducm_irq_ops;
#elif defined(STM32_PLATFORM)
	void *platform_irq_init_par = NULL;
	const struct no_os_irq_platform_ops *platform_irq_ops = &stm32_irq_ops;
#elif defined(MAXIM_PLATFORM)
	void *platform_irq_init_par = NULL;
	const struct no_os_irq_platform_ops *platform_irq_ops = &max_irq_ops;
#endif

	struct no_os_irq_init_param irq_init_param = {
		.irq_ctrl_id = INTC_DEVICE_ID,
		.platform_ops = platform_irq_ops,
		.extra = platform_irq_init_par
	};

	status = no_os_irq_ctrl_init(irq_desc, &irq_init_param);
	if (status < 0)
		return status;

	return no_os_irq_global_enable(*irq_desc);
}
#endif

/**
 * @brief Register devices for an iio application
 *
 * Configuration for communication is done in parameters.h
 * @param app - the iio application descriptor
 * @param app_init_param - the iio application initialization parameters
 * @return 0 on success, negative value otherwise
 */
int iio_app_init(struct iio_app_desc **app,
		 struct iio_app_init_param app_init_param)
{
	struct iio_device_init *iio_init_devs;
	struct iio_init_param iio_init_param;
	struct no_os_uart_desc *uart_desc;
	struct iio_app_desc *application;
	struct iio_data_buffer *buff;
	unsigned int i;
	int status;
	void *irq_desc = app_init_param.irq_desc;

	application = (struct iio_app_desc *)no_os_calloc(1, sizeof(*application));
	if (!application)
		return -ENOMEM;

	application->post_step_callback = app_init_param.post_step_callback;
	application->arg = app_init_param.arg;

#if defined(ADUCM_PLATFORM) || defined(STM32_PLATFORM)
	/* Only one irq controller can exist and be initialized in
	 * any of the iio_devices. */
	for (i = 0; i < app_init_param.nb_devices; i++) {
		if (app_init_param.devices[i].dev_descriptor->irq_desc) {
			irq_desc = (struct no_os_irq_ctrl_desc *)
				   app_init_param.devices[i].dev_descriptor->irq_desc;
			break;
		}
	}

	if (!irq_desc) {
		status = irq_setup((struct no_os_irq_ctrl_desc **)&irq_desc);
		if (status < 0)
			return status;
	}
#endif

	status = uart_setup(&uart_desc, &app_init_param.uart_init_params);
	if (status < 0)
		goto error_uart;

	status = print_uart_hello_message(&uart_desc,
					  &app_init_param.uart_init_params);
	if (status < 0)
		goto error;

	application->uart_desc = uart_desc;
#if defined(NO_OS_LWIP_NETWORKING)
	status = lwip_network_setup(application, app_init_param, &iio_init_param);
	if (status)
		goto error;
#elif defined(NO_OS_NETWORKING) || defined(LINUX_PLATFORM)
	status = network_setup(&iio_init_param, uart_desc, application->irq_desc);
	if(status < 0)
		goto error;
#else
	iio_init_param.phy_type = USE_UART;
	iio_init_param.uart_desc = uart_desc;
#endif

	iio_init_devs = no_os_calloc(app_init_param.nb_devices, sizeof(*iio_init_devs));
	if (!iio_init_devs) {
		status = -ENOMEM;
		goto error;
	}

	for (i = 0; i < app_init_param.nb_devices; ++i) {
		/*
		 * iio_app_device is from iio_app.h and we don't want to include
		 * this in iio.h.
		 * At the moment iio_device_init has the same parameters but
		 * it will change.
		 * When changes are done in iio. iio_app_device may be removed
		 *iio_init_param and use iio_device_init instead.
		 * This way faster changes can be done without changing all
		 * project for each change.
		 */
		iio_init_devs[i].name = app_init_param.devices[i].name;
		iio_init_devs[i].dev = app_init_param.devices[i].dev;
		iio_init_devs[i].dev_descriptor = app_init_param.devices[i].dev_descriptor;
		iio_init_devs[i].trigger_id = app_init_param.devices[i].default_trigger_id;
		buff = app_init_param.devices[i].read_buff ?
		       app_init_param.devices[i].read_buff :
		       app_init_param.devices[i].write_buff;
		if (buff) {
			iio_init_devs[i].raw_buf = buff->buff;
			iio_init_devs[i].raw_buf_len = buff->size;
		} else {
			iio_init_devs[i].raw_buf = NULL;
			iio_init_devs[i].raw_buf_len = 0;
		}
	}

	iio_init_param.devs = iio_init_devs;
	iio_init_param.nb_devs = app_init_param.nb_devices;
	iio_init_param.trigs = app_init_param.trigs;
	iio_init_param.nb_trigs = app_init_param.nb_trigs;
	iio_init_param.ctx_attrs = app_init_param.ctx_attrs;
	iio_init_param.nb_ctx_attr = app_init_param.nb_ctx_attr;

	status = iio_init(&application->iio_desc, &iio_init_param);
	if(status < 0)
		goto error;

	no_os_free(iio_init_devs);

	*app = application;

	return 0;
error:
	/** We might have to reinit UART, settings might have changed for IIO */
	uart_setup(&uart_desc, &app_init_param.uart_init_params);
error_uart:
	no_os_free(application);

	status = print_uart_error_message(&uart_desc,
					  &app_init_param.uart_init_params, status);
	no_os_uart_remove(uart_desc);

	return status;
}

/**
 * @brief Start an IIO application
 *
 * Configuration for communication is done through iio_app_init_param
 * @param app - the iio application parameters
 * @return 0 on success, negative value otherwise
 */
int iio_app_run(struct iio_app_desc *app)
{
	int status;

	do {
		status = iio_step(app->iio_desc);
		if (status && status != -EAGAIN && status != -ENOTCONN
		    && status != -NO_OS_EOVERRUN)
			return status;
		if (app->post_step_callback) {
			status = app->post_step_callback(app->arg);
			if (status)
				return status;
		}
	} while (true);
}

/**
 * @brief Remove resources allocated by the IIO application
 *
 * Configuration for communication is done through iio_app_init_param
 * @param app - the iio application parameters
 * @return 0 on success, negative value otherwise
 */
int iio_app_remove(struct iio_app_desc *app)
{
	int ret;

#if defined(ADUCM_PLATFORM) || (defined(XILINX_PLATFORM) && !defined(PLATFORM_MB)) || defined(STM32_PLATFORM)
	ret = no_os_irq_ctrl_remove(app->irq_desc);
	if (ret)
		return ret;
#endif

	if (app->uart_desc) {
		ret = no_os_uart_remove(app->uart_desc);
		if (ret)
			return ret;
	}

	ret = iio_remove(app->iio_desc);
	if (ret)
		return ret;

	no_os_free(app);

	return 0;
}

#endif
