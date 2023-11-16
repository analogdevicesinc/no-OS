/** 
 * \file fpgagen6_bf_project_version.h Automatically generated file with generator ver 0.0.16.0.
 * 
 * \brief Contains BitField functions to support FPGAGEN6 transceiver device.
 * 
 * FPGAGEN6 BITFIELD VERSION: 0.0.0.11
 * 
 * Disclaimer Legal Disclaimer
 * 
 * Copyright 2015 - 2023 Analog Devices Inc.
 * 
 * Released under the FPGAGEN6 API license, for more information see the "LICENSE.PDF" file in this zip file.
 */

#ifndef _FPGAGEN6_BF_PROJECT_VERSION_H_
#define _FPGAGEN6_BF_PROJECT_VERSION_H_

#include "adi_fpgagen6_types.h"
#include "adi_fpgagen6_error.h"
#include "../../private/bf/fpgagen6_bf_project_version_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif



/**
 * \brief The Device ID is unique among all of the TPG FPGA projects.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfProjectVersionChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_ProjectVersion_DeviceId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfProjectVersionChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);



/**
 * \brief Changes to the Fix Version indicate that changes have been made to the FPGA design that do not require any changes to software.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfProjectVersionChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_ProjectVersion_FixVersion_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfProjectVersionChanAddr_e baseAddr,
                                                                          uint16_t* const bfValue);



/**
 * \brief Changes in the Major Version indicate significant changes to the FPGA design that would result in a incompatibility with existing software.  Software would need to be modified to support existing functionality.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfProjectVersionChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_ProjectVersion_MajorVersion_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfProjectVersionChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);



/**
 * \brief Changes in Minor Version indicates that new functionality has been added or changes have been made that are backward compatible to existing software.  New functionality may require software changes to be fully utilized.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfProjectVersionChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_ProjectVersion_MinorVersion_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfProjectVersionChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);
#endif // _FPGAGEN6_BF_PROJECT_VERSION_H_

