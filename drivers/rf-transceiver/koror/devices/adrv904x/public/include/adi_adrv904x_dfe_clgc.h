/**
* Copyright 2015 - 2022 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_dfe_clgc.h
*
* \brief Contains ADRV904X CLGC function prototypes for
*    adi_adrv904x_dfe_clgc.c
*
* ADRV904X API Version: 2.10.0.4
*/


#ifndef _ADI_ADRV904X_DFE_CLGC_H_
#define _ADI_ADRV904X_DFE_CLGC_H_

#include "adi_adrv904x_dfe_cal_clgc_types.h"
#include "adi_adrv904x_dfe_framework_tracking_cal_t.h"
#include "adi_adrv904x_error.h"
#include "adi_common_error_types.h"
#include "adi_adrv904x_tx_types.h"

/** 
* \brief Configures the CLGC Capture Config for the requested Tx channel(s)
*
* This function configures a capture for the CLGC in the ADRV904x device. 
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization but before enabling the DPD tracking calibration.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] clgcTxChannelMask - One or multiple Tx channel for which CLGC configuration is required
* \param[in] clgcCaptureCfg - Pointer to the CLGC capture config structure
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ClgcCaptureConfigSet(adi_adrv904x_Device_t* const                         device,
                                                                   const uint32_t                                       clgcTxChannelMask,
                                                                   const adi_adrv904x_DfeAppCalClgcCaptureCfg_t* const  clgcCaptureCfg);

/**
* \brief Retrieves the CLGC Capture Config for the requested Tx channel from the device
*
* This function retrieves the CLGC capture config in the ADRV904x device.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] clgcTxChannelSel - Tx channel selection to read CLGC capture configuration. Only one Tx channel should be selected
* \param[out] clgcCaptureCfg - Pointer to the CLGC capture config structure which will be updated with the updated CLGC capture configuration.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ClgcCaptureConfigGet(adi_adrv904x_Device_t* const                     device,
                                                                   const adi_adrv904x_TxChannels_e                  clgcTxChannelSel,
                                                                   adi_adrv904x_DfeAppCalClgcCaptureCfg_t* const    clgcCaptureCfg);

/** 
* \brief Configures the CLGC Tracking Config for the requested Tx channel(s)
*
* This function configures a tracking for the CLGC in the ADRV904x device. 
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization but before enabling the DPD tracking calibration.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] clgcTxChannelMask - One or multiple Tx channel for which CLGC configuration is required
* \param[in] clgcTrackCfg - Pointer to the CLGC tracking config structure
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ClgcTrackingConfigSet(adi_adrv904x_Device_t* const                        device,
                                                                   const uint32_t                                       clgcTxChannelMask,
                                                                   const adi_adrv904x_DfeAppCalClgcTrackCfg_t* const    clgcTrackCfg);

/**
* \brief Retrieves the CLGC Tracking Config for the requested Tx channel from the device
*
* This function retrieves the CLGC tracking config in the ADRV904x device.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] clgcTxChannelSel - Tx channel selection to read CLGC capture configuration. Only one Tx channel should be selected
* \param[out] clgcTrackCfg - Pointer to the CLGC capture config structure which will be updated with the updated CLGC tracking configuration.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ClgcTrackingConfigGet(adi_adrv904x_Device_t* const                device,
                                                                   const adi_adrv904x_TxChannels_e              clgcTxChannelSel,
                                                                   adi_adrv904x_DfeAppCalClgcTrackCfg_t* const  clgcTrackCfg);


/** 
* \brief Start the CLGC Tracking Run for the requested Tx channel(s)
*
* This function start a CLGC tracking run in the ADRV904x device. 
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization but before enabling the DPD tracking calibration.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] clgcTxChannelMask - One or multiple Tx channel for which CLGC configuration is required
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ClgcTrackingRun(adi_adrv904x_Device_t* const  device,
                                                              const uint32_t                clgcTxChannelMask);

/** 
* \brief Reset the CLGC Tracking for the requested Tx channel(s)
*
* This function reset the CLGC tracking in the ADRV904x device. 
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization but before enabling the DPD tracking calibration.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] clgcTxChannelMask - One or multiple Tx channel for which CLGC configuration is required
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ClgcTrackingReset(adi_adrv904x_Device_t* const    device,
                                                                const uint32_t                  clgcTxChannelMask);

/**
* \brief Retrieves the CLGC Statuc for the requested Tx channel from the device
*
* This function retrieves the CLGC status in the ADRV904x device.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization.
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] clgcTxChannelSel - Tx channel selection to read CLGC status. Only one Tx channel should be selected
* \param[out] clgcStatus - Pointer to the CLGC status structure which will be updated with the updated CLGC status.
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeClgcStatusGet(adi_adrv904x_Device_t* const                 device,
                                                               const adi_adrv904x_TxChannels_e              clgcTxChannelSel,
                                                               adi_adrv904x_DfeAppCalClgcStatus_t* const    clgcStatus);

/**
* \brief Sets the CLGC Capture Buffers Access Configuration
*
* This function sets the CLGC Capture Buffers Access configuration in the FW.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] clgcTxChannelMask - One or multiple Tx channel selection to apply Capture Buffers Access configuration
* \param[in] clgcCapBufAccess - Pointer to the CLGC Capture Buffers Access config structure which contains the configuration to be set in FW
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeClgcCaptureBuffersAccessSet(adi_adrv904x_Device_t* const                                    device,
                                                                             const uint32_t                                                  clgcTxChannelMask,
                                                                             adi_adrv904x_DfeAppFrameworkTrackingCalCapBufAccessSet_t* const clgcCapBufAccess);

/**
* \brief Read the CLGC Capture Buffers Access configuration
*
* This function reads the CLGC Capture Buffers Access state and capture buffer addresses
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called after BBIC requests reading capture data by calling the function adi_adrv904x_DfeClgcCaptureBuffersAccessSet().
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure
* \param[in] clgcTxChannelSel - Tx channel selection to read CLGC Capture Buffers Access configuration. Only one Tx channel should be selected
* \param[out] clgcCapBufAccess - Pointer to the CLGC Capture BUffers Access structure
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeClgcCaptureBuffersAccessGet(adi_adrv904x_Device_t* const                                    device,
                                                                             const adi_adrv904x_TxChannels_e                                 clgcTxChannelSel,
                                                                             adi_adrv904x_DfeAppFrameworkTrackingCalCapBufAccessGet_t* const clgcCapBufAccess);
#endif /* _ADI_ADRV904X_DFE_CLGC_H_ */