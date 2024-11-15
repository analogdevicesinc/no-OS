/***************************************************************************//**
 *   @file   pico/pico_irq.h
 *   @brief  Header file for pico irq specifics.
 *   @author RBolboac (ramona.bolboaca@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef _PICO_IRQ_H_
#define _PICO_IRQ_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_irq.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @brief pico platform specific irq platform ops structure
 */
extern const struct no_os_irq_platform_ops pico_irq_ops;

#endif
