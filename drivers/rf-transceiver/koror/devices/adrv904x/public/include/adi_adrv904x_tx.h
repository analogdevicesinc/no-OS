/**
 * Copyright 2015 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adi_adrv904x_tx.h
 * \brief Contains top level ADRV904X related function prototypes for
 *        adi_adrv904x_tx.c
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef _ADI_ADRV904X_TX_H_
#define _ADI_ADRV904X_TX_H_

#include "adi_adrv904x_tx_types.h"
#include "adi_adrv904x_error.h"

#include "adi_adrv904x_dfe_svc_dfe_pwr_mtr_t.h"
#include "adi_adrv904x_carrier_reconfigure_types.h"

/**
 * \brief Get Tx RF output attenuation table data.
 *
 * A full TxAtten table can be read in a single call. Or the table can be read in several calls,
 * reading a subset of the table with each function call using the txAttenIndexOffset parameter.
 *
 * \pre This function may be called any time after device initialization.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure.
 * \param[in] txChannel The tx channel whose atten table to access.
 * \param[in] txAttenIndexOffset Index in tx atten table from which to start reading.
 * \param[in,out] txAttenTableRows A caller-supplied array where the table data is written.
 * \param[in] numTxAttenEntries The number of rows to read from the tx atten table.
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e  adi_adrv904x_TxAttenTableRead(adi_adrv904x_Device_t* const    device,
                                                                const adi_adrv904x_TxChannels_e txChannel,
                                                                const uint32_t                  txAttenIndexOffset,
                                                                adi_adrv904x_TxAttenTableRow_t  txAttenTableRows[],
                                                                const uint32_t                  numTxAttenEntries);

/**
* \brief This API function has to be used in order to set the Tx RF output attenuation
* for the Tx output channels
*
* The attenuation given by txAttenuation.txAttenuation_mdB will be applied to the txChannel or channels
* indicated by the txAttenuation.txChannelMask as follows
* 
*|  txChannel          |  Description                                  |
*|:-------------------:|:---------------------------------------------:|
*|   ADI_ADRV904X_TX0  |   sets attenuation txAttenuation_mdB for TX0  |
*|   ADI_ADRV904X_TX1  |   sets attenuation txAttenuation_mdB for TX1  |
*|   ADI_ADRV904X_TX2  |   sets attenuation txAttenuation_mdB for TX2  |
*|   ADI_ADRV904X_TX3  |   sets attenuation txAttenuation_mdB for TX3  |
*|   ADI_ADRV904X_TX4  |   sets attenuation txAttenuation_mdB for TX4  |
*|   ADI_ADRV904X_TX5  |   sets attenuation txAttenuation_mdB for TX5  |
*|   ADI_ADRV904X_TX6  |   sets attenuation txAttenuation_mdB for TX6  |
*|   ADI_ADRV904X_TX7  |   sets attenuation txAttenuation_mdB for TX7  |

* \pre This function may be called any time after device initialization
* \pre This feature requires the initialization to be complete and the attenuation table to be loaded.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure
* \param[in] txAttenuation array of structure type adi_adrv904x_TxAtten_t
*            which indicates the channel and the desired TxAttenuation in milli-dB
*            (Range: 0 to 41950 mdB)
* \param[in] numTxAttenConfigs array size passed in txAttenuation array
* 
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e  adi_adrv904x_TxAttenSet(adi_adrv904x_Device_t * const   device,
                                                          const adi_adrv904x_TxAtten_t    txAttenuation[],
                                                          const uint32_t                  numTxAttenConfigs);

/**
* \brief Reads back the attenuation setting for the Tx channel output for the selected channel
*
*  This function reads back the Tx attenuation setting for either the Tx0, Tx1, Tx2, Tx3 , Tx4, Tx5, Tx6, Tx7 RF channel currently applied to the transmit chain.
*  This function can work with SPI mode or pin controlled Tx attenuation mode using the increment/decrement GPIO
*  pins.
*
* \pre This feature requires the initialization to be complete and the attenuation table to be loaded.
* \pre The Tx data path must be powered up for the current attenuation value to be valid.  If the Tx data path
*  is powered down or the radio is off, the last Tx attenuation setting when the Tx output was previously active will be
*  read back.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device's data structure
* \param[in] txChannel channel selection to read the attenuation is of type adi_adrv904x_TxChannels_e
* \param[out] txAttenuation Pointer to structure of adi_adrv904x_TxAtten_t type to store the readback value of the desired Tx channel attenuation in milli-dB (Range: 0 to 41950 mdB)
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxAttenGet(adi_adrv904x_Device_t * const   device,
                                                         const adi_adrv904x_TxChannels_e txChannel,
                                                         adi_adrv904x_TxAtten_t * const  txAttenuation);

/**
* \brief Configures the Tx attenuation mechanism for one or more channels.
*
* \pre This function is called after the device is initialized. 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the adrv904x data structure
* \param[in] chanMask a bit mask indicating the channels to be configured. If the LSB of chanMask is set Tx channel 0
*     will be configured by the call and so on.
* \param[in] txAttenCfg the Tx atten configuration to be assigned to the channels indicated by chanMask
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxAttenCfgSet(adi_adrv904x_Device_t* const device,
                                                            const uint32_t chanMask,
                                                            adi_adrv904x_TxAttenCfg_t* const txAttenCfg);
/**
* \brief Gets the Tx atten configuration for a channel.
*
* \pre This function is called after the device is initialized. 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the adrv904x data structure
* \param[in] txChannel the Tx channel id whose Tx atten to fetch.
* \param[out] txAttenCfg the atten config is written here.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxAttenCfgGet(adi_adrv904x_Device_t* const device,
                                                            const adi_adrv904x_TxChannels_e txChannel,
                                                            adi_adrv904x_TxAttenCfg_t* const txAttenCfg);
                                                            
/**
 * \brief Set the attenuation levels in S0 or S1 registers.
 *
 * These registers form two of several possible sources for the Tx attenuation applied to a channel. Depending
 * on other Tx attenuation-update configuration, which can be set using adi_adrv904x_TxAttenUpdateCfgSet,
 * setting the S0, S1 registers may or may not result in an immediate change in a channel's Tx attenuation.
 *
 * \param[in,out] device Context variable - Pointer to the device data structure.
 * \param[in] chanMask Indicates the channels to which the attenuation value applies.
 * \param[in] levelMilliDB The Tx attenuation in 1/1000s of a decibel. Valid range 0 - 41950, equivalent to 0 - 41.95 dB.
 * \param[in] isS0 If non-zero the value into channels' S0 registers otherwise into their
 *   S1 registers.
 *
 * \retval adi_common_ErrAction_e
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxAttenS0S1Set(adi_adrv904x_Device_t* const   device,
                                                             const uint32_t                 chanMask,
                                                             const uint32_t                 levelMilliDB,
                                                             const uint8_t                  isS0);

/**
 * \brief Get the attenuation levels in S0 or S1 registers.
 *
 * \param[in,out] device Context variable - Pointer to the device data structure.
 * \param[in] txChannel The channel from which to read.
 * \param[out] levelMilliDB The retrieved value is written here.
 * \param[in] isS0 If non - zero the channels' S0 register is read otherwise the S1 register.
 *
 * \retval adi_common_ErrAction_e
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxAttenS0S1Get(adi_adrv904x_Device_t* const device,
                                                             const adi_adrv904x_TxChannels_e txChannel,
                                                             uint32_t* const levelMilliDB,
                                                             const uint8_t isS0);

/**
 * \brief Set the Tx attenuation update configuration for several channels.
 * 
 * Each Tx channel's update configuration consists of:
 * - an attenuation level source(S0, S0_OR_S1)
 * - an attenuation update trigger(GPIO, SPI, NONE)
 * - for the cases of GPIO trigger and S0_OR_S1 source some extra configuration information.
 * 
 * Limitations :
 * -Setting any channel to trigger on SPI will implicitly set any channels previously configured to trigger
 *   on GPIO to have a NONE trigger.
 * -Setting any channel to trigger on GPIO will implicitly set any channels previously configured to trigger
 *   on SPI to have a NONE trigger.
 * 
 * \param[in,out] device Context variable - pointer to the device data structure.
 * \param[in] chanMask Tx channels to which the configuration is applied
 * \param[in] cfg contains the attenuation update source and trigger settings. 
 * 
 * Return Value :
 * adi_common_ErrAction_e
 */
