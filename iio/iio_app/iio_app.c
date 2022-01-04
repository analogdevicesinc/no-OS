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
#include "no-os/uart.h"
#include "no-os/delay.h"

#include "gpio_extra.h"
#include "no-os/gpio.h"
#include "rtc_extra.h"
#include "no-os/irq.h"
#include "no-os/spi.h"
#include "no-os/rtc.h"

#if defined(ADUCM_PLATFORM) || defined(XILINX_PLATFORM)
#include "no-os/irq.h"
#include "irq_extra.h"
#include "uart_extra.h"
#endif
#if defined(STM32_PLATFORM)
#include "stm32_uart.h"
#endif

#ifdef MAXIM_PLATFORM
#include "maxim_uart.h"
#include "gpio_extra.h"
#include "spi_extra.h"
#include "irq_extra.h"
#include "rtc_extra.h"
#include "maxim_stdio.h"
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

#define UART_HWCONTROL_NONE 0
#define UART_DEVICE_ID 1

volatile uint32_t tst = 0;
volatile uint32_t uart_test = 0;
volatile uint32_t rtc_test = 0;

void uart_cb(void *ctx, uint32_t event, void *config)
{
	uart_test = 1000;
}

void gpio_cb(void *ctx, uint32_t event, void *config)
{
	tst = 1000;
}

void rtc_cb(void *ctx, uint32_t event, void *config)
{
	rtc_test = 0;
}

static inline uint32_t _calc_uart_xfer_time(uint32_t len, uint32_t baudrate)
{
	uint32_t ms = 1000ul * len * 8 / UART_BAUDRATE_DEFAULT;
	ms += ms / 10; // overhead
	return ms;
}

#if !defined(LINUX_PLATFORM) && !defined(USE_TCP_SOCKET)
static int32_t iio_print_uart_info_message(struct uart_desc **uart_desc,
		struct uart_init_param *uart_init_par,
		char *message, int32_t msglen)
{
	int32_t status;
	uint32_t delay_ms;

	if (UART_BAUDRATE_DEFAULT != UART_BAUDRATE) {
		uart_remove(*uart_desc);

		uart_init_par->baud_rate = UART_BAUDRATE_DEFAULT;
		status = uart_init(uart_desc, uart_init_par);
		if (status < 0)
			return status;
	}
	maxim_uart_stdio(*uart_desc);
	//int32_t er = 0;
	//er = printf("123");
	status = uart_write(*uart_desc, (uint8_t *)message, msglen);
	if (status < 0)
		return status;

	/*
		NVIC_DisableIRQ(RTC_IRQn);
		NVIC_DisableIRQ(GPIO0_IRQn);
		NVIC_DisableIRQ(UART0_IRQn);
		NVIC_DisableIRQ(UART1_IRQn);
		gpio_cfg_t param_extra2 = {
			.port = 0,
			.mask = 12,
			.pad = GPIO_PAD_PULL_UP,
			.func = GPIO_FUNC_OUT
		};

		gpio_cfg_t param_extra = {
			.port = 0,
			.mask = 12,
			.pad = GPIO_PAD_PULL_UP,
			.func = GPIO_FUNC_OUT
		};
		gpio_desc *desc;
		gpio_desc *desc2;
		struct gpio_init_param param = {
			.number = 8,
			.platform_ops = NULL,
			.extra = &param_extra
		};
		struct gpio_init_param param2 = {
			.number = 9,
			.platform_ops = NULL,
			.extra = &param_extra2
		};

		int32_t error = gpio_get(&desc, &param);
		uint8_t val = 20;

		gpio_set_value(desc, GPIO_LOW);
		gpio_get_value(desc, &val);

		struct rtc_init_maxim rtc_maxim = {
			.ms_load = 100
		};
		struct rtc_init_param rtc_init_p = {
			.id = 0,
			.freq = 32,
			.load = 10,
			.extra = &rtc_maxim
		};
		struct rtc_desc *rtc;
		uint32_t cnt = 0;
		int32_t rtc_get_err = 0;

		rtc_init(&rtc, &rtc_init_p);
		rtc_start(rtc);

		do{
			rtc_get_err = rtc_get_cnt(rtc, &cnt);
		}while(rtc_get_err);
		rtc_set_cnt(rtc, 10 * cnt);
		mdelay(117);
		rtc_get_cnt(rtc, &cnt);
	//	rtc_stop(rtc);


		struct callback_desc cb = {
			.ctx = NULL,
			.callback = uart_cb,
			.config = NULL
		};
		struct callback_desc gpio_callback = {
			.ctx = NULL,
			.callback = gpio_cb,
			.config = NULL
		};

		struct callback_desc rtc_callback = {
			.ctx = NULL,
			.callback = rtc_cb,
			.config = NULL
		};


		struct irq_init_param irq_param = {
			.irq_ctrl_id = 10,
			.platform_ops = NULL,
			.extra = NULL
		};

		struct irq_ctrl_desc *irq_desc;
		int32_t err = irq_ctrl_init(&irq_desc, &irq_param);
		err = uart_register_callback(0, &cb);
		err = uart_register_callback(1, &cb);
		err = rtc_register_callback(&rtc_cb);

		cb.callback = gpio_cb;
		gpio_get(&desc2, &param2);
		//error = gpio_get(&desc, &param);

		uint8_t v = 0;
		irq_desc->extra = desc;
		gpio_set_value(desc2, GPIO_LOW);
		gpio_get_value(desc, &v);

		gpio_set_value(desc, GPIO_HIGH);
		gpio_set_value(desc2, GPIO_HIGH);

		gpio_register_callback(irq_desc, IRQ_EDGE_BOTH, &gpio_cb);

		mdelay(3000);
	*/
	delay_ms = _calc_uart_xfer_time(msglen, UART_BAUDRATE_DEFAULT);
	mdelay(delay_ms);
	if (UART_BAUDRATE_DEFAULT != UART_BAUDRATE) {
		uart_remove(*uart_desc);
		uart_init_par->baud_rate = UART_BAUDRATE;
		return uart_init(uart_desc, uart_init_par);
	}

	return 0;
}
#endif

