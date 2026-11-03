/***************************************************************************//**
 *   @file   altera/parameters.c
 *   @brief  Definitions specific to Nios V hello_world example.
 *   @author Aurel Miron (aurel.miron@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "parameters.h"

/*
 * The Altera/Nios V test uses the BSP HAL's existing JTAG UART stdio hooks via
 * the normal libc/stdio path. No custom no-OS UART platform ops are required
 * here because the BSP already provides the transport layer.
 */
