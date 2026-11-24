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

#ifndef AXI_CLKGEN_H
#define AXI_CLKGEN_H

#include "axi_common.h"
#include "axi_clkgen_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

// **********************************************************************************
// **********************************************************************************

/**
 * \brief status (locked)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data status read value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_clkgen_status(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t *data);

/**
 * \brief config (desired clock frequency)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] clk_freq_hz frequency of board clock in Hz
 * \param[out] locked status of mmcm/pll value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_clkgen_config(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t clk_freq_hz, uint32_t *locked);

// **********************************************************************************
// **********************************************************************************

#ifdef __cplusplus
}
#endif

// **********************************************************************************
// **********************************************************************************

#endif

// **********************************************************************************
// **********************************************************************************
