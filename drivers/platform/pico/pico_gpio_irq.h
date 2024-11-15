/***************************************************************************//**
 *   @file   pico/pico_gpio_irq.h
 *   @brief  Header file for pico gpio irq specifics.
 *   @author Ramona Bolboaca (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef PICO_GPIO_IRQ_H
#define PICO_GPIO_IRQ_H

/******************************************************************************/
/************************* Include Files **************************************/
/******************************************************************************/

#include "no_os_irq.h"
#include "hardware/gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define PICO_GPIO_MAX_PIN_NB 30

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct pico_gpio_irq_desc
 * @brief pico platform specific GPIO IRQ descriptor
 */
struct pico_gpio_irq_desc {
	/** Pin trigger level set by application  */
	enum gpio_irq_level pin_trigger_lvl[PICO_GPIO_MAX_PIN_NB];
};

/**
 * @brief pico platform specific irq platform ops structure
 */
extern const struct no_os_irq_platform_ops pico_gpio_irq_ops;

#endif
