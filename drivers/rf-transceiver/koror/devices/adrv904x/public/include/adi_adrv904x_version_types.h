/**
 * \file adi_adrv904x_version_types.h
 *
 * \brief   Contains ADRV904X Version data structures.
 *
 * ADRV904X API Version: 2.10.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADI_ADRV904X_VERSION_TYPES_H__
#define __ADI_ADRV904X_VERSION_TYPES_H__

typedef struct adi_adrv904x_Version
{
    uint32_t majorVer;              /*!< API Major Version number */
    uint32_t minorVer;              /*!< API Minor Version number */
    uint32_t maintenanceVer;        /*!< API Maintenance number */
    uint32_t buildVer;              /*!< API Build Version number */
} adi_adrv904x_Version_t;

typedef adi_adrv904x_Version_t adrv904x_Version_t;

#endif /* __ADI_ADRV904X_VERSION_TYPES_H__ */

