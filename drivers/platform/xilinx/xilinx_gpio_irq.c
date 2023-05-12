/***************************************************************************//**
 *   @file   xilinx/xilinx_gpio_irq.c
 *   @brief  Implementation of Xilinx GPIO IRQ Generic Driver.
 *   @author Porumb Andrei (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
#include "xparameters.h"
#include "no_os_error.h"
#include "xilinx_gpio_irq.h"
#include "no_os_util.h"
#include "no_os_list.h"
#include "no_os_irq.h"
#include "no_os_alloc.h"

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Comparing function used by list.
 * @param desc_a - Compared item
 * @param desc_b - Reference item
 * @return - 0 if items match
 */
static int32_t call_cmp(struct xil_callback_desc *desc_a,
			struct xil_callback_desc *desc_b)
{
	return desc_a->pin_nb - desc_b->pin_nb;
}

/**
 * @brief Function called when GPIO IRQ occurs.
 * @param param - GPIO IRQ desc's extra field
 * @return - None.
 */
static void xil_gpio_irq_handler(struct xil_gpio_irq_desc *param)
{
	int32_t status = 0;
	int32_t pin_nb;
	struct xil_callback_desc *callback_desc;

	status = no_os_iterator_move_to_idx(param->it, 0);
	while(!status) {
		no_os_iterator_read(param->it, &callback_desc);
		status = no_os_iterator_move(param->it, 1);
		if(XGpioPs_IntrGetStatusPin(&param->my_Gpio, callback_desc->pin_nb))
			callback_desc->triggered = true;
	}

	status = no_os_iterator_move_to_idx(param->it, 0);
	while(!status) {
		no_os_iterator_read(param->it, &callback_desc);
		status = no_os_iterator_move(param->it, 1);
		if(callback_desc->triggered == true) {
			callback_desc->triggered = false;
			XGpioPs_IntrDisablePin(&param->my_Gpio, callback_desc->pin_nb);
			XGpioPs_IntrClearPin(&param->my_Gpio, callback_desc->pin_nb);
			callback_desc->callback.callback(callback_desc->callback.ctx);
			if(callback_desc->enabled)
				XGpioPs_IntrEnablePin(&param->my_Gpio, callback_desc->pin_nb);
		}
	}
}

