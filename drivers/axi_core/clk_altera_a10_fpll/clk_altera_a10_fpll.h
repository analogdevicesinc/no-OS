/***************************************************************************//**
 *   @file   clk_altera_a10_fpll.h
 *   @brief  Driver for the Altera FPLL.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef CLK_ALTERA_A10_FPLL_H_
#define CLK_ALTERA_A10_FPLL_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdbool.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
struct altera_a10_fpll {
	const char *name;
	uint32_t base;
	uint32_t parent_rate;
	bool initial_recalc;
};

struct altera_a10_fpll_init {
	const char *name;
	uint32_t base;
	uint32_t parent_rate;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t altera_a10_fpll_init(struct altera_a10_fpll **a10_fpll,
			     const struct altera_a10_fpll_init *init);
int32_t altera_a10_fpll_remove(struct altera_a10_fpll *fpll);
int32_t altera_a10_fpll_set_rate(struct altera_a10_fpll *fpll, uint32_t rate);
int32_t altera_a10_fpll_enable(struct altera_a10_fpll *fpll);
void altera_a10_fpll_disable(struct altera_a10_fpll *fpll);
#endif
