/***************************************************************************//**
 *   @file   clk_axi_clkgen.h
 *   @brief  Driver for the Analog Devices AXI CLKGEN.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef CLK_AXI_CLKGEN_H_
#define CLK_AXI_CLKGEN_H_

#include <stdint.h>

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

int32_t axi_clkgen_set_rate(struct axi_clkgen *clkgen, uint32_t rate);
int32_t axi_clkgen_get_rate(struct axi_clkgen *clkgen, uint32_t *rate);
int32_t axi_clkgen_init(struct axi_clkgen **clk,
			const struct axi_clkgen_init *init);
int32_t axi_clkgen_remove(struct axi_clkgen *clkgen);

#endif
