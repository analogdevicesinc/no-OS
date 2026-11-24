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
// Description:       AXI Internal Common Functions
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#ifndef AXI_UTILS_H
#define AXI_UTILS_H

#include "axi_common.h"

#ifdef __cplusplus
extern "C" {
#endif

// **********************************************************************************
// **********************************************************************************

/**
 * \brief register field write
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] addr register address
 * \param[in] pos field position
 * \param[in] mask field mask
 * \param[in] data field data
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_utils_field_write(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t addr, uint32_t pos, uint32_t mask, uint32_t data);

/**
 * \brief register field read
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] addr register address
 * \param[in] pos field position
 * \param[in] mask field mask
 * \param[out] data field data
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_utils_field_read(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t addr, uint32_t pos, uint32_t mask, uint32_t *data);

/**
 * \brief timer delay microseconds
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] addr register address
 * \param[in] data timer delay value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_utils_timer_delay_us(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t addr, uint32_t data);

/**
 * \brief clock frequency 16.16
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] addr register address
 * \param[out] data frequency
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_utils_count_freq_hz(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t addr, uint32_t *data);

/**
 * \brief clock domain transfer sync
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] cdc_state_offset of this instance
 * \param[in] timer_offset of this instance
 * \returns non zero value to indicate errors during configuration
 */

int32_t axi_utils_cdc_sync(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t cdc_state_offset, uint32_t timer_offset);

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
