/***************************************************************************//**
 *   @file   rcd.h
 *   @brief  RCD interface file.
 *   @author GMois (george.mois@analog.com), REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2023 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef RCD_H_
#define RCD_H_

#include "no_os_gpio.h"
#include "no_os_irq.h"

/*! Get RCD flag value */
int get_rcd_flag_state(void);

/*! Reset RCD flag value */
void reset_rcd_flag_state(void);

/*! Initialize RCD */
int rcd_init(struct no_os_irq_ctrl_desc *desc,
	     struct no_os_gpio_desc **rcm_test, struct no_os_gpio_desc **rcddc,
	     struct no_os_gpio_desc **rcdac);

#endif /* RCD_H_ */
