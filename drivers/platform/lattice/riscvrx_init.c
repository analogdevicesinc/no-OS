/*******************************************************************************
 *   @file   riscvrx_init.c
 *   @brief  Implementation of Lattice RISC-V RX initialization.
 *   @author Vilmos-Csaba Jozsa (vilmoscsaba.jozsa@analog.com)
********************************************************************************
 * Copyright 2024-2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include "no_os_init.h"
#include "hal.h"
/* ************************************************************************** */

extern void enter_mmode(void *);

/**
 * @brief Initialize the risc-v rx
 */
static void risc_v_rx_init()
{
	trap_init();
	plic_init();
	clint_init();
}

/**
 * @brief Initialize the risc-v rx and run in machine mode
 */
__attribute__((weak)) int no_os_init(void)
{
	enter_mmode(risc_v_rx_init);
	return 0;
}
