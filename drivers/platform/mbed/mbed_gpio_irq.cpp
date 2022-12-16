/***************************************************************************//**
 * @file   mbed_gpio_irq.cpp
 * @brief  Implementation of GPIO IRQ Mbed platform driver interfaces
********************************************************************************
 * Copyright (c) 2022 Analog Devices, Inc.
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

#include <stdio.h>
#include <mbed.h>

// Platform support needs to be C-compatible to work with other drivers
#ifdef __cplusplus
extern "C"
{
#endif

#include "no_os_error.h"
#include "no_os_irq.h"
#include "mbed_gpio_irq.h"
#include "mbed_irq.h"

using namespace std::chrono;

/**
* @struct mbed_gpio_irq_callback_desc.
* @brief Structure holding the callback functions for mbed gpio irqs.
* @note  The callback functions are directly mapped with IRQ IDs enum.
* Mbed doesn't provide a single callback function for all IRQ events.
* A seperate callback function is required to be registered
* for every IRQ instance. Also, the 'callback_function' required for
* mbed layer expects void argument and void return type.
*/
struct mbed_irq_callback_desc {
	struct no_os_callback_desc desc;
	void (*mbed_callback)(void);
};

/* Mbed gpio irq callback structure variable */
static struct mbed_irq_callback_desc mbed_gpio_irq_callback[NB_GPIO_IRQS];

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief	Mbed callback function for gpio/external interrupt ID1 event.
 * @return	none.
 * @note	This function is called when gpio/external IRQ event mapped to
 *			GPIO_IRQ_ID1 is triggered.
 */
static void mbed_gpio_irq_id1_callback(void)
{
	if (mbed_gpio_irq_callback[GPIO_IRQ_ID1].desc.callback)
		/* Invoke the application registered callback function */
		mbed_gpio_irq_callback[GPIO_IRQ_ID1].desc.callback(
			mbed_gpio_irq_callback[GPIO_IRQ_ID1].desc.ctx);
}

/**
 * @brief	Mbed callback function for gpio/external interrupt ID2 event.
 * @return	none.
 * @note	This function is called when gpio/external IRQ event mapped to
 *			GPIO_IRQ_ID2 is triggered.
 */
static void mbed_gpio_irq_id2_callback(void)
{
	if (mbed_gpio_irq_callback[GPIO_IRQ_ID2].desc.callback)
		/* Invoke the application registered callback function */
		mbed_gpio_irq_callback[GPIO_IRQ_ID2].desc.callback(
			mbed_gpio_irq_callback[GPIO_IRQ_ID2].desc.ctx);
}

/**
 * @brief	Mbed callback function for gpio/external interrupt ID2 event.
 * @return	none.
 * @note	This function is called when gpio/external IRQ event mapped to
 *			GPIO_IRQ_ID2 is triggered.
 */
static void mbed_gpio_irq_id3_callback(void)
{
	if (mbed_gpio_irq_callback[GPIO_IRQ_ID3].desc.callback)
		/* Invoke the application registered callback function */
		mbed_gpio_irq_callback[GPIO_IRQ_ID3].desc.callback(
			mbed_gpio_irq_callback[GPIO_IRQ_ID3].desc.ctx);
}

/**
 * @brief	Mbed callback function for gpio/external interrupt ID4 event.
 * @return	none.
 * @note	This function is called when gpio/external IRQ event mapped to
 *			GPIO_IRQ_ID4 is triggered.
 */
static void mbed_gpio_irq_id4_callback(void)
{
	if (mbed_gpio_irq_callback[GPIO_IRQ_ID4].desc.callback)
		/* Invoke the application registered callback function */
		mbed_gpio_irq_callback[GPIO_IRQ_ID4].desc.callback(
			mbed_gpio_irq_callback[GPIO_IRQ_ID4].desc.ctx);
}

/**
 * @brief	Mbed callback function for gpio/external interrupt ID5 event.
 * @return	none.
 * @note	This function is called when gpio/external IRQ event mapped to
 *			GPIO_IRQ_ID5 is triggered.
 */
static void mbed_gpio_irq_id5_callback(void)
{
	if (mbed_gpio_irq_callback[GPIO_IRQ_ID5].desc.callback)
		/* Invoke the application registered callback function */
		mbed_gpio_irq_callback[GPIO_IRQ_ID5].desc.callback(
			mbed_gpio_irq_callback[GPIO_IRQ_ID5].desc.ctx);
}

