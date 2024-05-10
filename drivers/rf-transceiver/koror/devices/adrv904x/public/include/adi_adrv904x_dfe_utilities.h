/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_dfe_utilities.h
* \brief Contains top level ADRV904X related function prototypes for
*        adi_adrv904x_dfe_utilities.c
*
* ADRV904X API Version: 2.10.0.4
*/

#ifndef _ADI_ADRV904X_DFE_UTILITIES_H_
#define _ADI_ADRV904X_DFE_UTILITIES_H_

#include "adi_adrv904x_dfe_utilities_types.h"
#include "adi_adrv904x_error.h"
#include "adi_adrv904x_dfe_svc_radio_ctrl_sequencer_t.h"

/**
* \brief Reads back the DFE SDK lib version loaded into the DFE processor
*
* This function reads the DFE processor SDK version number, Major, Minor, Maintenance, 
* Build number and the binary type after DFE initial.
*
* \pre This function may be called after the CPU binary have been initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] dfeSdkVersion dfeSdkVersion adi_adrv904x_CpuFwVersion_t pointer to DFE SDK version structure
*
* \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeSdkVersionGet(adi_adrv904x_Device_t* const         device,
                                                               adi_adrv904x_CpuFwVersion_t* const   dfeSdkVersion);

/**
* \brief Reads back the DFE processor application version loaded into the DFE processor
*
* This function Reads the DFE processor application version number, Major, Minor, 
* Maintenance, Build number and the binary type.
*
* \pre This function may be called after the CPU binary have been initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] dfeAppVersion dfeSdkVersion adi_adrv904x_CpuFwVersion_t pointer to DFE processor application version structure
*
* \retval ADI_ADRV904X_ERR_ACT_NONE Function completed successfully, no action required
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeAppVersionGet(adi_adrv904x_Device_t* const         device,
                                                               adi_adrv904x_CpuFwVersion_t* const   dfeAppVersion);

/**
* \brief Function to get Radio Sequencer Error Information
*
* \pre This function may be called after the RS binary have been initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] errorInfo Pointer to readback RS error information
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerErrorInfoGet(adi_adrv904x_Device_t* const                            device,
                                                                         adi_adrv904x_DfeSvcRadioCtrlSequencerErrorInfo_t* const errorInfo);

/**
* \brief Function to clear Radio Sequencer's SSB_SYNC phase error bit
*
* \pre This function should be called by BBIC when it receives an RS SSB_SYNC phase error interupt.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerSsbSyncPhaseErrorClear(adi_adrv904x_Device_t* const  device);

/**
* \brief Function to get DFE Logtrace buffer Information
*
* \pre This function may be called after the logtrace buffer have been initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] logTrace correct sequence character buffer data
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeLogTraceBufferGet(adi_adrv904x_Device_t* const             device,
                                                                   adi_adrv904x_DfeLogTraceBuffer_t* const  logTrace);

/**
* \brief Reads the active capture type selected with Tx to Orx mapping
*
* Based on the tx to orx mapping table used in stream settings, user can select either DPD or VSWR
* captures in RCI mode. This function can be used to retrieve the active capture type configured on the HW.
* When radio sequencer is being used to change tx to orx mapping, return value from this API may not be
* reliable.
*
* \pre This function may be called after the Stream processors have been initialized and loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] orx0CaptureType Capture type for Orx0 channel
* \param[out] orx1CaptureType Capture type for Orx0 channel
* 
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCaptureTypeGet(adi_adrv904x_Device_t* const            device,
                                                                adi_adrv904x_DfeCaptureType_e* const    orx0CaptureType,
                                                                adi_adrv904x_DfeCaptureType_e* const    orx1CaptureType);


#endif /* _ADI_ADRV904X_DFE_UTILITIES_H_ */
