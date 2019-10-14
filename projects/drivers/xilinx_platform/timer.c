/***************************************************************************//**
 *   @file   timer.c
 *   @brief  Implementation of Xilinx Timer Generic Driver.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdlib.h>
#include <xscutimer.h>
#include "error.h"
#include "timer.h"
#include "irq.h"
#include "xilinx_platform_drivers.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

static void timer_interrupt_handler(void *CallBackRef)
{
	// The Xilinx drivers automatically pass an instance of
	// the peripheral which generated in the interrupt into this
	// function, using the special parameter called "CallBackRef".
	// We will locally declare an instance of the timer, and assign
	// it to CallBackRef.  You'll see why in a minute.
	XScuTimer *my_Timer_LOCAL = (XScuTimer *) CallBackRef;

	// Here we'll check to see if the timer counter has expired.
	// Technically speaking, this check is not necessary.
	// We know that the timer has expired because that's the
	// reason we're in this handler in the first place!
	// However, this is an example of how a callback reference
	// can be used as a pointer to the instance of the timer
	// that expired.  If we had two timers then we could use the same
	// handler for both, and the "CallBackRef" would always tell us
	// which timer had generated the interrupt.
	if (XScuTimer_IsExpired(my_Timer_LOCAL)) {
		// Clear the interrupt flag in the timer, so we don't service
		// the same interrupt twice.
		XScuTimer_ClearInterruptStatus(my_Timer_LOCAL);

//		// Increment a counter so that we know how many interrupts
//		// have been generated.  The counter is a global variable
//		InterruptCounter++;
//
//		// Print something to the UART to show that we're in the interrupt handler
//		printf("\n\r** This message comes from the interrupt handler! (%d) **\n\r\n\n\r", InterruptCounter);
//
//		// Check to see if we've had more than the defined number of interrupts
//		if (InterruptCounter >= INTERRUPT_COUNT_TIMEOUT_VALUE)
//		{
//			// Stop the timer from automatically re-loading, so
//			// that we don't get any more interrupts.
//			// We couldn't do this if we didn't have the CallBackRef
//			XScuTimer_DisableAutoReload(my_Timer_LOCAL);
//		}
	}
}


/* Initialize the Timer peripheral. */
int32_t timer_init(struct timer_desc **desc, struct timer_init_par *par)
{
	int32_t status;
	XScuTimer_Config *ConfigPtr;
	struct timer_desc *descriptor;
	struct xil_timer_init_param *xil_uart_init_param;
	struct xil_timer_desc *xil_timer_desc;

	descriptor = calloc(1, sizeof(struct timer_desc));
	xil_timer_desc = calloc(1, sizeof(struct xil_uart_desc));
	descriptor->extra = xil_timer_desc;
	descriptor->device_id = par->device_id;
	xil_uart_init_param = par->extra;
	xil_timer_desc = descriptor->extra;
	xil_timer_desc->irq_id = xil_uart_init_param->irq_id;
	xil_timer_desc->irq_desc = xil_uart_init_param->irq_desc;
	xil_timer_desc->instance = calloc(1, sizeof(XScuTimer));

	ConfigPtr = XScuTimer_LookupConfig(par->device_id);
	/*
	 * This is where the virtual address would be used, this example
	 * uses physical address.
	 */
	status = XScuTimer_CfgInitialize(xil_timer_desc->instance, ConfigPtr,
					 ConfigPtr->BaseAddr);
	if (status != XST_SUCCESS) {
		return FAILURE;
	}

	status = irq_register(xil_timer_desc->irq_desc, xil_timer_desc->irq_id,
			      (Xil_ExceptionHandler) timer_interrupt_handler, xil_timer_desc->instance);
	status = irq_source_enable(xil_timer_desc->irq_desc, xil_timer_desc->irq_id);
	if (status < 0)
		return status;

	timer_write(descriptor, XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2);

	XScuTimer_EnableAutoReload(xil_timer_desc->instance);

	timer_start(descriptor);

	return SUCCESS;
}

/* Free the resources allocated by timer_init(). */
int32_t timer_remove(struct timer_desc *desc)
{
	struct xil_timer_desc *xil_timer_desc = desc->extra;
	free(xil_timer_desc->instance);
	free(xil_timer_desc);
	free(desc);

	return SUCCESS;
}

/* READ timer value. */
int32_t timer_read(struct timer_desc *desc, uint32_t data)
{
	return SUCCESS;
}

/* Write timer value. */
int32_t timer_write(struct timer_desc *desc, const uint32_t data)
{
	struct xil_timer_desc *xil_timer_desc = desc->extra;
	XScuTimer_LoadTimer(xil_timer_desc->instance, data);

	return SUCCESS;
}

/* Start timer. */
int32_t timer_start(struct timer_desc *desc)
{
	struct xil_timer_desc *xil_timer_desc = desc->extra;
	XScuTimer_Start(xil_timer_desc->instance);

	return SUCCESS;
}

/* Stop timer. */
int32_t timer_stop(struct timer_desc *desc)
{
	struct xil_timer_desc *xil_timer_desc = desc->extra;
	XScuTimer_Stop(xil_timer_desc->instance);

	return SUCCESS;
}

