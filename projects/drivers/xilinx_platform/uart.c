/******************************************************************************
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

#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "fifo.h"
#include "uart.h"
#include "irq.h"
#include "xilinx_platform_drivers.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Save received data into fifo.
 * @param desc - Instance descriptor containing a fifo element.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
static int32_t uart_fifo_insert(struct uart_desc *desc) {
	int32_t ret;
	struct xil_uart_desc *xil_uart_desc = desc->extra;
	struct irq_desc	*irq_desc = xil_uart_desc->irq_desc;

	if (xil_uart_desc->bytes_reveived > 0) {
		ret = irq_source_disable(irq_desc, xil_uart_desc->irq_id);
		if (ret < 0)
			return ret;
		ret = fifo_insert(&xil_uart_desc->fifo, xil_uart_desc->buff, xil_uart_desc->bytes_reveived);
		if (ret < 0)
			return ret;
		xil_uart_desc->bytes_reveived = 0;
		XUartPs_Recv(xil_uart_desc->instance, (u8*)(xil_uart_desc->buff), BUFF_LENGTH);
		ret = irq_source_enable(irq_desc, xil_uart_desc->irq_id);
		if (ret < 0)
			return ret;
	}

	return SUCCESS;
}

/**
 * @brief Read byte from fifo.
 * @param desc - Instance descriptor containing a fifo element
 * @param data - read value.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
static int32_t uart_read_byte(struct uart_desc *desc, uint8_t *data)
{
	struct xil_uart_desc *xil_uart_desc = desc->extra;
	int32_t ret;

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
		xil_uart_desc->fifo = fifo_remove(xil_uart_desc->fifo);
	}

	return SUCCESS;
}

/**
 * @brief Read data from UART device.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t uart_read(struct uart_desc *desc, uint8_t *data, uint32_t bytes_number)
{
	ssize_t ret;
    for (uint32_t i = 0; i < bytes_number; i++) {
    	ret = uart_read_byte(desc, &data[i]);
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
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t uart_write(struct uart_desc *desc, const uint8_t *data, uint32_t bytes_number)
{
	struct xil_uart_desc *xil_uart_desc = desc->extra;
	size_t total_sent = XUartPs_Send(xil_uart_desc->instance, (u8*)data, bytes_number);
	while (XUartPs_IsSending(xil_uart_desc->instance));
	if (total_sent < bytes_number)
		return FAILURE;

    return SUCCESS;
}

/**
 * @brief UART interrupt handler.
 * @param call_back_ref - Instance of UART.
 * @param event - Event that caused the interrupt.
 * @param data_len - number of bytes received.
 */
static void uart_irq_handler(void *call_back_ref, uint32_t event, uint32_t data_len)
{
	struct xil_uart_desc *xil_uart_desc = call_back_ref;
    /* All of the data has been received */
    if (event == XUARTPS_EVENT_RECV_DATA) {
    	xil_uart_desc->bytes_reveived = data_len;
    }

    /*
     * Data was received, but not the expected number of bytes, a
     * timeout just indicates the data stopped for 8 character times
     */
    if (event == XUARTPS_EVENT_RECV_TOUT) {
    	xil_uart_desc->bytes_reveived = data_len;
    }

    /*
     * Data was received with an error, keep the data but determine
     * what kind of errors occurred
     */
    if (event == XUARTPS_EVENT_RECV_ERROR) {
    	xil_uart_desc->total_error_count++;
    }

    /*
     * Data was received with an parity or frame or break error, keep the data
     * but determine what kind of errors occurred. Specific to Zynq Ultrascale+
     * MP.
     */
    if (event == XUARTPS_EVENT_PARE_FRAME_BRKE) {
    	xil_uart_desc->total_error_count++;
    }

    /*
     * Data was received with an overrun error, keep the data but determine
     * what kind of errors occurred. Specific to Zynq Ultrascale+ MP.
     */
    if (event == XUARTPS_EVENT_RECV_ORERR) {
    	xil_uart_desc->total_error_count++;
    }
}

