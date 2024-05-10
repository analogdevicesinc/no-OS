/**
 * Copyright 2015 - 2022 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adrv904x_carrier_reconfigure.h
 * \brief Contains ADRV904x Carrier Reconfigure function declarations
 *
 * ADRV904X API Version: 2.10.0.4
 */ 

#ifndef _ADRV904X_CARRIER_RECONFIGURE_H_
#define _ADRV904X_CARRIER_RECONFIGURE_H_

#include "adrv904x_reg_addr_macros.h"
#include "adrv904x_carrier_reconfigure_types.h"

#include "../../private/bf/adrv904x_bf_cddc_funcs.h"
#include "../../private/bf/adrv904x_bf_cddc_hb_dpath.h"

#ifndef CLIENT_IGNORE

/**
* \brief Entry point for cduc carrier delay configuration
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in, out] device Pointer to the ADRV904X device data structure
* \param[in] carrierConfigs carrier input config
* \param[in] carrierChannelFilter channel filter configuration
* \param[in] carrierConfigsOut holds the calculated values during reconfiguration
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CducDelayConfigurationCalculate(  adi_adrv904x_Device_t* const                        device, 
                                                                            const adi_adrv904x_CarrierRadioCfg_t* const         carrierConfigs,
                                                                            const adi_adrv904x_ChannelFilterOutputCfg_t* const  carrierChannelFilter,
                                                                            adi_adrv904x_CarrierReconfigProfileCfgOut_t* const  carrierConfigsOut);
/**
* \brief Entry point for cddc carrier delay configuration
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] carrierConfigs carrier input config
* \param[in] carrierChannelFilter channel filter configuration
* \param[in, out] carrierConfigsOut holds the calculated values during reconfiguration
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcDelayConfigurationCalculate(  adi_adrv904x_Device_t* const                        device, 
                                                                            const adi_adrv904x_CarrierRadioCfg_t* const         carrierConfigs,
                                                                            const adi_adrv904x_ChannelFilterOutputCfg_t* const  carrierChannelFilter,                                                         
                                                                            adi_adrv904x_CarrierReconfigProfileCfgOut_t* const  carrierConfigsOut);


/**
* \brief Called before reconfiguring. Initializes the top level Solution struct to default values
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \pre None
*
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] inJesdCfg User input of jesd config. To be copied to soln struct
* \param[in] inProfileCfgs User input of carrier radio input configs (array size = inNumProfiles). To be copied to soln struct
* \param[in] inNumProfiles Number of carrier radio profiles contained in inProfileCfgs
* \param[in,out] soln holds the calculated values during reconfiguration.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_ReconfigSolutionInit( adi_adrv904x_Device_t* const                device,
                                                                const adi_adrv904x_CarrierJesdCfg_t* const  inJesdCfg,
                                                                const adi_adrv904x_CarrierRadioCfg_t        inProfileCfgs[],
                                                                uint32_t                                    inNumProfiles,
                                                                adi_adrv904x_CarrierReconfigSoln_t* const   soln);

/**
* \brief Performs JESD calculations
* 
*        For all enabled carriers, calculates:
*           - rxCarrierConfigsOut.carriers.decimationInterpolationRatio
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \pre None
*
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] chanSelect is the channel parameter
* \param[in] carrierConfigs carrier settings for this channel
* \param[in,out] carrierConfigsOut holds the calculated values during reconfiguration
* \param[in] rxFlag 1 = Rx settings, 0 = Tx
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CarrierJesdParametersCalculate(   adi_adrv904x_Device_t* const                            device,
                                                                            const adrv904x_CarrierInitialCfg_t* const               initialCfg,
                                                                            const adi_adrv904x_CarrierRadioCfg_t* const             carrierConfigs,
                                                                            adi_adrv904x_CarrierReconfigProfileCfgOut_t* const      carrierConfigsOut,
                                                                            const uint8_t                                           rxFlag);

/**
* \brief Performs Rx Sample XBar JESD calculations
* 
*        For all enabled carriers, calculates:
*           - rxCarrierConfigsOut.carriers.decimationInterpolationRatio
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \pre None
*
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] jesdCfg - Pointer that holds the updated JESD settings to accomdate the new carrier settings
* \param[in,out] soln - Solution structure that holds the calculated values during reconfiguration
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CalculateRxSampleXbarSlotConfig(  adi_adrv904x_Device_t* const                    device,
                                                                            const adi_adrv904x_CarrierJesdCfg_t* const      inJesdCfg,
                                                                            adi_adrv904x_CarrierReconfigOutput_t* const     out);

/**
* \brief Performs Rx Sample XBar JESD calculations
* 
*        For all enabled carriers, calculates:
*           - rxCarrierConfigsOut.carriers.decimationInterpolationRatio
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \pre None
*
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] jesdCfg - Pointer that holds the updated JESD settings to accomdate the new carrier settings
* \param[in,out] soln - Solution structure that holds the calculated values during reconfiguration
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CalculateTxSampleXbarSlotConfig(   adi_adrv904x_Device_t* const                   device,
                                                                            const adi_adrv904x_CarrierJesdCfg_t* const      inJesdCfg,
                                                                            adi_adrv904x_CarrierReconfigOutput_t* const     out);

/**
* \brief Performs Rx Carrier Band Sorting for Dynamic Reconfiguration
* 
*        For all enabled carriers, calculates:
*           - rxCarrierConfigsOut.bandSelect
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \pre None
*
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] rxCarrierConfigs carrier settings for this channel
* \param[in,out] rxCarrierConfigsOut holds the calculated values for a single profile during reconfiguration
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_RxCarrierBandSorting( adi_adrv904x_Device_t* const                        device,
                                                                const adrv904x_CarrierInitialCfg_t* const           rxInitialCfg,
                                                                const adi_adrv904x_CarrierRadioCfg_t* const         rxCarrierConfigs,
                                                                adi_adrv904x_CarrierReconfigProfileCfgOut_t* const  rxCarrierConfigsOut);
/**
* \brief Performs Tx Carrier Band Sorting for Dynamic Reconfiguration
* 
*        For all enabled carriers, calculates:
*           - carrierConfigsOut.bandSelect
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \pre None
*
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] txCarrierConfigs carrier settings for this channel
* \param[in,out] txCarrierConfigsOut holds the calculated values for a single profile during reconfiguration
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCarrierBandSorting( adi_adrv904x_Device_t* const                        device,
                                                                const adrv904x_CarrierInitialCfg_t* const           txInitialCfg,
                                                                const adi_adrv904x_CarrierRadioCfg_t* const         txCarrierConfigs,
                                                                adi_adrv904x_CarrierReconfigProfileCfgOut_t* const  txCarrierConfigsOut);
                                                                


/**
* \brief Performs Rx Carrier NCO calculation for Dynamic Reconfiguration
* 
*        For all enabled carriers, calculates:
*           - rxCarrierConfigsOut.mixerEnable
*           - rxCarrierConfigsOut.ncoFreq_kHz
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \pre adrv904x_RxCarrierBandSorting
*
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] rxCarrierConfigs carrier settings for this channel
* \param[in,out] rxCarrierConfigsOut holds the calculated values for a single profile during reconfiguration
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_RxCarrierNcoReconfig( adi_adrv904x_Device_t* const                        device,
                                                                const adrv904x_CarrierInitialCfg_t* const           rxInitialCfg,
                                                                const adi_adrv904x_CarrierRadioCfg_t* const         rxCarrierConfigs,
                                                                adi_adrv904x_CarrierReconfigProfileCfgOut_t* const  rxCarrierConfigsOut);


/**
* \brief Performs Tx Carrier NCO calculation for Dynamic Reconfiguration
* 
*        For all enabled carriers, calculates:
*           - txCarrierConfigsOut.mixerEnable
*           - txCarrierConfigsOut.ncoFreq_kHz
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \pre adrv904x_TxCarrierBandSorting
*
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] txCarrierConfigs carrier settings for this channel
* \param[in,out] txCarrierConfigsOut holds the calculated values for a single profile during reconfiguration
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCarrierNcoReconfig( adi_adrv904x_Device_t* const                        device,
                                                                const adrv904x_CarrierInitialCfg_t* const           txInitialCfg,
                                                                const adi_adrv904x_CarrierRadioCfg_t* const         txCarrierConfigs,
                                                                adi_adrv904x_CarrierReconfigProfileCfgOut_t* const  txCarrierConfigsOut);



/**
* \brief Performs Tx Carrier Band Atten calculation for Dynamic Reconfiguration
* 
*        For all enabled carriers, calculates:
*           - txCarrierConfigsOut.bandAtten[01]
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \pre adrv904x_RxCarrierBandSorting
*
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] txCarrierConfigs carrier settings for this channel
* \param[in,out] txCarrierConfigsOut holds the calculated values for a single profile during reconfiguration
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCarrierBandAttenConfig( adi_adrv904x_Device_t* const device,
                                                                    const adi_adrv904x_CarrierRadioCfg_t* const txCarrierConfigs,
                                                                    adi_adrv904x_CarrierReconfigProfileCfgOut_t* const txCarrierConfigsOut);

/**
* \brief Writes the reconfigured rx carrier settings for the specified channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \pre adrv904x_RxCarrierDelayCalculate
*
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] chanSelect is the channel parameter
* \param[in] rxHwDelayBufferConfig holds the reconfiguration settings to be written to the device
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_RxCarrierDelaySet(    adi_adrv904x_Device_t* const device,
                                                                const uint8_t chanSelect,
                                                                const adi_adrv904x_CarrierHwDelayBufferConfig_t* const rxHwDelayBufferConfig);
/**
* \brief Writes the reconfigured rx carrier delay settings for the specified channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \pre adrv904x_RxCarrierNcoReconfig
*
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] chanSelect is the channel parameter
* \param[in] rxCarrierConfigs holds the reconfiguration settings for a single profile to be written to the device
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_RxCarrierConfigSet(   adi_adrv904x_Device_t* const device,
                                                                const uint8_t chanSelect,
                                                                const adi_adrv904x_CarrierReconfigProfileCfgOut_t* const rxCarrierConfigs);

/**
* \brief Writes the reconfigured tx carrier settings for the specified channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \pre adrv904x_TxCarrierNcoReconfig
* \pre adrv904x_TxCarrierBandAttenConfig
*
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] chanSelect is the channel parameter
* \param[in] txCarrierConfigs holds the reconfiguration settings for a single profile to be written to the device
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCarrierConfigSet(   adi_adrv904x_Device_t* const device,
                                                                const uint8_t chanSelect,
                                                                const adi_adrv904x_CarrierReconfigProfileCfgOut_t* const txCarrierConfigs);

/**
* \brief Writes the reconfigured tx carrier settings for the specified channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \pre adrv904x_TxCarrierDelayCalculate
*
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] chanSelect is the channel parameter
* \param[in] txHwDelayBufferConfig holds the reconfiguration settings to be written to the device
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCarrierDelaySet(   adi_adrv904x_Device_t* const device,
                                                                const uint8_t chanSelect,
                                                                const adi_adrv904x_CarrierHwDelayBufferConfig_t* const txHwDelayBufferConfig);

/**
* \brief Writes the reconfigured JESD link settings
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \pre adrv904x_CarrierJesdParametersCalculate
* 
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] carrierJesdConfig holds the reconfiguration settings to be written to the device
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_RxJesdConfigSet(  adi_adrv904x_Device_t* const device,
                                                            const adi_adrv904x_CarrierReconfigOutput_t* const reconfigOut);

/**
* \brief Writes the reconfigured channelized JESD settings
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \pre adrv904x_CarrierJesdParametersCalculate
* 
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] rxChannelMask holds bitmask of rx channels to reconfigure
* \param[in] carrierJesdConfig holds the reconfiguration settings to be written to the device
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CarrierRxJesdConfigSet(   adi_adrv904x_Device_t* const device,
                                                                    const uint32_t rxChannelMask,
                                                                    const adi_adrv904x_CarrierReconfigProfileCfgOut_t* const carrierJesdConfig);


/**
* \brief Writes the reconfigured JESD link settings
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \pre adrv904x_CarrierJesdParametersCalculate
* 
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] carrierJesdConfig holds the reconfiguration settings to be written to the device
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_TxJesdConfigSet(  adi_adrv904x_Device_t* const device,
                                                            const adi_adrv904x_CarrierReconfigOutput_t* const reconfigOut);

/**
* \brief Writes the reconfigured JESD settings
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \pre adrv904x_CarrierJesdParametersCalculate
* 
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] txChannelMask holds bitmask of rx channels to reconfigure
* \param[in] carrierJesdConfig holds the reconfiguration settings to be written to the device
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CarrierTxJesdConfigSet(   adi_adrv904x_Device_t* const device,
                                                                    const uint32_t txChannelMask,
                                                                    const adi_adrv904x_CarrierReconfigProfileCfgOut_t* const carrierJesdConfig);


/**
* \brief    Loads up the channel filter table for all carriers. Should be called one carrier at a time using the coeffIdx as a way to
*           keep track of where the next coefficient should be placed.
*           Throws an error if coeffIdx ever goes over the max number of filters allowed.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] carrierChannelFilterApplicationSel the application selection for the current carrier
* \param[in] carrierCfg carrier settings for the current carrier
* \param[out] coeffTable pointer to the start of the corresponding channel filter table
* \param[out] coeffTableSize pointer to the number of elements in the coeff table
* \param[out] numberOfFilterTaps pointer to unsigned integer that holds the number of filter taps for this application
* \param[out] assymetricFilterTaps pointer to an unsigned char that holds a 1 if asymmetric, 0 otherwise
* \param[in] rxFlag if 1, select rx cddc tables, tx cduc otherwise
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_ChannelFilterCoefsGet(    adi_adrv904x_Device_t* const    device,
                                                                    const adi_adrv904x_CarrierFilterApplicationType_e carrierChannelFilterApplicationSel,
                                                                    const adi_adrv904x_CarrierCfg_t* const carrierCfg,
                                                                    const int16_t** const coeffTable,
                                                                    int16_t* const coeffTableSize,
                                                                    uint32_t* const numberOfFilterTaps,
                                                                    uint8_t* const assymetricFilterTaps,
                                                                    const uint8_t rxFlag);

/**
* \brief    Performs Slot Shuffling algorithm that targets carrier delayMismatch to below desired threshold
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] carrierConfigs holds the new carrier radio settings for a reconfig
* \param[out] carrierConfigsOut holds the reconfiguration settings for a single profile to be written to the device
* \param[in] carrierChannelFilter hold the Channel Filter configuration
* \param[in] rxFlag if 1, select rx cddc tables, tx cduc otherwise
*
* \param[in] carrierCfg carrier settings for the current carrier

*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
                                                                        
ADI_API adi_adrv904x_ErrAction_e adrv904x_CarrierDelaySlotShuffleSet(   adi_adrv904x_Device_t* const                            device,
                                                                        const adi_adrv904x_CarrierRadioCfg_t* const             carrierConfigs,
                                                                        const adi_adrv904x_ChannelFilterOutputCfg_t* const      carrierChannelFilter,
                                                                        adi_adrv904x_CarrierReconfigProfileCfgOut_t* const      carrierConfigsOut,
                                                                        const uint8_t                                           rxFlag);

#endif //CLIENT_IGNORE
#endif
