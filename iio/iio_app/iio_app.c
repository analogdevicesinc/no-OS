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
#include "iio.h"
#include "iio_app.h"
#include "parameters.h"
#include "no_os_uart.h"
#include "no_os_delay.h"

#if defined(ADUCM_PLATFORM)
#include "aducm3029_uart.h"
#include "aducm3029_irq.h"
#endif
#if defined(ADUCM_PLATFORM) || defined(XILINX_PLATFORM)
#include "no_os_irq.h"
#include "no_os_error.h"
#endif
#if defined(XILINX_PLATFORM)
#include "xilinx_uart.h"
#include "xilinx_irq.h"
#endif
#if defined(STM32_PLATFORM)
#include <errno.h>
#include "stm32_uart.h"
#include "stm32_irq.h"
#include "no_os_irq.h"
#include "no_os_error.h"
#endif

#if defined(MAXIM_PLATFORM)
#include "maxim_irq.h"
#include "maxim_uart.h"
#include "no_os_irq.h"
#include "no_os_error.h"
#endif

#ifdef NO_OS_NETWORKING
/* Fix: Use static buffers instead of calloc for new connections */
#warning "iio may not work with WIFI on aducm3029."
#ifdef ADUCM3029_PLATFORM
#include "wifi.h"
#endif
#include "tcp_socket.h"
#endif

#ifdef LINUX_PLATFORM
#include "linux_socket.h"
#include "tcp_socket.h"
#include "no_os_error.h"
#endif

#ifdef PICO_PLATFORM
#include "no_os_error.h"
#endif

#ifdef ADUCM_PLATFORM
#define UART_OPS &aducm_uart_ops
#elif LINUX_PLATFORM
#define UART_OPS &linux_uart_ops
#elif defined MAXIM_PLATFORM
#define UART_OPS &max_uart_ops
#elif defined PICO_PLATFORM
#define UART_OPS &pico_uart_ops
#elif defined STM32_PLATFORM
#define UART_OPS &stm32_uart_ops
#elif defined XILINX_PLATFORM
#define UART_OPS &xil_uart_ops
#else
#error "UART_OPS not defined"
#endif

// The default baudrate iio_app will use to print messages to console.
#define UART_BAUDRATE_DEFAULT	115200

static inline uint32_t _calc_uart_xfer_time(uint32_t len, uint32_t baudrate)
{
	uint32_t ms = 1000ul * len * 8 / UART_BAUDRATE_DEFAULT;
	ms += ms / 10; // overhead
	return ms;
}

#if !defined(LINUX_PLATFORM) && !defined(NO_OS_NETWORKING)
static int32_t iio_print_uart_info_message(struct no_os_uart_desc **uart_desc,
		struct no_os_uart_init_param *uart_init_par,
		char *message, int32_t msglen)
{
	int32_t status;
	uint32_t delay_ms;

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
#if defined(LINUX_PLATFORM) || defined(NO_OS_NETWORKING)
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
#if defined(LINUX_PLATFORM) || defined(NO_OS_NETWORKING)
	(void)msglen;
	printf("%s", message);
	return 0;
#else
	return iio_print_uart_info_message(uart_desc, uart_init_par, message,
					   msglen);
#endif
}

