/**
 * Copyright 2015 - 2022 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adrv904x_carrier_reconfigure.h
 * \brief Contains ADRV904x Carrier Reconfigure function declarations
 *
 * ADRV904X API Version: 2.9.0.4
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
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] carrierConfigs carrier input config
* \param[in] carrierConfigsOut holds the calculated values during reconfiguration
* \param[in] carrierChannelFilter channel filter configuration
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CducDelayCalculate(   adi_adrv904x_Device_t* const device,
                                                                const adi_adrv904x_CarrierRadioCfg_t* const carrierConfigs,
                                                                adrv904x_CarrierDynamicReconfigProfileCfg_t* const carrierConfigsOut,
                                                                const adi_adrv904x_ChannelFilterOutputCfg_t* const carrierChannelFilter);
/**
* \brief Entry point for cddc carrier delay configuration
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in] carrierConfigs carrier input config
* \param[in] carrierConfigsOut holds the calculated values during reconfiguration
* \param[in] carrierChannelFilter channel filter configuration
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CddcDelayCalculate(   adi_adrv904x_Device_t* const device,
                                                                const adi_adrv904x_CarrierRadioCfg_t* const carrierConfigs,
                                                                adrv904x_CarrierDynamicReconfigProfileCfg_t* const carrierConfigsOut,
                                                                const adi_adrv904x_ChannelFilterOutputCfg_t* const carrierChannelFilter);

/**
* \brief Called before reconfiguring. Initializes the internal struct to default values
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \pre None
*
* \param[in] device Pointer to the ADRV904X device data structure
* \param[in,out] carrierConfigsOut holds the calculated values during reconfiguration
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_InternalReconfigStructInit(   adi_adrv904x_Device_t* const device,
                                                                        adrv904x_CarrierDynamicReconfigInternalCfg_t* const carrierConfigsOut);

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
                                                                            const adi_adrv904x_CarrierRadioCfg_t* const             carrierConfigs,
                                                                            adrv904x_CarrierDynamicReconfigProfileCfg_t* const      carrierConfigsOut,
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
* \param[in,out] carrierConfigsOut holds the calculated values during reconfiguration
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CalculateRxSampleXbarSlotConfig(  adi_adrv904x_Device_t* const                            device,
                                                                            const adi_adrv904x_CarrierJesdCfg_t* const   jesdCfg,
                                                                            adrv904x_CarrierDynamicReconfigInternalCfg_t* const     carrierConfigsOut);

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
* \param[in,out] carrierConfigsOut holds the calculated values during reconfiguration
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_CalculateTxSampleXbarSlotConfig(   adi_adrv904x_Device_t* const                          device,
                                                                            const adi_adrv904x_CarrierJesdCfg_t* const   jesdCfg,
                                                                            adrv904x_CarrierDynamicReconfigInternalCfg_t* const     carrierConfigsOut);

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
ADI_API adi_adrv904x_ErrAction_e adrv904x_RxCarrierBandSorting( adi_adrv904x_Device_t* const device,
                                                                const adi_adrv904x_CarrierRadioCfg_t* const rxCarrierConfigs,
                                                                adrv904x_CarrierDynamicReconfigProfileCfg_t* const rxCarrierConfigsOut);
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
ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCarrierBandSorting( adi_adrv904x_Device_t* const device,
                                                                const adi_adrv904x_CarrierRadioCfg_t* const txCarrierConfigs,
                                                                adrv904x_CarrierDynamicReconfigProfileCfg_t* const        rxCarrierConfigsOut);


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
ADI_API adi_adrv904x_ErrAction_e adrv904x_RxCarrierNcoReconfig( adi_adrv904x_Device_t* const device,
                                                                const adi_adrv904x_CarrierRadioCfg_t* const rxCarrierConfigs,
                                                                adrv904x_CarrierDynamicReconfigProfileCfg_t* const rxCarrierConfigsOut);


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
ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCarrierNcoReconfig( adi_adrv904x_Device_t* const device,
                                                                const adi_adrv904x_CarrierRadioCfg_t* const txCarrierConfigs,
                                                                adrv904x_CarrierDynamicReconfigProfileCfg_t* const txCarrierConfigsOut);



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
                                                                    adrv904x_CarrierDynamicReconfigProfileCfg_t* const txCarrierConfigsOut);

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
* \param[in] rxDelayParams holds the reconfiguration settings to be written to the device
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_RxCarrierDelaySet(   adi_adrv904x_Device_t* const device,
                                                                const uint8_t chanSelect,
                                                                const adrv904x_CarrierReconfigDelayParams_t* const rxDelayParams);

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
                                                                const adrv904x_CarrierDynamicReconfigProfileCfg_t* const rxCarrierConfigs);

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
                                                                const adrv904x_CarrierDynamicReconfigProfileCfg_t* const txCarrierConfigs);

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
* \param[in] txDelayParams holds the reconfiguration settings to be written to the device
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_TxCarrierDelaySet(   adi_adrv904x_Device_t* const device,
                                                                const uint8_t chanSelect,
                                                                const adrv904x_CarrierReconfigDelayParams_t* const txDelayParams);

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
                                                            const adrv904x_CarrierDynamicReconfigInternalCfg_t* const carrierJesdConfig);

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
                                                                    const adrv904x_CarrierDynamicReconfigProfileCfg_t* const carrierJesdConfig);


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
                                                            const adrv904x_CarrierDynamicReconfigInternalCfg_t* const carrierJesdConfig);

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
                                                                    const adrv904x_CarrierDynamicReconfigProfileCfg_t* const carrierJesdConfig);


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

#endif //CLIENT_IGNORE
#endif
