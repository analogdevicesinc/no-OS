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
// Description:       AXI clock generator
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#include "axi_clkgen.h"

// **********************************************************************************
// **********************************************************************************

int32_t axi_clkgen_status(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t *data) {

    return(AXI_REG_READ(a_device, peripheral_id, AXI_CLKGEN_STATUS_ADDR, data));
}

int32_t axi_clkgen_config(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t clk_freq_hz, uint32_t *locked) {

    int32_t i;
    int32_t rm;
    int32_t rd;
    int32_t od;

    uint32_t diff;
    uint32_t data;
    uint32_t ref_mul;
    uint32_t ref_div;
    uint32_t out_div;
    uint32_t min_diff;
    uint32_t ratio_k;
    uint32_t div_ratio_k;
    uint32_t out_clk_freq_khz;
    uint32_t ref_clk_freq_khz;
    uint32_t out_clk_freq_hz;
    uint32_t vco_clk_freq_hz;

    // locked status

    *locked = 0;

    // device clock range (low automatically takes care of 128 limit)
 
    if ((clk_freq_hz < 4690000) || (clk_freq_hz > 250000000)) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: invalid frequency (%dHz), "
            "must be >= 4.69KHz and <= 250MHz.\n", __func__, clk_freq_hz);
        return(-1);
    }

    // fraction is 125, so get kHz

    out_clk_freq_khz = clk_freq_hz/1000;
    ref_clk_freq_khz = AXI_CLKGEN_REF_CLK_FREQ_HZ/1000;

    ratio_k = (ref_clk_freq_khz * 1000)/out_clk_freq_khz;

    min_diff = (uint32_t) -1;

    // pick a vco frequency between 600Mhz and 1440MHz, divisor less than 64
 
    ref_mul = 1;
    ref_div = 1;
    out_div = 1;

    for (rd = 1; rd < 64; rd = rd + 1) {
        for (rm = 1000; rm < 64000; rm = rm + 125) {
            for (od = 1000; od < 64000; od = od + 125) {
                vco_clk_freq_hz = (rm * ref_clk_freq_khz)/rd;
                if ((vco_clk_freq_hz > 600000000) && (vco_clk_freq_hz < 1440000000)) {
                    div_ratio_k = (od * rd * 1000)/rm;
                    diff = (div_ratio_k > ratio_k) ? (div_ratio_k - ratio_k) :
                        (ratio_k - div_ratio_k);
                    if (min_diff > diff) {
                        min_diff = diff;
                        ref_mul = rm;
                        ref_div = rd;
                        out_div = od;
                    }
                    if (min_diff == 0) {
                        rd = 64;
                        rm = 64000;
                        od = 64000;
                        break;
                    }
                }
            }
        }
    }

    vco_clk_freq_hz = (ref_mul * ref_clk_freq_khz)/ref_div;
    out_clk_freq_hz = (vco_clk_freq_hz/out_div)*1000;

    if (out_clk_freq_hz != clk_freq_hz) {
        AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_WARNING, "%s: clock frequency (%dHz), "
            "requested %dHz).\n", __func__, out_clk_freq_hz, clk_freq_hz);
    }

    data = 0;
    data = data | ((ref_div & AXI_CLKGEN_VCO_DIV_MASK) << AXI_CLKGEN_VCO_DIV_POS);
    data = data | (((ref_mul/1000) & AXI_CLKGEN_VCO_MUL_INT_MASK) << AXI_CLKGEN_VCO_MUL_INT_POS);
    data = data | (((ref_mul%1000) & AXI_CLKGEN_VCO_MUL_FRAC_MASK) << AXI_CLKGEN_VCO_MUL_FRAC_POS);
    AXI_REG_WRITE(a_device, peripheral_id, AXI_CLKGEN_VCO_ADDR, data);

    data = 0;
    data = data | (((out_div/1000) & AXI_CLKGEN_OUT_DIV_INT_MASK) << AXI_CLKGEN_OUT_DIV_INT_POS);
    data = data | (((out_div%1000) & AXI_CLKGEN_OUT_DIV_FRAC_MASK) << AXI_CLKGEN_OUT_DIV_FRAC_POS);
    AXI_REG_WRITE(a_device, peripheral_id, AXI_CLKGEN_OUT_ADDR, data);

    AXI_REG_WRITE(a_device, peripheral_id, AXI_CLKGEN_CTL_ADDR, 0x3);
    for (i = 0; i < 10000; i++) {
        AXI_REG_READ(a_device, peripheral_id, AXI_CLKGEN_CTL_ADDR, &data);
        if (data == 0x2) {
            AXI_REG_READ(a_device, peripheral_id, AXI_CLKGEN_STATUS_ADDR, &data);
            if (data == 0x1) {
                *locked = 1;
                return(0);
            }
        }
    }

    AXI_LOG_PRINTF(a_device, peripheral_id, AXI_LOG_ERROR, "%s: mmcm is NOT locked, "
        "reference clock may not be active.\n", __func__);
    return(-2);
}

// **********************************************************************************
// **********************************************************************************
