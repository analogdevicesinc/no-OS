/***************************************************************************//**
 *   @file   maxim_irq.h
 *   @brief  Header file for maxim irq specifics.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef MAXIM_IRQ_H_
#define MAXIM_IRQ_H_

#include "max32690.h"
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
