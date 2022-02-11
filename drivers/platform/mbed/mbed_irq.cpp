/***************************************************************************//**
 * @file   mbed_irq.cpp
 * @brief  Implementation of Interrupt Mbed platform driver interfaces
********************************************************************************
 * Copyright (c) 2020-2022 Analog Devices, Inc.
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

#include "no-os/error.h"
#include "no-os/irq.h"
#include "mbed_irq.h"
#include "mbed_uart.h"

using namespace std::chrono;

/**
* @struct mbed_irq_callback_desc.
* @brief Structure holding the callback functions for mbed irqs.
* @note  The callback functions are directly mapped with 'irq_id' enum.
* Mbed doesn't provide a single callback function for all external/GPIO
* IRQ events. A seperate callback function is required to be registered
* for every IRQ instance. Also, the 'callback_function' required for
* mbed layer expects void argument and void return type.
*/
struct mbed_irq_callback_desc {
	struct callback_desc desc;
	void (*mbed_callback)(void);
};

/* Mbed irq callback structure variable */
static struct mbed_irq_callback_desc mbed_irq_callback[NB_INTERRUPTS];

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief	Mbed callback function for external interrupt ID1 event.
 * @return	none.
 * @note	This function is called when external IRQ event mapped to
 *			EXTERNAL_INT_ID1 is triggered.
 */
static void mbed_ext_int_id1_callback(void)
{
	if (mbed_irq_callback[EXTERNAL_INT_ID1].desc.callback)
		/* Invoke the application registered callback function */
		mbed_irq_callback[EXTERNAL_INT_ID1].desc.callback(
			mbed_irq_callback[EXTERNAL_INT_ID1].desc.ctx, EXTERNAL_INT_ID1, NULL);
}

/**
 * @brief	Mbed callback function for external interrupt ID2 event.
 * @return	none.
 * @note	This function is called when external IRQ event mapped to
 *			EXTERNAL_INT_ID2 is triggered.
 */
static void mbed_ext_int_id2_callback(void)
{
	if (mbed_irq_callback[EXTERNAL_INT_ID2].desc.callback)
		/* Invoke the application registered callback function */
		mbed_irq_callback[EXTERNAL_INT_ID2].desc.callback(
			mbed_irq_callback[EXTERNAL_INT_ID2].desc.ctx, EXTERNAL_INT_ID2, NULL);
}

/**
 * @brief	Mbed callback function for external interrupt ID3 event.
 * @return	none.
 * @note	This function is called when external IRQ event mapped to
 *			EXTERNAL_INT_ID3 is triggered.
 */
static void mbed_ext_int_id3_callback(void)
{
	if (mbed_irq_callback[EXTERNAL_INT_ID3].desc.callback)
		/* Invoke the application registered callback function */
		mbed_irq_callback[EXTERNAL_INT_ID3].desc.callback(
			mbed_irq_callback[EXTERNAL_INT_ID3].desc.ctx, EXTERNAL_INT_ID3, NULL);
}

/**
 * @brief	Mbed callback function for external interrupt ID4 event.
 * @return	none.
 * @note	This function is called when external IRQ event mapped to
 *			EXTERNAL_INT_ID4 is triggered.
 */
static void mbed_ext_int_id4_callback(void)
{
	if (mbed_irq_callback[EXTERNAL_INT_ID4].desc.callback)
		/* Invoke the application registered callback function */
		mbed_irq_callback[EXTERNAL_INT_ID4].desc.callback(
			mbed_irq_callback[EXTERNAL_INT_ID4].desc.ctx, EXTERNAL_INT_ID4, NULL);
}

/**
 * @brief	Mbed callback function for external interrupt ID5 event.
 * @return	none.
 * @note	This function is called when external IRQ event mapped to
 *			EXTERNAL_INT_ID5 is triggered.
 */
static void mbed_ext_int_id5_callback(void)
{
	if (mbed_irq_callback[EXTERNAL_INT_ID5].desc.callback)
		/* Invoke the application registered callback function */
		mbed_irq_callback[EXTERNAL_INT_ID5].desc.callback(
			mbed_irq_callback[EXTERNAL_INT_ID5].desc.ctx, EXTERNAL_INT_ID5, NULL);
}

/**
 * @brief	Mbed callback function for UART Rx ID1 event.
 * @return	none.
 * @note	This function is called when external IRQ event mapped to
 *			UART_RX_INT_ID1 is triggered.
 */
