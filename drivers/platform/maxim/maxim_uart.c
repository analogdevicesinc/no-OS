/***************************************************************************//**
 *   @file   maxim/maxim_uart.c
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
#include "maxim_stdio.h"
#include "mxc_sys.h"
#include "uart.h"
#include "maxim_uart.h"
#include "no-os/uart.h"
#include "no-os/irq.h"
#include "no-os/util.h"

/**
* @brief Callback descriptor that contains a function to be called when an
* interrupt occurs.
*/
static struct callback_desc *cb[2];

/**
* @brief Descriptors to hold the state of nonblocking read and writes
* on each port
*/
static mxc_uart_req_t rx_state[2];
static mxc_uart_req_t tx_state[2];

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

static void _uart_irq(uint8_t port)
{
	mxc_uart_regs_t *uart_regs;
	uint8_t	n_int;
	uint32_t reg_int_fl;
	uint32_t reg_int_en;

	uart_regs = (port == 0) ? MXC_UART0 : MXC_UART1;
	reg_int_fl = uart_regs->int_fl;
	reg_int_en = uart_regs->int_en;

	/** Handle nonblocking write and read, also clears the interrupt flags */
	MXC_UART_AsyncHandler(uart_regs);

	/** Disable all interrupts */
	uart_regs->int_en = 0x0;

	if (!cb[port])	{
		/** Enable the interrupts back */
		uart_regs->int_en = reg_int_en;

		return;
	}

	while(reg_int_fl) {
		n_int = find_first_set_bit(reg_int_fl);
		if (reg_int_fl & (reg_int_en & BIT(n_int))) {
			void *ctx = cb[port]->ctx;
			void *config = cb[port]->config;
			cb[port]->callback(ctx, n_int, config);
		}
		reg_int_fl >>= n_int + 1;
	}

	/** Enable the interrupts back */
	uart_regs->int_en = reg_int_en;
}

void UART0_IRQHandler()
{
	_uart_irq(0);
}

void UART1_IRQHandler()
{
	_uart_irq(1);
}

/**
 * @brief Read data from UART device. Blocking function.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return positive number of received bytes in case of success, negative error code otherwise.
 */
int32_t uart_read(struct uart_desc *desc, uint8_t *data, uint32_t bytes_number)
{
	int32_t ret;
	uint32_t total_read = 0;
	uint32_t read;

	if (!desc || !data || !bytes_number)
		return -EINVAL;

	while (bytes_number) {
		read = MXC_UART_ReadRXFIFO(MXC_UART_GET_UART(desc->device_id),
					   data + total_read, bytes_number);
		total_read += read;
		bytes_number -= read;
	}

	return total_read;
}

/**
 * @brief Write data to UART device. Blocking function.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t uart_write(struct uart_desc *desc, const uint8_t *data,
		   uint32_t bytes_number)
{
	uint32_t ret;
	uint32_t written = 0;
	uint32_t total_written = 0;
	mxc_uart_regs_t *uart_regs;

	if(!desc || !data || !bytes_number)
		return -EINVAL;

	uart_regs = MXC_UART_GET_UART(desc->device_id);
	while (bytes_number) {
		written = MXC_UART_WriteTXFIFO(MXC_UART_GET_UART(desc->device_id),
					       data + total_written, bytes_number);
		total_written += written;
		bytes_number -= written;
	}

	return total_written;
}

/**
 * @brief Read data from UART device. Non blocking function.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return positive number of received bytes in case of success, negative error code otherwise.
 */
int32_t uart_read_nonblocking(struct uart_desc *desc, uint8_t *data,
			      uint32_t bytes_number)
{
	int32_t ret;
	uint32_t id;

	if (!desc || !data || !bytes_number)
		return -EINVAL;

	id = desc->device_id;
	rx_state[id].uart = MXC_UART_GET_UART(id);
	rx_state[id].rxData = data;
	rx_state[id].rxLen = bytes_number;
	rx_state[id].txData = NULL;
	rx_state[id].txLen = 0;
	rx_state[id].callback = NULL;

	ret = MXC_UART_TransactionAsync(&rx_state[id]);

	if (ret == E_BUSY)
		return -EBUSY;

	return 0;
}

/**
 * @brief Write data to UART device. Non blocking function.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, errno codes otherwise.
 */

int32_t uart_write_nonblocking(struct uart_desc *desc, const uint8_t *data,
			       uint32_t bytes_number)
{
	int32_t ret;
	uint32_t id;

	if (!desc || !data || !bytes_number)
		return -EINVAL;

	id = desc->device_id;
	tx_state[id].uart = MXC_UART_GET_UART(id);
	tx_state[id].txData = data;
	tx_state[id].txLen = bytes_number;
	tx_state[id].rxData = NULL;
	tx_state[id].rxLen = 0;
	tx_state[id].callback = NULL;

	ret = MXC_UART_TransactionAsync(&tx_state[id]);

	if (ret == E_BUSY)
		return -EBUSY;

	return 0;
}

