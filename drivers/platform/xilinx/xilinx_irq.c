/***************************************************************************//**
 *   @file   xilinx/xilinx_irq.c
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

#include <xparameters.h>
#include <stdlib.h>
#include "no_os_error.h"
#include "no_os_irq.h"
#include "no_os_alloc.h"
#include "xilinx_irq.h"
#ifdef XPAR_XSCUGIC_NUM_INSTANCES
#include <xscugic.h>
#endif
#ifdef XPAR_XINTC_NUM_INSTANCES
#include <xintc.h>
#endif
#ifdef XPAR_XTMRCTR_NUM_INSTANCES
#include <xtmrctr.h>
#endif

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Initialize the IRQ interrupts.
 * @param desc - The IRQ controller descriptor.
 * @param param - The structure that contains the IRQ parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_irq_ctrl_init(struct no_os_irq_ctrl_desc **desc,
			  const struct no_os_irq_init_param *param)
{
	int32_t status;
	struct no_os_irq_ctrl_desc *descriptor;
	struct xil_irq_desc *xil_dev;
#ifdef XSCUGIC_H
	void *config;
#endif

	descriptor = (struct no_os_irq_ctrl_desc *)no_os_calloc(1, sizeof *descriptor);
	if(!descriptor)
		return -1;
	xil_dev = (struct xil_irq_desc *)no_os_calloc(1, sizeof *xil_dev);
	if(!xil_dev) {
		no_os_free(descriptor);
		return -1;
	}

	Xil_ExceptionInit();

	descriptor->irq_ctrl_id = param->irq_ctrl_id;
	descriptor->extra = xil_dev;
	xil_dev->type = ((struct xil_irq_init_param *)param->extra)->type;

	switch(xil_dev->type) {
	case IRQ_PS:
#ifdef XSCUGIC_H
		xil_dev->instance = no_os_calloc(1, sizeof(XScuGic));
		if(!xil_dev->instance)
			goto error;

		config = XScuGic_LookupConfig(descriptor->irq_ctrl_id);
		if(!config)
			goto ps_error;

		status = XScuGic_CfgInitialize(xil_dev->instance, config,
					       ((XScuGic_Config *)config)->CpuBaseAddress);
		if (status != 0)
			goto ps_error;

		Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
					     (Xil_ExceptionHandler)XScuGic_InterruptHandler,
					     xil_dev->instance);

		break;
ps_error:
		no_os_free(xil_dev->instance);
#endif
		goto error;
	case IRQ_PL:
#ifdef XINTC_H
		xil_dev->instance = no_os_calloc(1, sizeof(XIntc));
		if(!xil_dev->instance)
			goto error;

		status = XIntc_Initialize(xil_dev->instance, descriptor->irq_ctrl_id);
		if(status != 0)
			goto pl_error;

		status = XIntc_Start(xil_dev->instance, XIN_REAL_MODE);
		if(status != 0)
			goto pl_error;

		break;
pl_error:
		no_os_free(xil_dev->instance);
#endif
		goto error;
	default:
		goto error;

		break;
	}

	*desc = descriptor;

	return 0;

error:
	no_os_free(xil_dev);
	no_os_free(descriptor);

	return -1;
}

/**
 * @brief Enable global interrupts.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_irq_global_enable(struct no_os_irq_ctrl_desc *desc)
{
	/* Enable interrupts */
	Xil_ExceptionEnable();

	return 0;
}

/**
 * @brief Disable global interrupts.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_irq_global_disable(struct no_os_irq_ctrl_desc *desc)
{
	/* Disable interrupts */
	Xil_ExceptionDisable();

	return 0;
}

