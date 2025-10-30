 /**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */
 
 /**
 * \file adrv903x_cpu_archive_types.h
 * 
 * \brief   Contains CPU archive type definitions
 *
 * \details Contains CPU archive type definitions
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef __ADRV903X_CPU_ARCHIVE_TYPES_H__
#define __ADRV903X_CPU_ARCHIVE_TYPES_H__

#include "adi_adrv903x_platform_pack.h"

/* Magic number to indicate ADRV903X CPU archive file */
#define ADRV903X_CPU_ARCHIVE_MAGIC_NUM  (0xAD100001U)

/* CPU archive format revision 1 identifier */
#define ADRV903X_CPU_ARCHIVE_REV_1       (0x00000001U)

/**
* \brief Data structure to hold CPU archive header
*/
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuArchiveHeader
{
    uint32_t magicNum;
    uint32_t formatRev;
    uint32_t xsum;
} adrv903x_CpuArchiveHeader_t;)
  
#endif /* __ADRV903X_CPU_ARCHIVE_TYPES_H__ */


