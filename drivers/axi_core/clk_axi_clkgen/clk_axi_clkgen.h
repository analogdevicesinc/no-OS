/***************************************************************************//**
 *   @file   clk_axi_clkgen.h
 *   @brief  Driver for the Analog Devices AXI CLKGEN.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef CLK_AXI_CLKGEN_H_
#define CLK_AXI_CLKGEN_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
struct axi_clkgen {
	const char	*name;
	uint32_t	base;
	uint32_t	parent_rate;
};

struct axi_clkgen_init {
	const char	*name;
	uint32_t	base;
	uint32_t	parent_rate;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t axi_clkgen_set_rate(struct axi_clkgen *clkgen, uint32_t rate);
int32_t axi_clkgen_get_rate(struct axi_clkgen *clkgen, uint32_t *rate);
int32_t axi_clkgen_init(struct axi_clkgen **clk,
			const struct axi_clkgen_init *init);
int32_t axi_clkgen_remove(struct axi_clkgen *clkgen);

#endif
