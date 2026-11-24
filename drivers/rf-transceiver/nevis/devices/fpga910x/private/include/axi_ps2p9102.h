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
// Description:       AXI PS2P9102 Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#ifndef AXI_PS2P9102_H
#define AXI_PS2P9102_H

#include "axi_common.h"
#include "axi_ps2p9102_top.h"
#include "axi_ps2p9102_i2s.h"
#include "axi_ps2p9102_qspi.h"
#include "axi_ps2p9102_sspi.h"
#include "axi_ps2p9102_uart.h"

#ifdef __cplusplus
extern "C" {
#endif

// **********************************************************************************
// **********************************************************************************

/**
 * \brief gpio to ps2p mappings
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] gpio array, assigned ps2p peripheral identifiers
 * \param[in] size of array (must be 20)
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_iosel(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t gpio[], uint32_t size);

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
