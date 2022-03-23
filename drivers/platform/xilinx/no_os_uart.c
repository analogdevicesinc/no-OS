/******************************************************************************
* @file   xilinx/no_os_uart.c
********************************************************************************
*
* Copyright (C) 2010 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <xparameters.h>
#include <stdio.h>
#include <stdlib.h>
#include "no_os_error.h"
#include "no_os_uart.h"
#include "uart_extra.h"
#ifdef XPAR_XUARTPS_NUM_INSTANCES
#include "no_os_irq.h"
#include "no_os_fifo.h"
#include <xil_exception.h>
#include <xuartps.h>
#endif
#ifdef XPAR_XUARTLITE_NUM_INSTANCES
#include <xuartlite.h>
#include <xuartlite_l.h>
#endif

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define WRITE_SIZE 255

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

#ifdef XUARTPS_H
/**
 * @brief Save received data into fifo.
 * @param desc - Instance descriptor containing a fifo element.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t uart_fifo_insert(struct no_os_uart_desc *desc)
{
	int32_t ret;
	struct xil_uart_desc *xil_uart_desc = desc->extra;
	struct no_os_irq_ctrl_desc *irq_desc = xil_uart_desc->irq_desc;

	if (xil_uart_desc->bytes_received > 0) {
		ret = no_os_irq_disable(irq_desc, xil_uart_desc->irq_id);
		if (ret < 0)
			return ret;
		ret = no_os_fifo_insert(&xil_uart_desc->fifo, xil_uart_desc->buff,
					xil_uart_desc->bytes_received);
		if (ret < 0)
			return ret;
		xil_uart_desc->bytes_received = 0;
		switch(xil_uart_desc->type) {
		case UART_PS:
			XUartPs_Recv(xil_uart_desc->instance, (u8*)(xil_uart_desc->buff),
				     UART_BUFF_LENGTH);
			break;
		case UART_PL:

			break;
		default:
			return -1;
			break;
		}
		ret = no_os_irq_enable(irq_desc, xil_uart_desc->irq_id);
		if (ret < 0)
			return ret;
	}

	return 0;
}
#endif // XUARTPS_H

/**
 * @brief Read byte from fifo.
 * @param desc - Instance descriptor containing a fifo element
 * @param data - read value.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t no_os_uart_read_byte(struct no_os_uart_desc *desc, uint8_t *data)
{
	struct xil_uart_desc *xil_uart_desc = desc->extra;
#ifdef XUARTLITE_H
	XUartLite *instance = xil_uart_desc->instance;
#endif
#ifdef XUARTPS_H
	int32_t ret;
#endif

	switch(xil_uart_desc->type) {
	case UART_PS:
#ifdef XUARTPS_H
		while (xil_uart_desc->fifo == NULL) {
			/* nothing in fifo, wait until something is received */
			ret = uart_fifo_insert(desc);
			if (ret < 0)
				return ret;
		}

		*data = xil_uart_desc->fifo->data[xil_uart_desc->fifo_read_offset];
		xil_uart_desc->fifo_read_offset++;

		if (xil_uart_desc->fifo->len - xil_uart_desc->fifo_read_offset <= 0) {
			xil_uart_desc->fifo_read_offset = 0;
			xil_uart_desc->fifo = no_os_fifo_remove(xil_uart_desc->fifo);
		}
#endif // XUARTPS_H
		break;
	case UART_PL:
#ifdef XUARTLITE_H
		while (!(Xil_In32(instance->RegBaseAddress + XUL_STATUS_REG_OFFSET) &
			 XUL_SR_RX_FIFO_VALID_DATA));
		*data = Xil_In32(instance->RegBaseAddress + XUL_RX_FIFO_OFFSET);
#endif // XUARTLITE_H
		break;
	default:
		break;
	}

	return 0;
}