/**
 * @brief Disable specific GPIO IRQ pin.
 * @param desc - The GPIO IRQ controller descriptor.
 * @param irq_id - Pin number.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_gpio_irq_disable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id)
{
	int32_t status;
	struct xil_gpio_irq_desc *extra;
	struct xil_callback_desc *callback_desc;

	extra = desc->extra;

	status = no_os_iterator_move_to_idx(extra->it, 0);
	while(!status) {
		no_os_iterator_read(extra->it, &callback_desc);
		status = no_os_iterator_move(extra->it, 1);
		if(callback_desc->pin_nb == irq_id) {
			callback_desc->enabled = false;
			break;
		}
	}

	XGpioPs_IntrDisablePin(&extra->my_Gpio, irq_id);

	return 0;
}

/**
 * @brief Initialize the GPIO IRQ controller.
 * @param desc - The GPIO IRQ controller descriptor.
 * @param param - The GPIO IRQ controller initial parameters.
 * irq_ctrl_id from irq_init_param is the interrupt id for the parent controller
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_gpio_irq_ctrl_init(struct no_os_irq_ctrl_desc **desc,
			       const struct no_os_irq_init_param *param)
{
	int32_t i;
	int32_t status;
	struct no_os_irq_ctrl_desc *ldesc;
	static XGpioPs_Config *GPIO_Config;
	struct xil_gpio_irq_desc *xil_desc;
	struct xil_gpio_irq_init_param *xil_ip;
	struct no_os_callback_desc callback;

	xil_desc = (struct xil_gpio_irq_desc *)no_os_calloc(1, sizeof(*xil_desc));
	if(!xil_desc)
		return -ENOMEM;

	ldesc = (struct no_os_irq_ctrl_desc *)no_os_calloc(1, sizeof(*ldesc));
	if(!ldesc) {
		no_os_free(xil_desc);
		return -ENOMEM;
	}

	xil_ip = param->extra;
	GPIO_Config = XGpioPs_LookupConfig(xil_ip->gpio_device_id);
	status = XGpioPs_CfgInitialize(&xil_desc->my_Gpio, GPIO_Config,
				       GPIO_Config->BaseAddr);
	if(status)
		goto error_desc;

	ldesc->extra = xil_desc;
	xil_desc->parent_desc = xil_ip->parent_desc;

	status = no_os_list_init(&xil_desc->callback_list, NO_OS_LIST_DEFAULT,
				 call_cmp);
	if(status)
		goto error_list;

	status = no_os_iterator_init(&xil_desc->it, xil_desc->callback_list, 0);
	if(status) {
		no_os_iterator_remove(xil_desc->it);
		goto error_list;
	}

	ldesc->irq_ctrl_id = param->irq_ctrl_id;
	status = no_os_irq_trigger_level_set(xil_desc->parent_desc, ldesc->irq_ctrl_id,
					     NO_OS_IRQ_EDGE_RISING);
	if(status)
		goto error_op;

	status = no_os_irq_enable(xil_desc->parent_desc, ldesc->irq_ctrl_id);
	if(status)
		goto error_op;

	callback.callback = &xil_gpio_irq_handler;
	callback.ctx = ldesc->extra;
	status = no_os_irq_register_callback(xil_desc->parent_desc, ldesc->irq_ctrl_id,
					     &callback);
	if(status)
		goto error_op;

	*desc = ldesc;

	return 0;

error_op:
	no_os_iterator_remove(xil_desc->it);
error_list:
	no_os_list_remove(xil_desc->callback_list);
error_desc:
	no_os_free(xil_desc);
	no_os_free(ldesc);
	return status;
}

/**
 * @brief Set GPIO interrupt trigger level.
 * @param desc - The GPIO IRQ controller descriptor.
 * @param irq_id - Pin number.
 * @param trig - New trigger level for the GPIO interrupt.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_gpio_irq_trigger_level_set(struct no_os_irq_ctrl_desc *desc,
				       uint32_t irq_id,
				       enum no_os_irq_trig_level trig)
{
	static const int32_t trig_level[5] = {XGPIOPS_IRQ_TYPE_LEVEL_LOW,
					      XGPIOPS_IRQ_TYPE_LEVEL_HIGH,
					      XGPIOPS_IRQ_TYPE_EDGE_FALLING,
					      XGPIOPS_IRQ_TYPE_EDGE_RISING,
					      XGPIOPS_IRQ_TYPE_EDGE_BOTH
					     };
	struct xil_gpio_irq_desc *extra;

	extra = desc->extra;
	XGpioPs_SetIntrTypePin(&extra->my_Gpio, irq_id, trig_level[trig]);

	return 0;
}

/**
 * @brief Register pin specific GPIO interrupt.
 * @param desc - The GPIO IRQ controller descriptor.
 * @param irq_id - Pin number.
 * @param callback_desc - Callback.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_gpio_irq_register_callback(struct no_os_irq_ctrl_desc *desc,
				       uint32_t irq_id,
				       struct no_os_callback_desc *callback_desc)
{
	struct xil_callback_desc *dev_callback;
	struct xil_gpio_irq_desc *extra;

	dev_callback = (struct xil_callback_desc *)no_os_calloc(1,
			sizeof(*dev_callback));
	if(!dev_callback)
		return -1;

	dev_callback->pin_nb = irq_id;
	dev_callback->callback.callback = callback_desc->callback;
	dev_callback->callback.ctx = callback_desc->ctx;
	dev_callback->triggered = false;

	extra = desc->extra;
	XGpioPs_SetDirectionPin(&extra->my_Gpio, irq_id, 0);
	no_os_list_add_last(extra->callback_list, dev_callback);

	return 0;
}

/**
 * @brief Unregister pin specific GPIO interrupt.
 * @param desc - The GPIO IRQ controller descriptor.
 * @param irq_id - Pin number.
 * @param cb - Callback descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_gpio_irq_unregister_callback(struct no_os_irq_ctrl_desc *desc,
		uint32_t irq_id, struct no_os_callback_desc *cb)
{
	int32_t status;
	struct xil_gpio_irq_desc *extra;
	struct xil_callback_desc *dev_callback;
	struct xil_callback_desc search_callback;

	extra = desc->extra;
	search_callback.pin_nb = irq_id;
	status = no_os_list_get_find(extra->callback_list, &dev_callback,
				     &search_callback);
	if(status)
		return -ENXIO;

	no_os_free(dev_callback);

	return 0;
}

/**
 * @brief Enable specific GPIO IRQ pin.
 * @param desc - The GPIO IRQ controller descriptor.
 * @param irq_id - Pin number.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_gpio_irq_enable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id)
{
	int32_t status;
	struct xil_gpio_irq_desc *extra;
	struct xil_callback_desc *callback_desc;

	extra = desc->extra;

	status = no_os_iterator_move_to_idx(extra->it, 0);
	while(!status) {
		no_os_iterator_read(extra->it, &callback_desc);
		status = no_os_iterator_move(extra->it, 1);
		if(callback_desc->pin_nb == irq_id) {
			callback_desc->enabled = true;
			XGpioPs_IntrClearPin(&extra->my_Gpio, irq_id);
			break;
		}
	}

	XGpioPs_IntrEnablePin(&extra->my_Gpio, irq_id);

	return 0;
}

/**
 * @brief Remove the GPIO IRQ controller.
 * @param desc - The GPIO IRQ controller descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_gpio_irq_ctrl_remove(struct no_os_irq_ctrl_desc *desc)
{
	struct xil_gpio_irq_desc *extra;
	struct xil_callback_desc *callback_desc;

	if (!desc)
		return -EINVAL;

	extra = desc->extra;
	while (0 == no_os_list_get_first(extra->callback_list, &callback_desc))
		no_os_free(callback_desc);

	no_os_iterator_remove(extra->it);
	no_os_list_remove(extra->callback_list);
	no_os_free(extra);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Xilinx platform specific GPIO IRQ platform ops structure
 */
const struct no_os_irq_platform_ops xil_gpio_irq_ops = {
	.init = &xil_gpio_irq_ctrl_init,
	.enable = &xil_gpio_irq_enable,
	.disable = &xil_gpio_irq_disable,
	.trigger_level_set = &xil_gpio_irq_trigger_level_set,
	.register_callback = &xil_gpio_irq_register_callback,
	.unregister_callback = xil_gpio_irq_unregister_callback,
	.remove = &xil_gpio_irq_ctrl_remove
};
