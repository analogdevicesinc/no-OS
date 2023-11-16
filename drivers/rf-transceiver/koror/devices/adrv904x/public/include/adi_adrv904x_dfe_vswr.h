/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_dfe_vswr.h
* \brief Contains ADRV904X VSWR function prototypes for
*    adi_adrv904x_dfe_vswr.c
*
* ADRV904X API Version: 2.9.0.4
*/

#ifndef _ADI_ADRV904X_DFE_VSWR_H_
#define _ADI_ADRV904X_DFE_VSWR_H_

#include "adi_adrv904x_dfe_vswr_types.h"
#include "adi_adrv904x_error.h"
#include "adi_common_error_types.h"
#include "adi_adrv904x_dfe_cal_vswr_types.h"

/**
* \brief Write VSWR pulse RAM.
* 
* \pre This function is called after the device has been initialized. It's used to write a dataset to VSWR
* playback RAM. This dataset is added to signal on datapath after PreDPD HB1 block. This function call will
* throw error if playback data generation is enabled. It's users responsibility to make sure playback data isn't
* enabled while this API writes a new data set to playback RAM.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     txChannelMask - One or multiple Tx channel selection to apply playback data
* \param[in]     playbackData - Pointer to playback data structure to be written to VSWR playback RAM
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_VswrPlaybackDataWrite(adi_adrv904x_Device_t* const                    device,
                                                                    const uint32_t                                  txChannelMask,
                                                                    const adi_adrv904x_VswrPlaybackData_t* const    playbackData);

/**
* \brief Write VSWR pulse RAM.
* 
* \pre This function is called before the device has been initialized. It's used to write a dataset to VSWR
* playback RAM. This dataset is added to signal on datapath after PreDPD HB1 block. This function call will
* throw error if playback data generation is enabled. It's users responsibility to make sure playback data isn't
* enabled while this API writes a new data set to playback RAM.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     txChannelMask - One or multiple Tx channel selection to apply playback data
* \param[in]     playbackData - Pointer to playback data structure to be written to VSWR playback RAM
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_VswrPlaybackDataWrite_v2(adi_adrv904x_Device_t* const                    device,
                                                                       const uint32_t                                  txChannelMask,
                                                                       const adi_adrv904x_VswrPlaybackData_t* const    playbackData);


/**
* \brief Read VSWR pulse RAM.
* 
* \pre This function is called after the device has been initialized. It's used to read the dataset stored
* at VSWR playback RAM. This dataset is added to signal on datapath after PreDPD HB1 block. This function call will
* throw error if playback data generation is enabled. It's users responsibility to make sure playback data isn't
* enabled while this API reads playback RAM.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     txChannelSel - Tx channel selection to read playback data, only one tx channel needs to be selected
* \param[out]    playbackData - Pointer to playback data structure to be read from VSWR playback RAM
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_VswrPlaybackDataRead(adi_adrv904x_Device_t* const             device,
                                                                   const uint32_t                           txChannelSel,
                                                                   adi_adrv904x_VswrPlaybackData_t* const   playbackData);

/**
* \brief Set tracking config for VSWR tracking.
* 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     vswrTxChannelMask - Tx channel mask to set the configuration for. Multiple Tx channels can be selected
* \param[in]     cfg - Pointer to tracking config data structure
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_VswrTrackingConfigSet(adi_adrv904x_Device_t* const                     device,
                                                                    const uint32_t                                   vswrTxChannelMask,
                                                                    const adi_adrv904x_DfeAppCalVswrCfg_t* const     cfg);

/**
* \brief Read tracking config of VSWR tracking.
* 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     vswrTxChannelSel - Tx channel selection to read the configuration of. Only one channel should be selected
* \param[out]    cfg - Pointer to tracking config data structure
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_VswrTrackingConfigGet(adi_adrv904x_Device_t* const                     device,
                                                                    const adi_adrv904x_TxChannels_e                  vswrTxChannelSel,
                                                                    adi_adrv904x_DfeAppCalVswrCfg_t* const           cfg);

/**
* \brief Read the VSWR specific status data of selected channel. Common status header can be read
* by using adi_adrv904x_DfeTrackingCalStatusGet function.
* 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     vswrTxChannelSel - Tx channel selection to read the status of VSWR tracking
* \param[out]    vswrStatus - Pointer to status data structure
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_VswrStatusGet(adi_adrv904x_Device_t* const               device,
                                                            const adi_adrv904x_Channels_e              vswrTxChannelSel,
                                                            adi_adrv904x_DfeAppCalVswrStatus_t* const  vswrStatus);

/**
* \brief Reset VSWR for requested Tx channel/s
* 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     vswrTxChannelMask - Tx channel mask to reset VSWR tracking. Multiple channels can be selected
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_VswrReset(adi_adrv904x_Device_t* const device,
                                                        const uint32_t               vswrTxChannelMask);

#endif /* _ADI_ADRV904X_DFE_VSWR_H_ */