static void mbed_uart_rx_id1_callback(void)
{
	if (mbed_irq_callback[UART_RX_INT_ID1].desc.callback)
		/* Invoke the application registered callback function */
		mbed_irq_callback[UART_RX_INT_ID1].desc.callback(
			mbed_irq_callback[UART_RX_INT_ID1].desc.ctx, UART_RX_INT_ID1, NULL);
}

/**
 * @brief	Mbed callback function for ticker ID event.
 * @return	none.
 * @note	This function is called when external IRQ event mapped to
 *			TICKER_INT_ID is triggered.
 */
static void mbed_ticker_id_callback(void)
{
	if (mbed_irq_callback[TICKER_INT_ID].desc.callback)
		/* Invoke the application registered callback function */
		mbed_irq_callback[TICKER_INT_ID].desc.callback(
			mbed_irq_callback[TICKER_INT_ID].desc.ctx, TICKER_INT_ID, NULL);
}

/**
 * @brief	Store mbed callback functions into irq callback structure
 * @return	none.
 */
static void store_mbed_callbacks(void)
{
	mbed_irq_callback[EXTERNAL_INT_ID1].mbed_callback = mbed_ext_int_id1_callback;
	mbed_irq_callback[EXTERNAL_INT_ID2].mbed_callback = mbed_ext_int_id2_callback;
	mbed_irq_callback[EXTERNAL_INT_ID3].mbed_callback = mbed_ext_int_id3_callback;
	mbed_irq_callback[EXTERNAL_INT_ID4].mbed_callback = mbed_ext_int_id4_callback;
	mbed_irq_callback[EXTERNAL_INT_ID5].mbed_callback = mbed_ext_int_id5_callback;
	mbed_irq_callback[UART_RX_INT_ID1].mbed_callback = mbed_uart_rx_id1_callback;
	mbed_irq_callback[TICKER_INT_ID].mbed_callback = mbed_ticker_id_callback;
}

/**
 * @brief	Initialized the controller for the peripheral interrupts.
 * @param	desc[in, out] - Pointer where the configured instance is stored.
 * @param	param[in] - Configuration information for the instance.
 * @return	SUCCESS in case of success, negative error code otherwise.
 * @note	Supports only External, Ticker and Unbuffered UART Rx IRQs.
 */
int32_t mbed_irq_ctrl_init(struct irq_ctrl_desc **desc,
			   const struct irq_init_param *param)
{
	struct irq_ctrl_desc *irq_desc;
	struct mbed_irq_desc *mbed_irq_desc;

	if (!desc || !param)
		return -EINVAL;

	irq_desc = (struct irq_ctrl_desc *)calloc(1, sizeof(*irq_desc));
	if (!irq_desc)
		return -ENOMEM;

	mbed_irq_desc = (struct mbed_irq_desc *)calloc(1, sizeof(*mbed_irq_desc));
	if (!mbed_irq_desc)
		goto err_mbed_irq_desc;

	irq_desc->irq_ctrl_id = param->irq_ctrl_id;

	/* Copy the Mbed IRQ init parameters */
	mbed_irq_desc->int_mode = ((struct mbed_irq_init_param *)
				   param->extra)->int_mode;
	mbed_irq_desc->ext_int_pin = ((struct mbed_irq_init_param *)
				      param->extra)->ext_int_pin;
	mbed_irq_desc->ticker_period_usec = ((struct mbed_irq_init_param *)
					     param->extra)->ticker_period_usec;
	mbed_irq_desc->int_obj = ((struct mbed_irq_init_param *)
				  param->extra)->int_obj_type;

	irq_desc->extra = mbed_irq_desc;
	*desc = irq_desc;

	return SUCCESS;

err_mbed_irq_desc:
	free(irq_desc);

	return -ENOMEM;
}

/**
 * @brief	Enable specific interrupt.
 * @param	desc[in] - The IRQ controller descriptor.
 * @param	irq_id[in] - Interrupt identifier.
 * @return	SUCCESS in case of success, negative error code otherwise.
 */
