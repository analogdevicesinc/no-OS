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

#ifndef AXI_MSPI_H
#define AXI_MSPI_H

#include "axi_common.h"
#include "axi_mspi_top.h"
#include "axi_mspi_types.h"

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
 * \param[in] mspi_param parameters
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_config(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    axi_mspi_params_t *mspi_param);

/**
 * \brief config, dma
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] num_of_bytes per transfer
 * \param[in] num_of_frames per transfer
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_dma_config(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t num_of_bytes, uint32_t num_of_frames);

/**
 * \brief config, fifo
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_fifo_config(AXI_DEVICE_T *a_device, uint32_t peripheral_id);

/**
 * \brief fifo mode spi fast transfer
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] mosi_data byte array
 * \param[out] miso_data byte array
 * \param[in] max_16 length of array, must be less than 16 (still 32bit for erpc)
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_fifo_fast_xfer(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    const uint8_t mosi_data[], uint8_t miso_data[], uint32_t max_16);

/**
 * \brief fifo mode spi transfer
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] mosi_data byte array
 * \param[out] miso_data byte array
 * \param[in] count length of array
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_fifo_xfer(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    const uint8_t mosi_data[], uint8_t miso_data[], uint32_t count);
 
/**
 * \brief fifo mode spi mosi transfer (simplex mode, mosi only)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] mosi_data byte array
 * \param[in] count length of array
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_fifo_mosi_xfer(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    const uint8_t mosi_data[], uint32_t count);
 
/**
 * \brief slave select (active high version)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data current value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_slave_select_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t *data);

/**
 * \brief slave select (active high version)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data desired value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_slave_select_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t data);

/**
 * \brief dma finish (dma mode)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] timeout_us desired timeout value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_dma_finish(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t timeout_us);

/**
 * \brief finish (fifo mode)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] timeout_us desired timeout value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_mspi_finish(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t timeout_us);

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