ADI_API	adi_adrv904x_ErrAction_e adi_adrv904x_TxAttenUpdateCfgSet(adi_adrv904x_Device_t* const device,
                                                                  const uint32_t chanMask,
                                                                  const adi_adrv904x_TxAttenUpdateCfg_t* const cfg);

/**
 * \brief Get the Tx attenuation update configuration for a channel.
 * 
 * \param[in,out] device Context variable - pointer to the device data structure.
 * \param[in] txChannel Tx channel whose update configuration to get.
 * \param[out] cfg the attenuation update settings for chanId are written here. 
 * 
 * Return Value :
 * adi_common_ErrAction_e
 */
ADI_API	adi_adrv904x_ErrAction_e adi_adrv904x_TxAttenUpdateCfgGet(adi_adrv904x_Device_t* const device,
                                                                  const adi_adrv904x_TxChannels_e txChannel,
                                                                  adi_adrv904x_TxAttenUpdateCfg_t* const cfg);

/**
 * \brief Simultaneously update the Tx attenuation level for several channels.
 *
 * This function only has an effect for channels that have already had their attenuation update trigger set
 * to SPI using TxAttenUpdateCfgSet(). The effect of the attenuation update will be simultaneous across all
 * affected channels.
 *
 * \param[in,out] device Context variable - pointer to the device data structure.
 * \param[in] chanMask Tx channels on which to update Tx attenuation.
 *
 * Return Value :
 * adi_common_ErrAction_e
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxAttenUpdate(adi_adrv904x_Device_t *const device,
                                                            const uint32_t chanMask);

/**
* \brief This function reads back the LO Source Mapping to the requested Tx channel.
*
* The LO source select for a given channel is configured during init time. This function
* can be used to read back the configuration during run time.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X data structure
* \param[in]  txChannel Enum to select Tx Channel
* \param[out] txLoSource Pointer to store Tx channel LO source mapping read back
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxLoSourceGet(adi_adrv904x_Device_t * const   device,
                                                            const adi_adrv904x_TxChannels_e txChannel,
                                                            adi_adrv904x_LoSel_e * const    txLoSource);

/**
* \brief Function to set the Tx power monitor configuration. 
*
* This function can be used to configure peak power monitor and average power monitor blocks.
* A peak is detected if the signal I and Q samples are above a pre-set threshold (PowerMonitorCfg_t.peakPreThreshold)
* and (I^2+Q^2) of the signal exceeds another threshold(PowerMonitorCfg_t.peakThreshold) more than a preset
* count(PowerMonitorCfg_t.peakDetectOverloadCount) within time window configured with PowerMonitorCfg_t.peakDetectOverloadWindowSize.
* If number a valid peaks exceeds PowerMonitorCfg_t.peakCount within PowerMonitorCfg_t.peakDuration time period, peak detection
* error flag is asserted. The signal power (I^2+Q^2) is averaged over a PowerMonitorCfg_t.avgDuration time period and if
* this average value exceeds PowerMonitorCfg_t.avgPowerThreshold, average power error flag is asserted. 
*
* This function takes in an array of adi_adrv904x_PowerMonitorCfgRt_t structures to program the Tx channels.
* Each structure has a separate channel mask that indicates to which Tx channels that particular configuration applies.
* Example usage with two different configurations:
*
* txPowerMonitorCfg = (adi_adrv904x_PowerMonitorCfgRt_t *)malloc(2 * sizeof(adi_adrv904x_PowerMonitorCfgRt_t))
*
* txPowerMonitorCfg[0].txPowerMonitorCfg = txPowerMonitorCfg1; // Configuration 1 loaded with desired configuration values
* txPowerMonitorCfg[0].txChannelMask = (uint32_t)(ADI_ADRV904X_TX0 | ADI_ADRV904X_TX1 | ADI_ADRV904X_TX2 | ADI_ADRV904X_TX3);
*
* txPowerMonitorCfg[1].txPowerMonitorCfg = txPowerMonitorCfg2; // Configuration 2 loaded with desired configuration values
* txPowerMonitorCfg[1].txChannelMask = (uint32_t)(ADI_ADRV904X_TX4 | ADI_ADRV904X_TX5 | ADI_ADRV904X_TX6 | ADI_ADRV904X_TX7);
*
* retVal = adi_adrv904x_TxPowerMonitorCfgSet(device, txPowerMonitorCfg, 2);
*
* Note:   The function does not check for duplicated channel assignments across cfg structures. 
*           Invalid channel mask values will return an error.
*         Not all Tx channels must be assigned a configuration. If a channel mask is zero that configuration is ignored, ie no
*         error returned.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the adrv904x data structure
* \param[in] txPowerMonitorCfg array of structures of type adi_adrv904x_PowerMonitorCfgRt_t which will configure one or more channels
* \param[in] numPowerProtectCfgs number of configurations passed in the array
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxPowerMonitorCfgSet(adi_adrv904x_Device_t* const                    device,
                                                                   const adi_adrv904x_PowerMonitorCfgRt_t          txPowerMonitorCfg[],
                                                                   const uint32_t                                  numPowerProtectCfgs);



/**
* \brief Function to get the Tx power monitor configuration. 
*
* This function can be used to read back the power monitor configuration for selected channel.
* User can select the Tx channel by using adi_adrv904x_PowerMonitorCfgRt_t.txChannelMask
* Only one Tx channel should be selected at a time, otherwise this function will throw an error.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the adrv904x data structure
* \param[in,out] txPowerMonitorCfg Pointer to power monitor configuration to be populated
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxPowerMonitorCfgGet(adi_adrv904x_Device_t * const device,
                                                                   adi_adrv904x_PowerMonitorCfgRt_t* const txPowerMonitorCfg);
    

/**
* \brief This function reads the status of events causing Tx power ramp down
*
* Each Tx Channel(0-7) has independent ramp-down event status bits.
* Only one Tx channel should be selected at a time, otherwise this function will throw an error.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the adrv904x data structure
* \param[in] txChannel Channel selection to read error flags
* \param[out] eventBits Pointer to error mask to be populated by this function
*                    Bit   |  Description
*             ----------  -|-----------------------------------------------------------------------------
*                  [4-7]   | Reserved
*                  [3]     | Pll Unlock / Dfrm Error
*                  [2]     | SRD error
*                  [1]     | Pa Protection Peak Power Error
*                  [0]     | Pa Protection Average Power Error


*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxProtectionErrorGet(adi_adrv904x_Device_t* const    device,
                                                                   const adi_adrv904x_TxChannels_e txChannel,
                                                                   uint32_t* const                 eventBits);

/**
* \brief This function clears the error flags causing Tx power ramp down
*
* Each Tx Channel(0-7) has independent ramp-down event flags.
* Only one Tx channel should be selected at a time, otherwise this function will throw an error.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the adrv904x data structure
* \param[in] txChannel Channel selection to clear error flags
* \param[in] eventBits Mask selecting the error flags to be cleared
*                    Bit   |  Description
*             -----------  |-----------------------------------------------------------------------------
*                  [4-7]   | Reserved
*                  [3]     | Pll Unlock / Dfrm Error
*                  [2]     | SRD error
*                  [1]     | Pa Protection Peak Power Error
*                  [0]     | Pa Protection Average Power Error


*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxProtectionErrorClear(adi_adrv904x_Device_t * const   device,
                                                                     const adi_adrv904x_TxChannels_e txChannel,
                                                                     const uint32_t                  eventBits);


/**
* \brief Function to set the Tx Slew Rate Detector configuration
*        Slew rate is calculated as (I[n]-I[n-1])^2+(Q[n]-Q[n-1])^2 where n is the latest sample index of both I and Q samples.
*        If calculated slew is greater than a pre-set limit, txSlewRateDetectorCfg.srdOffset, slew rate error is asserted. 
*        User might assign slew rate event to GPIO with txSlewRateDetectorCfg.gpioSel.
*
*        This function takes in an array of adi_adrv904x_SlewRateDetectorCfgRt_t structures to program the Tx channels.
*        Each structure has a separate channel mask that indicates to which Tx channels that particular configuration applies.
*        Example usage with two different configurations:
*        
*           txSlewRateDetectorCfg = (adi_adrv904x_SlewRateDetectorCfgRt_t *)malloc(2 * sizeof(adi_adrv904x_SlewRateDetectorCfgRt_t))
*           
*           txSlewRateDetectorCfg[0].txSlewRateDetectorCfg = txSlewRateDetectorCfg1; // Configuration 1 loaded with desired configuration values
*           txSlewRateDetectorCfg[0].txChannelMask = (uint32_t)(ADI_ADRV904X_TX0 | ADI_ADRV904X_TX1 | ADI_ADRV904X_TX2 | ADI_ADRV904X_TX3);
*           
*           txSlewRateDetectorCfg[1].txSlewRateDetectorCfg = txSlewRateDetectorCfg2; // Configuration 2 loaded with desired configuration values
*           txSlewRateDetectorCfg[1].txChannelMask = (uint32_t)(ADI_ADRV904X_TX4 | ADI_ADRV904X_TX5 | ADI_ADRV904X_TX6 | ADI_ADRV904X_TX7);
*           
*           retVal = adi_adrv904x_TxSlewRateDetectorCfgSet(device, txSlewRateDetectorCfg, 2);
*           
*       Note:   The function does not check for duplicated channel assignments across cfg structures. 
*               Invalid channel mask values will return an error.
*               Not all Tx channels must be assigned a configuration. If a channel mask is zero that configuration is ignored, ie no
*               error returned.
*
* \pre This function is called after the device is initialized. 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the adrv904x data structure
* \param[in] txSlewRateDetectorCfg an array holding all the Slew Rate Detector configurations required with the assigned channel mask
* \param[in] numSlewRateDetCfgs the number of elements in txSlewRateDetectorCfg array

*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxSlewRateDetectorCfgSet(adi_adrv904x_Device_t* const                device,
                                                                       adi_adrv904x_SlewRateDetectorCfgRt_t        txSlewRateDetectorCfg[],
                                                                       const uint32_t                              numSlewRateDetCfgs);

/**
* \brief Function to get the Tx SRD configuration. 
*
* This function can be used to read back the slew rate detector configuration for selected channel.
* User can select the Tx channel by using adi_adrv904x_SlewRateDetectorCfgRt_t.txChannelMask
* Only one Tx channel should be selected at a time, otherwise this function will throw an error.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the adrv904x data structure
* \param[in,out] txSlewRateDetectorCfg Pointer to SRD configuration to be populated
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxSlewRateDetectorCfgGet(adi_adrv904x_Device_t * const               device,
                                                                       adi_adrv904x_SlewRateDetectorCfgRt_t * const txSlewRateDetectorCfg);

/**
* \brief This function reads SRL statistics.
*        Statistics might either be the maximum slew observed or the number of samples which have been
*        slew rate limited(Maximum = 65536 sample count) depending on the statistics mode.
*        Statistics mode can be selected with adi_adrv904x_TxSlewRateDetectorCfgSet.
* \pre This function is called after the device is initialized. 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the adrv904x data structure
* \param[in] txChannel enum to select Tx channel
* \param[in] clearStats 0:Don't clear statistics after reading / 1: Clear statistics after reading.
* \param[out] statisticsReadback Pointer to memory location where readback value will be written
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxSlewRateStatisticsRead(adi_adrv904x_Device_t * const device,
                                                                       const adi_adrv904x_TxChannels_e txChannel,
                                                                       const uint8_t clearStats,
                                                                       uint16_t * const statisticsReadback);

/**
* \brief This function enables/disables the sample hold for tx pa protection ramp. If 
* sample hold is enabled, last good sample (the sample before pa protection error flag is 
* asserted) is held after FIR1 block. Once the ramp down is completed, signal starts to travel
* in datapath again. This feature can be used to prevent offending sample to reach PA and can be
* useful especially for slew rate events.
* 
* \pre This function is called after the device is initialized. 
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the adrv904x data structure
* \param[in] txChannelMask Tx channel mask to select channel/s for which sample hold will be enabled/disabled
* \param[in] enable 1:Enable sample hold // 0:Disable sample hold

*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxProtectionRampSampleHoldEnableSet(adi_adrv904x_Device_t* const device,
                                                                                  const uint32_t txChannelMask,
                                                                                  const uint8_t enable);

/**
* \brief This function reads back sample hold ramp down configuration for selected Tx channel
* 
* \pre This function is called after the device is initialized. 
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the adrv904x data structure
* \param[in] txChannel Tx channel to read sample hold and ramp down configuration
* \param[out] enable Pointer to sample hold and ramp down configuration
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxProtectionRampSampleHoldEnableGet(adi_adrv904x_Device_t* const device,
                                                                                  const adi_adrv904x_TxChannels_e txChannel,
                                                                                  uint8_t* const enable);


/**
* \brief Function to set the Tx Protection Ramp configuration. This function configures Tx ramp-down in 
* case of Average Power Error/Peak Power Error/SRD Error/Pll Unlock/Dfrm Irq. Each Tx channel has 
* independent ramp-down configuration.
* 
* Ramp down functionality uses Tx attenuation block. This would set to tx attenuation index
* to maximum value to ramp down the tx power.
*
* \pre This function is called after the device is initialized. 
*
* This function takes in an array of adi_adrv904x_ProtectionRampCfgRt_t structures to program the Tx channels.
* Each structure has a separate channel mask that indicates to which Tx channels that particular configuration applies.
* Example usage with two different configurations:
*
* txProtectionRampCfg = (adi_adrv904x_ProtectionRampCfgRt_t *)malloc(2 * sizeof(adi_adrv904x_ProtectionRampCfgRt_t))
*
* txProtectionRampCfg[0].txProtectionRampCfg = txProtectionRampCfg1; // Configuration 1 loaded with desired configuration values
* txProtectionRampCfg[0].txChannelMask = (uint32_t)(ADI_ADRV904X_TX0 | ADI_ADRV904X_TX1 | ADI_ADRV904X_TX2 | ADI_ADRV904X_TX3);
*
* txProtectionRampCfg[1].txProtectionRampCfg = txProtectionRampCfg2; // Configuration 2 loaded with desired configuration values
* txProtectionRampCfg[1].txChannelMask = (uint32_t)(ADI_ADRV904X_TX4 | ADI_ADRV904X_TX5 | ADI_ADRV904X_TX6 | ADI_ADRV904X_TX7);
*
* retVal = adi_adrv904x_TxProtectionRampCfgSet(device, txProtectionRampCfg, 2);
*
* Note:   The function does not check for duplicated channel assignments across cfg structures. 
*         Invalid channel mask values will return an error.
*         Not all Tx channels must be assigned a configuration. If a channel mask is zero that configuration is ignored, ie no
*         error returned.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the adrv904x data structure
* \param[in] txProtectionRampCfg array of structures of type adi_adrv904x_ProtectionRampCfg_t which will configure one or more channels
* \param[in] numProtectionRampCfgs number of configurations passed in the array

*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxProtectionRampCfgSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_ProtectionRampCfgRt_t txProtectionRampCfg[],
                                                                     const uint32_t numProtectionRampCfgs);

/**
* \brief Function to read the Tx Protection Ramp configuration. 
*
* User can select the Tx channel by using adi_adrv904x_TxProtectionRampCfgRt_t.txChannelMask
* Only one Tx channel should be selected at a time, otherwise this function will throw an error.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the adrv904x data structure
* \param[in,out] txProtectionRampCfg Pointer to ramp down configuration to be populated with readback value
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxProtectionRampCfgGet(adi_adrv904x_Device_t* const                device,
                                                                     adi_adrv904x_ProtectionRampCfgRt_t* const   txProtectionRampCfg);

/**
* \brief This function reads average and peak power, average to peak ratio(if enabled),
* average power at the time when last average power error occurred, peak power at the time
* when last peak power error occurred. Power readback will be returned 0 if corresponding
* block(Avg Power/Peak Power/Average to Peak Ratio) is not enabled.
* 
* Average Power in dBFS = 10 * log10(avgPower/2^16)
* Power Error in dBFS = 10 * log10(avgErrorPower/2^16)
* Ratio calculation for avgPeakRatio : avgPower/peakPower = ((avgPeakRatio) / 2^15)

* \pre This function can be called after the device is initialized. 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the adrv904x data structure
* \param[in] txChannel enum to select Tx channel
* \param[out] powerMonitorStatus Pointer to memory location where readback value will be written
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxPowerMonitorStatusGet(adi_adrv904x_Device_t * const device,
                                                                      const adi_adrv904x_TxChannels_e txChannel,
                                                                      adi_adrv904x_TxPowerMonitorStatus_t * const powerMonitorStatus);


 /**
 * \brief Sets the specified Tx Mix NCO to the given frequency, band number, and enable state in the adi_adrv904x_TxNcoMixConfig_t structure.
 *
 * \pre This function can be called after the CPU has been initialized.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param[in] txNcoConfig Pointer to the TX NCO Mix configuration settings
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxNcoShifterSet(adi_adrv904x_Device_t* const device,
                                                              const adi_adrv904x_TxNcoMixConfig_t * const txNcoConfig);

/**
 * \brief Gets TX Mix NCO parameters in adi_adrv904x_TxNcoMixConfigReadbackResp_t structure.
 *
 * \pre This function can be called after the CPU has been initialized, firmware loaded and the TX Mix NCO configured.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param[in,out] txRbConfig Pointer to the TX NCO config read back settings
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxNcoShifterGet(adi_adrv904x_Device_t* const                        device,
                                                              adi_adrv904x_TxNcoMixConfigReadbackResp_t* const    txRbConfig);

/**
* \brief Sets the specified Tx CDUC NCO in the adi_adrv904x_TxCducNcoConfig_t structure.
*
* \pre This function can be called after the CPU has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] txCducNcoConfig Pointer to the TX CDUC NCO configuration settings
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxCducNcoSet(adi_adrv904x_Device_t* const device,
                                                           const adi_adrv904x_TxCducNcoConfig_t * const txCducNcoConfig);

/**
 * \brief Gets TX CDUC NCO parameters in adi_adrv904x_TxCducNcoConfigReadbackResp_t structure.
 *
 * \pre This function can be called after the CPU has been initialized, firmware loaded and the TX CDUC NCO configured.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param[in,out] txRbConfig Pointer to the TX NCO config read back settings
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxCducNcoGet(adi_adrv904x_Device_t* const device,
                                                           adi_adrv904x_TxCducNcoConfigReadbackResp_t* const txRbConfig);

/** 
* \brief Set Fractional Delay via pFIR (Used in CTC Mode 2)
* 
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in]  txChan  - Tx channel
* \param[in]  fracDelay - fractional Delay (0:63)
*  
* \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful 
*/ 

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxPfirFracDelaySet(adi_adrv904x_Device_t* const device,
                                                                 const uint8_t txChan,
                                                                 const uint8_t fracDelay);
