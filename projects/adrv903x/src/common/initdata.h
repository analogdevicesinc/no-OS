/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
 * \file initdata.h
 * \brief Contains extern declarations for ADRV903X data structures 
 *        initializations
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef INITDATA_H_
#define INITDATA_H_

#include "adi_adrv903x_api.h"

ADI_API_EX adi_adrv903x_Version_t initStructApiVersion;
ADI_API_EX adi_adrv903x_CpuFwVersion_t initStructArmVersion;
ADI_API_EX adi_adrv903x_Version_t initStructStreamVersion;
ADI_API_EX adi_adrv903x_Init_t deviceInitStruct;
ADI_API_EX adi_adrv903x_PostMcsInit_t utilityInit;

#endif /* INITDATA_H_ */

