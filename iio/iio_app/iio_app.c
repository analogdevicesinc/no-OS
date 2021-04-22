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
#include "iio.h"
#include "iio_app.h"
#include "parameters.h"
#include "uart.h"
#include "delay.h"

#if defined(ADUCM_PLATFORM) || defined(XILINX_PLATFORM)
#include "irq.h"
#include "irq_extra.h"
#include "uart_extra.h"
#endif
#if defined(STM32_PLATFORM)
#include "stm32_uart.h"
#endif

#ifdef USE_TCP_SOCKET
#include "wifi.h"
#include "tcp_socket.h"
#endif

#ifdef LINUX_PLATFORM
#include "linux_socket.h"
#include "tcp_socket.h"
#endif

// The default baudrate iio_app will use to print messages to console.
#define UART_BAUDRATE_DEFAULT	115200

char *uart_data_size[] = {
	"5",
	"6",
	"7",
	"8",
	"9"
};

char *uart_parity[] = {
	"none",
	"mark",
	"space",
	"odd",
	"even"
};

char *uart_stop[] = {
	"1",
	"2",
};

static inline uint32_t _calc_uart_xfer_time(uint32_t len, uint32_t baudrate)
{
	uint32_t ms = 1000ul * len * 8 / UART_BAUDRATE_DEFAULT;
	ms += ms / 10; // overhead
	return ms;
}

int32_t iio_app_run(struct iio_app_device *devices, int32_t len)
{
	int32_t			status;
	char message[512];
	struct iio_desc		*iio_desc;
	struct iio_init_param	iio_init_param;
	struct uart_init_param	uart_init_par;
#if defined(ADUCM_PLATFORM) || defined(XILINX_PLATFORM)
	struct irq_init_param	irq_init_param;
	struct irq_ctrl_desc	*irq_desc;
#endif
	struct uart_desc		*uart_desc;
#ifdef USE_TCP_SOCKET
	struct tcp_socket_init_param	socket_param;
	struct wifi_init_param		wifi_param;
	struct wifi_desc		*wifi;
#elif defined(LINUX_PLATFORM)
	struct tcp_socket_init_param	socket_param;
#endif

#ifdef XILINX_PLATFORM
	/* Xilinx platform dependent initialization for IRQ. */
	struct xil_irq_init_param platform_irq_init_par;

	platform_irq_init_par = (struct xil_irq_init_param ) {
#ifdef XPAR_INTC_SINGLE_DEVICE_ID
		.type = IRQ_PL,
#else
		.type = IRQ_PS,
#endif
	};
#endif // XILINX_PLATFORM

#ifdef ADUCM_PLATFORM
	/* Dummy value for Aducm platform dependent initialization for IRQ. */
	int32_t platform_irq_init_par = 0;
#endif //ADUCM_PLATFORM

#if defined(ADUCM_PLATFORM) || defined(XILINX_PLATFORM)
	irq_init_param = (struct irq_init_param ) {
		.irq_ctrl_id = INTC_DEVICE_ID,
		.extra = &platform_irq_init_par
	};

	status = irq_ctrl_init(&irq_desc, &irq_init_param);
	if(status < 0)
		return status;
#endif

#ifdef XILINX_PLATFORM
	/* Xilinx platform dependent initialization for UART. */
	struct xil_uart_init_param platform_uart_init_par;

	platform_uart_init_par = (struct xil_uart_init_param) {
#ifdef XPAR_XUARTLITE_NUM_INSTANCES
		.type = UART_PL,
#else
		.type = UART_PS,
#endif
		.irq_id = UART_IRQ_ID,
		.irq_desc = irq_desc,
	};
#endif // XILINX_PLATFORM

#ifdef ADUCM_PLATFORM
	/* Aducm platform dependent initialization for UART. */
	struct aducm_uart_init_param platform_uart_init_par = {
		/* TODO: cleanup, these can be set using generic uart_init_param. */
		.parity = UART_NO_PARITY,
		.stop_bits = UART_ONE_STOPBIT,
		.word_length = UART_WORDLEN_8BITS
	};
#endif // ADUCM_PLATFORM

#ifdef STM32_PLATFORM
	/* stm32 platform dependent initialization for UART. */
	struct stm32_uart_init_param platform_uart_init_par = {
		.mode = UART_MODE_TX_RX,
		.hw_flow_ctl = UART_HWCONTROL_NONE,
		.over_sampling = UART_OVERSAMPLING_16,
	};
#endif

#if defined(XILINX_PLATFORM) || defined(ADUCM_PLATFORM) || defined(STM32_PLATFORM)
	uart_init_par = (struct uart_init_param) {
		.device_id = UART_DEVICE_ID,
		.baud_rate = UART_BAUDRATE_DEFAULT,
		.size = UART_CS_8,
		.parity = UART_PAR_NO,
		.stop = UART_STOP_1,
		.extra = &platform_uart_init_par
	};
#endif

#if defined(ADUCM_PLATFORM) || defined(XILINX_PLATFORM)
	status = irq_global_enable(irq_desc);
	if (status < 0)
		return status;
#endif
#ifndef LINUX_PLATFORM
	status = uart_init(&uart_desc, &uart_init_par);
	if (status < 0)
		return status;

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
				  uart_data_size[uart_init_par.size],
				  uart_parity[uart_init_par.parity],
				  uart_stop[uart_init_par.stop]);

	uint32_t delay_ms = _calc_uart_xfer_time(msglen, UART_BAUDRATE_DEFAULT);
	status = uart_write(uart_desc, (uint8_t *)message, msglen);
	if (status < 0)
		return status;

	if (UART_BAUDRATE_DEFAULT != UART_BAUDRATE) {
		// make sure the whole message gets printed befure uart gets reinitialized
		mdelay(delay_ms);

		uart_remove(uart_desc);

		uart_init_par.baud_rate = UART_BAUDRATE;
		status = uart_init(&uart_desc, &uart_init_par);
		if (status < 0)
			return status;
	}
