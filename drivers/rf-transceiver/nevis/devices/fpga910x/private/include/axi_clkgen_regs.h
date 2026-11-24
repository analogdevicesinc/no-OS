// **********************************************************************************
// **********************************************************************************
// ----------------------------------------------------------------------------------
// ################
// ##   ###########   Analog Devices Inc.
// ##      ########
// ##         #####   Copyright (c) 2019 Analog Devices Inc. All rights reserved.
// ##            ##   This file is the confidential and proprietary property of ADI.
// ##         #####   Possession or use of this file requires a written license.
// ##      ########   The licensing information may be found at: www.analog.com
// ##   ###########
// ################
// ----------------------------------------------------------------------------------
// Description:       AXI clock generator register defines
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#ifndef AXI_CLKGEN_REGS_H
#define AXI_CLKGEN_REGS_H

#include "axi_common.h"

#define AXI_CLKGEN_REF_CLK_FREQ_HZ 200000000
#define AXI_CLKGEN_STATUS_ADDR 0x004
#define AXI_CLKGEN_VCO_ADDR 0x200
#define AXI_CLKGEN_VCO_DIV_POS 0
#define AXI_CLKGEN_VCO_DIV_MASK 0xff
#define AXI_CLKGEN_VCO_MUL_INT_POS 8
#define AXI_CLKGEN_VCO_MUL_INT_MASK 0xff
#define AXI_CLKGEN_VCO_MUL_FRAC_POS 16
#define AXI_CLKGEN_VCO_MUL_FRAC_MASK 0x3ff
#define AXI_CLKGEN_OUT_ADDR 0x208
#define AXI_CLKGEN_OUT_DIV_INT_POS 0
#define AXI_CLKGEN_OUT_DIV_INT_MASK 0xff
#define AXI_CLKGEN_OUT_DIV_FRAC_POS 8
#define AXI_CLKGEN_OUT_DIV_FRAC_MASK 0x3ff
#define AXI_CLKGEN_CTL_ADDR 0x25c

#endif

// **********************************************************************************
// **********************************************************************************
