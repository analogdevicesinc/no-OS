/***************************************************************************//**
 *   @file   maxim_irq.h
 *   @brief  Header file for maxim irq specifics.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#ifndef MAXIM_IRQ_H_
#define MAXIM_IRQ_H_

#include "max32660.h"
#include "no_os_irq.h"
#include "no_os_list.h"
#include "uart.h"

/**
 * @brief Struct used to store a (peripheral, callback) pair
 */
struct irq_action {
	uint32_t irq_id;
	void *handle;
	void (*callback)(void *context);
	void *ctx;
};

/**
 * @brief Struct that stores all the actions for a specific event
 */
struct event_list {
	enum no_os_irq_event event;
	struct no_os_list_desc *actions;
};

/**
 * @brief maxim platform specific irq platform ops structure
 */
extern const struct no_os_irq_platform_ops max_irq_ops;

/**
 * @brief Platform specific uart callback function
 */
void max_uart_callback(mxc_uart_req_t *, int);

/**
 * @brief irq_action compare function
 */
int32_t irq_action_cmp(void *data1, void *data2);

#endif