/**
 * \brief Sets the specified Tx Test NCO to the given frequency, band number, attenuation control, and enable state 
 *  in the adi_adrv904x_TxTestNcoConfig_t structure.
 *
 * \pre This function can be called after the CPU has been initialized.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param[in] txNcoConfig Pointer to the TX NCO Test config settings
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxTestToneSet(adi_adrv904x_Device_t* const                    device,
                                                            const adi_adrv904x_TxTestNcoConfig_t * const    txNcoConfig);

/**
 * \brief Gets TX Test NCO parameters in adi_adrv904x_TxTestNcoConfigReadbackResp_t structure.
 *
 * \pre This function can be called after the CPU has been initialized, firmware loaded and the TX Test NCO configured.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param[in,out] txRbConfig Pointer to the RX NCO config read back settings
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxTestToneGet(adi_adrv904x_Device_t* const                        device,
                                                            adi_adrv904x_TxTestNcoConfigReadbackResp_t* const   txRbConfig);

/**
* \brief Function to configure Tx decimated power measurement block.
*
* This function can be used to configure decimated power measurements for Tx channel.
* Measurement point and measurement duration can be selected by user through configuration structure.
* This function configures Tx dec power block. 
* 
* Note: The function does not check for duplicated channel assignments across cfg structures.
* Invalid channel mask values will return an error.

* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] txDecPowerCfg array of structures of type adi_adrv904x_TxDecimatedPowerCfg_t which will configure one or more channels
* \param[in] numOfDecPowerCfgs number of configurations passed in the array
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxDecimatedPowerCfgSet(adi_adrv904x_Device_t * const device,
                                                                     adi_adrv904x_TxDecimatedPowerCfg_t txDecPowerCfg[],
                                                                     const uint32_t numOfDecPowerCfgs);

/**
* \brief Function to read back Tx decimated power measurement configuration
*
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] txChannel Tx channel selection to read decimated power block configuration
* \param[out] txDecPowerCfg Decimated power configuration of selected channel/dec power block
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxDecimatedPowerCfgGet(adi_adrv904x_Device_t * const device,
                                                                     const adi_adrv904x_TxChannels_e txChannel,
                                                                     adi_adrv904x_TxDecimatedPowerCfg_t * const txDecPowerCfg);


/**
* \brief Function to readback the decimated power measurement for selected Tx channel.
* Power readback value is an unsigned 8 bit value with 0.25dB resolution.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] txChannel Channel selection to read decimated power measurement
* \param[out] powerReadBack Pointer to power measurement readback value
* \param[out] powerPeakReadBack Pointer to power measurement readback value
* 
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxDecimatedPowerGet(adi_adrv904x_Device_t * const device,
                                                                  const adi_adrv904x_TxChannels_e txChannel,
                                                                  uint8_t * const powerReadBack,
                                                                  uint8_t * const powerPeakReadBack);

/**
 * \brief Sets the specified Tx Atten Phase in the adi_adrv904x_TxAttenPhaseCfg_t structure.
 *
 * \pre This function can be called after the CPU has been initialized.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param[in] txAttenPhaseCfg Pointer to the TX Atten Phase configuration settings
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxAttenPhaseSet(adi_adrv904x_Device_t* const device,
                                                              const adi_adrv904x_TxAttenPhaseCfg_t * const txAttenPhaseCfg);

/**
 * \brief Gets TX Atten Phase in adi_adrv904x_TxAttenPhaseCfg_t structure.
 *
 * \pre This function can be called after the CPU has been initialized, firmware loaded and the TX Atten Phase configured.
 *
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param[in,out] txRbConfig Pointer to the TX Atten Phase config read back settings
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxAttenPhaseGet(adi_adrv904x_Device_t* const                        device,
                                                              adi_adrv904x_TxAttenPhaseCfg_t* const    txRbConfig);


/**
* \brief Configures Discontinuous Transmit (DTX) features 
*
* DTX provides the ability to shut down the clock to save power through three different mode: Automatic mode, SPI controlled mode, or Pin Control mode.
* dtx_mode_enable	Operation	          Notes
* 2'b00	            DTx mode disable	
* 2'b01	            Automatic mode.	      Triggers the power down stream on observing a specified number of zero samples (dtx_zero_counter) on Tx input data. Power up stream is triggered as soon as a non zero sample is seen. The number of samples defined is at the slower of the 2 DUC in clock rates.
* 2'b10	            SPI controlled mode	  In this mode, power down is triggered on setting the dtx_force bitfield. Clearing that bit triggers the power up stream.
* 2'b11	            Pin controlled mode	  In this mode, a GPIO pin specified by the dtx_force_gpio_select controls Power up/down. Rising edge on the pin triggers the power down stream and a falling edge triggers a power up stream
*
* Configure the DTX registers. Set the DTX Mode, DTX zero counter for the Tx channels
*
* The DTX configs can be broadcast / multicast based
* on the channel mask parameter in adi_adrv904x_DtxCfg_t structure.
*
*             dtxCfg[i]->txChannelMask | Tx Channels to set DTX
*     ---------------------------------|--------------------------------------
*                              bit[0]  |  1 = Tx0 DTX is updated
*                                      |
*                              bit[1]  |  1 = Tx1 DTX is updated
*                                      |
*                              bit[2]  |  1 = Tx2 DTX is updated
*                                      |
*                              bit[3]  |  1 = Tx3 DTX is updated
*                                      |
*                              bit[4]  |  1 = Tx4 DTX is updated
*                                      |
*                              bit[5]  |  1 = Tx5 DTX is updated
*                                      |
*                              bit[6]  |  1 = Tx6 DTX is updated
*                                      |
*                              bit[7]  |  1 = Tx7 DTX is updated

*
*     Eg: To update DTX for channels Tx3 and Tx0, the dtxCfg[i]->txChannelMask
*         should be set to 0x00000009
*
*     Eg: To update DTX for all Tx channels, the dtxCfg[i]->txChannelMask
*         should be set to 0x000000FF
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \pre This function may be called any time after device initialization
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] dtxCfg Array of the channelized DTX settings
* \param[in] numDtxCfgs Size of dtxCfg array
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DtxCfgSet(adi_adrv904x_Device_t* const device,
                                                        const adi_adrv904x_DtxCfg_t  dtxCfg[],
                                                        const uint32_t               numDtxCfgs);
/**
* \brief Reads the Discontinuous Transmit (DTX) settings for the requested Tx channel
*
* This function reads the DTX settings for the requested
* channel.
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
*
* \pre This function may be called any time after device initialization
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] txChannel adi_adrv904x_TxChannels_e enum type to select channel from.
* \param[out] dtxCfg Pointer to the DTX structure for the requested channel.
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*
*
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DtxCfgGet(adi_adrv904x_Device_t* const    device,
                                                        const adi_adrv904x_TxChannels_e txChannel,
                                                        adi_adrv904x_DtxCfg_t* const    dtxCfg);
/**
* \brief Configures Discontinuous Transmit (DTX) GPIO features
*
*
* Given design constraints and that GPIOs are a shared resource there
* are limitations on the set of valid configurations.  This API only
* configures the GPIO settings.  For all other settings please use
* the adi_adrv904x_DtxCfgSet() API.
*
* The channel should be selected through dtxGpioCfg structure
* dtxGpioCfg.dtxGpioTxSel[0] = GPIO for channel 0;
*  ...........
* dtxGpioCfg.dtxGpioTxSel[7] = GPIO for channel 7;
*
* Errors will be returned under the following conditions:
*  - The requested GPIO is being used by another resource
*  - The configuration requested is invalid
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] dtxGpioCfg Pointer to the DTX GPIO structure
*
* 
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*
*
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DtxGpioCfgSet(adi_adrv904x_Device_t* const     device,
                                                            const adi_adrv904x_DtxGpioCfg_t* dtxGpioCfg);

/**
* \brief Reads the Discontinuous Transmit (DTX) for the requested Tx channel
*
* This function reads the DTX GPIO settings for the requested
* channel.
* The channel should be selected through dtxGpioCfg structure
* dtxGpioCfg.dtxGpioTxSel[0] = GPIO for channel 0;
*  ...........
* dtxGpioCfg.dtxGpioTxSel[7] = GPIO for channel 7;
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] dtxGpioCfg Pointer to the DTX GPIO structure
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DtxGpioCfgGet(adi_adrv904x_Device_t* const     device,
                                                            adi_adrv904x_DtxGpioCfg_t* const dtxGpioCfg);



/**
* \brief Set the Discontinuous Transmit (DTX) Force bit for the requested Tx channels. 
*
* In SPI Mode, power down is triggered on setting the dtx_force bitfield. Clearing that bit triggers the power up stream.
* 
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] txChannelMask Tx Channels for which the DTX Force to be update.
* \param[in] dtxForce DTX status bit to be read
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DtxForceSet(adi_adrv904x_Device_t* const device,
                                                          const uint32_t txChannelMask,
                                                          const uint8_t dtxForce);

/**
* \brief Reads the Discontinuous Transmit (DTX) status for the requested Tx channel
*
* This function reads the DTX status for the requested
* channel.
* 
* Status bits logic
* bit0: set by dtx_power_down; clear by dtx_power_up ( or cleared by spi_clear_dtx_status_bit0, not required)
* bit1: set by dtx_power_down; clear by spi_clear_dtx_status_bit1 (executed at the end of power up stream)
*
* \dep_begin
* \dep{device->halDevInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] txChannel Tx Channels for which the DTX Status to read.
* \param[out] dtxStatus DTX status bit to be read
*
* \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DtxStatusGet(adi_adrv904x_Device_t* const device,
                                                           const adi_adrv904x_TxChannels_e txChannel,
                                                           uint8_t * const dtxStatus);



/**
 * \brief       Set the Tx TSSI or ORx TSSI power measurement configuration
 * This function sets the configuration of the TSSI power measurement.
 * Must use adi_adrv904x_DfePwrMtrTssiEnable() to enable the meter.
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in]  channelMask     - Tx or ORx channel number mask; multiple channels allowed
 * \param [in]  pPwrMtrTssiCfg  - pointer to power meter config structure
 * \param [in]  meter           - TSSI Tx (1 or 2) or TSSI ORx
 * \param [in]  source          - source type (only applies to TSSI Tx meters)
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrTssiConfigSet(adi_adrv904x_Device_t* const                 device,
                                                                     const uint32_t                               channelMask,
                                                                     const adi_adrv904x_DfeSvcDfePwrMtrTssiCfg_t* pPwrMtrTssiCfg,
                                                                     const adi_adrv904x_DfeSvcDfePwrMtrTssi_e     meter,
                                                                     const adi_adrv904x_DfeSvcDfePwrMtrTssiSrc_e  source);
/**
 * \brief       Get the Tx TSSI or ORx TSSI power measurement configuration
 * This function gets the configuration of the TSSI power measurement
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in]  channelSel      - Tx or ORx channel number mask; only one channel allowed
 * \param [out] pPwrMtrTssiCfg  - pointer to power meter config structure
 * \param [in]  meter           - TSSI Tx (1 or 2) or TSSI ORx
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrTssiConfigGet(adi_adrv904x_Device_t* const             device,
                                                                     const uint32_t                           channelSel,
                                                                     adi_adrv904x_DfeSvcDfePwrMtrTssiCfg_t*   pPwrMtrTssiCfg,
                                                                     const adi_adrv904x_DfeSvcDfePwrMtrTssi_e meter);

/**
 * \brief Set Post CFR gain for selected tx channels.
 * Linear gain value configured in the HW is 2.6 fixed point value with 0.015625 resolution. User is advised 
 * to readback the gain with adi_adrv904x_PostCfrGainGet to see the actual value configured on HW due to loss
 * of precision
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in] txChannelMask - Tx channel mask, one or multiple Tx channels are allowed
 * \param [in] gain_milli    - Linear CFR gain, Range [0-4000] which corresponds to 0-4.0 linear gain
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_PostCfrGainSet(adi_adrv904x_Device_t* const device,
                                                             const uint32_t txChannelMask,
                                                             const uint32_t gain_milli);

/**
 * \brief Get Post CFR gain for selected tx channels.
 * Linear gain value configured in the HW is 2.6 fixed point value with 0.015625 resolution. Readback
 * value might be slightly different from gain value set with adi_adrv904x_PostCfrGainSet
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in]  txChannelSel     - Tx channel selection. Only one Tx channel should be selected
 * \param [out] gain_milli       - Pointer to CFR gain to be populated
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_PostCfrGainGet(adi_adrv904x_Device_t* const device,
                                                             const uint32_t txChannelSel,
                                                             uint32_t * const gain_milli);

/**
 * \brief       Enable Tx TSSI or ORx TSSI power measurement
 * This function enables the DFE TSSI power measurement
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in]  channelMask - Tx or ORx channel number mask; multiple channels allowed
 * \param [in]  enable      - enable/disable power meter
 * \param [in]  meter       - TSSI Tx (1 or 2) or TSSI ORx
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrTssiEnable(adi_adrv904x_Device_t* const             device,
                                                                  const uint32_t                           channelMask,
                                                                  const uint8_t                            enable,
                                                                  const adi_adrv904x_DfeSvcDfePwrMtrTssi_e meter);

/**
 * \brief       Enable Tx TSSI 1 and/or TSSI 2 and ORx TSSI power measurement (for CLGC)
 * This function enables the DFE TSSI power measurement for ORx and either or both TSSI 1 and TSSI 2
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in]  txChannelMask - Tx channel mask; multiple channels allowed
 * \param [in]  enable        - enable/disable power meter
 * \param [in]  useTxMeter1   - Use Tx TSSI 1
 * \param [in]  useTxMeter2   - Use Tx TSSI 2
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrClgcTssiEnable(adi_adrv904x_Device_t* const device,
                                                                      const uint32_t               txChannelMask,
                                                                      const uint8_t                enable,
                                                                      const uint8_t                useTxMeter1,
                                                                      const uint8_t                useTxMeter2);

/**
 * \brief       Manually start the power measurement
 * This function starts the DFE TSSI power measurement manually
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in]  channelMask - Tx or ORx channel number mask; multiple channels allowed
 * \param [in]  start       - 1: unpause or start measuring, 0:pause or stop measuring
 * \param [in]  meter       - TSSI Tx (1 or 2) or TSSI ORx
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrTssiManualStart(adi_adrv904x_Device_t* const             device,
                                                                       const uint32_t                           channelMask,
                                                                       const uint8_t                            start,
                                                                       const adi_adrv904x_DfeSvcDfePwrMtrTssi_e meter);
/**
 * \brief       Readback dB and linear power measurement
 * This function reads back the DFE power measurement and if the set threshold was exceeded
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in]  channelSel - Tx or ORx channel number mask (only one channel allowed for readback)
 * \param [in]  meter      - TSSI Tx (1 or 2) or TSSI ORx
 * \param [out] pPwrMeas   - Pointer to power meter read back values, dB and linear
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrTssiReadBack(adi_adrv904x_Device_t* const                  device,
                                                                    const uint32_t                                channelSel,
                                                                    const adi_adrv904x_DfeSvcDfePwrMtrTssi_e      meter,
                                                                    adi_adrv904x_DfeSvcDfePwrMtrTssiMeasResult_t* pPwrMeas);

/**
 * \brief       Readback power measurements for CLGC/TPC
 * This function reads back the DFE power measurements for CLGC/TPC
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in]  txChannelSel - Tx channel number mask (only one channel allowed for readback)
 * \param [in]  useTxMeter1  - True if use TSSI Tx 1
 * \param [in]  useTxMeter2  - True if use TSSI Tx 2
 * \param [out] pPwrMeasTx1  - Pointer to power meter read back for Tx TSSI 1
 * \param [out] pPwrMeasTx2  - Pointer to power meter read back for Tx TSSI 2
 * \param [out] pPwrMeasOrx  - Pointer to power meter read back for Orx TSSI
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrClgcTssiReadBack(adi_adrv904x_Device_t* const device,
                                                                        const uint32_t               txChannelSel,
                                                                        const uint8_t                useTxMeter1,
                                                                        const uint8_t                useTxMeter2,
                                                                        uint64_t                     *pPwrMeasTx1,
                                                                        uint64_t                     *pPwrMeasTx2,
                                                                        uint64_t                     *pPwrMeasOrx);

/**
 * \brief DPD input power meter configuration set
 *
 * The DPD input power meter can be used by itself to measure the pre-DPD Tx
 * power for switching DPD model or can be used along with DPD output power
 * meter to detect sudden DPD gain variation. Use the Enable/pause command to start the measurement.
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in] txChannelMask - Tx channel number mask; multiple channels allowed
 * \param [in] pPwrMtrDpdIn  - The pointer to input power meter configuration structure
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrDpdInConfigSet(adi_adrv904x_Device_t* const                device,
                                                                      const uint32_t                              txChannelMask,
                                                                      const adi_adrv904x_DfeSvcDfePwrMtrDpdCfg_t* pPwrMtrDpdIn);

/**
 * \brief DPD input power meter configuration get
 *
 * Get the DPD Input power measure configuration
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in] txChannelSel - Tx channel number mask; single channel only
 * \param [out] pPwrMtrDpdIn - The pointer to input power meter configuration structure
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrDpdInConfigGet(adi_adrv904x_Device_t* const          device,
                                                                      const uint32_t                        txChannelSel,
                                                                      adi_adrv904x_DfeSvcDfePwrMtrDpdCfg_t* pPwrMtrDpdIn);

/**
 * \brief DPD output power meter configuration set
 *
 * The DPD output power meter is usually used along with DPD input power meter
 * to detect sudden DPD gain variation. Use the Enable/pause command to start the measurement.
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in] txChannelMask - Tx channel mask; multiple channels allowed
 * \param [in] pPwrMtrDpdOut - The pointer to output power meter configuration structure
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrDpdOutConfigSet(adi_adrv904x_Device_t* const                device,
                                                                       const uint32_t                              txChannelMask,
                                                                       const adi_adrv904x_DfeSvcDfePwrMtrDpdCfg_t* pPwrMtrDpdOut);

/**
 * \brief DPD output power meter configuration get
 *
 * Get the DPD output power measure configuration
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in] txChannelSel - Tx channel number mask; single channel only
 * \param [out] pPwrMtrDpdOut - The pointer to output power meter configuration structure
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrDpdOutConfigGet(adi_adrv904x_Device_t* const          device,
                                                                       const uint32_t                        txChannelSel,
                                                                       adi_adrv904x_DfeSvcDfePwrMtrDpdCfg_t* pPwrMtrDpdOut);

/**
 * \brief DPD input power meter enable/disable
 *
 * Use this command AFTER setting the config.
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in] txChannelMask - Tx channel number mask; multiple channels allowed
 * \param [in] enable  - 1 to enable; 0 to disable
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrDpdInEnable(adi_adrv904x_Device_t* const device,
                                                                   const uint32_t               txChannelMask,
                                                                   const uint8_t                enable);
/**
 * \brief DPD output power meter enable/disable
 *
 * Use this command AFTER setting the config.
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in] txChannelMask - Tx channel number mask; multiple channels allowed
 * \param [in] enable  - 1 to enable; 0 to disable
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrDpdOutEnable(adi_adrv904x_Device_t* const device,
                                                                    const uint32_t               txChannelMask,
                                                                    const uint8_t                enable);
/**
 * \brief DPD In/Out power measurement readback
 *
 * enable dpd out power meter
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in] txChannelSel - Tx channel mask (only one channel allowed for readback)
 * \param [out] pPwrDpdInOut - Pointer to DPD power measurement readback structure
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrDpdInOutReadBack(adi_adrv904x_Device_t* const              device,
                                                                        const adi_adrv904x_TxChannels_e           txChannelSel,
                                                                        adi_adrv904x_DfeSvcDfePwrMtrDpdResults_t* pPwrDpdInOut);
/**
 * \brief Tx carrier power measurement config set
 *
 * Set the configuration but do not enable. Use adi_adrv904x_DfePwrMtrTxCarrierMeasEnable for that.
 *
 * How to use example:
 *  Configure CH3 and CH4, carriers 1 and 2 only.
 *      channelMask = 0x18 (11000 b)
 *      carrierMask = 0x6  (0110b)
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in] txCarrierMask - Mask of channels and carriers to set - 8 bit masks (multiple channels and carriers allowed)
 * \param [in] pPwrTxCarrierCfg - Pointer to carrier power measurement config set structure
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrTxCarrierCfgSet(adi_adrv904x_Device_t* const                         device,
                                                                       const adi_adrv904x_TxCarrierMask_t* const            txCarrierMask,
                                                                       const adi_adrv904x_DfeSvcDfeTxCarrierPowerMeasCfg_t* pPwrTxCarrierCfg);
/**
 * \brief Tx carrier power measurement config get
 *
 * How to use example:
 *  Get the config for CH4, carrier 0.
 *      channelMask = 0x10 (10000 b)
 *      carrierMask = 0x1
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in] txCarrierSel - Mask of channels and carriers to set - 8 bit masks (only one channel and carrier allowed)
 * \param [out] pPwrTxCarrrierCfgResp - Pointer to carrier power measurement config response structure
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrTxCarrierCfgGet(adi_adrv904x_Device_t* const                   device,
                                                                       const adi_adrv904x_TxCarrierMask_t* const      txCarrierSel,
                                                                       adi_adrv904x_DfeSvcDfeTxCarrierPowerMeasCfg_t* pPwrTxCarrrierCfgResp);

/**
 * \brief Tx carrier power measurement enable/disable
 *
 * How to use example:
 *  Enable/disable measurements on On CH1 and CH2, enable carrier 1 and disable carrier 0:
 *      txCarrierMask.channelMask = 0x3
 *      txCarrierMask.carrierMask = 0x3
 *      enable = 1 (enable the given channels and carriers from txCarrierMask)
 * 
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in] txCarrierMask - Mask of channels and carriers to set - 8 bit masks (multiple channels and carriers allowed)
 * \param [in] enable - Enable flag
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrTxCarrierMeasEnable(adi_adrv904x_Device_t* const              device,
                                                                           const adi_adrv904x_TxCarrierMask_t* const txCarrierMask,
                                                                           const uint8_t                             enable);

/**
 * \brief Tx carrier power measurement pause/resume (start)
 *
 * How to use example:
 *  Start or Unpause CH2,CH8: carrier 1:
 *      txCarrierMask.channelMask = 0x82
 *      txCarrierMask.carrierMask = 0x2
 *      start = 1 (start or unpause the given channels and carriers from txCarrierMask)
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in] txCarrierMask - Mask of channels and carriers to set - 8 bit masks (multiple channels and carriers allowed)
 * \param [in] start  - start flag
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrTxCarrierManualStart(adi_adrv904x_Device_t* const              device,
                                                                            const adi_adrv904x_TxCarrierMask_t* const txCarrierMask,
                                                                            const uint8_t                             start);

/**
 * \brief Tx carrier power measurement get results
 *
 * How to use example:
 *  Get measurement results for CH3, carrier 0:
 *      channelMask = 0x8 (1000b)
 *      carrierMask = 0x1
 *
 * \param [in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param [in] txCarrierSel - Mask of channels and carriers to set - 8 bit masks (only one channel and carrier allowed)
 * \param [out] pPwrTxCarrrierMeasResult - Pointer to structure that holds the results
 *
 * \retval adi_adrv904x_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrTxCarrierMeasGet(adi_adrv904x_Device_t* const                      device,
                                                                        const adi_adrv904x_TxCarrierMask_t* const         txCarrierSel,
                                                                        adi_adrv904x_DfeSvcDfeTxCarrierPowerMeasResult_t* pPwrTxCarrrierMeasResult);

/**
* \brief Configures Hard clipper configuration for selected channel/s
* 
* Use hardClipperConfig -> hardClipperSel Select PostAtten or PreDpd set the configuration for hc hardClipperEnable/hardClipperThreshold.
* 
* \pre This function is called after the device has been initialized. User can configure PostAtten/PreDpd hard clipper
* for multiple channels at the same time.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     hardClipperConfig - Pointer to hard clipper configuration struct to be applied
* \param[in]     hcTxChannelMask - One or multiple Tx channel selection to apply hard clipper config
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_HardClipperConfigSet(adi_adrv904x_Device_t* const device,
                                                                   const adi_adrv904x_HardClipperConfig_t * const hardClipperConfig,
                                                                   const uint32_t hcTxChannelMask);

/**
* \brief Reads hard clipper configuration of selected channel
* 
* \pre This function is called after the device has been initialized.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in,out] hardClipperConfig - Pointer to readback hard clipper configuration struct
* \param[in]     hcTxChannelSel - Tx channel selection to read hard clipper config
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_HardClipperConfigGet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_HardClipperConfig_t * const hardClipperConfig,
                                                                   const uint32_t hcTxChannelSel);

/**
* \brief Read Hard Clipper statistics of selected channel
* 
* \pre This function is called after the device has been initialized. User should always read the PostAtten and PreDpd Hard Clipper
* statistics.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     txChannelSel - Tx channel selection to read statistics
* \param[out]    hardClipperStat - Number of peaks clipped at hard clipper
* \param[in]     clearStats - 1:Clear statistics upon reading // 0: Don't clear
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_HardClipperStatisticsGet(adi_adrv904x_Device_t* const device,
                                                                       const adi_adrv904x_TxChannels_e txChannelSel,
                                                                       adi_adrv904x_HardClipperStatistics_t * const hardClipperStat,
                                                                       uint8_t clearStats);

/**
* \brief Adjust Tx Carrier Gain in mdB. Valid range is -90dB to +36dB.
*
* Note: Gain adjustment is converted to a linear gain value in 7.16 format 
* internally, which will affect actual value applied. It is suggested to
* readback the actual value applied in this function by calling 
* adi_adrv904x_TxCarrierGainAdjustGet() afterward.
* 
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] txCarrierMask Tx Carrier selection from which to set the gain adjustment
* \param[in] gain_mdB Gain adjustment to make to selected Tx Carriers. Signed 32bit value, resolution 1 mdB
* 
* \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxCarrierGainAdjustSet(adi_adrv904x_Device_t * const              device,
                                                                     const adi_adrv904x_TxCarrierMask_t * const txCarrierMask,
                                                                     const int32_t                              gain_mdB);   

/**
 * \brief Readback current Tx Carrier Gain Adjustment in mdB. Expected range is -90dB to +36dB.
 * 
 * \dep_begin
 * \dep{device->common.devHalInfo}
 * \dep_end
 *
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
 * \param[in] txCarrierSel Tx Carrier selection from which to get the gain adjustment
 * \param[out] gain_mdB Pointer to location to store current gain adjustment for selected Tx Carrier. Signed 32bit value, resolution 1 mdB.
 * 
 * \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
 */
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxCarrierGainAdjustGet(adi_adrv904x_Device_t * const              device,
                                                                     const adi_adrv904x_TxCarrierMask_t * const txCarrierSel,
                                                                     int32_t * const                            gain_mdB);

