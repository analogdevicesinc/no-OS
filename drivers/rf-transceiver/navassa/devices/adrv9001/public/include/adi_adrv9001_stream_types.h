/**
 * \file
 * \brief Contains ADRV9001 API Stream data types
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2015 - 2019 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_STREAM_TYPES_H_
#define _ADI_ADRV9001_STREAM_TYPES_H_

/* System includes */
#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define ADI_ADRV9001_STREAM_BINARY_IMAGE_FILE_SIZE_BYTES	(32*1024)
#define ADRV9001_MAX_NUM_STREAM			5		/* Maximum number of stream processors */

/**
* \brief Data structure to hold stream processor version information
*/
typedef struct adi_adrv9001_StreamVersion
{
    uint8_t majorVer;
    uint8_t minorVer;
    uint8_t maintVer;
    uint8_t buildVer;
} adi_adrv9001_StreamVersion_t;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_STREAM_TYPES_H_ */
