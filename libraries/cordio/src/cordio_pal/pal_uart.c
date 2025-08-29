/***************************************************************************//**
 *   @file   pal_uart.c
 *   @brief  UART platform specific implementation used by Cordio.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
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
// #include "common_data.h"
#include "no_os_uart.h"
#include "pal_uart.h"
#include "maxim_uart.h"

struct pal_cb_ctx {
	struct no_os_uart_desc *uart;
	void (*wsf_cb)(void);
};

static void (*tx_cb)(void);
static struct pal_cb_ctx terminal_rx_cb;
static struct pal_cb_ctx terminal_tx_cb;
static struct no_os_uart_desc *uart_hci;
static struct no_os_uart_desc *uart_terminal;

static const struct max_uart_init_param uart_extra_ip = {
	.flow = 0
};

static const struct no_os_uart_init_param hci_uart_ip = {
	.device_id = 2,
	.irq_id = UART2_IRQn,
	.asynchronous_rx = true,
	.baud_rate = 1000000,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = &uart_extra_ip,
	.platform_ops = &max_uart_ops,
};

static const struct no_os_uart_init_param terminal_uart_ip = {
	.device_id = 0,
	.irq_id = UART0_IRQn,
	.asynchronous_rx = true,
	.baud_rate = 9600,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.extra = &uart_extra_ip,
	.platform_ops = &max_uart_ops,
};

void pal_get_terminal_uart(struct no_os_uart_desc **terminal)
{
	*terminal = uart_terminal;
}

static void pal_uart_rx_callback(void *ctx)
{
	struct pal_cb_ctx *data = ctx;
	struct no_os_uart_desc *uart = data->uart;

	data->wsf_cb();
}

static void pal_uart_tx_callback(void *ctx)
{
	struct pal_cb_ctx *data = ctx;
	struct no_os_uart_desc *uart = data->uart;

	data->wsf_cb();
}

void PalUartInit(PalUartId_t id, const PalUartConfig_t *pCfg)
{
	int ret;
	struct max_uart_desc *max_uart;
	struct no_os_callback_desc uart_rx_cb = {
		.callback = pal_uart_rx_callback,
		.event = NO_OS_EVT_UART_RX_COMPLETE,
		.peripheral = NO_OS_UART_IRQ,
	};
	struct no_os_callback_desc uart_tx_cb = {
		.callback = pal_uart_tx_callback,
		.event = NO_OS_EVT_UART_TX_COMPLETE,
		.peripheral = NO_OS_UART_IRQ,
	};

	switch (id) {
	case PAL_UART_ID_CHCI:
		ret = no_os_uart_init(&uart_hci, &hci_uart_ip);
		if (ret)
			return;

		break;
	case PAL_UART_ID_TERMINAL:
		ret = no_os_uart_init(&uart_terminal, &terminal_uart_ip);
		no_os_uart_stdio(uart_terminal);
		if (ret)
			return;

		terminal_rx_cb.uart = uart_terminal;
		terminal_rx_cb.wsf_cb = pCfg->rdCback;
		terminal_tx_cb.uart = uart_terminal;
		terminal_tx_cb.wsf_cb = pCfg->wrCback;

		max_uart = uart_terminal->extra;
		uart_rx_cb.ctx = &terminal_rx_cb;
		uart_rx_cb.handle = MXC_UART_GET_UART(uart_terminal->device_id);
		uart_tx_cb.ctx = &terminal_tx_cb;
		uart_tx_cb.handle = MXC_UART_GET_UART(uart_terminal->device_id);

		ret = no_os_irq_register_callback(max_uart->nvic,
						  MXC_UART_GET_IRQ(uart_terminal->device_id),
						  &uart_rx_cb);

		ret = no_os_irq_register_callback(max_uart->nvic,
						  MXC_UART_GET_IRQ(uart_terminal->device_id),
						  &uart_tx_cb);

		break;
	default:
		return;
	}

}

static uint8_t palUartGetNum(PalUartId_t uartId)
{
	return 3;
}

void PalUartReadData(PalUartId_t id, uint8_t *pData, uint16_t len)
{
	struct no_os_uart_desc *uart_desc;
	int ret;

	switch (id) {
	case PAL_UART_ID_CHCI:
		uart_desc = uart_hci;
		break;
	case PAL_UART_ID_TERMINAL:
		uart_desc = uart_terminal;
		break;
	default:
		return;
	}

	ret = no_os_uart_read_nonblocking(uart_desc, pData, len);
}

void PalUartWriteData(PalUartId_t id, const uint8_t *pData, uint16_t len)
{
	struct no_os_uart_desc *uart_desc;
	int ret;

	switch (id) {
	case PAL_UART_ID_CHCI:
		uart_desc = uart_hci;
		break;
	case PAL_UART_ID_TERMINAL:
		uart_desc = uart_terminal;
		break;
	default:
		return;
	}

	no_os_uart_write_nonblocking(uart_desc, pData, len);
}

PalUartState_t PalUartGetState(PalUartId_t id)
{
	struct no_os_uart_desc *uart_desc;
	mxc_uart_regs_t *uart;
	int ret;

	switch (id) {
	case PAL_UART_ID_CHCI:
		uart_desc = uart_hci;
		break;
	case PAL_UART_ID_TERMINAL:
		uart_desc = uart_terminal;
		break;
	default:
		return;
	}

	uart = MXC_UART_GET_UART(uart_desc->device_id);

	if (uart->status & NO_OS_GENMASK(1, 0))
		return PAL_UART_STATE_BUSY;

	return PAL_UART_STATE_READY;
}