/** 
* \brief Set the Tx carrier band attenuation 
* 
* \note This function should not be called when an alarm is active because the attenuation may not be updated correctly.
*
* \dep_begin 
* \dep{device->common.devHalInfo} 
* \dep_end 
* 
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings 
* \param[in] txBandMask - pointer to the Tx channel and band mask 
* \param[in] atten_mdB - Band Attenuation for selected Tx Carrier. Unsigned 32bit value, resolution 1 mdB.
*  
* \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful 
*/ 
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxCarrierBandAttenSet(adi_adrv904x_Device_t * const           device, 
                                                                    const adi_adrv904x_TxBandMask_t * const txBandMask,
                                                                    const uint32_t atten_mdB);

/** 
 * \brief Get the Tx carrier band attenuation 
 * 
 * \dep_begin 
 * \dep{device->common.devHalInfo} 
 * \dep_end 
 * 
 * \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings 
 * \param[in] txBandSel - pointer to the given Tx channel and band mask (only one Tx, one band allowed) 
 * \param[out] atten_mdB Pointer to location to store current attenuation for selected Tx Carrier. Unsigned 32bit value, resolution 1 mdB.
 *
 * \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful 
 */ 
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxCarrierBandAttenGet(adi_adrv904x_Device_t * const      device, 
                                                                    const adi_adrv904x_TxBandMask_t * const txBandSel,
                                                                    uint32_t * const atten_mdB);

