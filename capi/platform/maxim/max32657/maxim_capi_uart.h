/*******************************************************************************
 *   @file   maxim_capi_uart.h
 *   @brief  Header file for UART functions with CAPI
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef MAXIM_CAPI_UART_H_
#define MAXIM_CAPI_UART_H_

#include "uart.h"
#include "capi_dma.h"
#include "capi_uart.h"
#include "maxim_capi_dma.h"
#include "maxim_capi_irq.h"
#include "maxim_capi_gpio.h"

#define MAX_CAPI_UART_DEFAULT_BAUD 115200

struct max_capi_uart_extra {
	/** GPIO voltage selection */
	enum max_capi_gpio_vssel vssel;
	/** OPTIONAL - DMA config */
	struct capi_dma_config *dma_config;
};

struct max_capi_uart_priv {
	/** Identifier */
	uint32_t id;
	/** UART registers */
	mxc_uart_regs_t *uart;
	/** Clock source storage */
	mxc_uart_clock_t clk_src;
	/** Line config storage */
	struct capi_uart_line_config line_config;
	/** DMA handle */
	struct capi_dma_handle *dma_handle;
	/** Async callback */
	capi_uart_callback callback;
	/** Callback arg */
	void *callback_arg;
	/** UART request storage for async */
	mxc_uart_req_t async_req;
	/** DMA transfer storage for async */
	struct capi_dma_transfer dma_xfer;
	/** DMA transfer extra struct storage for async */
	struct max_capi_dma_xfer_extra dma_xfer_extra;
};

extern struct capi_uart_ops max_capi_uart_ops;

int max_capi_uart_stdio_enable(struct capi_uart_handle *handle);

#endif /* MAXIM_CAPI_UART_H_ */
