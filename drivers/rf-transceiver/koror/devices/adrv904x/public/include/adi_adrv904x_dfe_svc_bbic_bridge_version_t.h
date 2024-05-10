/**
 * Copyright 2020 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adi_adrv904x_dfe_svc_bbic_bridge_version_t.h
 * \brief Contains data types for DFE SDK version handling service
 *
 * DFE SDK API Version: 2.10.0.4
 */

#ifndef __ADI_ADRV904X_DFE_SVC_BBIC_BRIDGE_VERSION_T_H__
#define __ADI_ADRV904X_DFE_SVC_BBIC_BRIDGE_VERSION_T_H__

#include <stdint.h>
#include "adi_adrv904x_platform_pack.h"

/**
 * \brief Version number and type structure for SDK and application
 */
ADI_ADRV904X_PACK_START
typedef struct
{
    uint32_t majorRev;            /*!< Major version number */
    uint32_t minorRev;            /*!< Minor version number */
    uint32_t maintRev;            /*!< Maintain version number */
    uint32_t buildNum;            /*!< Build version number */
    uint32_t buildType;           /*!< Build type (0, release; 1, debug) */
} adi_adrv904x_DfeSvcBbicBridgeVersion_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Revision information structure for SDK
 */
    ADI_ADRV904X_PACK_START
typedef struct
{
    uint8_t gitHash[44];           /*!< SDK code repo git commit */
    uint8_t branchInfo[80];        /*!< SDK code repo branch information */
} adrv904x_DfeSvcBbicBridgeSdkRevision_t;
ADI_ADRV904X_PACK_FINISH

#endif /* __ADI_ADRV904X_DFE_SVC_BBIC_BRIDGE_VERSION_T_H__ */



