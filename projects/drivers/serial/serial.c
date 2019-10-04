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
#include "xuartps.h"
#include "comm_util.h"

#ifdef XPAR_INTC_0_DEVICE_ID
#include "xintc.h"
#else
#include "xscugic.h"
#endif

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#ifdef XPAR_INTC_0_DEVICE_ID
#define INTC		XIntc
#define UART_DEVICE_ID		XPAR_XUARTPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_INTC_0_DEVICE_ID
#define UART_INT_IRQ_ID		XPAR_INTC_0_UARTPS_0_VEC_ID
#else
#define INTC		XScuGic
#define UART_DEVICE_ID		XPAR_XUARTPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define UART_INT_IRQ_ID		XPAR_XUARTPS_1_INTR
#endif

#define BUFF_LENGTH 256
static char buff[BUFF_LENGTH];
static uint32_t bytes_reveived = 0;

static struct fifo *serial_fifo = NULL;
int32_t TotalErrorCount;

XUartPs UartPs;		/* Instance of the UART Device */
INTC InterruptController;	/* Instance of the Interrupt Controller */

static ssize_t serial_ps_intr(INTC *IntcInstPtr, XUartPs *UartInstPtr,
                              u16 DeviceId, u16 UartIntrId);

static ssize_t serial_setup_interrupt_system(INTC *IntcInstancePtr,
        XUartPs *UartInstancePtr,
        u16 UartIntrId);

static void serial_handler(void *CallBackRef, u32 Event, uint32_t EventData);

void uart_keep_alive (void) {
	if (bytes_reveived > 0) {
		fifo_insert_tail(&serial_fifo, buff, bytes_reveived);
		bytes_reveived = 0;
		XUartPs_Recv(&UartPs, (u8*)buff, BUFF_LENGTH);
	}
}

int32_t comm_read_byte(struct fifo **fifo, char *buf)
{
	while (*fifo == NULL) { /* nothing in fifo */
		uart_keep_alive();
	}

	*buf = (*fifo)->data[(*fifo)->index];
	(*fifo)->index++;

	if ((*fifo)->len - (*fifo)->index <= 0) {
		(*fifo) = fifo_remove_head(*fifo);
	}
	return 1;
}

/***************************************************************************//**
 * @brief network_read
*******************************************************************************/
ssize_t serial_read(char *buf, size_t len)
{
    for (uint32_t i = 0; i < len; i++) {
		comm_read_byte(&serial_fifo, &buf[i]);
	}
	return len;
}

/***************************************************************************//**
 * @brief serial_write_data
*******************************************************************************/
ssize_t serial_write_data(const char *buf, size_t len)
{
    for ( int32_t i = 0; i < len; i++)
        outbyte(buf[i]);
    return 0;
}

/***************************************************************************//**
 * @brief serial_init
*******************************************************************************/
ssize_t serial_init(void)
{
    int32_t Status;

    /* Run the UartPs Interrupt example, specify the the Device ID */
    Status = serial_ps_intr(&InterruptController, &UartPs,
                            UART_DEVICE_ID, UART_INT_IRQ_ID);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    XUartPs_Recv(&UartPs, (u8*)buff, BUFF_LENGTH);

    return XST_SUCCESS;
}

