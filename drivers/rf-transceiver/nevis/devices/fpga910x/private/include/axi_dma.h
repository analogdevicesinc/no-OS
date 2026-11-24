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
// Description:       AXI DMA Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#ifndef AXI_DMA_H
#define AXI_DMA_H

#include "axi_common.h"
#include "axi_dma_top.h"
#include "axi_dma_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// **********************************************************************************
// **********************************************************************************

/**
 * \brief dma reset (last resort error recovery)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_dma_reset(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id);

/**
 * \brief dma start
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_dma_start(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id);

/**
 * \brief dma stop
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_dma_stop(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id);

/**
 * \brief dma busy
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current register value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_dma_busy(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id, uint32_t *data);

/**
 * \brief dma status
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data current register value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_dma_status(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id, uint32_t *data);

/**
 * \brief dma status clear
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_dma_status_clear(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id);

/**
 * \brief start address set 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data address (64bits)
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_dma_start_address_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id, uint64_t data);

/**
 * \brief start address get
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data address (64bits)
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_dma_start_address_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id, uint64_t *data);

/**
 * \brief base address set 
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] data address (64bits)
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_dma_base_address_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id, uint64_t data);

/**
 * \brief base address get
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] data address (64bits)
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_dma_base_address_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id, uint64_t *data);

/**
 * \brief dma configure
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] dma_param parameters
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_dma_config(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id, axi_dma_params_t *dma_param);

/**
 * \brief dma begin (graceful start)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_dma_begin(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id);

/**
 * \brief dma report (status and error conditions)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_dma_report(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id);

/**
 * \brief dma finish (waits to finish OR times out)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] timeout_us the time out count in microseconds
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_dma_finish(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id, uint32_t timeout_us);

/**
 * \brief dma flush (flush dma (no wait to finish) OR times out)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] timeout_us the time out count in microseconds
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_dma_flush(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id, uint32_t timeout_us);

/**
 * \brief dma run (config + begin + finish)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[in] dma_param parameters
 * \param[in] timeout_us the time out count in microseconds
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_dma_run(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id, axi_dma_params_t *dma_param, uint32_t timeout_us);

/**
 * \brief aximm performance monitoring
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] module_id of this instance
 * \param[out] duration read from register
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_dma_xfer_time_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t module_id, uint32_t *duration);

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
