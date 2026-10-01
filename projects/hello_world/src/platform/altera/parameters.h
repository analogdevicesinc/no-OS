/***************************************************************************//**
 *   @file   altera/parameters.h
 *   @brief  Definitions specific to Nios V hello_world example.
 *   @author Aurel Miron (aurel.miron@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include <stddef.h>
#include <stdint.h>

/*
 * For the Nios V / Intel FPGA BSP test, the console is the BSP-provided JTAG
 * UART. There is no no-OS UART platform driver for this target yet, so
 * main.c prints via the BSP's own stdio implementation directly instead of
 * going through no_os_uart_init().
 */

#endif /* __PARAMETERS_H__ */