/***************************************************************************//**
 * @brief serial_ps_intr
*******************************************************************************/
static ssize_t serial_ps_intr(INTC *IntcInstPtr, XUartPs *UartInstPtr,
                              u16 DeviceId, u16 UartIntrId)
{
    XUartPs_Config *Config;
    int32_t Status;
    u32 IntrMask;

    if (XGetPlatform_Info() == XPLAT_ZYNQ_ULTRA_MP) {
#ifdef XPAR_XUARTPS_1_DEVICE_ID
        DeviceId = XPAR_XUARTPS_1_DEVICE_ID;
#endif
    }

    /*
     * Initialize the UART driver so that it's ready to use
     * Look up the configuration in the config table, then initialize it.
     */
    Config = XUartPs_LookupConfig(DeviceId);
    if (NULL == Config) {
        return XST_FAILURE;
    }

    Status = XUartPs_CfgInitialize(UartInstPtr, Config, Config->BaseAddress);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    /* Check hardware build */
    Status = XUartPs_SelfTest(UartInstPtr);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    /*
     * Connect the UART to the interrupt subsystem such that interrupts
     * can occur. This function is application specific.
     */
    Status = serial_setup_interrupt_system(IntcInstPtr, UartInstPtr, UartIntrId);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    /*
     * Setup the handlers for the UART that will be called from the
     * interrupt context when data has been sent and received, specify
     * a pointer to the UART driver instance as the callback reference
     * so the handlers are able to access the instance data
     */
    XUartPs_SetHandler(UartInstPtr, (XUartPs_Handler)serial_handler, UartInstPtr);

    /*
     * Enable the interrupt of the UART so interrupts will occur, setup
     * a local loopback so data that is sent will be received.
     */
    IntrMask =
        XUARTPS_IXR_TOUT | XUARTPS_IXR_PARITY | XUARTPS_IXR_FRAMING |
        XUARTPS_IXR_OVER | XUARTPS_IXR_TXEMPTY | XUARTPS_IXR_RXFULL |
        XUARTPS_IXR_RXOVR;

    if (UartInstPtr->Platform == XPLAT_ZYNQ_ULTRA_MP) {
        IntrMask |= XUARTPS_IXR_RBRK;
    }

    XUartPs_SetInterruptMask(UartInstPtr, IntrMask);

    XUartPs_SetOperMode(UartInstPtr, XUARTPS_OPER_MODE_NORMAL);
    XUartPs_SetBaudRate(UartInstPtr, 921600);
    /*
     * Set the receiver timeout. If it is not set, and the last few bytes
     * of data do not trigger the over-water or full interrupt, the bytes
     * will not be received. By default it is disabled.
     *
     * The setting of 8 will timeout after 8 x 4 = 32 character times.
     * Increase the time out value if baud rate is high, decrease it if
     * baud rate is low.
     */
    XUartPs_SetRecvTimeout(UartInstPtr, 8);

    return XST_SUCCESS;
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
static ssize_t serial_setup_interrupt_system(INTC *IntcInstancePtr,
        XUartPs *UartInstancePtr,
        u16 UartIntrId)
{
    int32_t Status;

#ifdef XPAR_INTC_0_DEVICE_ID
#ifndef TESTAPP_GEN
    /*
     * Initialize the interrupt controller driver so that it's ready to
     * use.
     */
    Status = XIntc_Initialize(IntcInstancePtr, INTC_DEVICE_ID);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }
#endif
    /*
     * Connect the handler that will be called when an interrupt
     * for the device occurs, the handler defined above performs the
     * specific interrupt processing for the device.
     */
    Status = XIntc_Connect(IntcInstancePtr, UartIntrId,
                           (XInterruptHandler) XUartPs_InterruptHandler, UartInstancePtr);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

#ifndef TESTAPP_GEN
    /*
     * Start the interrupt controller so interrupts are enabled for all
     * devices that cause interrupts.
     */
    Status = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }
#endif
    /*
     * Enable the interrupt for uart
     */
    XIntc_Enable(IntcInstancePtr, UartIntrId);

#ifndef TESTAPP_GEN
    /*
     * Initialize the exception table.
     */
    Xil_ExceptionInit();

    /*
     * Register the interrupt controller handler with the exception table.
     */
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
                                 (Xil_ExceptionHandler) XIntc_InterruptHandler,
                                 IntcInstancePtr);
#endif
#else
#ifndef TESTAPP_GEN
    XScuGic_Config *IntcConfig; /* Config for interrupt controller */

    /* Initialize the interrupt controller driver */
    IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
    if (NULL == IntcConfig) {
        return XST_FAILURE;
    }

    Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
                                   IntcConfig->CpuBaseAddress);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    /*
     * Connect the interrupt controller interrupt handler to the
     * hardware interrupt handling logic in the processor.
     */
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
                                 (Xil_ExceptionHandler) XScuGic_InterruptHandler,
                                 IntcInstancePtr);
#endif

    /*
     * Connect a device driver handler that will be called when an
     * interrupt for the device occurs, the device driver handler
     * performs the specific interrupt processing for the device
     */
    Status = XScuGic_Connect(IntcInstancePtr, UartIntrId,
                             (Xil_ExceptionHandler) XUartPs_InterruptHandler,
                             (void *) UartInstancePtr);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    /* Enable the interrupt for the device */
    XScuGic_Enable(IntcInstancePtr, UartIntrId);

#endif
#ifndef TESTAPP_GEN
    /* Enable interrupts */
    Xil_ExceptionEnable();
#endif

    return XST_SUCCESS;
}
