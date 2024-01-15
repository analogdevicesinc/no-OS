/***************************************************************************//**
 *   @file   maxim_uart.c
 *   @brief  Source file of UART driver for STM32
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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

/******************************************************************************/
/************************* Include Files **************************************/
/******************************************************************************/

#include <errno.h>
#include <stdlib.h>
#include "maxim_irq.h"
#include "maxim_uart.h"
#include "mxc_sys.h"
#include "mxc_errors.h"
#include "no_os_irq.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_lf256fifo.h"
#include "uart.h"

/**
* @brief Descriptors to hold the state of nonblocking read and writes
* on each port
*/
mxc_uart_req_t uart_irq_state[MXC_UART_INSTANCES];
bool is_callback;

static uint8_t c;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Read data from UART device. Blocking function.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return positive number of received bytes in case of success, negative error code otherwise.
 */
static int32_t max_uart_read(struct no_os_uart_desc *desc, uint8_t *data,
			     uint32_t bytes_number)
{
	int32_t ret;
	uint32_t i = 0;

	if (!desc || !data || !bytes_number)
		return -EINVAL;

	if (desc->rx_fifo) {
		for (i = 0; i < bytes_number; i++) {
			ret = lf256fifo_read(desc->rx_fifo, &data[i]);
			if (ret)
				return i ? (int32_t)i : -EAGAIN;
		}
		return i;
	}

	ret = MXC_UART_Read(MXC_UART_GET_UART(desc->device_id), data,
			    (int *)&bytes_number);
	if (ret != E_SUCCESS)
		return -EIO;

	return bytes_number;
}

/**
 * @brief Write data to UART device. Blocking function.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, errno codes otherwise.
 */
static int32_t max_uart_write(struct no_os_uart_desc *desc, const uint8_t *data,
			      uint32_t bytes_number)
{
	int32_t ret;
	int32_t transfered = 0;
	int block_size = 8;

	if(!desc || !data || !bytes_number)
		return -EINVAL;

	while (bytes_number) {
		if (bytes_number > 8)
			block_size = 8;
		else
			block_size = bytes_number;

		while(!(MXC_UART_GetStatus(MXC_UART_GET_UART(desc->device_id)) &
			MXC_F_UART_STATUS_TX_EM));
		ret = MXC_UART_Write(MXC_UART_GET_UART(desc->device_id),
				     (uint8_t *)(data + transfered),
				     &block_size);

		transfered += block_size;
		bytes_number -= block_size;

		if (ret != E_SUCCESS)
			return -EIO;
	}

	return transfered;
}

/**
 * @brief Read data from UART device. Non blocking function.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return positive number of received bytes in case of success, negative error code otherwise.
 */
static int32_t max_uart_read_nonblocking(struct no_os_uart_desc *desc,
		uint8_t *data,
		uint32_t bytes_number)
{
	int32_t ret;
	uint32_t id;

	if (!desc || !data || !bytes_number)
		return -EINVAL;

	id = desc->device_id;
	uart_irq_state[id].uart = MXC_UART_GET_UART(id);
	uart_irq_state[id].rxData = data;
	uart_irq_state[id].rxLen = bytes_number;
	uart_irq_state[id].txData = NULL;
	uart_irq_state[id].txLen = 0;
	uart_irq_state[id].rxCnt = 0;
	uart_irq_state[id].callback = max_uart_callback;

	if (!is_callback) {
		ret = MXC_UART_TransactionAsync(&uart_irq_state[id]);
		if (ret == E_BUSY)
			return -EBUSY;
	}

	return 0;
}

/**
 * @brief Write data to UART device. Non blocking function.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, errno codes otherwise.
 */

static int32_t max_uart_write_nonblocking(struct no_os_uart_desc *desc,
		const uint8_t *data,
		uint32_t bytes_number)
{
	int32_t ret;
	uint32_t id;

	if (!desc || !data || !bytes_number)
		return -EINVAL;

	id = desc->device_id;
	uart_irq_state[id].uart = MXC_UART_GET_UART(id);
	uart_irq_state[id].txData = data;
	uart_irq_state[id].txLen = bytes_number;
	uart_irq_state[id].rxData = NULL;
	uart_irq_state[id].rxLen = 0;
	uart_irq_state[id].txCnt = 0;
	uart_irq_state[id].callback = max_uart_callback;

	if (!is_callback) {
		ret = MXC_UART_TransactionAsync(&uart_irq_state[id]);
		if (ret == E_BUSY)
			return -EBUSY;
	}

	return 0;
}

/**
 * @brief UART receive interrupt callback function
 * @param context - UART context
 * @return none
 */
void uart_rx_callback(void *context)
{
	struct no_os_uart_desc *d = context;
	lf256fifo_write(d->rx_fifo, c);
	max_uart_read_nonblocking(d, &c, 1);
}

/**
 * @brief Initialize the UART communication peripheral.
 * @param desc - The UART descriptor.
 * @param param - The structure that contains the UART parameters.
 * @return 0 in case of success, errno codes otherwise.
 */
static int32_t max_uart_init(struct no_os_uart_desc **desc,
			     struct no_os_uart_init_param *param)
{
	int32_t ret;
	int32_t stop, size, flow, parity;
	mxc_uart_regs_t *uart_regs;
	struct max_uart_init_param *eparam;
	struct no_os_uart_desc *descriptor;
	struct max_uart_desc *max_uart;

	if (!param || !param->extra)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	max_uart = no_os_calloc(1, sizeof(*max_uart));
	if (!descriptor) {
		ret = -ENOMEM;
		goto error;
	}
	descriptor->extra = max_uart;
	uart_regs = MXC_UART_GET_UART(param->device_id);
	eparam = param->extra;

