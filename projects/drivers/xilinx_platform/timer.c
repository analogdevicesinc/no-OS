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
#include "xilinx_platform_drivers.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/* Initialize the Timer peripheral. */
int32_t timer_init(struct timer_desc **desc, struct timer_init_par *par) {
	int32_t Status;
	XScuTimer_Config *ConfigPtr;
    struct timer_desc *descriptor;
    xil_timer_init_param *xil_uart_init_param;
    xil_timer_desc *xil_uart_desc;

    descriptor = calloc(1, sizeof(struct timer_desc));
    xil_uart_desc = calloc(1, sizeof(struct xil_uart_desc));
    descriptor->extra = xil_uart_desc;
    descriptor->device_id = par->device_id;
    xil_uart_init_param = par->extra;
    xil_uart_desc = descriptor->extra;
    xil_uart_desc->irq_id = xil_uart_init_param->irq_id;
    xil_uart_desc->irq_desc = xil_uart_init_param->irq_desc;
    xil_uart_desc->instance = calloc(1, sizeof(XUartPs));

	ConfigPtr = XScuTimer_LookupConfig(par->device_id);
	/*
	 * This is where the virtual address would be used, this example
	 * uses physical address.
	 */
	Status = XScuTimer_CfgInitialize(xil_uart_desc->instance, ConfigPtr,
					ConfigPtr->BaseAddr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}


	return SUCCESS;
}

/* Free the resources allocated by timer_init(). */
int32_t timer_remove(struct timer_desc *desc) {
	return SUCCESS;
}

/* READ timer value. */
int32_t timer_read(struct timer_desc *desc, uint32_t *data) {
	return SUCCESS;
}

/* Write timer value. */
int32_t timer_write(struct timer_desc *desc, const uint32_t *data) {
	return SUCCESS;
}
