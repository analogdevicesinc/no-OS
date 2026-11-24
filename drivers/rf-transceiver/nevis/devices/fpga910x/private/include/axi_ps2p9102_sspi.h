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
// Description:       AXI SPI Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#ifndef AXI_PS2P9102_SSPI_H
#define AXI_PS2P9102_SSPI_H

#include "axi_common.h"
#include "axi_ps2p9102_top.h"
#include "axi_ps2p9102_sspi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// **********************************************************************************
// **********************************************************************************

/**
 * \brief config, common parameters
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] sspi_param parameters
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_sspi_config(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    axi_ps2p9102_sspi_params_t *sspi_param);

/**
 * \brief mosi data
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data register value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_sspi_mosi_data(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint8_t *data);

/**
 * \brief miso data
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data register value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_sspi_miso_data(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint8_t data);

/**
 * \brief data transfer
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] mosi_data received bytes
 * \param[in] miso_data transmitted bytes
 * \param[in] count number of bytes
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_sspi_xfer(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint8_t mosi_data[], const uint8_t miso_data[], uint32_t count);

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
