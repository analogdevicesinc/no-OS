/***************************************************************************//**
 *   @file   maxim_uart.h
 *   @brief  Header file of the UART driver for the maxim platform
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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

#include "capi_uart.h"
#include "max32655.h"
#include "uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Private handle structure for MAX32655 UART CAPI implementation
 */
struct maxim_capi_uart_handle {
	struct capi_uart_handle capi_handle;  /**< Must be first field */
	mxc_uart_regs_t *uart_regs;          /**< Maxim UART registers */
	uint32_t device_id;                   /**< UART device ID */
	capi_uart_callback callback;         /**< User callback */
	void *callback_arg;                   /**< User callback argument */
	struct capi_uart_line_config line_config; /**< Current line configuration */
	bool fifo_enabled;                    /**< FIFO enable state */
	mxc_uart_req_t async_req;            /**< Async request structure */
};

#define CAPI_UART_HANDLE &(struct capi_uart_handle){		\
		.priv = &(struct maxim_capi_uart_handle){},	\
	}

/**
 * @brief MAX32655 UART CAPI operations structure
 *
 * This structure contains function pointers for all UART CAPI operations
 * specific to the MAX32655 platform.
 */
extern const struct capi_uart_ops maxim_uart_ops;

#ifdef __cplusplus
}
#endif

#endif /* MAXIM_CAPI_UART_H_ */