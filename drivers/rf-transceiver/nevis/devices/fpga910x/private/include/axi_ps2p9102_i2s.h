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
// Description:       AXI I2S Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#ifndef AXI_PS2P9102_I2S_H
#define AXI_PS2P9102_I2S_H

#include "axi_common.h"
#include "axi_ps2p9102_top.h"

#ifdef __cplusplus
extern "C" {
#endif

// **********************************************************************************
// **********************************************************************************

/**
 * \brief transmit data set
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data register value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_i2s_tx_data(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint8_t data);

/**
 * \brief receive data get
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data register value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_i2s_rx_data(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint8_t *data);

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
