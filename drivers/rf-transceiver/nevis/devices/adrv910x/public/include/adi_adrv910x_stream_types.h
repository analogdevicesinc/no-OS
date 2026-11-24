/**
 * \file
 * \brief Contains ADRV910X API Stream data types
 *
 * Copyright (c) 2015 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_ADRV910X_STREAM_TYPES_H_
#define _ADI_ADRV910X_STREAM_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#define ADI_ADRV910X_STREAM_BINARY_IMAGE_FILE_SIZE_BYTES (32*1024)
/** \brief Maximum number of stream processors */
#define ADRV910X_MAX_NUM_STREAM 5

/**
 * \brief Data structure to hold stream processor version information
 */
typedef struct adi_adrv910x_StreamVersion
{
    uint8_t majorVer;
    uint8_t minorVer;
    uint8_t maintVer;
    uint8_t buildVer;
} adi_adrv910x_StreamVersion_t;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV910X_STREAM_TYPES_H_ */
