/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adi_adrv9025_dynamic_slicer_types.h
* \brief Contains ADRV9025 dynamic slicer data types
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_DYNAMIC_SLICER_TYPES_H_
#define _ADI_ADRV9025_DYNAMIC_SLICER_TYPES_H_

#include <stdint.h>
#include "adi_adrv9025_types.h"
#include "adi_adrv9025_rx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* \brief Data structure to hold ADRV9025 Rx dynamic slicer configuration. In the dynamic slicer mode
*        the slicer control word is embedded in the JESD data only when there is a gain change / slicer control word update.
*/
typedef struct adi_adrv9025_RxDynamicSlicerConfig
{
    uint32_t rxChannelMask;                                   /*!< Mask consisting of ORed Rx Channels for which the dynamic slicer settings are applied */
    uint8_t dynamicSlicerEnable;                              /*!< Enables the dynamic slicer mode of operation. Slicer data is embedded in the JESD data whenever there is a gain change */
    uint8_t dynamicSlicerAgcSyncEnable;                       /*!< This enables a flag in the dynamic slicer data which goes high whenever there is a gain change */
    uint32_t dynamicSlicerSyncHeader;                         /*!< A 32 bit unique identification pattern preceding the slicer control word embedded in the JESD data */
    adi_adrv9025_RxIntSampleResolution_e intSampleResolution; /*!< Resolution of Rx Data  */
    adi_adrv9025_RxSlicerEmbeddedBits_e  intEmbeddedBits;     /*!< No. of embedded slicer bits in Rx Data. For dynamic slicer operation embedding of 1 bit at LSB / embedding 1 bit at MSB are the only modes allowed */
    adi_adrv9025_IntSlicerStepSizes_e intSlicerStepSize;      /*!< Step size to be used by the internal slicer module. Valid step sizes are 1dB, 2dB, 3dB, 4dB, 6dB, 8dB */
} adi_adrv9025_RxDynamicSlicerConfig_t;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_DYNAMIC_SLICER_TYPES_H_ */