/** 
* \brief Set the Tx Post DPD Digital Attenuation
* 
* \dep_begin 
* \dep{device->common.devHalInfo} 
* \dep_end 
* 
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings 
* \param[in] txDpdMask - pointer to the Tx channel and id mask
* \param[in] attenParams - Data structure to hold the Post DPD atten parameters.
*  
* \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful 
*/          
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_PostDpdAttenSet(adi_adrv904x_Device_t * const             device,
                                                              const adi_adrv904x_TxDpdMask_t * const    txDpdMask,
                                                              const adi_adrv904x_PostDpdAtten_t * const attenParams);
/** 
* \brief Get the Tx Post DPD Digital Attenuation
* 
* \dep_begin 
* \dep{device->common.devHalInfo} 
* \dep_end 
* 
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings 
* \param[in] txDpdSel - pointer to the Tx channel and id mask  (only one Tx, one id allowed)
* \param[out] attenParams - Data structure to hold the Post DPD atten parameters.
*  
* \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful 
*/                                                                        
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_PostDpdAttenGet(adi_adrv904x_Device_t * const           device,
                                                              const adi_adrv904x_TxDpdMask_t * const  txDpdSel,
                                                              adi_adrv904x_PostDpdAtten_t * const     attenParams);

/**
* \brief Write to Tx playback RAM.
* 
* This function is called after the device has been initialized. It's used to write a dataset to TX
* playback RAM. It's users responsibility to make sure data capture isn't enabled while this API writes
* a new data set to playback RAM.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device settings data structure
* \param[in]     txChannelMask - One or multiple Tx channel selection to apply playback data
* \param[in]     bandSelect - Select the TX DUC band
* \param[in]     playbackData - Pointer to playback data structure to be written to TX playback RAM
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxPlaybackDataWrite(adi_adrv904x_Device_t* const device,
                                                                  const uint32_t txChannelMask,
                                                                  const uint8_t  bandSelect,
                                                                  const adi_adrv904x_TxPlaybackData_t* const playbackData);

/**
* \brief Read from Tx playback RAM.
* 
* This function is called after the device has been initialized. It's used to read the dataset stored
* at TX playback RAM. It's users responsibility to make sure data capture isn't enabled while this API reads
* from playback RAM.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device settings data structure
* \param[in]     txChannelSel - Tx channel selection to read playback data, only one tx channel needs to be selected
* \param[in]     bandSelect - Select the TX DUC band
* \param[out]    playbackData - Pointer to playback data structure to be read from TX playback RAM
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxPlaybackDataRead(adi_adrv904x_Device_t* const device, 
                                                                 const adi_adrv904x_TxChannels_e txChannelSel,
                                                                 const uint8_t  bandSelect,
                                                                 adi_adrv904x_TxPlaybackData_t* const playbackData);

/**
* \brief Start data playback from Tx playback RAM.
* 
* This function is called after the device has been initialized. It's used to start a data playback from Tx playback RAM.
* It's users responsibility to make sure data capture isn't enabled while this API reads from playback RAM.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device settings data structure
* \param[in]     txChannelSel - Tx channel selection to read playback data, only one tx channel needs to be selected
* \param[in]     bandSelect   - Select the TX DUC band
* \param[in]     dataRate_kHz - Select the data rate for the playback. The playback happens at 1/2 the programmed rate
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxPlaybackStart(adi_adrv904x_Device_t* const device, 
                                                              const adi_adrv904x_TxChannels_e txChannelSel,
                                                              const uint8_t  bandSelect,
                                                              const uint32_t dataRate_kHz);

/**
* \brief Stop data playback from Tx playback RAM.
* 
* This function is called after the device has been initialized. It's used to stop a data playback from Tx playback RAM.
* It's users responsibility to make sure data capture isn't enabled while this API reads from playback RAM.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device settings data structure
* \param[in]     txChannelSel - Tx channel selection to read playback data, only one tx channel needs to be selected
* \param[in]     bandSelect - Select the TX DUC band
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxPlaybackStop(adi_adrv904x_Device_t* const device, 
                                                             const adi_adrv904x_TxChannels_e txChannelSel,
                                                             const uint8_t  bandSelect);


/** 
* \brief Reconfigure Tx carriers dynamically without reinitialization. Wrapper for adi_adrv904x_TxDynamicCarrierReconfigure to select channel filter coefficients
*           provided by ADI
* 
*           Note: The API currently only uses the first profile in txCarrierConfigs.carrierCfgs input struct.
* 
* \pre Part must be put in the appropriate state for this function to work as intendend. At a minimum the following steps must have been performed:
*           Disable tracking cals
*           Disable PAs
*           Disable Tx/Rx/ORx channels. GPIOs are not toggling
* 
* 
* \post After this function is called at a minimum the following steps must be performed before the device can be used as intended:
*           Provide new CFR pulse/config thru API
*           Change RFLO
*           Re- Enable channels then tracking cals
*           Re-Enable PAs
*           
* \dep_begin 
* \dep{device->common.devHalInfo} 
* \dep_end 
* 
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings 
* \param[in] jesdCfg - Pointer that holds the updated JESD settings to accomdate the new carrier settings
* \param[in] txCarrierConfigs - Tx Carrier Reconfigure Settings. Struct may be modified by API for endianness before being passed to firmware.
* \param[in] txCarrierChannelFilterApplicationSel - Tx Carrier Channel Filter application select for each carrier in each profile
* \param[in] numCarrierProfiles number of profiles passed in the arrays. Max is four.
*
* \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful 
*/ 
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxDynamicCarrierReconfigureWithFilterSelect(  adi_adrv904x_Device_t* const                                device,
                                                                                            adi_adrv904x_CarrierJesdCfg_t* const                        jesdCfg,
                                                                                            adi_adrv904x_CarrierRadioCfg_t                              txCarrierConfigs[],
                                                                                            const adi_adrv904x_CarrierChannelFilterApplicationSel_t     txCarrierChannelFilterApplicationSel[],
                                                                                            const uint32_t                                              numCarrierProfiles );

