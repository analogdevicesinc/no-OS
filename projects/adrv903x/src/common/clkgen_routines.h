/***************************************************************************//**
 *   @file   clkgen_routines.h
 *   @brief  AXI clkgen setup for ADRV903X project.
 *   @author Analog Devices Inc.
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __CLKGEN_ROUTINES_H__
#define __CLKGEN_ROUTINES_H__

#include "no_os_print_log.h"
#include "clk_axi_clkgen.h"

int clkgen_setup(struct axi_clkgen **rx_clkgen, struct axi_clkgen **tx_clkgen);
int clkgen_remove(struct axi_clkgen *rx_clkgen, struct axi_clkgen *tx_clkgen);

#endif /* __CLKGEN_ROUTINES_H__ */
