 /**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */
 
 /**
 * \file adrv904x_cpu_archive_types.h
 * 
 * \brief   Contains CPU archive type definitions
 *
 * \details Contains CPU archive type definitions
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef __ADRV904X_CPU_ARCHIVE_TYPES_H__
#define __ADRV904X_CPU_ARCHIVE_TYPES_H__

#include <stdint.h>
#include "adi_adrv904x_platform_pack.h"

/* Magic number to indicate ADRV904X CPU archive file */
#define ADRV904X_CPU_ARCHIVE_MAGIC_NUM  (0xAD100001U)

/* CPU archive format revision 1 identifier */
#define ADRV904X_CPU_ARCHIVE_REV_1       (0x00000001U)

/* Magic number to indicate ADRV904X CPU archive file */
#define ADRV904X_DFE_CPU_ARCHIVE_MAGIC_NUM   (0xAD100002U)

/* CPU archive format revision 1 identifier */
#define ADRV904X_DFE_CPU_ARCHIVE_REV_1       (0x00000001U)

/**
* \brief Data structure to hold CPU archive header
*/
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuArchiveHeader
{
    uint32_t magicNum;
    uint32_t formatRev;
    uint32_t xsum;
} adrv904x_CpuArchiveHeader_t;)
  
#endif /* __ADRV904X_CPU_ARCHIVE_TYPES_H__ */