/**
 * @brief UART interrupt init.
 * @param desc - Instance of UART containing a pointer to handler.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
static int32_t uart_irq_init(struct uart_desc *descriptor)
{
    uint32_t uart_irq_mask;
    struct xil_uart_desc *xil_uart_desc = descriptor->extra;

    int32_t status = irq_register(xil_uart_desc->irq_desc, xil_uart_desc->irq_id,
        		                             (Xil_ExceptionHandler) XUartPs_InterruptHandler, xil_uart_desc->instance);
    if (status < 0)
    	return status;
    XUartPs_SetHandler(xil_uart_desc->instance, uart_irq_handler, xil_uart_desc);
    /*
     * Enable the interrupt of the UART so interrupts will occur, setup
     * a local loopback so data that is sent will be received.
     */
    uart_irq_mask =
        XUARTPS_IXR_TOUT | XUARTPS_IXR_PARITY | XUARTPS_IXR_FRAMING |
        XUARTPS_IXR_OVER | /*XUARTPS_IXR_TXEMPTY | */XUARTPS_IXR_RXFULL |
        XUARTPS_IXR_RXOVR;

    if (xil_uart_desc->instance->Platform == XPLAT_ZYNQ_ULTRA_MP) {
        uart_irq_mask |= XUARTPS_IXR_RBRK;
    }

    XUartPs_SetInterruptMask(xil_uart_desc->instance, uart_irq_mask);

    status = irq_source_enable(xil_uart_desc->irq_desc, xil_uart_desc->irq_id);
    if (status < 0)
        return status;

    return SUCCESS;
}

/**
 * @brief Initialize the UART communication peripheral.
 * @param desc - The UART descriptor.
 * @param init_param - The structure that contains the UART parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t uart_init(struct uart_desc **desc, struct uart_init_par *par)
{
    int32_t status;
    struct uart_desc *descriptor;
    XUartPs_Config *config;
    struct xil_uart_init_param *xil_uart_init_param;
    struct xil_uart_desc *xil_uart_desc;

    descriptor = calloc(1, sizeof(struct uart_desc));
    xil_uart_desc = calloc(1, sizeof(struct xil_uart_desc));
    descriptor->extra = xil_uart_desc;
    descriptor->baud_rate = par->baud_rate;
    descriptor->device_id = par->device_id;
    xil_uart_init_param = par->extra;
    xil_uart_desc = descriptor->extra;
    xil_uart_desc->irq_id = xil_uart_init_param->irq_id;
    xil_uart_desc->irq_desc = xil_uart_init_param->irq_desc;
    xil_uart_desc->instance = calloc(1, sizeof(XUartPs));

    /*
     * Initialize the UART driver so that it's ready to use
     * Look up the configuration in the config table, then initialize it.
     */
    config = XUartPs_LookupConfig(descriptor->device_id);
    if (!config) {
        return FAILURE;
    }

    status = XUartPs_CfgInitialize(xil_uart_desc->instance, config, config->BaseAddress);
    if (status != XST_SUCCESS) {
        return FAILURE;
    }

    XUartPs_SetOperMode(xil_uart_desc->instance, XUARTPS_OPER_MODE_NORMAL);

    status = XUartPs_SetBaudRate(xil_uart_desc->instance, descriptor->baud_rate);
    if (status != XST_SUCCESS) {
		return FAILURE;
	}

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
    if (status != XST_SUCCESS) {
        return FAILURE;
    }
    *desc = descriptor;

    XUartPs_Recv(xil_uart_desc->instance, (u8*)xil_uart_desc->buff, BUFF_LENGTH);

    return SUCCESS;
}

/**
 * @brief Free the resources allocated by uart_init().
 * @param desc - The UART descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t uart_remove(struct uart_desc *desc) {
	struct xil_uart_desc *xil_uart_desc = desc->extra;
	free(xil_uart_desc->instance);
	free(xil_uart_desc);
	free(desc);

	return SUCCESS;
}

/**
 * @brief Get number of UART errors.
 * @param desc - The UART descriptor.
 * @return number of errors.
 */
uint32_t uart_get_errors(struct uart_desc *desc) {
	struct xil_uart_desc *xil_uart_desc = desc->extra;
	uint32_t total_error_count = xil_uart_desc->total_error_count;
	xil_uart_desc->total_error_count = 0;

	return total_error_count;
}