/**
 * @brief Read data from UART device.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_uart_read(struct no_os_uart_desc *desc, uint8_t *data,
			uint32_t bytes_number)
{
	int ret;
	for (uint32_t i = 0; i < bytes_number; i++) {
		ret = no_os_uart_read_byte(desc, &data[i]);
		if (ret < 0)
			return ret;
	}

	return bytes_number;
}

/**
 * @brief Write data to UART device.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_uart_write(struct no_os_uart_desc *desc, const uint8_t *data,
			 uint32_t bytes_number)
{
	struct xil_uart_desc *xil_uart_desc = desc->extra;
#ifdef XUARTLITE_H
	XUartLite *instance = xil_uart_desc->instance;
#endif
#ifdef XUARTPS_H
	uint32_t bytes_sent, len;
#endif
	uint32_t offset = 0;

	switch(xil_uart_desc->type) {
	case UART_PS:
#ifdef XUARTPS_H
		while (offset < bytes_number) {
			len = ((bytes_number - offset) < WRITE_SIZE) ? (bytes_number - offset) :
			      WRITE_SIZE;
			bytes_sent = XUartPs_Send(xil_uart_desc->instance, (u8*)(data + offset),
						  len);
			offset += bytes_sent;
		}
		break;
#endif // XUARTPS_H
	case UART_PL:
#ifdef XUARTLITE_H
		while (offset < bytes_number) {
			while ((Xil_In32(instance->RegBaseAddress + XUL_STATUS_REG_OFFSET) &
				XUL_SR_TX_FIFO_FULL));
			Xil_Out32(instance->RegBaseAddress + XUL_TX_FIFO_OFFSET, data[offset]);
			offset++;
		}
		while (!(Xil_In32(instance->RegBaseAddress + XUL_STATUS_REG_OFFSET) &
			 XUL_SR_TX_FIFO_EMPTY));
#endif // XUARTLITE_H
		break;
	default:
		return -1;
	}

	return bytes_number;
}

#ifdef XUARTPS_H
/**
 * @brief UART interrupt handler.
 * @param call_back_ref - Instance of UART.
 * @param event - Event that caused the interrupt.
 * @param data_len - number of bytes received.
 */
static void uart_irq_handler(void *call_back_ref, uint32_t event,
			     uint32_t data_len)
{
	struct xil_uart_desc *xil_uart_desc = call_back_ref;

	switch(xil_uart_desc->type) {
	case UART_PS:
		switch(event) {
		/* All of the data has been received */
		case XUARTPS_EVENT_RECV_DATA:
		/*
		 * Data was received, but not the expected number of bytes, a
		 * timeout just indicates the data stopped for configured character time
		 */
		case XUARTPS_EVENT_RECV_TOUT:
			xil_uart_desc->bytes_received = data_len;
			break;
		/*
		 * Data was received with an error, keep the data but determine
		 * what kind of errors occurred
		 */
		case XUARTPS_EVENT_RECV_ERROR:
		/*
		 * Data was received with an parity or frame or break error, keep the data
		 * but determine what kind of errors occurred. Specific to Zynq Ultrascale+
		 * MP.
		 */
		case XUARTPS_EVENT_PARE_FRAME_BRKE:
		/*
		 * Data was received with an overrun error, keep the data but determine
		 * what kind of errors occurred. Specific to Zynq Ultrascale+ MP.
		 */
		case XUARTPS_EVENT_RECV_ORERR:
			xil_uart_desc->total_error_count++;
			break;
		default:
			break;
		}
		break;
	case UART_PL:

		break;
	default:
		break;
	}
}
#endif // XUARTPS_H

#ifdef XUARTPS_H
/**
 * @brief UART interrupt init.
 * @param desc - Instance of UART containing a pointer to handler.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t uart_irq_init(struct no_os_uart_desc *descriptor)
{
	int32_t status;
	uint32_t uart_irq_mask;
	struct xil_uart_desc *xil_uart_desc = descriptor->extra;
	struct no_os_callback_desc callback_desc;

	switch(xil_uart_desc->type) {
	case UART_PS:
		callback_desc.legacy_callback = (void (*)())XUartPs_InterruptHandler;
		callback_desc.ctx = xil_uart_desc->instance;
		status = no_os_irq_register_callback(xil_uart_desc->irq_desc,
						     xil_uart_desc->irq_id,
						     &callback_desc);
		if (status < 0)
			return status;
		XUartPs_SetHandler(xil_uart_desc->instance, uart_irq_handler, xil_uart_desc);
		/*
		 * Enable the interrupt of the UART so interrupts will occur
		 */
		uart_irq_mask =
			XUARTPS_IXR_TOUT | XUARTPS_IXR_PARITY | XUARTPS_IXR_FRAMING |
			XUARTPS_IXR_OVER | XUARTPS_IXR_TXEMPTY | XUARTPS_IXR_RXFULL |
			XUARTPS_IXR_RXOVR | XUARTPS_IXR_RBRK;

		XUartPs_SetInterruptMask(xil_uart_desc->instance, uart_irq_mask);

		break;
	case UART_PL:

		break;
	default:
		return -1;
	}

	status = no_os_irq_enable(xil_uart_desc->irq_desc, xil_uart_desc->irq_id);
	if (status < 0)
		return status;

	return 0;
}
#endif // XUARTPS_H