/**
 * @brief	Store mbed callback functions into irq callback structure
 * @return	none.
 */
static void store_mbed_callbacks(void)
{
	mbed_gpio_irq_callback[GPIO_IRQ_ID1].mbed_callback = mbed_gpio_irq_id1_callback;
	mbed_gpio_irq_callback[GPIO_IRQ_ID2].mbed_callback = mbed_gpio_irq_id2_callback;
	mbed_gpio_irq_callback[GPIO_IRQ_ID3].mbed_callback = mbed_gpio_irq_id3_callback;
	mbed_gpio_irq_callback[GPIO_IRQ_ID4].mbed_callback = mbed_gpio_irq_id4_callback;
	mbed_gpio_irq_callback[GPIO_IRQ_ID5].mbed_callback = mbed_gpio_irq_id5_callback;
}

/**
 * @brief	Initialize the GPIO IRQ controller.
 * @param	desc[in, out] - Pointer where the configured instance is stored.
 * @param	param[in] - Configuration information for the instance.
 * @return	0 in case of success, negative error code otherwise.
 */
int32_t mbed_gpio_irq_ctrl_init(struct no_os_irq_ctrl_desc **desc,
				const struct no_os_irq_init_param *param)
{
	struct no_os_irq_ctrl_desc *irq_desc;
	struct mbed_gpio_irq_desc *mbed_gpio_irq_desc;

	if (!desc || !param)
		return -EINVAL;

	irq_desc = (struct no_os_irq_ctrl_desc *)calloc(1, sizeof(*irq_desc));
	if (!irq_desc)
		return -ENOMEM;

	mbed_gpio_irq_desc = (struct mbed_gpio_irq_desc *)calloc(1,
			     sizeof(*mbed_gpio_irq_desc));
	if (!mbed_gpio_irq_desc)
		goto err_mbed_irq_desc;

	irq_desc->irq_ctrl_id = param->irq_ctrl_id;

	/* Copy the Mbed IRQ init parameters */
	mbed_gpio_irq_desc->gpio_irq_pin = ((struct mbed_gpio_irq_init_param *)
					    param->extra)->gpio_irq_pin;

	irq_desc->extra = mbed_gpio_irq_desc;
	*desc = irq_desc;

	return 0;

err_mbed_irq_desc:
	free(irq_desc);

	return -ENOMEM;
}

/**
 * @brief	Enable specific interrupt.
 * @param	desc[in] - The IRQ controller descriptor.
 * @param	irq_id[in] - Interrupt identifier.
 * @return	0 in case of success, negative error code otherwise.
 */
int32_t mbed_gpio_irq_enable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id)
{
	InterruptIn *ext_interrupt;

	if (!desc || !desc->extra || (irq_id >= NB_GPIO_IRQS))
		return -EINVAL;

	ext_interrupt = (InterruptIn *)(((struct mbed_gpio_irq_desc *)(
			desc->extra))->int_obj);
	ext_interrupt->enable_irq();

	return 0;
}

/**
 * @brief	Disable specific interrupt.
 * @param	desc[in] - The IRQ controller descriptor.
 * @param	irq_id[in] - Interrupt identifier.
 * @return	0 in case of success, negative error code otherwise.
 */
int32_t mbed_gpio_irq_disable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id)
{
	InterruptIn *ext_interrupt;

	if (!desc || !desc->extra || (irq_id >= NB_GPIO_IRQS))
		return -EINVAL;

	ext_interrupt = (InterruptIn *)(((struct mbed_gpio_irq_desc *)(
			desc->extra))->int_obj);
	ext_interrupt->disable_irq();

	return 0;
}

/**
* @brief	Registers a IRQ callback function to irq controller.
* @param	desc[in] - The IRQ controller descriptor.
* @param	irq_id[in] - Interrupt identifier.
* @param	callback_desc - Descriptor of the callback. If it is NULL, the
*			callback will be unregistered
* @return	0 in case of success, negative error code otherwise.
*/
int32_t mbed_gpio_irq_register_callback(struct no_os_irq_ctrl_desc *desc,
					uint32_t irq_id,
					struct no_os_callback_desc *callback_desc)
{
	InterruptIn *ext_interrupt;
	struct mbed_gpio_irq_desc *mbed_gpio_irq_desc;

	if (!desc || !desc->extra || !callback_desc || (irq_id >= NB_GPIO_IRQS))
		return -EINVAL;

	mbed_gpio_irq_desc = (struct mbed_gpio_irq_desc *)desc->extra;

