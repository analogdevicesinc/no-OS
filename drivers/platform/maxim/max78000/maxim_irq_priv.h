/***************************************************************************//**
 *   @file   maxim_irq_priv.h
 *   @brief  MAX78000 IRQ driver internal interface.
 *   @author Victor Pascu (victor.pascu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_IRQ_PRIV_H_
#define MAXIM_IRQ_PRIV_H_

#include "maxim_irq.h"
#include "maxim_core.h"

int max_irq_global_enable(struct no_os_irq_ctrl_desc *desc);
int max_irq_global_disable(struct no_os_irq_ctrl_desc *desc);
int max_irq_enable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id);
int max_irq_disable(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id);
int max_irq_set_priority(struct no_os_irq_ctrl_desc *desc, uint32_t irq_id,
			 uint32_t priority_level);

#endif /* MAXIM_IRQ_PRIV_H_ */
