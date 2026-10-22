/***************************************************************************//**
 *   @file   maxim_core.h
 *   @brief  Host test stub for the MAX78000 core helpers.
 *   @author Victor Pascu (victor.pascu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef MAXIM_CORE_H_
#define MAXIM_CORE_H_
#include <stdint.h>
#define MAXIM_IRQ_HANDLER
#define MAXIM_RISCV_TIME_TIMER 3
uint32_t maxim_irq_save(void);
void maxim_irq_restore(uint32_t state);
void NVIC_ClearPendingIRQ(uint32_t id);
void NVIC_EnableIRQ(uint32_t id);
#endif
