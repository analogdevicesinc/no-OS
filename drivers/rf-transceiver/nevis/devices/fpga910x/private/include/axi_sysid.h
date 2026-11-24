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
// Description:       AXI SYSID Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#ifndef AXI_SYSID_H
#define AXI_SYSID_H

#include "axi_common.h"
#include "axi_sysid_top.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief gpio input get, read gpio input (system specific)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] index of the gpio pin
 * \param[out] data of this gpio pin
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_gpio_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t index, uint32_t *data);

/**
 * \brief gpio output set, write to gpio output (system specific)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] index of the gpio pin
 * \param[in] data register value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_gpio_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t index, uint32_t data);

/**
 * \brief status input get, read status input (system specific)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] index of the status pin
 * \param[out] data of this status pin
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_status_data_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t index, uint32_t *data);

/**
 * \brief control output set, write to control output (system specific)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] index of the control pin
 * \param[in] data register value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_control_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t index, uint32_t data);

/**
 * \brief pwm output set, write to pwm output (system specific)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] index of the pwm pin
 * \param[in] data register value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_pwm_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t index, uint32_t data);

/**
 * \brief pwm config
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] index of the pwm pin
 * \param[in] on_time_ms of this pwm pin in milli-seconds
 * \param[in] off_time_ms of this pwm pin in milli-seconds
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_pwm_config(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t index, uint32_t on_time_ms, uint32_t off_time_ms);

/**
 * \brief read platform parameters
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data register value
 * \param[in] size of data array
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_sys_params_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t data[], uint32_t size);

/**
 * \brief read platform build info (HH-MM-SS, MM-DD-YYYY)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data register value
 * \param[in] size of data array
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_sys_build_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t data[], uint32_t size);

/**
 * \brief read platform string (0)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data register value
 * \param[in] size of data array
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_sys_info_0_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t data[], uint32_t size);

/**
 * \brief read platform string (1)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data register value
 * \param[in] size of data array
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_sysid_sys_info_1_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t data[], uint32_t size);

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