#if defined(NO_OS_NETWORKING) || defined(LINUX_PLATFORM)
static int32_t network_setup(struct iio_init_param *iio_init_param,
			     struct no_os_uart_desc *uart_desc,
			     void *irq_desc)
{
	static struct tcp_socket_init_param socket_param;

#ifdef LINUX_PLATFORM
	socket_param.net = &linux_net;
#endif
#ifdef ADUCM3029_PLATFORM
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
#elif defined(STM32_PLATFORM)
	static struct stm32_uart_init_param platform_uart_init_par = {
		.huart = IIO_APP_HUART,
	};
#elif defined(MAXIM_PLATFORM)
	static struct max_uart_init_param platform_uart_init_par = {
		.flow = UART_FLOW_DIS
	};
#elif defined(PICO_PLATFORM)
	static struct pico_uart_init_param platform_uart_init_par = {
		.uart_tx_pin = UART_TX_PIN,
		.uart_rx_pin = UART_RX_PIN,
	};
#endif
	static struct no_os_uart_init_param luart_par = {
		.device_id = UART_DEVICE_ID,
		/* TODO: remove this ifdef when asynchrounous rx is implemented on every platform. */
#if defined(STM32_PLATFORM) || defined(MAXIM_PLATFORM) || defined(ADUCM_PLATFORM) || defined(PICO_PLATFORM)
		.irq_id = UART_IRQ_ID,
		.asynchronous_rx = true,
#endif
		.baud_rate = UART_BAUDRATE_DEFAULT,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = UART_OPS,
#ifndef ADUCM_PLATFORM
		.extra = &platform_uart_init_par
#endif
	};
	*uart_init_par = &luart_par;

	return no_os_uart_init(uart_desc, &luart_par);
#endif
}

#if defined(ADUCM_PLATFORM) || (defined(XILINX_PLATFORM) && !defined(PLATFORM_MB)) || (defined(STM32_PLATFORM)) || defined(MAXIM_PLATFORM)
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
 * @brief IIO Application API with trigger initialization.
 * @param devices  - IIO devices to be used.
 * @param nb_devs  - Number of devices to be used.
 * @param trigs    - IIO triggers to be used.
 * @param nb_trigs - Number of triggers to be used.
 * @param irq_desc - IRQ descriptor to be used
 * @param iio_desc - IIO descriptor to be returned.
 * @return 0 in case of success or negative value otherwise.
 */
int32_t iio_app_run_with_trigs(struct iio_app_device *devices, uint32_t nb_devs,
			       struct iio_trigger_init *trigs, int32_t nb_trigs,
			       void *irq_desc, struct iio_desc **iio_desc)
{
	int32_t			status;
	struct iio_init_param	iio_init_param;
	struct no_os_uart_desc	*uart_desc;
	struct no_os_uart_init_param	*uart_init_par;
	struct iio_device_init	*iio_init_devs;
	uint32_t		i;
	struct iio_data_buffer *buff;

#if defined(ADUCM_PLATFORM) || (defined(XILINX_PLATFORM) && !defined(PLATFORM_MB)) || defined(STM32_PLATFORM)
	/* Only one irq controller can exist and be initialized in
	 * any of the iio_devices. */
	for (i = 0; i < nb_devs; i++) {
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

#if defined(NO_OS_NETWORKING) || defined(LINUX_PLATFORM)
	status = network_setup(&iio_init_param, uart_desc, irq_desc);
	if(status < 0)
		goto error;
#else
	iio_init_param.phy_type = USE_UART;
	iio_init_param.uart_desc = uart_desc;
#endif

	iio_init_devs = calloc(nb_devs, sizeof(*iio_init_devs));
	if (!iio_init_devs)
		return -ENOMEM;

	for (i = 0; i < nb_devs; ++i) {
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
	iio_init_param.nb_devs = nb_devs;
	iio_init_param.trigs = trigs;
	iio_init_param.nb_trigs = nb_trigs;
	iio_init_param.cntx_attrs = NULL;
	status = iio_init(iio_desc, &iio_init_param);
	if(status < 0)
		goto error;

	free(iio_init_devs);

	do {
		status = iio_step(*iio_desc);
	} while (true);
error:
	status = print_uart_error_message(&uart_desc, uart_init_par, status);
	return status;
}

int32_t iio_app_run(struct iio_app_device *devices, uint32_t len)
{
	struct iio_desc	*iio_desc;
	void *irq_desc = NULL;

	return iio_app_run_with_trigs(devices, len, NULL, 0, irq_desc, &iio_desc);
}

#endif
