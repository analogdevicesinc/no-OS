/***************************************************************************//**
 *   @file   no_os_irq.h
 *   @brief  Header file of IRQ interface.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef _NO_OS_IRQ_H_
#define _NO_OS_IRQ_H_

#include <stdint.h>

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
	NO_OS_EVT_RTC,
	NO_OS_EVT_XINT,
	NO_OS_EVT_TIM_ELAPSED,
	NO_OS_EVT_TIM_PWM_PULSE_FINISHED,
	NO_OS_EVT_LPTIM_PWM_PULSE_FINISHED,
	NO_OS_EVT_DMA_RX_COMPLETE,
	NO_OS_EVT_DMA_RX_HALF_COMPLETE,
	NO_OS_EVT_DMA_TX_COMPLETE,
	NO_OS_EVT_USB,
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
	NO_OS_RTC_IRQ,
	NO_OS_TIM_IRQ,
	NO_OS_LPTIM_IRQ,
	NO_OS_TDM_DMA_IRQ,
	NO_OS_TIM_DMA_IRQ,
	NO_OS_SPI_DMA_IRQ,
	NO_OS_DMA_IRQ,
	NO_OS_USB_IRQ,
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
	/**
	 * This is intended to store irq controller specific configurations,
	 * it should not be a reference to any peripheral descriptor.
	*/
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
	/* Reference counter */
	uint32_t ref;
	/**
	 * This is intended to store irq controller specific configurations,
	 * it should not be a reference to any peripheral descriptor.
	*/
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
	/** Parameter to be passed when the callback is called */
	void *ctx;
	/** Platform specific event that triggers the calling of the callback. */
	enum no_os_irq_event event;
	/** Interrupt source peripheral specifier. */
	enum no_os_irq_peripheral peripheral;
	/** This will be used to store HAL specific descriptors */
	void *handle;
};

/**
 * @struct no_os_irq_platform_ops
 * @brief Structure holding IRQ function pointers that point to the platform
 * specific function
 */
struct no_os_irq_platform_ops {
	/** Initialize a interrupt controller peripheral. */
	int (*init)(struct no_os_irq_ctrl_desc **desc,
		    const struct no_os_irq_init_param *param);
	/** Register a callback to handle the irq events */
	int (*register_callback)(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id,
				 struct no_os_callback_desc *callback);
	/** Unregisters a generic IRQ handling function */
	int (*unregister_callback)(struct no_os_irq_ctrl_desc *desc,
				   uint32_t irq_id,
				   struct no_os_callback_desc *callback);
	/** Global interrupt enable */
	int (*global_enable)(struct no_os_irq_ctrl_desc *desc);
	/** Global interrupt disable */
	int (*global_disable)(struct no_os_irq_ctrl_desc *desc);
	/** Set interrupt trigger level. */
	int (*trigger_level_set)(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id,
				 enum no_os_irq_trig_level trig);
	/** Enable specific interrupt */
	int (*enable)(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id);
	/** Disable specific interrupt */
	int (*disable)(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id);
	/** Set the priority level for a specific interrupt */
	int (*set_priority)(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id,
			    uint32_t priority_level);
	/** Get the priority level for a specific interrupt */
	int (*get_priority)(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id,
			    uint32_t *priority_level);
	/** IRQ remove function pointer */
	int (*remove)(struct no_os_irq_ctrl_desc *desc);
	/** Clear pending interrupt */
	int(*clear_pending)(struct no_os_irq_ctrl_desc* desc, uint32_t irq_id);
};

/* Initialize a interrupt controller peripheral. */
int no_os_irq_ctrl_init(struct no_os_irq_ctrl_desc **desc,
			const struct no_os_irq_init_param *param);

/* Free the resources allocated by no_os_irq_ctrl_init(). */
int no_os_irq_ctrl_remove(struct no_os_irq_ctrl_desc *desc);

/* Register a callback to handle the irq events */
int no_os_irq_register_callback(struct no_os_irq_ctrl_desc *desc,
				uint32_t irq_id,
				struct no_os_callback_desc *callback_desc);

/* Unregisters a generic IRQ handling function */
int no_os_irq_unregister_callback(struct no_os_irq_ctrl_desc *desc,
				  uint32_t irq_id,
				  struct no_os_callback_desc *callback_desc);

/* Global interrupt enable */
int no_os_irq_global_enable(struct no_os_irq_ctrl_desc *desc);

/* Global interrupt disable */
int no_os_irq_global_disable(struct no_os_irq_ctrl_desc *desc);

/* Set interrupt trigger level. */
int no_os_irq_trigger_level_set(struct no_os_irq_ctrl_desc *desc,
				uint32_t irq_id,
				enum no_os_irq_trig_level trig);

/* Enable specific interrupt */
int no_os_irq_enable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id);

/* Disable specific interrupt */
int no_os_irq_disable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id);

/** Set the priority level for a specific interrupt */
int no_os_irq_set_priority(struct no_os_irq_ctrl_desc *desc,
			   uint32_t irq_id,
			   uint32_t priority_level);

/** Get the priority level for a specific interrupt */
int no_os_irq_get_priority(struct no_os_irq_ctrl_desc *desc,
			   uint32_t irq_id,
			   uint32_t *priority_level);

/* Clear the pending interrupts */
int no_os_irq_clear_pending(struct no_os_irq_ctrl_desc* desc,
			    uint32_t irq_id);
#endif // _NO_OS_IRQ_H_
