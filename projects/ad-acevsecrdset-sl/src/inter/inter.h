/***************************************************************************//**
 *   @file   inter.h
 *   @brief  GPIO interrupt phase interface file.
 *   @author GMois (george.mois@analog.com), REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef INTER_H_
#define INTER_H_

#include "state_machine.h"
#include "parameters.h"
#include "no_os_irq.h"

/*! Get GPIO flag value. */
int get_gpio_flag_state(void);

/*! Reset GPIO flag value. */
void reset_gpio_low_flag_state(void);

/*! Get GPIO opto1 flag value. */
int get_gpio_opto1_flag_state(void);

/*! Reset GPIO opto1 flag value. */
void reset_gpio_opto1_flag_state(void);

/*! Get GPIO opto2 flag value. */
int get_gpio_opto2_flag_state(void);

/*! Reset GPIO opto2 flag value. */
void reset_gpio_opto2_flag_state(void);

/*! Initialize interrupt phase */
int inter_init(struct stout *stout);

#endif /* INTER_H_ */
