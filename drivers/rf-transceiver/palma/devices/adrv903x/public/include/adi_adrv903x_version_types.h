/**
 * \file adi_adrv903x_version_types.h
 *
 * \brief   Contains ADRV903X Version data structures.
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADI_ADRV903X_VERSION_TYPES_H__
#define __ADI_ADRV903X_VERSION_TYPES_H__

typedef struct adi_adrv903x_Version
{
    uint32_t majorVer;              /*!< API Major Version number */
    uint32_t minorVer;              /*!< API Minor Version number */
    uint32_t maintenanceVer;        /*!< API Maintenance number */
    uint32_t buildVer;              /*!< API Build Version number */
} adi_adrv903x_Version_t;

typedef adi_adrv903x_Version_t adrv903x_Version_t;

#endif /* __ADI_ADRV903X_VERSION_TYPES_H__ */

