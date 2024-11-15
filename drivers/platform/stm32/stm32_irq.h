/***************************************************************************//**
 *   @file   stm32/stm32_irq.h
 *   @brief  Header file for stm32 irq specifics.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef STM32_IRQ_H
#define STM32_IRQ_H

#include "no_os_irq.h"
#include "stm32_hal.h"

/**
 * @brief stm32 platform specific irq platform ops structure
 */
extern const struct no_os_irq_platform_ops stm32_irq_ops;

#endif