	/* Store the static mbed callback functions */
	store_mbed_callbacks();

	/* Create a new external interrupt object */
	ext_interrupt = new InterruptIn((PinName)(mbed_gpio_irq_desc->gpio_irq_pin));
	if (!ext_interrupt)
		return -ENOMEM;

	mbed_gpio_irq_desc->int_obj = (mbed::InterruptIn *)ext_interrupt;

	/* Callback is attached from 'mbed_irq_enable' as ext_int attach
	 * immediately start invoking the interrupt */
	mbed_gpio_irq_callback[irq_id].desc.callback = callback_desc->callback;
	mbed_gpio_irq_callback[irq_id].desc.ctx = callback_desc->ctx;

	return 0;
}

/**
 * @brief	Unregister a IRQ callback function.
 * @param	desc[in] - The IRQ controller descriptor.
 * @param	irq_id[in] - Interrupt identifier.
 * @param   callback_desc[in] - Descriptor of the callback.
 * @return	0 in case of success, negative error code otherwise.
 */
int32_t mbed_gpio_irq_unregister(struct no_os_irq_ctrl_desc *desc,
				 uint32_t irq_id,
				 struct no_os_callback_desc *callback_desc)
{
	int32_t ret;

	ret = mbed_gpio_irq_disable(desc, irq_id);
	if (ret)
		return ret;

	mbed_gpio_irq_callback[irq_id].desc.callback = NULL;

	return 0;
}

/**
 * @brief	Set GPIO trigger level.
 * @param	desc[in] - The IRQ controller descriptor.
 * @param	irq_id[in] - Interrupt identifier.
 * @param   trig[in] - Trigger level.
 * @return	0 in case of success, negative error code otherwise.
 */
int32_t mbed_gpio_irq_trigger_level_set(struct no_os_irq_ctrl_desc *desc,
					uint32_t irq_id,
					enum no_os_irq_trig_level trig)
{
	InterruptIn *ext_interrupt;

	if (!desc || !desc->extra || (irq_id >= NB_GPIO_IRQS))
		return -EINVAL;

	ext_interrupt = (InterruptIn *)(((struct mbed_gpio_irq_desc *)(
			desc->extra))->int_obj);

	switch (trig) {
	case NO_OS_IRQ_EDGE_FALLING:
		ext_interrupt->fall(mbed_gpio_irq_callback[irq_id].mbed_callback);
		break;

	case NO_OS_IRQ_EDGE_RISING:
		ext_interrupt->rise(mbed_gpio_irq_callback[irq_id].mbed_callback);
		break;

	default:
		/* Mbed can support only rising/falling edge levels */
		return -EINVAL;
	}

	ext_interrupt->disable_irq();

	return 0;
}

/**
 * @brief	Free the resources allocated by mbed_irq_ctrl_init()
 * @param	desc[in, out] - Interrupt controller descriptor.
 * @return	0 in case of success, negative error code otherwise.
 */
int32_t mbed_gpio_irq_ctrl_remove(struct no_os_irq_ctrl_desc *desc)
{
	uint8_t irq_id;

	if (!desc || !desc->extra)
		return -EINVAL;

	if (!(((struct mbed_gpio_irq_desc *)desc->extra)->int_obj))
		return -EINVAL;

	/* Unregister all callbacks */
	for (irq_id = GPIO_IRQ_ID1; irq_id < (uint8_t)NB_GPIO_IRQS; irq_id++) {
		if (mbed_gpio_irq_unregister(desc, irq_id,
					     &mbed_gpio_irq_callback[irq_id].desc) != 0)
			return -EIO;
	}

	delete((InterruptIn *)(((struct mbed_gpio_irq_desc *)desc->extra)->int_obj));
	free(desc->extra);
	free(desc);

	return 0;
}

/**
 * @brief Mbed platform specific IRQ platform ops structure
 */
const struct no_os_irq_platform_ops mbed_gpio_irq_ops = {
	.init = &mbed_gpio_irq_ctrl_init,
	.register_callback = &mbed_gpio_irq_register_callback,
	.unregister_callback = &mbed_gpio_irq_unregister,
	.trigger_level_set = &mbed_gpio_irq_trigger_level_set,
	.enable = &mbed_gpio_irq_enable,
	.disable = &mbed_gpio_irq_disable,
	.remove = &mbed_gpio_irq_ctrl_remove
};

#ifdef __cplusplus // Closing extern c
}
#endif
