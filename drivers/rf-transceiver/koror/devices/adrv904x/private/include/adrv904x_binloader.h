/**
* Copyright 2015 - 2022 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adrv904x_binloader.h
 * \brief Contains ADRV904X binary loader related private function prototypes for
 *        adrv904x_binloader.c which helps adi_adrv904x_utilities.c
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef _ADRV904X_BINLOADER_H_
#define _ADRV904X_BINLOADER_H_

#include "adi_adrv904x_error.h"

/**
* \brief This function loads ADRV904X api version, firmware and stream versions,
* ADRV904X Init and PostMcsInit data structures from the Binary Image that also has the profile.
*
* This function reads the CPU Binary Image file from a specified location(e.g.SD card)
* and, if it contains more information than the profile, populates the structs adi_adrv904x_Version_t for 
* the api version, adi_adrv904x_CpuFwVersion_t for the firmware version, adi_adrv904x_Version_t for 
* the stream version, adi_adrv904x_Init_t init structure and adi_adrv904x_PostMcsInit_t postMcsInit structures.
* 
* If profile.bin contains these structures, this function must be called before 
* adi_adrv904x_PreMcsInit, adi_adrv904x_PreMcsInit_NonBroadcast and adi_adrv904x_PostMcsInit 
* as these functions require the structures to be already populated.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in,out] file Pointer to Binary file
* \param[out] apiVer Pointer to the ADRV904X api version data structure to be populated
* \param[out] fwVer Pointer to the ADRV904X firmware version data structure to be populated
* \param[out] streamVer Pointer to the ADRV904X stream version data structure to be populated
* \param[out] init Pointer to the ADRV904X Init data structure to be populated
* \param[out] postMcsInit Pointer to the ADRV904X PostMcsInit data structure to be populated
* \param[in] fileOffset Offset to be applied in the binary file to jump to structures of interest
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_LoadBinFile(adi_adrv904x_Device_t* const       device, 
                                                      FILE* const                        file,
                                                      adi_adrv904x_Version_t* const      apiVer, 
                                                      adi_adrv904x_CpuFwVersion_t* const fwVer, 
                                                      adi_adrv904x_Version_t* const      streamVer, 
                                                      adi_adrv904x_Init_t* const         initStruct, 
                                                      adi_adrv904x_PostMcsInit_t* const  postMcsInitStruct, 
                                                      const uint32_t                     fileOffset);

#endif