#endif

#ifdef USE_TCP_SOCKET
	wifi_param.irq_desc = irq_desc;
	wifi_param.uart_desc = uart_desc;
#ifdef ADUCM_PLATFORM
	wifi_param.uart_irq_conf = uart_desc;
#endif //ADUCM_PLATFORM
	wifi_param.uart_irq_id = UART_IRQ_ID;

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
	socket_param.max_buff_size = 0;

	iio_init_param.phy_type = USE_NETWORK;
	iio_init_param.tcp_socket_init_param = &socket_param;

#elif defined(LINUX_PLATFORM)
	socket_param.net = &linux_net;
	socket_param.max_buff_size = 0;

	iio_init_param.phy_type = USE_NETWORK;
	iio_init_param.tcp_socket_init_param = &socket_param;
#else
	iio_init_param.phy_type = USE_UART;
	iio_init_param.uart_desc = uart_desc;
#endif//USE_TCP_SOCKET

	status = iio_init(&iio_desc, &iio_init_param);
	if(status < 0)
		goto error;

	int32_t i;
	for (i = 0; i < len; i++) {
		status = iio_register(iio_desc,
				      devices[i].dev_descriptor,
				      devices[i].name,
				      devices[i].dev,
				      devices[i].read_buff,
				      devices[i].write_buff);
		if (status < 0)
			goto error;
	}

	do {
		status = iio_step(iio_desc);
	} while (true);
error:
#ifndef LINUX_PLATFORM
	if (UART_BAUDRATE_DEFAULT != UART_BAUDRATE) {
		uart_remove(uart_desc);

		uart_init_par.baud_rate = UART_BAUDRATE_DEFAULT;
		status = uart_init(&uart_desc, &uart_init_par);
		if (status < 0)
			return status;
	}

	msglen = sprintf(message, "TinyIIOD server failed with %d.\n", status);
	uart_write(uart_desc, (uint8_t *)message, msglen);

	delay_ms = _calc_uart_xfer_time(msglen, UART_BAUDRATE_DEFAULT);
	mdelay(delay_ms);
	uart_remove(uart_desc);
#endif
	return status;
}

#endif
