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
// Description:       AXI board gpio
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#ifndef AXI_BDGPIO_H
#define AXI_BDGPIO_H

#include "axi_common.h"
#include "axi_bdgpio_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

// **********************************************************************************
// **********************************************************************************

/**
 * \brief gpio data (all bits)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data read value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_bdgpio_data_all(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t *data);

/**
 * \brief gpio data (position)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] pos of this pin
 * \param[out] data read value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_bdgpio_data(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t pos, uint32_t *data);

/**
 * \brief gpio data set (all bits)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data write value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_bdgpio_data_all_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t data);

/**
 * \brief gpio data set (position)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] pos of this pin
 * \param[in] data write value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_bdgpio_data_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t pos, uint32_t data);

/**
 * \brief gpio enable set (all bits)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data write value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_bdgpio_enable_all_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t data);


/**
 * \brief gpio enable set (position)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] pos of this pin
 * \param[in] data write value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_bdgpio_enable_set(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t pos, uint32_t data);

/**
 * \brief gpio enable get (all bits)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data read value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_bdgpio_enable_all_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t *data);

/**
 * \brief gpio enable get (position)
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] pos of this pin
 * \param[out] data read value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_bdgpio_enable_get(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint32_t pos, uint32_t *data);

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
