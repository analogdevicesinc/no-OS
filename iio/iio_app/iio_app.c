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

#undef USE_TCP_SOCKET

#include <stdlib.h>
#include <stdio.h>
#include "iio.h"
#include "iio_app.h"
#include "parameters.h"
#include "no_os_uart.h"
#include "no_os_delay.h"

#if defined(ADUCM_PLATFORM) || defined(XILINX_PLATFORM)
#include "no_os_irq.h"
#include "no_os_error.h"
#include "irq_extra.h"
#include "uart_extra.h"
#endif
#if defined(STM32_PLATFORM)
#include <errno.h>
#include "stm32_uart.h"
#endif

#ifdef USE_TCP_SOCKET
/* Fix: Use static buffers instead of calloc for new connections */
#warning "iio may not work with WIFI on aducm3029."
#include "wifi.h"
#include "tcp_socket.h"
#endif

#ifdef LINUX_PLATFORM
#include "linux_socket.h"
#include "tcp_socket.h"
#include "no_os_error.h"
#endif

// The default baudrate iio_app will use to print messages to console.
#define UART_BAUDRATE_DEFAULT	115200

static inline uint32_t _calc_uart_xfer_time(uint32_t len, uint32_t baudrate)
{
	uint32_t ms = 1000ul * len * 8 / UART_BAUDRATE_DEFAULT;
	ms += ms / 10; // overhead
	return ms;
}

#if !defined(LINUX_PLATFORM) && !defined(USE_TCP_SOCKET)
static int32_t iio_print_uart_info_message(struct no_os_uart_desc **uart_desc,
		struct no_os_uart_init_param *uart_init_par,
		char *message, int32_t msglen)
{
	int32_t status;
	uint32_t delay_ms;

	if (UART_BAUDRATE_DEFAULT != UART_BAUDRATE) {
		no_os_uart_remove(*uart_desc);

		uart_init_par->baud_rate = UART_BAUDRATE_DEFAULT;
		status = no_os_uart_init(uart_desc, uart_init_par);
		if (status < 0)
			return status;
	}
	status = no_os_uart_write(*uart_desc, (uint8_t *)message, msglen);
	if (status < 0)
		return status;

	delay_ms = _calc_uart_xfer_time(msglen, UART_BAUDRATE_DEFAULT);
	no_os_mdelay(delay_ms);
	if (UART_BAUDRATE_DEFAULT != UART_BAUDRATE) {
		no_os_uart_remove(*uart_desc);
		uart_init_par->baud_rate = UART_BAUDRATE;
		return no_os_uart_init(uart_desc, uart_init_par);
	}

	return 0;
}
#endif

static int32_t print_uart_hello_message(struct no_os_uart_desc **uart_desc,
					struct no_os_uart_init_param *uart_init_par)
{
#if defined(LINUX_PLATFORM) || defined(USE_TCP_SOCKET)
	return 0;
#else
	const char *uart_data_size[] = { "5", "6", "7", "8", "9" };
	const char *uart_parity[] = { "none", "mark", "space", "odd", "even" };
	const char *uart_stop[] = { "1", "2" };
	char message[512];
	uint32_t msglen = sprintf(message,
				  "Running TinyIIOD server...\n"
				  "If successful, you may connect an IIO client application by:\n"
				  "1. Disconnecting the serial terminal you use to view this message.\n"
				  "2. Connecting the IIO client application using the serial backend configured as shown:\n"
				  "\tBaudrate: %d\n"
				  "\tData size: %s bits\n"
				  "\tParity: %s\n"
				  "\tStop bits: %s\n"
				  "\tFlow control: none\n",
				  UART_BAUDRATE,
				  uart_data_size[uart_init_par->size],
				  uart_parity[uart_init_par->parity],
				  uart_stop[uart_init_par->stop]);

	return iio_print_uart_info_message(uart_desc, uart_init_par, message,
					   msglen);
#endif
}

static int32_t print_uart_error_message(struct no_os_uart_desc **uart_desc,
					struct no_os_uart_init_param *uart_init_par,
					int32_t status)
{
	char message[512];
	uint32_t msglen = sprintf(message,
				  "TinyIIOD server failed with code %d.\n",
				  (int)status);
#if defined(LINUX_PLATFORM) || defined(USE_TCP_SOCKET)
	(void)msglen;
	printf("%s", message);
	return 0;
#else
	return iio_print_uart_info_message(uart_desc, uart_init_par, message,
					   msglen);
#endif
}

#if defined(USE_TCP_SOCKET) || defined(LINUX_PLATFORM)
static int32_t network_setup(struct iio_init_param *iio_init_param,
			     struct no_os_uart_desc *uart_desc,
			     void *irq_desc)
{
	static struct tcp_socket_init_param socket_param;

#ifdef LINUX_PLATFORM
	socket_param.net = &linux_net;
#else
	int32_t status;
	static struct wifi_desc *wifi;
	struct wifi_init_param wifi_param = {
		.irq_desc = irq_desc,
		.uart_desc = uart_desc,
		.uart_irq_conf = uart_desc,
		.uart_irq_id = UART_IRQ_ID
	};
	status = wifi_init(&wifi, &wifi_param);
	if (status < 0)
		return status;

	status = wifi_connect(wifi, WIFI_SSID, WIFI_PWD);
	if (status < 0)
		return status;

	char buff[100];
	wifi_get_ip(wifi, buff, 100);
	printf("Tinyiiod ip is: %s\n", buff);

	wifi_get_network_interface(wifi, &socket_param.net);
#endif

	socket_param.max_buff_size = 0;
	iio_init_param->phy_type = USE_NETWORK;
	iio_init_param->tcp_socket_init_param = &socket_param;

	return 0;
}
#endif