/** 
* \brief    Reconfigure Tx carriers dynamically without reinitialization. 
* 
*           Note: The API currently only uses the first profile in txCarrierConfigs.carrierCfgs input struct.
* 
* \pre Part must be put in the appropriate state for this function to work as intendend. At a minimum the following steps must have been performed:
*           Disable tracking cals
*           Disable PAs
*           Disable Tx/Rx/ORx channels. GPIOs are not toggling
* 
* 
* \post After this function is called at a minimum the following steps must be performed before the device can be used as intended:
*           Provide new CFR pulse/config thru API
*           Change RFLO
*           Re- Enable channels then tracking cals
*           Re-Enable PAs
*           
* \dep_begin 
* \dep{device->common.devHalInfo} 
* \dep_end 
* 
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings 
* \param[in] jesdCfg - Pointer that holds the updated JESD settings to accomdate the new carrier settings
* \param[in] txCarrierConfigs - Tx Carrier Reconfigure Settings. Struct may be modified by API for endianness before being passed to firmware.
* \param[in] txCarrierChannelFilter - Tx Carrier Channel Filter Settings. Struct may be modified by API for endianness before being passed to firmware.
* \param[in] numCarrierProfiles number of profiles passed in the arrays. Max is four.
*
* \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful 
*/ 
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxDynamicCarrierReconfigure(  adi_adrv904x_Device_t* const                    device,
                                                                            adi_adrv904x_CarrierJesdCfg_t* const            jesdCfg,
                                                                            adi_adrv904x_CarrierRadioCfg_t                  txCarrierConfigs[],
                                                                            adi_adrv904x_ChannelFilterCfg_t                 txCarrierChannelFilter[],
                                                                            const uint32_t                                  numCarrierProfiles);

