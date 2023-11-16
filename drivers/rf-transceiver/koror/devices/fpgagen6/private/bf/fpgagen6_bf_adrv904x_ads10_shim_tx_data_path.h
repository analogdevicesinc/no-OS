/** 
 * \file fpgagen6_bf_adrv904x_ads10_shim_tx_data_path.h Automatically generated file with generator ver
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

#ifndef _FPGAGEN6_BF_ADRV904X_ADS10_SHIM_TX_DATA_PATH_H_
#define _FPGAGEN6_BF_ADRV904X_ADS10_SHIM_TX_DATA_PATH_H_

#include "adi_fpgagen6_types.h"
#include "adi_fpgagen6_error.h"
#include "../../private/bf/fpgagen6_bf_adrv904x_ads10_shim_tx_data_path_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6Ads10ShimTxDataPathChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Ads10ShimTxDataPath_InstanceId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                       const fpgagen6_BfAdrvgen6Ads10ShimTxDataPathChanAddr_e baseAddr,
                                                                                       uint8_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6Ads10ShimTxDataPathChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Ads10ShimTxDataPath_Scratch_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAdrvgen6Ads10ShimTxDataPathChanAddr_e baseAddr,
                                                                                    const uint32_t bfValue);


/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6Ads10ShimTxDataPathChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Ads10ShimTxDataPath_Scratch_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAdrvgen6Ads10ShimTxDataPathChanAddr_e baseAddr,
                                                                                    uint32_t* const bfValue);



/**
 * \brief This is self-clearing and resets the Data Path FIFOs in the Shim layer.  This should be performed before initiating all transmissions.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6Ads10ShimTxDataPathChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Ads10ShimTxDataPath_SoftReset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                                      const fpgagen6_BfAdrvgen6Ads10ShimTxDataPathChanAddr_e baseAddr,
                                                                                      const uint8_t bfValue);


/**
 * \brief This is self-clearing and resets the Data Path FIFOs in the Shim layer.  This should be performed before initiating all transmissions.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6Ads10ShimTxDataPathChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Ads10ShimTxDataPath_SoftReset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                      const fpgagen6_BfAdrvgen6Ads10ShimTxDataPathChanAddr_e baseAddr,
                                                                                      uint8_t* const bfValue);



/**
 * \brief This is a local parameter in HDL and follows a MAJOR.MINOR.PATCH format, integers only.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAdrvgen6Ads10ShimTxDataPathChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_Adrvgen6Ads10ShimTxDataPath_Version_BfGet(adi_fpgagen6_Device_t* const device,
                                                                                    const fpgagen6_BfAdrvgen6Ads10ShimTxDataPathChanAddr_e baseAddr,
                                                                                    uint32_t* const bfValue);
#endif // _FPGAGEN6_BF_ADRV904X_ADS10_SHIM_TX_DATA_PATH_H_

