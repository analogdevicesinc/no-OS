/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file initdata.h
 * \brief Contains extern declarations for ADRV904X data structures 
 *        initializations
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef INITDATA_H_
#define INITDATA_H_

#include "adi_adrv904x_api.h"

ADI_API_EX adi_adrv904x_Version_t initStructApiVersion;
ADI_API_EX adi_adrv904x_CpuFwVersion_t initStructArmVersion;
ADI_API_EX adi_adrv904x_Version_t initStructStreamVersion;
ADI_API_EX adi_adrv904x_Init_t deviceInitStruct;
ADI_API_EX adi_adrv904x_PostMcsInit_t utilityInit;

#endif /* INITDATA_H_ */