/** 
* \brief Retrieve the current tx Carrier settings for the selected channel
*           
* \dep_begin 
* \dep{device->common.devHalInfo} 
* \dep_end 
* 
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings 
* \param[in] txChannel - Tx Channel to be retrieved
* \param[out] txCarrierConfigs - Tx Carrier Reconfigure Settings
* \param[out] carrierRuntimeSettings - Tx Carrier Runtime Settings
*
* \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful 
*/ 
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxCarrierSettingsGet( adi_adrv904x_Device_t* const    device,
                                                                    const uint32_t txChannel,
                                                                    adi_adrv904x_CarrierRadioCfg_t* const txCarrierConfigs,
                                                                    adi_adrv904x_CarrierRuntime_t* const  carrierRuntimeSettings);


/** 
* \brief Retrieve the current tx jesd settings
*
* \dep_begin 
* \dep{device->common.devHalInfo} 
* \dep_end 
* 
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[out] txCarrierJesdCfg - Tx Carrier Reconfigure Settings
*
* \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful 
*/ 
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxCarrierJesdCfgGet(  adi_adrv904x_Device_t* const    device,
                                                                    adi_adrv904x_CarrierJesdCfg_t* const txCarrierJesdCfg);

/**
* \brief Retrieve the current tx Carrier delay settings for the selected channel
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure containing settings
* \param[in] txChannel adi_adrv904x_TxChannels_e enum type to read latency values
* \param[out] txCarrierLatencyCfg - Tx Carrier delay settings 
*
* \retval adi_common_ErrAction_e - ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxCarrierLatencyGet(  adi_adrv904x_Device_t* const device,
                                                                    const adi_adrv904x_TxChannels_e txChannel,
                                                                    adi_adrv904x_CarrierReconfigLatencyCfg_t* const txCarrierLatencyCfg);
#endif /* _ADI_ADRV904X_TX_H_ */

