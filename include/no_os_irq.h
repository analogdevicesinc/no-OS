/***************************************************************************//**
 *   @file   no_os_irq.h
 *   @brief  Header file of IRQ interface.
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

#ifndef _NO_OS_IRQ_H_
#define _NO_OS_IRQ_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum no_os_irq_uart_event_e
 * @brief Possible events for uart interrupt
 * @todo replace this with enum no_os_irq_event
 */
enum no_os_irq_uart_event_e {
	/** Write operation finalized */
	NO_OS_IRQ_WRITE_DONE,
	/** Read operation finalized */
	NO_OS_IRQ_READ_DONE,
	/** An error occurred */
	NO_OS_IRQ_ERROR
};

enum no_os_irq_event {
	NO_OS_EVT_GPIO,
	NO_OS_EVT_UART_TX_COMPLETE,
	NO_OS_EVT_UART_RX_COMPLETE,
	NO_OS_EVT_UART_ERROR,
};

enum no_os_irq_trig_level {
	NO_OS_IRQ_LEVEL_LOW,
	NO_OS_IRQ_LEVEL_HIGH,
	NO_OS_IRQ_EDGE_FALLING,
	NO_OS_IRQ_EDGE_RISING,
	NO_OS_IRQ_EDGE_BOTH
};

enum no_os_irq_peripheral {
	NO_OS_GPIO_IRQ,
	NO_OS_UART_IRQ,
};

/**
 * @struct no_os_irq_platform_ops
 * @brief Structure holding IRQ function pointers that point to the platform
 * specific function
 */
struct no_os_irq_platform_ops ;

/**
 * @struct no_os_irq_init_param
 * @brief Structure holding the initial parameters for Interrupt Request.
 */
struct no_os_irq_init_param {
	/** Interrupt request controller ID. */
	uint32_t	irq_ctrl_id;
	/** Platform specific IRQ platform ops structure. */
	const struct no_os_irq_platform_ops *platform_ops;
	/** IRQ extra parameters (device specific) */
	void		*extra;
};

/**
 * @struct irq_desc
 * @brief Structure for Interrupt Request descriptor.
 */
struct no_os_irq_ctrl_desc {
	/** Interrupt request controller ID. */
	uint32_t	irq_ctrl_id;
	/** Platform specific IRQ platform ops structure. */
	const struct no_os_irq_platform_ops *platform_ops;
	/** IRQ extra parameters (device specific) */
	void		*extra;
};

/**
 * @struct no_os_callback_desc
 * @brief Structure describing a callback to be registered
 * @todo: remove this, use struct irq_callback instead.
 */
struct no_os_callback_desc {
	/** Callback to be called when the event an event occurs. */
	void (*callback)(void *context);
	/**
	 * Callback to be called when the event an event occurs
	 *  @param ctx - Same as \ref no_os_callback_desc.ctx
	 *  @param event - Event that generated the callback
	 *  @param extra - Platform specific data
	 */
	void (*legacy_callback)(void *ctx, uint32_t event,
				void *extra); /* TODO: remove this. */
	/** Parameter to be passed when the callback is called */
	void *ctx;
	/** Platform specific configuration for a callback */
	void *legacy_config; /* TODO: remove this. */
	/** Platform specific event that triggers the calling of the callback. */
	enum no_os_irq_event event;
	/** Interrupt source peripheral specifier. */
	enum no_os_irq_peripheral peripheral;
};

/**
 * @struct no_os_irq_platform_ops
 * @brief Structure holding IRQ function pointers that point to the platform
 * specific function
 */
struct no_os_irq_platform_ops {
	/** Initialize a interrupt controller peripheral. */
	int32_t (*init)(struct no_os_irq_ctrl_desc **desc,
			const struct no_os_irq_init_param *param);
	/** Register a legacy_callback to handle the irq events */
	int32_t (*register_callback)(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id,
				     struct no_os_callback_desc *callback);
	/** Unregisters a generic IRQ handling function */
	int32_t (*unregister_callback)(struct no_os_irq_ctrl_desc *desc,
				       uint32_t irq_id,
				       struct no_os_callback_desc *callback);
	/** Global interrupt enable */
	int32_t (*global_enable)(struct no_os_irq_ctrl_desc *desc);
	/** Global interrupt disable */
	int32_t (*global_disable)(struct no_os_irq_ctrl_desc *desc);
	/** Set interrupt trigger level. */
	int32_t (*trigger_level_set)(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id,
				     enum no_os_irq_trig_level trig);
	/** Enable specific interrupt */
	int32_t (*enable)(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id);
	/** Disable specific interrupt */
	int32_t (*disable)(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id);
	/** IRQ remove function pointer */
	int32_t (*remove)(struct no_os_irq_ctrl_desc *desc);
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Initialize a interrupt controller peripheral. */
int32_t no_os_irq_ctrl_init(struct no_os_irq_ctrl_desc **desc,
			    const struct no_os_irq_init_param *param);

/* Free the resources allocated by no_os_irq_ctrl_init(). */
int32_t no_os_irq_ctrl_remove(struct no_os_irq_ctrl_desc *desc);

/* Register a callback to handle the irq events */
int32_t no_os_irq_register_callback(struct no_os_irq_ctrl_desc *desc,
				    uint32_t irq_id,
				    struct no_os_callback_desc *callback_desc);

/* Unregisters a generic IRQ handling function */
int32_t no_os_irq_unregister_callback(struct no_os_irq_ctrl_desc *desc,
				      uint32_t irq_id,
				      struct no_os_callback_desc *callback_desc);

/* Global interrupt enable */
int32_t no_os_irq_global_enable(struct no_os_irq_ctrl_desc *desc);

/* Global interrupt disable */
int32_t no_os_irq_global_disable(struct no_os_irq_ctrl_desc *desc);

/* Set interrupt trigger level. */
int32_t no_os_irq_trigger_level_set(struct no_os_irq_ctrl_desc *desc,
				    uint32_t irq_id,
				    enum no_os_irq_trig_level trig);

/* Enable specific interrupt */
int32_t no_os_irq_enable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id);

/* Disable specific interrupt */
int32_t no_os_irq_disable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id);

#endif // _NO_OS_IRQ_H_
