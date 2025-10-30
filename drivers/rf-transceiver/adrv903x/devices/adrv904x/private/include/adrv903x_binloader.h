/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
 * \file adrv903x_binloader.h
 * \brief Contains ADRV903X binary loader related private function prototypes for
 *        adrv903x_binloader.c which helps adi_adrv903x_utilities.c
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef _ADRV903X_BINLOADER_H_
#define _ADRV903X_BINLOADER_H_

#include "adi_adrv903x_error.h"

/**
* \brief This function loads ADRV903X api version, firmware and stream versions,
* ADRV903X Init and PostMcsInit data structures from the Binary Image that also has the profile.
*
* This function reads the CPU Binary Image file from a specified location(e.g.SD card)
* and, if it contains more information than the profile, populates the structs adi_adrv903x_Version_t for 
* the api version, adi_adrv903x_CpuFwVersion_t for the firmware version, adi_adrv903x_Version_t for 
* the stream version, adi_adrv903x_Init_t init structure and adi_adrv903x_PostMcsInit_t postMcsInit structures.
* 
* If profile.bin contains these structures, this function must be called before 
* adi_adrv903x_PreMcsInit, adi_adrv903x_PreMcsInit_NonBroadcast and adi_adrv903x_PostMcsInit 
* as these functions require the structures to be already populated.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV903X device data structure containing settings
* \param[in,out] file Pointer to Binary file
* \param[out] apiVer Pointer to the ADRV903X api version data structure to be populated
* \param[out] fwVer Pointer to the ADRV903X firmware version data structure to be populated
* \param[out] streamVer Pointer to the ADRV903X stream version data structure to be populated
* \param[out] init Pointer to the ADRV903X Init data structure to be populated
* \param[out] postMcsInit Pointer to the ADRV903X PostMcsInit data structure to be populated
* \param[in] fileOffset Offset to be applied in the binary file to jump to structures of interest
*
* \retval adi_adrv903x_ErrAction_e - ADI_ADRV903X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv903x_ErrAction_e adrv903x_LoadBinFile(adi_adrv903x_Device_t* const       device, 
                                                      FILE* const                        file,
                                                      adi_adrv903x_Version_t* const      apiVer, 
                                                      adi_adrv903x_CpuFwVersion_t* const fwVer, 
                                                      adi_adrv903x_Version_t* const      streamVer, 
                                                      adi_adrv903x_Init_t* const         initStruct, 
                                                      adi_adrv903x_PostMcsInit_t* const  postMcsInitStruct, 
                                                      const uint32_t                     fileOffset);

#endif