/**
 * @brief Enable specific interrupt.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_irq_enable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id)
{
	struct xil_irq_desc *xil_dev = desc->extra;

	switch(xil_dev->type) {
	case IRQ_PS:
#ifdef XSCUGIC_H
		XScuGic_Enable(xil_dev->instance, irq_id);
#endif
		break;
	case IRQ_PL:
#ifdef XINTC_H
		XIntc_Enable(xil_dev->instance, irq_id);
#endif
		break;
	default:

		return -1;
	}

	return 0;
}

/**
 * @brief Disable specific interrupt.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_irq_disable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id)
{
	struct xil_irq_desc *xil_dev = desc->extra;

	switch(xil_dev->type) {
	case IRQ_PS:
#ifdef XSCUGIC_H
		XScuGic_Disable(xil_dev->instance, irq_id);
#endif
		break;
	case IRQ_PL:
#ifdef XINTC_H
		XIntc_Disable(xil_dev->instance, irq_id);
#endif
		break;
	default:

		return -1;
	}

	return 0;
}

/**
 * @brief Register a callback to handle the irq events.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @param callback_desc - Callback descriptor
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_irq_register_callback(struct no_os_irq_ctrl_desc *desc,
				  uint32_t irq_id,
				  struct no_os_callback_desc *callback_desc)
{
	struct xil_irq_desc *xil_dev = desc->extra;

	switch(xil_dev->type) {
	case IRQ_PS:
#ifdef XSCUGIC_H
		return XScuGic_Connect(xil_dev->instance, irq_id,
				       (Xil_InterruptHandler) callback_desc->callback,
				       callback_desc->ctx);
#endif
		break;
	case IRQ_PL:
#ifdef XINTC_H
		if (irq_id == XPAR_AXI_INTC_AXI_TIMER_INTERRUPT_INTR) {
			int32_t ret;

			ret = XIntc_Connect(xil_dev->instance, irq_id,
					    XTmrCtr_InterruptHandler, NULL);
			if (NO_OS_IS_ERR_VALUE(ret))
				return -1;
			XTmrCtr_SetHandler(xil_dev->instance,
					   (XTmrCtr_Handler)callback_desc->callback,
					   callback_desc->ctx);

			return 0;
		}
		return XIntc_Connect(xil_dev->instance, irq_id,
				     (XInterruptHandler) callback_desc->callback,
				     callback_desc->ctx);
#endif
		break;
	default:
		break;
	}

	return -1;
}

/**
 * @brief Set interrupt trigger level.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @param trig - New trigger level for the interrupt.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_irq_trigger_level_set(struct no_os_irq_ctrl_desc *desc,
				  uint32_t irq_id,
				  enum no_os_irq_trig_level trig)
{
	struct xil_irq_desc *xil_dev = desc->extra;

	if(trig == NO_OS_IRQ_EDGE_BOTH)
		return -EINVAL;

	switch(xil_dev->type) {
	case IRQ_PS:
#ifdef XSCUGIC_H
		;
		uint8_t priority, trigger;

		XScuGic_GetPriorityTriggerType(xil_dev->instance, irq_id, &priority,
					       &trigger);
		trigger = trig;
		XScuGic_SetPriorityTriggerType(xil_dev->instance, irq_id, priority,
					       trigger);

		return 0;
#endif
		break;
	case IRQ_PL:
#ifdef XINTC_H
		/* Interrupt trigger determined in hardware. TODO: verify */
		return 0;
#endif
		break;
	default:
		break;
	}

	return -1;
}

/**
 * @brief Unregisters a generic IRQ handling function.
 * @param desc - The IRQ controller descriptor.
 * @param irq_id - Interrupt identifier.
 * @param cb - Callback descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_irq_unregister_callback(struct no_os_irq_ctrl_desc *desc,
				    uint32_t irq_id, struct no_os_callback_desc *cb)
{
	struct xil_irq_desc *xil_dev = desc->extra;

	switch(xil_dev->type) {
	case IRQ_PS:
#ifdef XSCUGIC_H
		XScuGic_Disconnect(xil_dev->instance, irq_id);
#endif
		break;
	case IRQ_PL:
#ifdef XINTC_H
		XIntc_Disconnect(xil_dev->instance, irq_id);
#endif
		break;
	default:

		return -1;
	}

	return 0;
}

/**
 * @brief Free the resources allocated by no_os_irq_ctrl_init().
 * @param desc - The IRQ control descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_irq_ctrl_remove(struct no_os_irq_ctrl_desc *desc)
{
	struct xil_irq_desc *xil_dev = desc->extra;
	no_os_free(xil_dev->instance);
	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Xilinx platform specific IRQ platform ops structure
 */
const struct no_os_irq_platform_ops xil_irq_ops = {
	.init = &xil_irq_ctrl_init,
	.register_callback = &xil_irq_register_callback,
	.unregister_callback = &xil_irq_unregister_callback,
	.global_enable = &xil_irq_global_enable,
	.global_disable = &xil_irq_global_disable,
	.trigger_level_set = &xil_irq_trigger_level_set,
	.enable = &xil_irq_enable,
	.disable = &xil_irq_disable,
	.remove = &xil_irq_ctrl_remove
};
