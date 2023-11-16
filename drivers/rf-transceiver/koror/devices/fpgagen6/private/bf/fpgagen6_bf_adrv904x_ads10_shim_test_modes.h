/** 
 * \file fpgagen6_bf_adrv904x_ads10_shim_test_modes.h Automatically generated file with generator ver
 * 0.0.16.0.
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

#ifndef _FPGAGEN6_BF_ADRV904X_ADS10_SHIM_TEST_MODES_H_
#define _FPGAGEN6_BF_ADRV904X_ADS10_SHIM_TEST_MODES_H_

#include "adi_fpgagen6_types.h"
#include "adi_fpgagen6_error.h"
#include "../../private/bf/fpgagen6_bf_adrv904x_ads10_shim_test_modes_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif



/**
 * \brief All IP cores allow (if applicable) multiple instantiations in master-slave configuration. The master instance identifier is 0x0.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6Ads10ShimTestModesChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Ads10ShimTestModes_InstanceId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                      const fpgagen6_BfAdrvgen6Ads10ShimTestModesChanAddr_e baseAddr,
                                                                                      uint32_t* const bfValue);



/**
 * \brief Set this bit to one to loop transmit data streams to receive data streams in a near end loopback fashion.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6Ads10ShimTestModesChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Ads10ShimTestModes_NearendLoopback_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                           const fpgagen6_BfAdrvgen6Ads10ShimTestModesChanAddr_e baseAddr,
                                                                                           const uint8_t bfValue);


/**
 * \brief Set this bit to one to loop transmit data streams to receive data streams in a near end loopback fashion.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6Ads10ShimTestModesChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Ads10ShimTestModes_NearendLoopback_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                           const fpgagen6_BfAdrvgen6Ads10ShimTestModesChanAddr_e baseAddr,
                                                                                           uint8_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6Ads10ShimTestModesChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Ads10ShimTestModes_Scratch_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAdrvgen6Ads10ShimTestModesChanAddr_e baseAddr,
                                                                                   const uint32_t bfValue);


/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6Ads10ShimTestModesChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Ads10ShimTestModes_Scratch_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAdrvgen6Ads10ShimTestModesChanAddr_e baseAddr,
                                                                                   uint32_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6Ads10ShimTestModesChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Ads10ShimTestModes_Timer_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAdrvgen6Ads10ShimTestModesChanAddr_e baseAddr,
                                                                                 const uint32_t bfValue);


/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6Ads10ShimTestModesChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Ads10ShimTestModes_Timer_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                 const fpgagen6_BfAdrvgen6Ads10ShimTestModesChanAddr_e baseAddr,
                                                                                 uint32_t* const bfValue);



/**
 * \brief This is a local parameter in HDL and follows a MAJOR.MINOR.PATCH format, integers only.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6Ads10ShimTestModesChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Ads10ShimTestModes_Version_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                   const fpgagen6_BfAdrvgen6Ads10ShimTestModesChanAddr_e baseAddr,
                                                                                   uint32_t* const bfValue);
#endif // _FPGAGEN6_BF_ADRV904X_ADS10_SHIM_TEST_MODES_H_