	descriptor->device_id = param->device_id;
	descriptor->baud_rate = param->baud_rate;

	switch(param->parity) {
	case NO_OS_UART_PAR_NO:
		parity = MXC_UART_PARITY_DISABLE;
		break;
	case NO_OS_UART_PAR_ODD:
		parity = MXC_UART_PARITY_ODD_0;
		break;
	case NO_OS_UART_PAR_EVEN:
		parity = MXC_UART_PARITY_EVEN_0;
		break;
	default:
		ret = -EINVAL;
		goto error;
	}

	switch(param->size) {
	case NO_OS_UART_CS_5:
		size = 5;
		break;
	case NO_OS_UART_CS_6:
		size = 6;
		break;
	case NO_OS_UART_CS_7:
		size = 7;
		break;
	case NO_OS_UART_CS_8:
		size = 8;
		break;
	default:
		ret = -EINVAL;
		goto error;
	}

	switch(param->stop) {
	case NO_OS_UART_STOP_1_BIT:
		stop = MXC_UART_STOP_1;
		break;
	case NO_OS_UART_STOP_2_BIT:
		stop = MXC_UART_STOP_2;
		break;
	default:
		ret = -EINVAL;
		goto error;
	}

	switch (eparam->flow) {
	case MAX_UART_FLOW_DIS:
		flow = MXC_UART_FLOW_DIS;
		break;
	case MAX_UART_FLOW_EN:
		flow = MXC_UART_FLOW_EN;
		break;
	default:
		ret = -EINVAL;
		goto error;
	}

	ret = MXC_UART_Init(uart_regs, descriptor->baud_rate, MXC_UART_IBRO_CLK);
	if (ret != E_NO_ERROR) {
		ret = -EINVAL;
		goto error;
	}

	ret = MXC_UART_SetDataSize(uart_regs, size);
	if (ret != E_NO_ERROR) {
		ret = -EINVAL;
		goto error;
	}

	ret = MXC_UART_SetParity(uart_regs, parity);
	if (ret != E_NO_ERROR) {
		ret = -EINVAL;
		goto error;
	}

	ret = MXC_UART_SetStopBits(uart_regs, stop);
	if (ret != E_NO_ERROR) {
		ret = -EINVAL;
		goto error;
	}

	ret = MXC_UART_SetFlowCtrl(uart_regs, flow, 8);
	if (ret != E_NO_ERROR) {
		ret = -EINVAL;
		goto error;
	}

	*desc = descriptor;

	if (param->asynchronous_rx) {
		ret = lf256fifo_init(&descriptor->rx_fifo);
		if (ret)
			goto error;

		struct no_os_irq_init_param nvic_ip = {
			.platform_ops = &max_irq_ops,
		};

		ret = no_os_irq_ctrl_init(&max_uart->nvic, &nvic_ip);
		if (ret)
			goto error;

		struct no_os_callback_desc uart_rx_cb = {
			.callback = uart_rx_callback,
			.ctx = descriptor,
			.event = NO_OS_EVT_UART_RX_COMPLETE,
			.peripheral = NO_OS_UART_IRQ,
			.handle = MXC_UART_GET_UART(descriptor->device_id)
		};

		ret = no_os_irq_register_callback(max_uart->nvic,
						  MXC_UART_GET_IRQ(descriptor->device_id),
						  &uart_rx_cb);
		if (ret)
			goto error_nvic;

		ret = no_os_irq_enable(max_uart->nvic, MXC_UART_GET_IRQ(descriptor->device_id));
		if (ret)
			goto error_nvic;

		ret = max_uart_read_nonblocking(descriptor, &c, 1);
		if (ret)
			goto error_nvic;
	}

	return 0;
error_nvic:
	no_os_irq_ctrl_remove(max_uart->nvic);
error:
	no_os_free(max_uart);
	no_os_free(descriptor);
	MXC_UART_Shutdown(uart_regs);

	return ret;
}

/**
 * @brief Free the resources allocated by max_uart_init().
 * @param desc - The UART descriptor.
 * @return 0 in case of success, errno codes otherwise.
 */
static int32_t max_uart_remove(struct no_os_uart_desc *desc)
{
	struct max_uart_desc *extra;
	struct no_os_callback_desc discard = {
		.peripheral = NO_OS_UART_IRQ,
		.event = NO_OS_EVT_UART_RX_COMPLETE,
		.handle = MXC_UART_GET_UART(desc->device_id)
	};
	int ret;

	if (!desc)
		return -EINVAL;

	extra = desc->extra;

	if (desc->rx_fifo) {
		no_os_irq_disable(extra->nvic, MXC_UART_GET_IRQ(desc->device_id));
		no_os_irq_unregister_callback(extra->nvic,
					      MXC_UART_GET_IRQ(desc->device_id),
					      &discard);
		no_os_irq_ctrl_remove(extra->nvic);
		lf256fifo_remove(desc->rx_fifo);
		no_os_free(desc->rx_fifo);
	}

	MXC_UART_Shutdown(MXC_UART_GET_UART(desc->device_id));
	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Not implemented.
 * @param desc - The UART descriptor.
 * @return -ENOSYS
 */
static uint32_t max_uart_get_errors(struct no_os_uart_desc *desc)
{
	return -ENOSYS;
}

/**
 * @brief Maxim platform specific UART platform ops structure
 */
const struct no_os_uart_platform_ops max_uart_ops = {
	.init = &max_uart_init,
	.read = &max_uart_read,
	.write = &max_uart_write,
	.read_nonblocking = &max_uart_read_nonblocking,
	.write_nonblocking = &max_uart_write_nonblocking,
	.get_errors = &max_uart_get_errors,
	.remove = &max_uart_remove
};