static int32_t print_uart_hello_message(struct uart_desc **uart_desc,
					struct uart_init_param *uart_init_par)
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

static int32_t print_uart_error_message(struct uart_desc **uart_desc,
					struct uart_init_param *uart_init_par,
					int32_t status)
{
	char message[512];
	uint32_t msglen = sprintf(message,
				  "TinyIIOD server failed with code %d.\n",
				  (int)status);
#if defined(LINUX_PLATFORM) || defined(USE_TCP_SOCKET)
	(void)msglen;
	printf(message);
	return 0;
#else
	return iio_print_uart_info_message(uart_desc, uart_init_par, message,
					   msglen);
#endif
}

#if defined(USE_TCP_SOCKET) || defined(LINUX_PLATFORM)
static int32_t network_setup(struct iio_init_param *iio_init_param,
			     struct uart_desc *uart_desc,
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

static int32_t uart_setup(struct uart_desc **uart_desc,
			  struct uart_init_param **uart_init_par,
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
		.mode = UART_MODE_TX_RX,
		.hw_flow_ctl = UART_HWCONTROL_NONE,
		.over_sampling = UART_OVERSAMPLING_16,
	};
#elif defined(MAXIM_PLATFORM)
	static struct maxim_uart_init_param platform_uart_init_par = {
		.port = 1,
		.parity = UART_PARITY_DISABLE,
		.size = UART_DATA_SIZE_8_BITS,
		.stop = UART_STOP_1,
		.flow = UART_FLOW_CTRL_DIS,
		.pol = UART_FLOW_POL_DIS
	};

#endif
	static struct uart_init_param luart_par = {
		.device_id = UART_DEVICE_ID,
		.baud_rate = UART_BAUDRATE_DEFAULT,
		.size = UART_CS_8,
		.parity = UART_PAR_NO,
		.stop = UART_STOP_1_BIT,
		.extra = &platform_uart_init_par
	};
	*uart_init_par = &luart_par;

	return uart_init(uart_desc, &luart_par);
#endif
}

#if defined(ADUCM_PLATFORM) || (defined(XILINX_PLATFORM) && !defined(PLATFORM_MB))
static int32_t irq_setup(struct irq_ctrl_desc **irq_desc)
{
	int32_t status;
#if defined(XILINX_PLATFORM) && !defined(PLATFORM_MB)
	struct xil_irq_init_param p = {
		.type = IRQ_PS,
	};
	struct xil_irq_init_param *platform_irq_init_par = &p;
	const struct irq_platform_ops *platform_irq_ops = &xil_irq_ops;
#elif defined(ADUCM_PLATFORM)
	void *platform_irq_init_par = NULL;
	const struct irq_platform_ops *platform_irq_ops = &aducm_irq_ops;
#endif

	struct irq_init_param irq_init_param = {
		.irq_ctrl_id = INTC_DEVICE_ID,
		.platform_ops = platform_irq_ops,
		.extra = platform_irq_init_par
	};

	status = irq_ctrl_init(irq_desc, &irq_init_param);
	if (status < 0)
		return status;

	return irq_global_enable(*irq_desc);
}
#endif

int32_t iio_app_run(struct iio_app_device *devices, int32_t len)
{
	int32_t			status;
	struct iio_desc		*iio_desc;
	struct iio_init_param	iio_init_param;
	struct uart_desc	*uart_desc;
	struct uart_init_param	*uart_init_par;
	void			*irq_desc = NULL;


#if defined(ADUCM_PLATFORM) || (defined(XILINX_PLATFORM) && !defined(PLATFORM_MB))
	status = irq_setup((struct irq_ctrl_desc **)&irq_desc);
	if (status < 0)
		return status;
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
	status = print_uart_error_message(&uart_desc, uart_init_par, status);
	return status;
}

#endif