/**
 * @brief Initialize the UART communication peripheral.
 * @param desc - The UART descriptor.
 * @param param - The structure that contains the UART parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_uart_init(struct no_os_uart_desc **desc,
			struct no_os_uart_init_param *param)
{
	struct no_os_uart_desc *descriptor;
	struct xil_uart_init_param *xil_uart_init_param;
	struct xil_uart_desc *xil_uart_desc;
#ifdef XUARTPS_H
	XUartPs_Config *config;
#endif // XUARTPS_H
#ifdef XUARTLITE_H
	XUartLite_Config *config;
#endif // XUARTLITE_H
	int32_t status;

	descriptor = calloc(1, sizeof(struct no_os_uart_desc));
	if (!descriptor)
		return -1;
	xil_uart_desc = calloc(1, sizeof(struct xil_uart_desc));
	if (!xil_uart_desc)
		goto error_free_descriptor;

	descriptor->extra = xil_uart_desc;
	descriptor->baud_rate = param->baud_rate;
	descriptor->device_id = param->device_id;
	xil_uart_init_param = param->extra;
	xil_uart_desc = descriptor->extra;
	xil_uart_desc->irq_id = xil_uart_init_param->irq_id;
	xil_uart_desc->irq_desc = xil_uart_init_param->irq_desc;
	xil_uart_desc->type = xil_uart_init_param->type;

	switch(xil_uart_desc->type) {
	case UART_PS:
#ifdef XUARTPS_H
		xil_uart_desc->instance = calloc(1, sizeof(XUartPs));
		if (!(xil_uart_desc->instance))
			goto error_free_xil_uart_desc;
		/*
		 * Initialize the UART driver so that it's ready to use
		 * Look up the configuration in the config table, then initialize it.
		 */
		config = XUartPs_LookupConfig(descriptor->device_id);
		if (!config)
			goto error_free_instance;

		status = XUartPs_CfgInitialize(xil_uart_desc->instance, config,
					       config->BaseAddress);
		if (status != XST_SUCCESS)
			goto error_free_instance;

		XUartPs_SetOperMode(xil_uart_desc->instance, XUARTPS_OPER_MODE_NORMAL);

		status = XUartPs_SetBaudRate(xil_uart_desc->instance, descriptor->baud_rate);
		if (status != XST_SUCCESS)
			goto error_free_instance;

		/*
		 * Set the receiver timeout. If it is not set, and the last few bytes
		 * of data do not trigger the over-water or full interrupt, the bytes
		 * will not be received. By default it is disabled.
		 *
		 * The setting of 8 will timeout after 8 x 4 = 32 character times.
		 * Increase the time out value if baud rate is high, decrease it if
		 * baud rate is low.
		 */
		XUartPs_SetRecvTimeout(xil_uart_desc->instance, 8);

		status = uart_irq_init(descriptor);
		if (status != XST_SUCCESS)
			goto error_free_instance;

		*desc = descriptor;

		XUartPs_Recv(xil_uart_desc->instance, (u8*)xil_uart_desc->buff,
			     UART_BUFF_LENGTH);

		break;
#endif // XUARTPS_H
	case UART_PL:
#ifdef XUARTLITE_H
		xil_uart_desc->instance = calloc(1, sizeof(XUartLite));
		if (!(xil_uart_desc->instance))
			goto error_free_xil_uart_desc;

		config = XUartLite_LookupConfig(descriptor->device_id);
		if (!config)
			goto error_free_instance;

		status = XUartLite_CfgInitialize(xil_uart_desc->instance, config,
						 config->RegBaseAddr);
		if (status != XST_SUCCESS)
			goto error_free_instance;

		/* Discard old data */
		while (XUartLite_Recv(xil_uart_desc->instance, (uint8_t *)&status, 1));

		*desc = descriptor;
#endif // XUARTLITE_H
		break;
	default:
		goto error_free_xil_uart_desc;
		break;
	}

	return 0;

error_free_instance:
	free(xil_uart_desc->instance);
error_free_xil_uart_desc:
	free(xil_uart_desc);
error_free_descriptor:
	free(descriptor);

	return -1;
}

/**
 * @brief Free the resources allocated by no_os_uart_init().
 * @param desc - The UART descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t no_os_uart_remove(struct no_os_uart_desc *desc)
{
	struct xil_uart_desc *xil_uart_desc = desc->extra;
	free(xil_uart_desc->instance);
	free(xil_uart_desc);
	free(desc);

	return 0;
}

/**
 * @brief Get number of UART errors.
 * @param desc - The UART descriptor.
 * @return number of errors.
 */
uint32_t no_os_uart_get_errors(struct no_os_uart_desc *desc)
{
	struct xil_uart_desc *xil_uart_desc = desc->extra;
	uint32_t total_error_count = xil_uart_desc->total_error_count;
	xil_uart_desc->total_error_count = 0;

	return total_error_count;
}

int32_t no_os_uart_read_nonblocking(struct no_os_uart_desc *desc, uint8_t *data,
				    uint32_t bytes_number)
{
	return -EINVAL;
}
