/**
 * \file adi_adrv904x_version_types.h
 *
 * \brief   Contains ADRV904X Version data structures.
 *
 * ADRV904X API Version: 2.15.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
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

