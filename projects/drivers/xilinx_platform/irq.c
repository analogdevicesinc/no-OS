/***************************************************************************//**
 *   @file   irq.c
 *   @brief  Implementation of Xilinx IRQ Generic Driver.
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
#include <xscugic.h>
#include "error.h"
#include "irq.h"
#include "xilinx_platform_drivers.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the IRQ interrupts.
 * @param desc - The IRQ descriptor.
 * @param init_param - The structure that contains the IRQ parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t irq_ctrl_init(struct irq_desc **desc,
		 const struct irq_init_param *param)
{
    int32_t status;
    XScuGic_Config *IntcConfig; /* Config for interrupt controller */
    XScuGic *gic;
    struct xil_irq_desc *xil_dev;

    struct irq_desc *descriptor = calloc(1, sizeof(struct irq_desc));
    if (!descriptor)
    	return FAILURE;

    descriptor->extra = calloc(1, sizeof(struct xil_irq_desc));
	if(!(descriptor->extra)) {
		free(descriptor);
		return FAILURE;
	}

    gic = calloc(1, sizeof(XScuGic));
	if (!gic) {
		free(descriptor->extra);
		free(descriptor);
		return FAILURE;
	}

	xil_dev = descriptor->extra;
	xil_dev->gic = gic;
    /* Initialize the interrupt controller driver */
    IntcConfig = XScuGic_LookupConfig(param->irq_id);
    if (NULL == IntcConfig) {
        return FAILURE;
    }

    status = XScuGic_CfgInitialize(gic, IntcConfig,
                                   IntcConfig->CpuBaseAddress);
    if (status != XST_SUCCESS) {
        return FAILURE;
    }

    /*
     * Connect the interrupt controller interrupt handler to the
     * hardware interrupt handling logic in the processor.
     */
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
                                 (Xil_ExceptionHandler) XScuGic_InterruptHandler,
								 gic);
    *desc = descriptor;

    return SUCCESS;
}

/**
 * @brief Enable global interrupts.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t irq_enable(void) {
	/* Enable interrupts */
	Xil_ExceptionEnable();

	return SUCCESS;
}

/**
 * @brief Disable global interrupts.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t irq_disable(void) {
	/* Disable interrupts */
	Xil_ExceptionDisable();

	return SUCCESS;
}

/**
 * @brief Enable specific interrupt.
 * @param desc - The IRQ descriptor.
 * @param irq_id - Interrupt identifier.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t irq_source_enable(struct irq_desc *desc, uint32_t irq_id) {
	struct xil_irq_desc *xil_dev = desc->extra;
    XScuGic_Enable(xil_dev->gic, irq_id);

	return SUCCESS;
}

/**
 * @brief Disable specific interrupt.
 * @param desc - The IRQ descriptor.
 * @param irq_id - Interrupt identifier.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t irq_source_disable(struct irq_desc *desc, uint32_t irq_id) {
	struct xil_irq_desc *xil_dev = desc->extra;
    XScuGic_Disable(xil_dev->gic, irq_id);

	return SUCCESS;
}

/**
 * @brief Registers a generic IRQ handling function.
 * @param desc - The IRQ descriptor.
 * @param irq_id - Interrupt identifier.
 * @param irq_handler - The IRQ handler.
 * @param dev_instance - device instance.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t irq_register(struct irq_desc *desc, uint32_t irq_id, void (*irq_handler)(void *data), void *dev_instance) {

	int32_t status;
	struct xil_irq_desc *xil_dev = desc->extra;
	status = XScuGic_Connect(xil_dev->gic, irq_id,
                             irq_handler,
							 dev_instance);
    if (status != XST_SUCCESS) {
        return FAILURE;
    }

	return SUCCESS;
}

/**
 * @brief Unregisters a generic IRQ handling function.
 * @param desc - The IRQ descriptor.
 * @param irq_id - Interrupt identifier.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t irq_unregister(struct irq_desc *desc, uint32_t irq_id) {
	struct xil_irq_desc *xil_dev = desc->extra;
	XScuGic_Disconnect(xil_dev->gic, irq_id);

	return SUCCESS;
}

/**
 * @brief Free the resources allocated by irq_ctrl_init().
 * @param desc - The IRQ descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t irq_ctrl_remove(struct irq_desc *desc) {
	struct xil_irq_desc *xil_dev = desc->extra;
	free(xil_dev->gic);
	free(desc);

	return SUCCESS;
}
