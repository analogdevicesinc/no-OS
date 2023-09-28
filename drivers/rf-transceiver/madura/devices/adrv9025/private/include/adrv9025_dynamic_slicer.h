/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adrv9025_dynamic_slicer.h
* \brief Contains ADRV9025 private function prototypes for
*        adrv9025_dynamic_slicer.c that helps adi_adrv9025_dynamic_slicer.c
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADRV9025_DYNAMIC_SLICER_H_
#define _ADRV9025_DYNAMIC_SLICER_H_

#include "adi_adrv9025_dynamic_slicer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* \brief Sets up the dynamic slicer config
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param dynamicSlicerCfg Pointer to the dynamic slicer configuration structure
* \param enable Parameter to enable(1)/disable(0) dynamic slicer configuration
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxDynamicSlicerEnableSet(adi_adrv9025_Device_t*                device,
                                          adi_adrv9025_RxDynamicSlicerConfig_t* dynamicSlicerCfg,
                                          uint8_t                               enable);

/**
* \brief Performs range check on Rx dynamic slicer settings
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{device->devStateInfo}
* \dep_end
*
* \param device Structure pointer to the ADRV9025 device data structure
* \param rxDynamicSlicerConfig Array of Rx dynamic slicer config settings
* \param arraySize No. of elements in rxDynamicSlicerConfig array
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid parameter passed
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9025_RxDynamicSlicerConfigSetRangeCheck(adi_adrv9025_Device_t*               device,
                                                    adi_adrv9025_RxDynamicSlicerConfig_t rxDynamicSlicerConfig[],
                                                    uint8_t                              arraySize);

#ifdef __cplusplus
}
#endif

#endif
