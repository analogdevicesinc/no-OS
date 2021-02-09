/**
 * \file
 * \brief Contains ADRV9001 receive related function prototypes for adi_adrv9001_rx.c
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2015 - 2018 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_RX_H_
#define _ADI_ADRV9001_RX_H_

#include "adi_adrv9001_types.h"
#include "adi_adrv9001_rx_types.h"
#include "adi_common_error_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Programs the gain table settings for Rx channels.
 *
 * This function can be called by the user to load a custom gain table or
 * to reconfigure the gain table.The gain table for a receiver type is set with the
 * parameters passed by adi_adrv9001_RxGainTableRow_t gainTablePtr array.
 * The array length (n) is dependent upon receiver type.
 * The (n) value is conveyed by numGainIndicesInTable.
 * All gain tables have a maximum index and a minimum index specified by
 * MAX_RX_GAIN_TABLE_NUMINDICES and MIN_RX_GAIN_TABLE_INDEX
 * The minimum gain index is application dependent, this can be modified
 * in the user space, the absolute maximum and minimum indices are specified
 * by MAX_GAIN_TABLE_INDEX and MIN_GAIN_TABLE_INDEX
 *
 * The Rx max gain index is user configurable. A separate call has to be made
 * to adi_adrv9001_RxMinMaxGainIndexSet() API to update the min and max gain
 * indices for a given Rx Channel. Updating min and max gain indices are
 * decoupled from the main gain table loading so that the user has flexibility
 * to load multiple gain table regions and switch between them during runtime.
 *
 * Partial gain table loads can be done through this API in case of memory constraints / multiple region loading
 * For example, consider a 256 row gain table which needs to be loaded in 4 consecutive calls.
 * In this case the config parameters for partial loads would be
 * Partial Load 1 : gainTableRow[] = gainTable[63:0], gainIndexOffset = 63, numGainIndicesInTable = 64
 * Partial Load 2 : gainTableRow[] = gainTable[127:64], gainIndexOffset = 127, numGainIndicesInTable = 64
 * Partial Load 3 : gainTableRow[] = gainTable[191:128], gainIndexOffset = 191, numGainIndicesInTable = 64
 * Partial Load 4 : gainTableRow[] = gainTable[255:192], gainIndexOffset = 255, numGainIndicesInTable = 64
 *
 * After this multiple partial gain table load, call the function adi_adrv9001_RxMinMaxGainIndexSet(minIndex = 0, maxIndex = 255).
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function called automatically during initialization to load
 *      the default gain table. If the BBIC desires to change or update the
 *      gain table, it may call this function after initialization but before
 *      running init cals.
 *
 * \param[in] adrv9001           Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channelMask        An OR'd combination of adi_common_ChannelNumber_e
 *                               specifying the Rx Channels for which to write the gain table
 * \param[in] gainIndexOffset    The starting gain index from which the gain table is written
 * \param[in] gainTableRows      Array of gain table row entries to write
 * \param[in] arraySize          The number of gain table rows to write
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_GainTable_Write(adi_adrv9001_Device_t *adrv9001,
                                        uint32_t channelMask,
                                        uint8_t  gainIndexOffset,
                                        adi_adrv9001_RxGainTableRow_t gainTableRows[],
                                        uint32_t arraySize);

/**
 * \brief Reads the gain table entries for the specified Rx channel
 *
 * This function can be called by the user to read back the currently programmed gain table
 * for a given channel. This function reads the current gain table settings from ADRV9001 gain table Static RAMs
 * for the requested channel and stores it in the provided memory reference of type adi_adrv9001_RxGainTableCfg_t
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in] adrv9001               Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel               The Rx Channel from which to read the gain table
 * \param[in]  gainIndexOffset       The gain index from which gain table read back should start
 * \param[out] gainTableRows         Read back array for gain table row entries which will be updated with the read back values
 * \param[in]  arraySize             The size of the gainTableRows array; the max number of gain table rows to read
 * \param[out] numGainIndicesRead    The actual no. of gain indices read. Pass NULL if this info is not needed
 *
 * \pre This function can be called by the user anytime after initialization.
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_GainTable_Read(adi_adrv9001_Device_t *adrv9001,
                                       adi_common_ChannelNumber_e channel,
                                       uint8_t  gainIndexOffset,
                                       adi_adrv9001_RxGainTableRow_t gainTableRows[],
                                       uint32_t arraySize,
                                       uint16_t *numGainIndicesRead);

 /**
 * \brief Sets the Manual Gain Index for the given Rx channel
 *
 * The maximum index is 255 and the minimum index is application specific.
 *
 * If the value passed in the gainIndex parameter is within range of the gain
 * table minimum and maximum indices, the Rx channel gain index will be written
 * to the transceiver.
 * Else, an error will be returned.
 *
 * The default gain table can take values between 0xB7 and 0xFF,
 * even though every index is accessible from 0x00 to 0xFF.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 * \note The new gain only takes effect in States where Clocks are enabled:
 *       Clocks are enabled in the following transitions:
 *           CALIBRATED->PRIMED
 *           PRIMED->RF_ENABLED
 *
 *       Clocks are disabled in the following transitions:
 *           RF_ENABLED->PRIMED
 *
 * \pre If gain control mode is ADI_ADRV9001_RX_GAIN_CONTROL_MODE_PIN: 
 *      channel state is any of CALIBRATED, PRIMED, RF_ENABLED 
 *      and can only take action in RF_ENABLED
 * \pre If gain control mode is ADI_ADRV9001_RX_GAIN_CONTROL_MODE_SPI:
 *      channel state is any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in] adrv9001      Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel       The Rx Channel for which to set the gain
 * \param[in] gainIndex     The gain table index to set the channel to
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_Gain_Set(adi_adrv9001_Device_t *adrv9001,
                                 adi_common_ChannelNumber_e channel,
                                 uint8_t gainIndex);

/**
 * \brief Reads the current AGC Gain Index for the requested channel.
 *        - The current gain index value read will be Rx Gain index in Rx mode
 *        - The current gain index value read will be ORx Gain index in ORx mode
 *
 * This function reads the gain index of the channel last time clocks were enabled
 *
 * \note Message type: \ref timing_direct "Direct register access
 * 
 * \pre Channel state is any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *      However, gain indices are tracked only after the channel state is RF_ENABLED
 *      and returns the gainIndex of the channel last time clocks were enabled
 *
 * \param[in]  adrv9001     Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel      The Rx Channel from which to read the gain
 * \param[out] gainIndex    The current gain table index the channel is set to
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_Gain_Get(adi_adrv9001_Device_t *adrv9001,
                                 adi_common_ChannelNumber_e channel,
                                 uint8_t *gainIndex);

/**
 * \brief Reads back the RSSI (Received Signal Strength Indicator) status for the given Rx channel
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is RF_ENABLED
 *
 * \param[in]  adrv9001      Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel       RX Channel for which RSSI status to be read back
 * \param[out] rxRssiStatus  Pointer to structure (of type adi_adrv9001_RxRssiStatus_t) to store the RSSI status
 *                           read back from ADRV9001 device for the given Rx channel.
 *                           The structure contains 'mantissa' and 'exponent' values of linear power.
 *                           Linear power is calculated by this formula: linear power = (mantissa * 2^-15) * 2^-exponent
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_Rssi_Read(adi_adrv9001_Device_t *adrv9001,
                                  adi_common_ChannelNumber_e channel,
                                  adi_adrv9001_RxRssiStatus_t *rxRssiStatus);

/**
 * \brief This function gets the decimated Power (Dec Power) for the specified channel.
 *
 * This function can be used to get the Dec Power of RX channel in runtime.
 *
 * The location of the power measurement is given by agcCfg->power->powerInputSelect
 * The number of samples used by power measurement is given by 8*2^(agcCfg->power->powerMeasurementDuration) at the IQ rate,
 * if measured at RFIR output. This number of samples must be less than the agcCfg->gainUpdateCounter.
 * If the receiver is disabled during the power measurement, this function returns a '0' for rxDecPower_mdBFS
 *
 * The resolution of this function is 250mdB.
 * The dynamic range of this function is 60dB. Signals lower than -60dBFS may not be measured accurately.
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in]  adrv9001          Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel           An enum ( of type adi_common_ChannelNumber_e) to select Rx Channel.
 * \param[out] rxDecPower_mdBFS  Pointer to store the ADRV9001 Dec Power return. Value returned is in mdBFS.
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_DecimatedPower_Get(adi_adrv9001_Device_t *adrv9001,
                                           adi_common_ChannelNumber_e channel,
                                           uint16_t *rxDecPower_mdBFS);

/**
 * \brief Configure the Rx interface gain control
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is CALIBRATED
 *
 * \param[in] adrv9001               Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel                The Rx channel for which to configure the interface gain control
 * \param[in] rxInterfaceGainConfig  The desired Rx interface gain control configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_InterfaceGain_Configure(adi_adrv9001_Device_t *adrv9001,
                                                adi_common_ChannelNumber_e channel,
                                                adi_adrv9001_RxInterfaceGainCtrl_t *rxInterfaceGainConfig);

/**
 * \brief Set the Rx interface gain
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is any of PRIMED, RF_ENABLED
 *
 * \pre This function may be called only when the specified channel is in RF_ENABLED state.
 * 'gainControlMode' in adi_adrv9001_RxInterfaceGainCtrl_t struct must be ADI_ADRV9001_RX_INTERFACE_GAIN_CONTROL_MANUAL
 *  to set the Rx interface gain
 *
 * \param[in] adrv9001   Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel    The Rx channel for which the interface gain has to be configured
 * \param[in] gain       The gain value to be set for the given Rx channel
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_InterfaceGain_Set(adi_adrv9001_Device_t *adrv9001,
                                          adi_common_ChannelNumber_e channel,
                                          adi_adrv9001_RxInterfaceGain_e gain);

/**
 * \brief Inspect the Rx interface gain control configuration
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv9001                  Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel                   The Rx channel for which to inspect the interface gain control configuration
 * \param[out] rxInterfaceGainConfig     The current Rx interface gain control configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_InterfaceGain_Inspect(adi_adrv9001_Device_t *adrv9001,
                                              adi_common_ChannelNumber_e channel,
                                              adi_adrv9001_RxInterfaceGainCtrl_t *rxInterfaceGainConfig);

/**
 * \brief Get the Rx interface gain for the given Rx channel
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv9001  Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel   The Rx channel from which to read the interface gain
 * \param[out] gain      The current Rx interface gain
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_InterfaceGain_Get(adi_adrv9001_Device_t *adrv9001,
                                          adi_common_ChannelNumber_e channel,
                                          adi_adrv9001_RxInterfaceGain_e *gain);

/**
 * \brief Set the NCO frequency to correct for small deviations in Rx LO frequency
 *
 * \note Message type: \ref timing_prioritymailbox "High-priority mailbox command"
 *
 * \param[in] adrv9001              Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel               The Rx channel for which to set the NCO frequency
 * \param[in] frequencyOffset_Hz    The desired offset frequency, denoted in Hz
 * \param[in] immediate             Whether to change the frequency immediately (true), or
 *                                  update it at the start of the next available frame (false)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_FrequencyCorrection_Set(adi_adrv9001_Device_t *adrv9001,
                                                adi_common_ChannelNumber_e channel,
                                                int32_t frequencyOffset_Hz,
                                                bool immediate);


/**
 * \brief Set the enabledness of dynamic switch between Low Power and High Power ADCs
 * 
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED
 *
 * \param[in] adrv9001   Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel    The Rx Channel for which to set the enabledness of the ADC dynamic switch
 * \param[in] enable     A boolean flag to enable or disable dynamic switching between LP and HP ADCs
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_AdcSwitchEnable_Set(adi_adrv9001_Device_t *adrv9001, 
                                            adi_common_ChannelNumber_e channel,
                                            bool enable);

/**
 * \brief Get the enabledness of dynamic switch between Low Power and High Power ADCs
 * 
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv9001   Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel    The Rx Channel for which to get the current enabledness of the ADC dynamic switch
 * \param[out] enable     A boolean flag to get the current enabledness of the ADC dynamic switch
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_AdcSwitchEnable_Get(adi_adrv9001_Device_t *adrv9001, 
                                            adi_common_ChannelNumber_e channel,
                                            bool *enable);

/**
 * \brief Configure ADC dynamic switch settings for the specified channel
 * 
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state must be CALIBRATED
 *
 * \param[in] adrv9001         Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel          The Rx Channel for which to set the enabledness of the ADC dynamic switch
 * \param[in] switchConfig     The desired ADC dynamic switch configuration
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_AdcSwitch_Configure(adi_adrv9001_Device_t *adrv9001, 
                                            adi_common_ChannelNumber_e channel,
                                            adi_adrv9001_AdcSwitchCfg_t *switchConfig);

/**
 * \brief Inspect the current ADC dynamic switch settings for the specified channel
 * 
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv9001         Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel          The Rx Channel for which to get the current enabledness of the ADC dynamic switch
 * \param[out] switchConfig     The current ADC dynamic switch configuration
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_AdcSwitch_Inspect(adi_adrv9001_Device_t *adrv9001, 
                                          adi_common_ChannelNumber_e channel,
                                          adi_adrv9001_AdcSwitchCfg_t *switchConfig);

/**
 * \brief Get the current ADC type for the specified channel
 * 
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv9001   Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel    The Channel for which to get the ADC type
 * \param[out] adcType    The current ADC type
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Rx_AdcType_Get(adi_adrv9001_Device_t *adrv9001, 
                                    adi_common_ChannelNumber_e channel,
                                    adi_adrv9001_AdcType_e *adcType);

/**
* \brief Configure GPIO pins to route the ADRV9001 Rx1 and Rx2 gain indices
*
* \note  gain index ranges from 183 to 255 and occupies 8-bits[7:0]. So the 'msb' is always be '1'.
*        Hence the 'msb' control out mux register for gain index is replaced with 'gt_gain_index', which allowstoggles/triggers on the gain_change.
*        So in BBIC, the user must replace the 'msb' received with '1' always.
*        
* \pre Channel state must be CALIBRATED
*      Level of GPIO pins is only reflective of channel gainIndex when channel is in the RF_ENABLED state
*
* \param[in] adrv9001           Context variable - Pointer to the ADRV9001 device data structure
* \param[in] channel            The channel of the specified port for which to route gain index to the specified GPIO pin
* \param[in] gainIndexPinCfg    The desired DGPIO pin selection to route gain index for the specified channel
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv9001_Rx_GainIndex_Gpio_Configure(adi_adrv9001_Device_t *adrv9001,
                                                 adi_common_ChannelNumber_e channel,
                                                 adi_adrv9001_GainIndexPinCfg_t *gainIndexPinCfg);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_RX_H_ */
