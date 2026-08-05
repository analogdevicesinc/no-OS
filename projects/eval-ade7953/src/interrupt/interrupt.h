/***************************************************************************//**
 *   @file   interrupt.h
 *   @brief  Data ready interrupt file.
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __INTER_H__
#define __INTER_H__

#include "no_os_irq.h"

/* IRQ config */
#define GPIO_IRQ_OPS                &max_gpio_irq_ops
#define GPIO_CTRL_IRQ_ID            0
#define GPIO_IRQ_EXTRA              &gpio_extra_ip
#define NVIC_GPIO_IRQ               GPIO2_IRQn

/*! Get IRQN flag value. */
int get_irq_flag_state(void);

/*! Reset IRQN flag value. */
void reset_irq_flag_state(void);

/*! Initialize IRQN interrupt */
int inter_init_irq(struct ade7953_dev *dev);

/*! Get ZX flag value */
int get_zx_flag_state(void);

/*! Reset ZX flag value */
void reset_zx_flag_state(void);

/*! Initialize ZX interrupt */
int inter_init_zx(struct ade7953_dev *dev);

#endif /* __INTER__H__ */
