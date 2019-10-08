/***************************************************************************//**
 *   @file   serial.c
 *   @brief  Header file of Serial interface.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "error.h"
#include "fifo.h"
#include "xuartps.h"
#include "uart.h"
#include "irq.h"
#include "xilinx_platform_drivers.h"

#define BUFF_LENGTH 256
static char buff[BUFF_LENGTH];
static volatile uint32_t bytes_reveived = 0;
static int32_t total_error_count;

static ssize_t uart_irq_init(struct uart_desc *descriptor);

//static ssize_t serial_setup_interrupt_system(struct uart_desc *descriptor);

static void serial_handler(void *call_back_ref, u32 event, uint32_t data_len);

static void uart_receive (struct uart_desc *desc) {
	if (bytes_reveived > 0) {
		fifo_insert_tail(&desc->fifo, buff, bytes_reveived);
		bytes_reveived = 0;
		XUartPs_Recv(desc->instance, (u8*)buff, BUFF_LENGTH);
	}
}

static ssize_t uart_read_byte(struct uart_desc *desc, char *buf)
{
	while (desc->fifo == NULL) {
		/* nothing in fifo, wait until something is received */
		uart_receive(desc);
	}

	*buf = desc->fifo->data[desc->fifo->index];
	desc->fifo->index++;

	if (desc->fifo->len - desc->fifo->index <= 0) {
		desc->fifo = fifo_remove_head(desc->fifo);
	}
	return SUCCESS;
}

/***************************************************************************//**
 * @brief network_read
*******************************************************************************/
ssize_t uart_read(struct uart_desc *desc, char *buf, size_t len)
{
	ssize_t ret;
    for (uint32_t i = 0; i < len; i++) {
    	ret = uart_read_byte(desc, &buf[i]);
    	if (ret < 0)
    		return ret;
	}

	return len;
}

/***************************************************************************//**
 * @brief serial_write_data
*******************************************************************************/
ssize_t uart_write(struct uart_desc *desc, const char *buf, size_t len)
{
	size_t total_sent = XUartPs_Send(desc->instance, (u8*)buf, len);

	while (XUartPs_IsSending(desc->instance));
	if (total_sent < len)
		return FAILURE;

    return SUCCESS;
}

/***************************************************************************//**
 * @brief serial_init
*******************************************************************************/
ssize_t uart_init(struct uart_desc **desc, struct uart_init_par *par)
{
    int32_t status;
    struct uart_desc *descriptor;

    descriptor = calloc(1, sizeof(struct uart_desc));
    descriptor->baud_rate = par->baud_rate;
    descriptor->device_id = par->device_id;
    descriptor->irq_id = par->irq_id;
    descriptor->instance = calloc(1, sizeof(XUartPs));
    descriptor->irq_desc = par->irq_desc;

    status = uart_irq_init(descriptor);
    if (status != XST_SUCCESS) {
        return FAILURE;
    }
    *desc = descriptor;

    XUartPs_Recv(descriptor->instance, (u8*)buff, BUFF_LENGTH);

    return SUCCESS;
}

ssize_t uart_remove(struct uart_desc *desc) {
	return SUCCESS;
}

/***************************************************************************//**
 * @brief uart_irq_init
*******************************************************************************/
static ssize_t uart_irq_init(struct uart_desc *descriptor)
{
    XUartPs_Config *config;
    int32_t status;
    u32 intr_mask;
    /*
     * Initialize the UART driver so that it's ready to use
     * Look up the configuration in the config table, then initialize it.
     */
    config = XUartPs_LookupConfig(descriptor->device_id);
    if (NULL == config) {
        return FAILURE;
    }

    status = XUartPs_CfgInitialize(descriptor->instance, config, config->BaseAddress);
    if (status != XST_SUCCESS) {
        return FAILURE;
    }

    irq_register(descriptor->irq_desc, descriptor->irq_id,
    		                             (Xil_ExceptionHandler) XUartPs_InterruptHandler,
    		                             descriptor->instance);

    /*
     * Setup the handlers for the UART that will be called from the
     * interrupt context when data has been sent and received, specify
     * a pointer to the UART driver instance as the callback reference
     * so the handlers are able to access the instance data
     */
    XUartPs_SetHandler(descriptor->instance, serial_handler, descriptor->instance);

    /*
     * Enable the interrupt of the UART so interrupts will occur, setup
     * a local loopback so data that is sent will be received.
     */
    intr_mask =
        XUARTPS_IXR_TOUT | XUARTPS_IXR_PARITY | XUARTPS_IXR_FRAMING |
        XUARTPS_IXR_OVER | XUARTPS_IXR_TXEMPTY | XUARTPS_IXR_RXFULL |
        XUARTPS_IXR_RXOVR;

    if (descriptor->instance->Platform == XPLAT_ZYNQ_ULTRA_MP) {
        intr_mask |= XUARTPS_IXR_RBRK;
    }

    XUartPs_SetInterruptMask(descriptor->instance, intr_mask);

    XUartPs_SetOperMode(descriptor->instance, XUARTPS_OPER_MODE_NORMAL);
    XUartPs_SetBaudRate(descriptor->instance, descriptor->baud_rate);
    /*
     * Set the receiver timeout. If it is not set, and the last few bytes
     * of data do not trigger the over-water or full interrupt, the bytes
     * will not be received. By default it is disabled.
     *
     * The setting of 8 will timeout after 8 x 4 = 32 character times.
     * Increase the time out value if baud rate is high, decrease it if
     * baud rate is low.
     */
    XUartPs_SetRecvTimeout(descriptor->instance, 8);

    irq_source_enable(descriptor->irq_desc, descriptor->irq_id);

    irq_enable();

    return SUCCESS;
}

/**************************************************************************/
/**
* serial_handler
***************************************************************************/
static void serial_handler(void *call_back_ref, uint32_t event, uint32_t data_len)
{
    /* All of the data has been received */
    if (event == XUARTPS_EVENT_RECV_DATA) {
    	bytes_reveived = data_len;
    }

    /*
     * Data was received, but not the expected number of bytes, a
     * timeout just indicates the data stopped for 8 character times
     */
    if (event == XUARTPS_EVENT_RECV_TOUT) {
    	bytes_reveived = data_len;
    }

    /*
     * Data was received with an error, keep the data but determine
     * what kind of errors occurred
     */
    if (event == XUARTPS_EVENT_RECV_ERROR) {
        total_error_count++;
    }

    /*
     * Data was received with an parity or frame or break error, keep the data
     * but determine what kind of errors occurred. Specific to Zynq Ultrascale+
     * MP.
     */
    if (event == XUARTPS_EVENT_PARE_FRAME_BRKE) {
        total_error_count++;
    }

    /*
     * Data was received with an overrun error, keep the data but determine
     * what kind of errors occurred. Specific to Zynq Ultrascale+ MP.
     */
    if (event == XUARTPS_EVENT_RECV_ORERR) {
        total_error_count++;
    }
}



