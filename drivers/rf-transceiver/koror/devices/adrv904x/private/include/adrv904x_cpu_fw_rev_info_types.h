/**
 * \file adrv904x_cpu_fw_rev_info_types.h
 *
 * \brief   Contains device revision information.
 *
 * \details Contains device revision information.
 *
 * ADRV904X API Version: 2.10.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV903x API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_FW_REV_INFO_TYPES_H__
#define __ADRV904X_CPU_FW_REV_INFO_TYPES_H__

#include "adi_adrv904x_platform_pack.h"

/*
 ********************************************************************************************************
 * Declarations used below to reference global data
 ********************************************************************************************************
 */
ADI_ADRV904X_PACK_START
typedef struct adrv904x_DeviceRevisionData
{
    uint32_t majorRev;
    uint32_t minorRev;
    uint32_t maintRev;
    uint32_t buildNum;
    uint32_t buildType;
    uint8_t  gitHash[41];
    uint8_t  branchInfo[80];
    uint32_t reserved[12];
} adrv904x_DeviceRevisionData_t;
ADI_ADRV904X_PACK_FINISH

#endif /* __ADRV904X_CPU_FW_REV_INFO_TYPES_H__*/