int32_t mbed_irq_enable(struct irq_ctrl_desc *desc, uint32_t irq_id)
{
	InterruptIn *ext_interrupt;
	mbed::UnbufferedSerial *uart_rx_port;
	mbed::Ticker *ticker;

	if (!desc || !desc->extra)
		return -EINVAL;

	switch (irq_id) {
	case EXTERNAL_INT_ID1:
	case EXTERNAL_INT_ID2:
	case EXTERNAL_INT_ID3:
	case EXTERNAL_INT_ID4:
	case EXTERNAL_INT_ID5:
		ext_interrupt = (InterruptIn *)(((struct mbed_irq_desc *)(
				desc->extra))->int_obj);

		/* Attach (enable) external interrupt based on mode */
		if (((struct mbed_irq_desc *)(desc->extra))->int_mode == EXT_IRQ_FALL)
			ext_interrupt->fall(mbed_irq_callback[irq_id].mbed_callback);
		else if (((struct mbed_irq_desc *)(desc->extra))->int_mode == EXT_IRQ_RISE)
			ext_interrupt->rise(mbed_irq_callback[irq_id].mbed_callback);
		else
			return -EINVAL;
		break;

	case TICKER_INT_ID:
		ticker = (mbed::Ticker *)(((struct mbed_irq_desc *)(desc->extra))->int_obj);
		ticker->attach(mbed_irq_callback[TICKER_INT_ID].mbed_callback,
			       microseconds(((struct mbed_irq_desc *)(desc->extra))->ticker_period_usec));
		break;

	case UART_RX_INT_ID1:
		uart_rx_port = (mbed::UnbufferedSerial *)(((struct mbed_irq_desc *)(
					desc->extra))->int_obj);
		uart_rx_port->attach(mbed_irq_callback[UART_RX_INT_ID1].mbed_callback,
				     UnbufferedSerial::RxIrq);
		break;

	default:
		return -EINVAL;
	}

	return SUCCESS;
}

/**
 * @brief	Disable specific interrupt.
 * @param	desc[in] - The IRQ controller descriptor.
 * @param	irq_id[in] - Interrupt identifier.
 * @return	SUCCESS in case of success, negative error code otherwise.
 */
int32_t mbed_irq_disable(struct irq_ctrl_desc *desc, uint32_t irq_id)
{
	InterruptIn *ext_interrupt;
	mbed::UnbufferedSerial *uart_rx_port;
	mbed::Ticker *ticker;

	if (!desc || !desc->extra)
		return -EINVAL;

	switch (irq_id) {
	case EXTERNAL_INT_ID1:
	case EXTERNAL_INT_ID2:
	case EXTERNAL_INT_ID3:
	case EXTERNAL_INT_ID4:
	case EXTERNAL_INT_ID5:
		ext_interrupt = (InterruptIn *)(((struct mbed_irq_desc *)(
				desc->extra))->int_obj);

		/* Detach (disable) external interrupt */
		if (((struct mbed_irq_desc *)(desc->extra))->int_mode == EXT_IRQ_FALL)
			ext_interrupt->fall(NULL);
		else if (((struct mbed_irq_desc *)(desc->extra))->int_mode == EXT_IRQ_RISE)
			ext_interrupt->rise(NULL);
		else
			return -EINVAL;
		break;

	case TICKER_INT_ID:
		ticker = (mbed::Ticker *)(((struct mbed_irq_desc *)(desc->extra))->int_obj);
		ticker->detach();
		break;

	case UART_RX_INT_ID1:
		uart_rx_port = (mbed::UnbufferedSerial *)(((struct mbed_irq_desc *)(
					desc->extra))->int_obj);
		uart_rx_port->attach(NULL, UnbufferedSerial::RxIrq);
		break;

	default:
		return -EINVAL;
	}

	return SUCCESS;
}

/**
* @brief	Registers a IRQ callback function to irq controller.
* @param	desc[in] - The IRQ controller descriptor.
* @param	irq_id[in] - Interrupt identifier.
* @param	callback_desc - Descriptor of the callback. If it is NULL, the
*			callback will be unregistered
* @return	SUCCESS in case of success, negative error code otherwise.
*/
int32_t mbed_irq_register_callback(struct irq_ctrl_desc *desc,
				   uint32_t irq_id,
				   struct callback_desc *callback_desc)
{
	mbed::Ticker *ticker;
	InterruptIn *ext_interrupt;
	struct mbed_irq_desc *mbed_irq_desc;

	if (!desc || !desc->extra || !callback_desc)
		return -EINVAL;

	mbed_irq_desc = (struct mbed_irq_desc *)desc->extra;

	/* Store the static mbed callback functions */
	store_mbed_callbacks();