/**
 * @brief Initialize the UART communication peripheral.
 * @param desc - The UART descriptor.
 * @param param - The structure that contains the UART parameters.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t uart_init(struct uart_desc **desc, struct uart_init_param *param)
{
	int32_t ret;
	int32_t stop, size, flow, parity;
	mxc_uart_regs_t *uart_regs;
	struct max_uart_init_param *eparam;
	struct uart_desc *descriptor;
	sys_map_t map;

	if (!param || !param->extra)
		return -EINVAL;

	descriptor = calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	map = MAP_A;
	uart_regs = MXC_UART_GET_UART(param->device_id);
	eparam = param->extra;

	descriptor->device_id = param->device_id;
	descriptor->baud_rate = param->baud_rate;

	switch(param->parity) {
	case UART_PAR_NO:
		parity = MXC_UART_PARITY_DISABLE;
		break;
	case UART_PAR_MARK:
		parity = MXC_UART_PARITY_MARK;
		break;
	case UART_PAR_SPACE:
		parity = MXC_UART_PARITY_SPACE;
		break;
	case UART_PAR_ODD:
		parity = MXC_UART_PARITY_ODD;
		break;
	case UART_PAR_EVEN:
		parity = MXC_UART_PARITY_EVEN;
		break;
	default:
		ret = -EINVAL;
		goto error;
	}

	switch(param->size) {
	case UART_CS_5:
		size = 5;
		break;
	case UART_CS_6:
		size = 6;
		break;
	case UART_CS_7:
		size = 7;
		break;
	case UART_CS_8:
		size = 8;
		break;
	default:
		ret = -EINVAL;
		goto error;
	}

	switch(param->stop) {
	case UART_STOP_1_BIT:
		stop = MXC_UART_STOP_1;
		break;
	case UART_STOP_2_BIT:
		stop = MXC_UART_STOP_2;
		break;
	default:
		ret = -EINVAL;
		goto error;
	}

	switch (eparam->flow) {
	case UART_FLOW_DIS:
		flow = MXC_UART_FLOW_DIS;
		break;
	case UART_FLOW_LOW:
		flow = MXC_UART_FLOW_EN_LOW;
		break;
	case UART_FLOW_HIGH:
		flow = MXC_UART_FLOW_EN_HIGH;
		break;
	default:
		ret = -EINVAL;
		goto error;
	}

	ret = MXC_UART_Init(uart_regs, descriptor->baud_rate, map);

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

	return 0;
error:
	free(descriptor);
	MXC_UART_Shutdown(uart_regs);

	return ret;
}

/**
 * @brief Free the resources allocated by uart_init().
 * @param desc - The UART descriptor.
 * @return 0 in case of success, errno codes otherwise.
 */
int32_t uart_remove(struct uart_desc *desc)
{
	if (!desc)
		return -EINVAL;

	MXC_UART_Shutdown(MXC_UART_GET_UART(desc->device_id));
	free(desc);

	return 0;
}

/**
 * @brief Not implemented.
 * @param desc - The UART descriptor.
 * @return -ENOSYS
 */
uint32_t uart_get_errors(struct uart_desc *desc)
{
	return -ENOSYS;
}

/**
 * @brief Initialize the UART irq descriptor
 * @param desc - The UART irq descriptor.
 * @param desc - The init parameter.
 * @return 0 in case of success, errno codes otherwise.
 */
static int32_t max_uart_irq_ctrl_init(struct irq_ctrl_desc **desc,
				      const struct irq_init_param *param)
{
	struct irq_ctrl_desc *descriptor;

	if (!param)
		return -EINVAL;

	descriptor = calloc(1, sizeof(*descriptor));

	if (!descriptor)
		return -EINVAL;

	descriptor->irq_ctrl_id = param->irq_ctrl_id;
	descriptor->platform_ops = param->platform_ops;
	descriptor->extra = param->extra;

	*desc = descriptor;

	return 0;
}

/**
 * @brief Free the UART irq descriptor
 * @param desc - The UART descriptor.
 * @return 0 in case of success, errno codes otherwise.
 */
static int32_t max_uart_irq_ctrl_remove(struct irq_ctrl_desc *desc)
{
	if (!desc)
		return -EINVAL;

	free(desc);

	return 0;
}

/**
 * @brief Register a function to be called when an interrupt occurs.
 * @param desc - The UART irq descriptor.
 * @param irq_id - The port on which the callback is registered.
 * @param callback_desc - The callback_descriptor.
 * @return 0 in case of success, errno codes otherwise.
 */
static int32_t max_uart_register_callback(struct irq_ctrl_desc *desc,
		uint32_t irq_id,
		struct callback_desc *callback_desc)
{
	if (!desc || !callback_desc || irq_id >= N_PORTS)
		return -EINVAL;

	if (!cb[irq_id]) {
		cb[irq_id] = calloc(1, sizeof(*cb[irq_id]));
		if (!cb[irq_id])
			return -ENOMEM;
	}

	cb[irq_id]->ctx = callback_desc->ctx;
	cb[irq_id]->callback = callback_desc->callback;
	cb[irq_id]->config = callback_desc->config;

	return 0;
}

/**
 * @brief Unregister a callback function.
 * @param desc - The UART irq descriptor.
 * @param irq_id - The UART port.
 * @return 0 in case of success, errno codes otherwise.
 */
static int32_t max_uart_unregister_callback(struct irq_ctrl_desc *desc,
		uint32_t irq_id)
{
	if (irq_id >= N_PORTS || !cb[irq_id])
		return -EINVAL;

	free(cb[irq_id]);
	cb[irq_id] = NULL;

	return 0;
}

const struct irq_platform_ops max_uart_irq_ops = {
	.init = &max_uart_irq_ctrl_init,
	.remove = &max_uart_irq_ctrl_remove,
	.register_callback = &max_uart_register_callback,
	.unregister = &max_uart_unregister_callback
};