static int32_t uart_setup(struct no_os_uart_desc **uart_desc,
			  struct no_os_uart_init_param **uart_init_par,
			  void *irq_desc)
{
#ifdef LINUX_PLATFORM
	*uart_desc = NULL;
	return 0;
#else
#ifdef XILINX_PLATFORM
	static struct xil_uart_init_param platform_uart_init_par = {
#ifdef XPAR_XUARTLITE_NUM_INSTANCES
		.type = UART_PL,
#else
		.type = UART_PS,
		.irq_id = UART_IRQ_ID
#endif
	};
	platform_uart_init_par.irq_desc = irq_desc;
#elif defined(ADUCM_PLATFORM)
	static struct aducm_uart_init_param platform_uart_init_par = {
		/* TODO: cleanup, these can be set using generic uart_init_param.*/
		.parity = UART_NO_PARITY,
		.stop_bits = UART_ONE_STOPBIT,
		.word_length = UART_WORDLEN_8BITS
	};
#elif defined(STM32_PLATFORM)
	static struct stm32_uart_init_param platform_uart_init_par = {
		.huart = IIO_APP_HUART,
	};
#endif
	static struct no_os_uart_init_param luart_par = {
		.device_id = UART_DEVICE_ID,
		.irq_id = UART_IRQ_ID,
		.asynchronous_rx = true,
		.baud_rate = UART_BAUDRATE_DEFAULT,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.extra = &platform_uart_init_par
	};
	*uart_init_par = &luart_par;

	return no_os_uart_init(uart_desc, &luart_par);
#endif
}

#if defined(ADUCM_PLATFORM) || (defined(XILINX_PLATFORM) && !defined(PLATFORM_MB))
static int32_t irq_setup(struct no_os_irq_ctrl_desc **irq_desc)
{
	int32_t status;
#if defined(XILINX_PLATFORM) && !defined(PLATFORM_MB)
	struct xil_irq_init_param p = {
		.type = IRQ_PS,
	};
	struct xil_irq_init_param *platform_irq_init_par = &p;
	const struct no_os_irq_platform_ops *platform_irq_ops = &xil_irq_ops;
#elif defined(ADUCM_PLATFORM)
	void *platform_irq_init_par = NULL;
	const struct no_os_irq_platform_ops *platform_irq_ops = &aducm_irq_ops;
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

int32_t iio_app_run(struct iio_app_device *devices, uint32_t len)
{
	int32_t			status;
	struct iio_desc		*iio_desc;
	struct iio_init_param	iio_init_param;
	struct no_os_uart_desc	*uart_desc;
	struct no_os_uart_init_param	*uart_init_par;
	void			*irq_desc = NULL;
	struct iio_device_init	*iio_init_devs;
	uint32_t		i;
	struct iio_data_buffer *buff;

#if defined(ADUCM_PLATFORM) || (defined(XILINX_PLATFORM) && !defined(PLATFORM_MB))
	/* Only one irq controller can exist and be initialized in
	 * any of the iio_devices. */
	for (i = 0; i < len; i++) {
		if (devices[i].dev_descriptor->irq_desc) {
			irq_desc = (struct no_os_irq_ctrl_desc *)devices[i].dev_descriptor->irq_desc;
			break;
		}
	}

	if (!irq_desc) {
		status = irq_setup((struct no_os_irq_ctrl_desc **)&irq_desc);
		if (status < 0)
			return status;
	}
#endif

	status = uart_setup(&uart_desc, &uart_init_par, irq_desc);
	if (status < 0)
		return status;

	status = print_uart_hello_message(&uart_desc, uart_init_par);
	if (status < 0)
		return status;

#if defined(USE_TCP_SOCKET) || defined(LINUX_PLATFORM)
	status = network_setup(&iio_init_param, uart_desc, irq_desc);
	if(status < 0)
		goto error;
#else
	iio_init_param.phy_type = USE_UART;
	iio_init_param.uart_desc = uart_desc;
#endif

	iio_init_devs = calloc(len, sizeof(*iio_init_devs));
	if (!iio_init_devs)
		return -ENOMEM;

	for (i = 0; i < len; ++i) {
		/*
		 * iio_app_device is from iio_app.h and we don't want to include
		 * this in iio.h.
		 * At the moment iio_device_init has the same parameters but
		 * it will change.
		 * When changes are done in iio. iio_app_device may be removed
		 * and use iio_device_init instead.
		 * This way faster changes can be done without changing all
		 * project for each change.
		 */
		iio_init_devs[i].name = devices[i].name;
		iio_init_devs[i].dev = devices[i].dev;
		iio_init_devs[i].dev_descriptor = devices[i].dev_descriptor;
		buff = devices[i].read_buff ? devices[i].read_buff :
		       devices[i].write_buff;
		if (buff) {
			iio_init_devs[i].raw_buf = buff->buff;
			iio_init_devs[i].raw_buf_len = buff->size;
		} else {
			iio_init_devs[i].raw_buf = NULL;
			iio_init_devs[i].raw_buf_len = 0;
		}
	}

	iio_init_param.devs = iio_init_devs;
	iio_init_param.nb_devs = len;
	status = iio_init(&iio_desc, &iio_init_param);
	if(status < 0)
		goto error;

	free(iio_init_devs);

	do {
		status = iio_step(iio_desc);
	} while (true);
error:
	status = print_uart_error_message(&uart_desc, uart_init_par, status);
	return status;
}

#endif
