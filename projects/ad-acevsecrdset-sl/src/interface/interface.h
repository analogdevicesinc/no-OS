/***************************************************************************/ /**
*   @file   interface.h
*   @brief  User interface interface file.
*   @author GMois (george.mois@analog.com), REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2023 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef __INTERFACE_H__
#define __INTERFACE_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "state_machine.h"
#include "parameters.h"
#include "no_os_gpio.h"

/******************************************************************************/
/************************* Functions Declarations******************************/
/******************************************************************************/
/*! Interface initialization routines */
int interface_init(struct no_os_gpio_desc **gpio_led_desc);

/*! Turn LED on */
int interface_led_on(struct no_os_gpio_desc *gpio_led_desc);

/*! Turn LED off */
int interface_led_off(struct no_os_gpio_desc *gpio_led_desc);

/*! Toggle LED */
int interface_toggle_led(struct no_os_gpio_desc *gpio_led_desc);

/*! Display error code */
int interface_disp(struct stout *stout);

/*! Get interface blink time (multimple of 20ms) */
int interface_blink_time(struct stout *stout);

#endif /* __INTERFACE_H__ */