	switch (irq_id) {
	case EXTERNAL_INT_ID1:
	case EXTERNAL_INT_ID2:
	case EXTERNAL_INT_ID3:
	case EXTERNAL_INT_ID4:
	case EXTERNAL_INT_ID5:
		/* Create a new external interrupt object */
		ext_interrupt = new InterruptIn((PinName)(mbed_irq_desc->ext_int_pin));
		if (!ext_interrupt)
			return -ENOMEM;

		mbed_irq_desc->int_obj = (mbed::InterruptIn *)ext_interrupt;

		/* Callback is attached from 'mbed_irq_enable' as ext_int attach
		 * immediately start invoking the interrupt */
		mbed_irq_callback[irq_id].desc.callback = callback_desc->callback;
		mbed_irq_callback[irq_id].desc.ctx = callback_desc->ctx;
		break;

	case TICKER_INT_ID:
		/* Ticker is a special mbed class used for generating recurring interrupt.
		* The object of this class is created during interrupt initialization as:
		* 1) To avoid having seperate module for it.
		* 2) To avoid having multiple instances of Ticker class.
		* */
		ticker = new Ticker();
		if (!ticker)
			return -ENOMEM;

		mbed_irq_desc->int_obj = (mbed::Ticker *)ticker;

		/* Callback is attached from 'mbed_irq_enable' as ticker attach immediately
		 * start invoking the interrupt */
		mbed_irq_callback[irq_id].desc.callback = callback_desc->callback;
		mbed_irq_callback[irq_id].desc.ctx = callback_desc->ctx;
		break;

	case UART_RX_INT_ID1:
		/* Callback is attached from 'mbed_irq_enable' as uart attach immediately
		 * start invoking the interrupt */
		mbed_irq_callback[irq_id].desc.callback = callback_desc->callback;
		mbed_irq_callback[irq_id].desc.ctx = callback_desc->ctx;
		break;

	default:
		return -EINVAL;
	}

	return SUCCESS;
}

/**
 * @brief	Unregister a IRQ callback function.
 * @param	desc[in] - The IRQ controller descriptor.
 * @param	irq_id[in] - Interrupt identifier.
 * @return	SUCCESS in case of success, negative error code otherwise.
 */
int32_t mbed_irq_unregister(struct irq_ctrl_desc *desc, uint32_t irq_id)
{
	int32_t ret;

	if (!desc || !desc->extra)
		return -EINVAL;

	if (irq_id >= NB_INTERRUPTS)
		return -EINVAL;

	ret = mbed_irq_disable(desc, irq_id);
	if (ret)
		return ret;

	mbed_irq_callback[irq_id].desc.callback = NULL;

	return SUCCESS;
}

/**
 * @brief	Free the resources allocated by mbed_irq_ctrl_init()
 * @param	desc[in, out] - Interrupt controller descriptor.
 * @return	SUCCESS in case of success, negative error code otherwise.
 */
int32_t mbed_irq_ctrl_remove(struct irq_ctrl_desc *desc)
{
	uint8_t irq_id;

	if (!desc || !desc->extra)
		return -EINVAL;

	if (!(((struct mbed_irq_desc *)desc->extra)->int_obj))
		return -EINVAL;

	/* Unregister all callbacks */
	for (irq_id = 0; irq_id < (uint8_t)NB_INTERRUPTS; irq_id++) {
		if (mbed_irq_unregister(desc, irq_id) != SUCCESS)
			return -EIO;
	}

	switch (desc->irq_ctrl_id) {
	case EXTERNAL_INT_ID1:
	case EXTERNAL_INT_ID2:
	case EXTERNAL_INT_ID3:
	case EXTERNAL_INT_ID4:
	case EXTERNAL_INT_ID5:
		delete((InterruptIn *)(((struct mbed_irq_desc *)desc->extra)->int_obj));
		break;

	case TICKER_INT_ID:
		/* Ticker doesn't have destructor implementation, so nothing to free.
		 * Object detach is done from irq_unregister()->irq_disable() functions */
		break;

	case UART_RX_INT_ID1:
		/* Should be freed from application as new object is not
		 * created for UART instance in this file.
		 * Object detach is done from irq_unregister()->irq_disable() functions */
		break;

	default:
		return -EINVAL;
	}

	free(desc->extra);
	free(desc);

	return SUCCESS;
}

/**
 * @brief Mbed platform specific IRQ platform ops structure
 */
const struct irq_platform_ops mbed_irq_ops = {
	.init = &mbed_irq_ctrl_init,
	.register_callback = &mbed_irq_register_callback,
	.unregister = &mbed_irq_unregister,
	.enable = &mbed_irq_enable,
	.disable = &mbed_irq_disable,
	.remove = &mbed_irq_ctrl_remove
};

#ifdef __cplusplus // Closing extern c
}
#endif
