/***************************************************************************//**
 *   @file   inter.h
 *   @brief  Data ready interrupt file.
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __INTER_H__
#define __INTER_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "no_os_irq.h"

/******************************************************************************/
/************************* Functions Declarations *****************************/
/******************************************************************************/

/*! Get GPIO flag value. */
int get_gpio_flag_state(void);

/*! Reset GPIO flag value. */
void reset_gpio_low_flag_state(void);

/*! Initialize interrupt phase */
int inter_init(struct ade7913_dev *dev);

#endif /* __INTER__H__ */
