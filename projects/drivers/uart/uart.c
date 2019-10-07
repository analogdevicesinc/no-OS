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
#include <fifo.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "error.h"
#include "xuartps.h"
#include "uart.h"
#ifdef XPAR_INTC_0_DEVICE_ID
#include "xintc.h"
#else
#include "xscugic.h"
#endif

#define BUFF_LENGTH 256
static char buff[BUFF_LENGTH];
static volatile uint32_t bytes_reveived = 0;
static int32_t TotalErrorCount;

static ssize_t serial_ps_intr(struct uart_desc *descriptor);

static ssize_t serial_setup_interrupt_system(struct uart_desc *descriptor);

static void serial_handler(void *CallBackRef, u32 Event, uint32_t EventData);

static void uart_receive (struct uart_desc *desc) {
	if (bytes_reveived > 0) {
		fifo_insert_tail(&desc->fifo, buff, bytes_reveived);
		bytes_reveived = 0;
		XUartPs_Recv(desc->UartInstancePtr, (u8*)buff, BUFF_LENGTH);
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
    for ( int32_t i = 0; i < len; i++)
        outbyte(buf[i]);
    return SUCCESS;
}

/***************************************************************************//**
 * @brief serial_init
*******************************************************************************/
ssize_t uart_init(struct uart_desc **desc, struct uart_init_par *par)
{
    int32_t Status;
    struct uart_desc *descriptor;

    descriptor = calloc(1, sizeof(struct uart_desc));
    descriptor->baud_rate = par->baud_rate;
    descriptor->UartDeviceId = par->id;
    descriptor->uart_irq_id = par->uart_irq_id;
    descriptor->ctrl_irq_id = par->ctrl_irq_id;
    descriptor->IntcInstancePtr = calloc(1, sizeof(XScuGic));
    descriptor->UartInstancePtr = calloc(1, sizeof(XUartPs));


    /* Run the UartPs Interrupt example, specify the the Device ID */
    Status = serial_ps_intr(descriptor);
    if (Status != XST_SUCCESS) {
        return FAILURE;
    }

    XUartPs_Recv(descriptor->UartInstancePtr, (u8*)buff, BUFF_LENGTH);
    *desc = descriptor;

    return SUCCESS;
}

/***************************************************************************//**
 * @brief serial_ps_intr
*******************************************************************************/
static ssize_t serial_ps_intr(struct uart_desc *descriptor)
{
    XUartPs_Config *Config;
    int32_t Status;
    u32 IntrMask;

    /*
     * Initialize the UART driver so that it's ready to use
     * Look up the configuration in the config table, then initialize it.
     */
    Config = XUartPs_LookupConfig(descriptor->UartDeviceId);
    if (NULL == Config) {
        return FAILURE;
    }

    Status = XUartPs_CfgInitialize(descriptor->UartInstancePtr, Config, Config->BaseAddress);
    if (Status != XST_SUCCESS) {
        return FAILURE;
    }

    /* Check hardware build */
    Status = XUartPs_SelfTest(descriptor->UartInstancePtr);
    if (Status != XST_SUCCESS) {
        return FAILURE;
    }

    /*
     * Connect the UART to the interrupt subsystem such that interrupts
     * can occur. This function is application specific.
     */
    Status = serial_setup_interrupt_system(descriptor);
    if (Status != XST_SUCCESS) {
        return FAILURE;
    }

    /*
     * Setup the handlers for the UART that will be called from the
     * interrupt context when data has been sent and received, specify
     * a pointer to the UART driver instance as the callback reference
     * so the handlers are able to access the instance data
     */
    XUartPs_SetHandler(descriptor->UartInstancePtr, (XUartPs_Handler)serial_handler, descriptor->UartInstancePtr);

    /*
     * Enable the interrupt of the UART so interrupts will occur, setup
     * a local loopback so data that is sent will be received.
     */
    IntrMask =
        XUARTPS_IXR_TOUT | XUARTPS_IXR_PARITY | XUARTPS_IXR_FRAMING |
        XUARTPS_IXR_OVER | XUARTPS_IXR_TXEMPTY | XUARTPS_IXR_RXFULL |
        XUARTPS_IXR_RXOVR;

    if (descriptor->UartInstancePtr->Platform == XPLAT_ZYNQ_ULTRA_MP) {
        IntrMask |= XUARTPS_IXR_RBRK;
    }

    XUartPs_SetInterruptMask(descriptor->UartInstancePtr, IntrMask);

    XUartPs_SetOperMode(descriptor->UartInstancePtr, XUARTPS_OPER_MODE_NORMAL);
    XUartPs_SetBaudRate(descriptor->UartInstancePtr, descriptor->baud_rate);
    /*
     * Set the receiver timeout. If it is not set, and the last few bytes
     * of data do not trigger the over-water or full interrupt, the bytes
     * will not be received. By default it is disabled.
     *
     * The setting of 8 will timeout after 8 x 4 = 32 character times.
     * Increase the time out value if baud rate is high, decrease it if
     * baud rate is low.
     */
    XUartPs_SetRecvTimeout(descriptor->UartInstancePtr, 8);

    return SUCCESS;
}

/**************************************************************************/
/**
*
* This function is the handler which performs processing to handle data events
* from the device.  It is called from an interrupt context. so the amount of
* processing should be minimal.
*
* This handler provides an example of how to handle data for the device and
* is application specific.
*
* @param	CallBackRef contains a callback reference from the driver,
*		in this case it is the instance pointer for the XUartPs driver.
* @param	Event contains the specific kind of event that has occurred.
* @param	EventData contains the number of bytes sent or received for sent
*		and receive events.
*
* @return	None.
*
* @note		None.
*
***************************************************************************/
static void serial_handler(void *CallBackRef, u32 Event, uint32_t EventData)
{
    /* All of the data has been received */
    if (Event == XUARTPS_EVENT_RECV_DATA) {
    	bytes_reveived = EventData;
    }

    /*
     * Data was received, but not the expected number of bytes, a
     * timeout just indicates the data stopped for 8 character times
     */
    if (Event == XUARTPS_EVENT_RECV_TOUT) {
    	bytes_reveived = EventData;
    }

    /*
     * Data was received with an error, keep the data but determine
     * what kind of errors occurred
     */
    if (Event == XUARTPS_EVENT_RECV_ERROR) {
        TotalErrorCount++;
    }

    /*
     * Data was received with an parity or frame or break error, keep the data
     * but determine what kind of errors occurred. Specific to Zynq Ultrascale+
     * MP.
     */
    if (Event == XUARTPS_EVENT_PARE_FRAME_BRKE) {
        TotalErrorCount++;
    }

    /*
     * Data was received with an overrun error, keep the data but determine
     * what kind of errors occurred. Specific to Zynq Ultrascale+ MP.
     */
    if (Event == XUARTPS_EVENT_RECV_ORERR) {
        TotalErrorCount++;
    }
}


/*****************************************************************************/
/**
*
* This function sets up the interrupt system so interrupts can occur for the
* Uart. This function is application-specific. The user should modify this
* function to fit the application.
*
* @param	IntcInstancePtr is a pointer to the instance of the INTC.
* @param	UartInstancePtr contains a pointer to the instance of the UART
*		driver which is going to be connected to the interrupt
*		controller.
* @param	UartIntrId is the interrupt Id and is typically
*		XPAR_<UARTPS_instance>_INTR value from xparameters.h.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None.
*
****************************************************************************/
static ssize_t serial_setup_interrupt_system(struct uart_desc *descriptor)
{
    int32_t Status;
    XScuGic_Config *IntcConfig; /* Config for interrupt controller */

    /* Initialize the interrupt controller driver */
    IntcConfig = XScuGic_LookupConfig(descriptor->ctrl_irq_id);
    if (NULL == IntcConfig) {
        return FAILURE;
    }

    Status = XScuGic_CfgInitialize(descriptor->IntcInstancePtr, IntcConfig,
                                   IntcConfig->CpuBaseAddress);
    if (Status != XST_SUCCESS) {
        return FAILURE;
    }

    /*
     * Connect the interrupt controller interrupt handler to the
     * hardware interrupt handling logic in the processor.
     */
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
                                 (Xil_ExceptionHandler) XScuGic_InterruptHandler,
								 descriptor->IntcInstancePtr);


    /*
     * Connect a device driver handler that will be called when an
     * interrupt for the device occurs, the device driver handler
     * performs the specific interrupt processing for the device
     */
    Status = XScuGic_Connect(descriptor->IntcInstancePtr, descriptor->uart_irq_id,
                             (Xil_ExceptionHandler) XUartPs_InterruptHandler,
                             (void *) descriptor->UartInstancePtr);
    if (Status != XST_SUCCESS) {
        return FAILURE;
    }

    /* Enable the interrupt for the device */
    XScuGic_Enable(descriptor->IntcInstancePtr, descriptor->uart_irq_id);



    /* Enable interrupts */
    Xil_ExceptionEnable();


    return SUCCESS;
}



