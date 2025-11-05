/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file pal_uart_capi.c
 * @brief UART platform specific implementation using CAPI UART for Cordio
 */

#include <stddef.h>
#include "no_os_util.h"
#include "capi_uart.h"
#include "maxim_uart.h"
#include "pal_uart.h"

struct pal_cb_ctx {
	struct capi_uart_handle *uart;
	void (*wsf_wr_cb)(void);
	void (*wsf_rd_cb)(void);
};

static struct pal_cb_ctx terminal_rx_cb;
static struct pal_cb_ctx terminal_tx_cb;
static struct capi_uart_handle *uart_hci;
static struct capi_uart_handle *uart_terminal;

/* UART line configuration for HCI UART (1 Mbps) */
static struct capi_uart_line_config hci_line_config = {
	.baudrate = 1000000,
	.size = CAPI_UART_DATA_BITS_8,
	.parity = CAPI_UART_PARITY_NONE,
	.stop_bits = CAPI_UART_STOP_1_BIT,
	.flow_control = CAPI_UART_FLOW_CONTROL_NONE,
	.address_mode = CAPI_UART_ADDRESS_MODE_DISABLED,
	.device_address = 0,
	.sticky_parity = false,
	.loopback = false
};

/* UART line configuration for terminal UART (115200 bps) */
static struct capi_uart_line_config terminal_line_config = {
	.baudrate = 115200,
	.size = CAPI_UART_DATA_BITS_8,
	.parity = CAPI_UART_PARITY_NONE,
	.stop_bits = CAPI_UART_STOP_1_BIT,
	.flow_control = CAPI_UART_FLOW_CONTROL_NONE,
	.address_mode = CAPI_UART_ADDRESS_MODE_DISABLED,
	.device_address = 0,
	.sticky_parity = false,
	.loopback = false
};

/* HCI UART configuration (device ID 2) */
static const struct capi_uart_config hci_uart_config = {
	.identifier = 2,
	.dma_handle = NULL,
	.clk_freq_hz = 0,  /* Use default */
	.line_config = &hci_line_config,
	.extra = NULL,
	.ops = NULL  /* Will be populated by build system */
};

/* Terminal UART configuration (device ID 0) */
static const struct capi_uart_config terminal_uart_config = {
	.identifier = 0,
	.dma_handle = NULL,
	.clk_freq_hz = 0,  /* Use default */
	.line_config = &terminal_line_config,
	.extra = NULL,
	.ops = &maxim_capi_uart_ops  /* Will be populated by build system */
};

void pal_get_terminal_uart(struct capi_uart_handle **terminal)
{
	*terminal = uart_terminal;
}

/* CAPI UART callback for async events */
static void pal_uart_capi_callback(enum capi_uart_async_event event, void *arg, int event_extra)
{
	struct pal_cb_ctx *ctx = (struct pal_cb_ctx *)arg;

	if (!ctx || !ctx->wsf_wr_cb || !ctx->wsf_rd_cb)
		return;

	switch (event) {
	case CAPI_UART_EVENT_RX_DONE:
		/* Invoke user callback on RX complete */
		ctx->wsf_rd_cb();
		break;
	case CAPI_UART_EVENT_TX_DONE:
		/* Invoke user callback on TX complete */
		ctx->wsf_wr_cb();
		break;
	case CAPI_UART_EVENT_RX_TIMEOUT:
	case CAPI_UART_EVENT_TX_ABORTED:
	case CAPI_UART_EVENT_INTERRUPT:
	default:
		/* Handle other events if needed */
		break;
	}
}

void PalUartInit(PalUartId_t id, const PalUartConfig_t *pCfg)
{
	int ret;

	switch (id) {
	case PAL_UART_ID_CHCI:
		/* Initialize HCI UART */
		ret = capi_uart_init(&uart_hci, &hci_uart_config);
		if (ret)
			return;

		break;

	case PAL_UART_ID_TERMINAL:
		/* Initialize Terminal UART */
		ret = capi_uart_init(&uart_terminal, &terminal_uart_config);
		if (ret)
			return;

		capi_uart_stdio(uart_terminal);

		/* Set up callback context */
		if (pCfg) {
			// terminal_rx_cb.uart = uart_terminal;
			// terminal_rx_cb.wsf_cb = pCfg->rdCback;
			terminal_tx_cb.uart = uart_terminal;
			terminal_tx_cb.wsf_wr_cb = pCfg->wrCback;
			terminal_tx_cb.wsf_rd_cb = pCfg->rdCback;

			/* Register callback for async operations */
			ret = capi_uart_register_callback(uart_terminal,
							  pal_uart_capi_callback,
							  &terminal_tx_cb);
			if (ret)
				return;
		}

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
	struct capi_uart_handle *uart_handle;
	int ret;

	switch (id) {
	case PAL_UART_ID_CHCI:
		uart_handle = uart_hci;
		break;
	case PAL_UART_ID_TERMINAL:
		uart_handle = uart_terminal;
		break;
	default:
		return;
	}

	if (!uart_handle)
		return;

	/* Use async receive for non-blocking operation */
	ret = capi_uart_receive_async(uart_handle, pData, len);
}

void PalUartWriteData(PalUartId_t id, const uint8_t *pData, uint16_t len)
{
	struct capi_uart_handle *uart_handle;
	int ret;

	switch (id) {
	case PAL_UART_ID_CHCI:
		uart_handle = uart_hci;
		break;
	case PAL_UART_ID_TERMINAL:
		uart_handle = uart_terminal;
		break;
	default:
		return;
	}

	if (!uart_handle)
		return;

	/* Use async transmit for non-blocking operation */
	ret = capi_uart_transmit_async(uart_handle, (uint8_t *)pData, len);
}

PalUartState_t PalUartGetState(PalUartId_t id)
{
	struct capi_uart_handle *uart_handle;
	uint16_t tx_count, rx_count;
	mxc_uart_regs_t *maxim_uart;
	int ret;

	switch (id) {
	case PAL_UART_ID_CHCI:
		uart_handle = uart_hci;
		break;
	case PAL_UART_ID_TERMINAL:
		uart_handle = uart_terminal;
		break;
	default:
		return PAL_UART_STATE_ERROR;
	}

	if (!uart_handle)
		return PAL_UART_STATE_ERROR;

	maxim_uart = MXC_UART_GET_UART(0);
	// while(maxim_uart->status & NO_OS_GENMASK(1, 0));

	if (maxim_uart->status & NO_OS_GENMASK(1, 0)){
		return PAL_UART_STATE_BUSY;
	}

	// /* Check if TX or RX FIFO has pending data */
	// ret = capi_uart_get_tx_fifo_count(uart_handle, &tx_count);
	// if (ret == 0 && tx_count > 0)
	// 	return PAL_UART_STATE_BUSY;

	// ret = capi_uart_get_rx_fifo_count(uart_handle, &rx_count);
	// if (ret == 0 && rx_count > 0)
	// 	return PAL_UART_STATE_BUSY;

	return PAL_UART_STATE_READY;
}
