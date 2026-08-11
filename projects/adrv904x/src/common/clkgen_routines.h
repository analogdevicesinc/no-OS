/***************************************************************************//**
 *   @file   adrv904x/src/common/clkgen_routines.h
 *   @brief  Defines clkgen init and settings to be used by adrv904x examples.
 *   @author GMois (george.mois@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __CLKGEN_ROUTINES_H__
#define __CLKGEN_ROUTINES_H__

#include "platform_includes.h"
#include "no_os_print_log.h"
#include "clk_axi_clkgen.h"
#include "adrv904x.h"

/* Initialize clk_axi_clkgen components. */
int clkgen_setup(struct axi_clkgen **rx_clkgen, struct axi_clkgen **tx_clkgen,
		 struct axi_clkgen **orx_clkgen, uint8_t jesd204c_en);
/* Remove clk_axi_clkgen components */
int clkgen_remove(struct axi_clkgen *rx_clkgen, struct axi_clkgen *tx_clkgen,
		  struct axi_clkgen *orx_clkgen);

#endif /* __CLKGEN_ROUTINES_H__ */