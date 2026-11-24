/**
 * \file
 * \brief Contains ADRV910X API configuration and run-time type definitions
 *
 * Copyright (c) 2022 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_FPGA910X_TYPES_H_
#define _ADI_FPGA910X_TYPES_H_

#include "adi_adrv910x_common.h"
#include "adi_common_types.h"
#include "adi_common.h"

/**
 * \brief FPGA version number
 */
typedef struct adi_fpga910x_Version
{
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
} adi_fpga910x_Version_t;

#ifndef CLIENT_IGNORE
/**
 * \brief Data structure to hold FPGA910X device instance settings
 */
typedef struct adi_fpga910x_Device
{
    adi_common_Device_t common; /*!< Common layer structure */
} adi_fpga910x_Device_t;
#endif

#endif /* _ADI_FPGA910X_TYPES_H_ */
