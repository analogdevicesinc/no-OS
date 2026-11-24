/**
 * \file
 * \brief Functions for configuring receiver (Rx) features
 *
 * Copyright (c) 2022 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_ADRV910X_RX_H_
#define _ADI_ADRV910X_RX_H_

#include "adi_adrv910x_rx_types.h"
#include "adi_adrv910x_common_types.h"
#include "adi_adrv910x_types.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Programs the gain table settings for Rx channels.
 *
 * This function can be called by the user to load a custom gain table or
 * to reconfigure the gain table.The gain table for a receiver type is set with the
 * parameters passed by adi_adrv910x_RxGainTableRow_t gainTablePtr array.
 * The array length (n) is dependent upon receiver type.
 * The (n) value is conveyed by numGainIndicesInTable.
 * All gain tables have a maximum index and a minimum index specified by
 * MAX_RX_GAIN_TABLE_NUMINDICES and MIN_RX_GAIN_TABLE_INDEX
 * The minimum gain index is application dependent, this can be modified
 * in the user space, the absolute maximum and minimum indices are specified
 * by MAX_GAIN_TABLE_INDEX and MIN_GAIN_TABLE_INDEX
 *
 * The Rx max gain index is user configurable. A separate call has to be made
 * to adi_adrv910x_RxMinMaxGainIndexSet() API to update the min and max gain
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
 * After this multiple partial gain table load, call the function adi_adrv910x_RxMinMaxGainIndexSet(minIndex = 0, maxIndex = 255).
 *
 * \note Message type: \ref timing_direct "Direct register access"
 * \pre This function called automatically during initialization to load the default gain table. If the BBIC desires to change or update the gain table, it may call this function after initialization but before running init cals.
 *
 * \param[in] adrv910x         Context variable - Pointer to the ADRV910X device data structure
 * \param[in] port             The desired Rx or ORx port
 * \param[in] channel          The Rx Channel from which to write the gain table specifying the Rx Channels for which to write the gain table
 * \param[in] gainIndexOffset  The starting gain index from which the gain table is written
 * \param[in] gainTableRows    Array of gain table row entries to write
 * \param[in] arraySize        The number of gain table rows to write
 * \param[in] gainTableType    Gain table loaded during ADRV910X initialization
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Rx_GainTable_Write(adi_adrv910x_Device_t *adrv910x,
                                        adi_common_Port_e port,
                                        adi_common_ChannelNumber_e channel,
                                        uint8_t gainIndexOffset,
                                        adi_adrv910x_RxGainTableRow_t gainTableRows[],
                                        uint32_t arraySize,
                                        adi_adrv910x_RxGainTableType_e gainTableType);

/**
 * \brief Reads the gain table entries for the specified Rx channel
 *
 * This function can be called by the user to read back the currently programmed gain table
 * for a given channel. This function reads the current gain table settings from ADRV910X gain table Static RAMs
 * for the requested channel and stores it in the provided memory reference of type adi_adrv910x_RxGainTableCfg_t
 *
 * \note Message type: \ref timing_direct "Direct register access"
 * \pre This function can be called by the user anytime after initialization.
 *
 * \param[in]  adrv910x            Context variable - Pointer to the ADRV910X device data structure
 * \param[in]  channel             The Rx Channel from which to read the gain table
 * \param[in]  gainIndexOffset     The gain index from which gain table read back should start
 * \param[out] gainTableRows       Read back array for gain table row entries which will be updated with the read back values
 * \param[in]  arraySize           The size of the gainTableRows array; the max number of gain table rows to read
 * \param[out] numGainIndicesRead  The actual no. of gain indices read. Pass NULL if this info is not needed
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Rx_GainTable_Read(adi_adrv910x_Device_t *adrv910x,
                                       adi_common_ChannelNumber_e channel,
                                       uint8_t gainIndexOffset,
                                       adi_adrv910x_RxGainTableRow_t gainTableRows[],
                                       uint32_t arraySize,
                                       uint16_t *numGainIndicesRead);

/**
 * \brief Configures Rx gain control mode for the specified channel.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 * \pre This function should be called before Rx operation is initiated.
 *
 * \param[in] adrv910x      Context variable - Pointer to the ADRV910X device data structure
 * \param[in] channel       The Rx channel for which to set the gain control mode
 * \param[in] gainCtrlMode  The desired gain control mode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Rx_GainControl_Mode_Set(adi_adrv910x_Device_t *adrv910x,
                                             adi_common_ChannelNumber_e channel,
                                             adi_adrv910x_RxGainControlMode_e gainCtrlMode);

/**
 * \brief Reads current Rx gain control mode configuration
 *
 * \note Message type: \ref timing direct "Direct register access"
 *
 * \param[in]  adrv910x      Context variable - Pointer to the ADRV910X device data structure
 * \param[in]  channel       The Rx channel from which the gain control mode read back is requested
 * \param[out] gainCtrlMode  The current gain control mode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Rx_GainControl_Mode_Get(adi_adrv910x_Device_t *adrv910x,
                                             adi_common_ChannelNumber_e channel,
                                             adi_adrv910x_RxGainControlMode_e *gainCtrlMode);

/**
 * \brief Set the min and max gain indexes for AGC for the specified channel.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in] adrv910x      Context variable - Pointer to the ADRV910X device data structure
 * \param[in] channel       The Rx Channel for which to set min and max gain indices
 * \param[in] minGainIndex  The lower limit of the gain index. Valid range is from 0 to 255.
 * \param[in] maxGainIndex  The upper limit of the gain index. Valid range is from 0 to 255.
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Rx_GainControl_MinMaxGainIndex_Set(adi_adrv910x_Device_t *adrv910x,
                                                        adi_common_ChannelNumber_e channel,
                                                        uint8_t minGainIndex,
                                                        uint8_t maxGainIndex);

/**
 * \brief Get the min and max gain indexes for AGC for the specified Rx channel
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in]  adrv910x      Context variable - Pointer to the ADRV910X device data structure
 * \param[in]  channel       The Rx Channel from which to get min and max gain indices
 * \param[out] minGainIndex  The lower limit of the gain index
 * \param[out] maxGainIndex  The upper limit of the gain index
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Rx_GainControl_MinMaxGainIndex_Get(adi_adrv910x_Device_t *adrv910x,
                                                        adi_common_ChannelNumber_e channel,
                                                        uint8_t *minGainIndex,
                                                        uint8_t *maxGainIndex);

/**
 * \brief Configure Automatic Gain Control (AGC) for the specified Rx channel
 *
 * \note This function does NOT enable AGC mode. Call adi_adrv910x_Rx_GainCtrlMode_Set() afterwards
 * \note Message type: \ref timing_direct "Direct register access"
 * \pre Channel state is any of #ADI_ADRV910X_CHANNEL_STANDBY, #ADI_ADRV910X_CHANNEL_CALIBRATED, #ADI_ADRV910X_CHANNEL_PRIMED
 *
 * \param[in] adrv910x  Context variable - Pointer to the ADRV910X device data structure
 * \param[in] channel   The Rx Channel for which to configure AGC
 * \param[in] agcCfg    The AGC configuration to apply
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Rx_GainControl_Configure(adi_adrv910x_Device_t *adrv910x,
                                              adi_common_ChannelNumber_e channel,
                                              adi_adrv910x_GainControlCfg_t *agcCfg);

/**
 * \brief Inspect the Automatic Gain Control (AGC) configuration for the specified Rx channel
 *
 * \note Message type: \ref timing_direct "Direct register access"
 * \pre Channel state is any of #ADI_ADRV910X_CHANNEL_CALIBRATED, #ADI_ADRV910X_CHANNEL_PRIMED, or #ADI_ADRV910X_CHANNEL_RF_ENABLED
 *
 * \param[in]  adrv910x  Context variable - Pointer to the ADRV910X device data structure
 * \param[in]  channel   The Rx Channel for which to configure AGC
 * \param[out] agcCfg    The current AGC configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Rx_GainControl_Inspect(adi_adrv910x_Device_t *adrv910x,
                                            adi_common_ChannelNumber_e channel,
                                            adi_adrv910x_GainControlCfg_t *agcCfg);

/**
 * \brief Sets the manual gain index for the specified Rx channel
 *
 * This function can be called by the user to set the gain index for the channel given. The maximum value is 255
 * and the minimum is application specific. The default value is 187.
 *
 * If the value passed in the gainIndex parameter is within the range of the gain table minimum and maximum indices,
 * the Rx channel will be written to the transceiver.
 *
 * \note Message type: \ref timing direct "Direct register access"
 * \note The new gain only takes effect in states where clocks are enabled
 *
 * \param[in] adrv910x   Context variable - Pointer to the ADRV910X device data structure
 * \param[in] channel    The Rx channel from which to set the gain index value
 * \param[in] gainIndex  The actual gain index value to set
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Rx_Gain_Set(adi_adrv910x_Device_t *adrv910x,
                                 adi_common_ChannelNumber_e channel,
                                 uint8_t gainIndex);

/**
 * \brief Reads the current gain index for a selected Rx channel
 *
 * This function reads the gain index of the channel from the last time the clocks were enabled
 *
 * \note Message type: \ref timing direct "Direct register access"
 *
 * \param[in]  adrv910x   Context variable - Pointer to the ADRV910X device data structure
 * \param[in]  channel    The Rx channel from which to read the gain index
 * \param[out] gainIndex  The current gain table index that the channel is set to
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Rx_Gain_Get(adi_adrv910x_Device_t *adrv910x,
                                 adi_common_ChannelNumber_e channel,
                                 uint8_t *gainIndex);

/**
 * \brief Configure the Rx interface gain control
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * \pre Channel state is #ADI_ADRV910X_CHANNEL_CALIBRATED
 *
 * \param[in] adrv910x               Context variable - Pointer to the ADRV910X device data structure
 * \param[in] channel                The Rx channel for which to configure the interface gain control
 * \param[in] rxInterfaceGainConfig  The desired Rx interface gain control configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Rx_InterfaceGain_Configure(adi_adrv910x_Device_t *adrv910x,
                                                adi_common_ChannelNumber_e channel,
                                                adi_adrv910x_RxInterfaceGainCtrl_t *rxInterfaceGainConfig);

/**
 * \brief Set the Rx interface gain
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * \pre Channel state is any of #ADI_ADRV910X_CHANNEL_RF_ENABLED
 * \pre This function may be called only when the specified channel is in #ADI_ADRV910X_CHANNEL_RF_ENABLED state. 'gainControlMode' in adi_adrv910x_RxInterfaceGainCtrl_t struct must be ADI_ADRV910X_RX_INTERFACE_GAIN_CONTROL_MANUAL to set the Rx interface gain
 *
 * \param[in] adrv910x  Context variable - Pointer to the ADRV910X device data structure
 * \param[in] channel   The Rx channel for which the interface gain has to be configured
 * \param[in] gain      The gain value to be set for the given Rx channel
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Rx_InterfaceGain_Set(adi_adrv910x_Device_t *adrv910x,
                                          adi_common_ChannelNumber_e channel,
                                          adi_adrv910x_RxInterfaceGain_e gain);

/**
 * \brief Inspect the Rx interface gain control configuration
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * \pre Channel state any of #ADI_ADRV910X_CHANNEL_CALIBRATED, #ADI_ADRV910X_CHANNEL_PRIMED, #ADI_ADRV910X_CHANNEL_RF_ENABLED
 *
 * \param[in]  adrv910x               Context variable - Pointer to the ADRV910X device data structure
 * \param[in]  channel                The Rx channel for which to inspect the interface gain control configuration
 * \param[out] rxInterfaceGainConfig  The current Rx interface gain control configuration
 * \param[out] gainTableType          The current gain table type loaded
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Rx_InterfaceGain_Inspect(adi_adrv910x_Device_t *adrv910x,
                                              adi_common_ChannelNumber_e channel,
                                              adi_adrv910x_RxInterfaceGainCtrl_t *rxInterfaceGainConfig,
                                              adi_adrv910x_RxGainTableType_e *gainTableType);

/**
 * \brief Get the Rx interface gain for the given Rx channel
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * \pre Channel state any of #ADI_ADRV910X_CHANNEL_CALIBRATED, #ADI_ADRV910X_CHANNEL_PRIMED, #ADI_ADRV910X_CHANNEL_RF_ENABLED
 *
 * \param[in]  adrv910x  Context variable - Pointer to the ADRV910X device data structure
 * \param[in]  channel   The Rx channel from which to read the interface gain
 * \param[out] gain      The current Rx interface gain
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Rx_InterfaceGain_Get(adi_adrv910x_Device_t *adrv910x,
                                          adi_common_ChannelNumber_e channel,
                                          adi_adrv910x_RxInterfaceGain_e *gain);

/**
 * \brief Reads back the RSSI (Received Signal Strength Indicator) status for the given Rx channel
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * \pre Channel state is #ADI_ADRV910X_CHANNEL_RF_ENABLED
 *
 * \param[in]  adrv910x         Context variable - Pointer to the ADRV910X device data structure
 * \param[in]  channel          RX Channel for which RSSI status to be read back
 * \param[out] rxRssiPower_mdB  The measured Rx RSSI power, denoted in mdB
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Rx_Rssi_Read(adi_adrv910x_Device_t *adrv910x,
                                  adi_common_ChannelNumber_e channel,
                                  uint32_t *rxRssiPower_mdB);

/**
 * \brief Configure LOID settings
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * \pre channel state is any of #ADI_ADRV910X_CHANNEL_STANDBY, #ADI_ADRV910X_CHANNEL_CALIBRATED
 *
 * \param[in] adrv910x    Context variable - Pointer to the ADRV910X device data structure
 * \param[in] channel     The channel to configure
 * \param[in] loidConfig  The desired LOID configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Rx_Loid_Configure(adi_adrv910x_Device_t *adrv910x,
                                       adi_common_ChannelNumber_e channel,
                                       adi_adrv910x_RxrfdcLoidCfg_t *loidConfig);

/**
 * \brief Inspect LOID settings
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * \pre channel state is any of #ADI_ADRV910X_CHANNEL_STANDBY, #ADI_ADRV910X_CHANNEL_CALIBRATED, #ADI_ADRV910X_CHANNEL_PRIMED, #ADI_ADRV910X_CHANNEL_RF_ENABLED
 *
 * \param[in]  adrv910x    Context variable - Pointer to the ADRV910X device data structure
 * \param[in]  channel     The channel to configure
 * \param[out] loidConfig  The current LOID configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Rx_Loid_Inspect(adi_adrv910x_Device_t *adrv910x,
                                     adi_common_ChannelNumber_e channel,
                                     adi_adrv910x_RxrfdcLoidCfg_t *loidConfig);

/**
 * \brief Reads current measured residual DC value
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * \pre channel state is of #ADI_ADRV910X_CHANNEL_CALIBRATED, #ADI_ADRV910X_CHANNEL_PRIMED, #ADI_ADRV910X_CHANNEL_RF_ENABLED
 *
 * \param[in]  adrv910x      Context variable - Pointer to the ADRV910X device data structure
 * \param[in]  channel       The channel to configure
 * \param[out] rfdcResidual  The current measured residual DC (in -dBFS)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Rx_Loid_Status_Get(adi_adrv910x_Device_t *adrv910x,
                                        adi_common_ChannelNumber_e channel,
                                        uint8_t *rfdcResidual);

/**
 * \brief Configures the Image Interferer Detection(High Blocker Mode) settings for LO frequency reconfiguration
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * \pre channel state is of #ADI_ADRV910X_CHANNEL_STANDBY or #ADI_ADRV910X_CHANNEL_CALIBRATED
 * \pre only supports RxNB
 *
 * \param[in] adrv910x       Context variable - Pointer to the ADRV910X device data structure
 * \param[in] channel        The channel to configure
 * \param[in] imageIDConfig  The desired rx Image Interferer Detection(High Blocker Mode) settings
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Rx_ImageID_Configure(adi_adrv910x_Device_t *adrv910x,
                                          adi_common_ChannelNumber_e channel,
                                          adi_adrv910x_ImageIDCfg_t *imageIDConfig);

/**
 * \brief Inspects the Image Interferer Detection(High Blocker Mode) settings for LO frequency reconfiguration
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * \pre channel state is of #ADI_ADRV910X_CHANNEL_STANDBY or #ADI_ADRV910X_CHANNEL_CALIBRATED
 * \pre only supports RxNB
 *
 * \param[in]  adrv910x       Context variable - Pointer to the ADRV910X device data structure
 * \param[in]  channel        The channel to configure
 * \param[out] imageIDConfig  The current rx Image Interferer Detection(High Blocker Mode) settings
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Rx_ImageID_Inspect(adi_adrv910x_Device_t *adrv910x,
                                        adi_common_ChannelNumber_e channel,
                                        adi_adrv910x_ImageIDCfg_t *imageIDConfig);

/**
 * \brief Sets the NCO2 Frequency Tuning Word for the specified Rx channel
 *
 * This function writes the 32-bit NCO2 frequency tuning word (FTW) to the
 * specified Rx channel (RX1 or RXNB) registers. The FTW is split into 4 bytes
 * and latched atomically via the FtwUpdate strobe.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in] adrv910x        Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in] channel        The channel to configure
 * \param[in] ncoControlWord  32-bit NCO2 frequency tuning word to program
 *
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_FULL Recovery action for SPI error
 */
int32_t adi_adrv910x_Rx_Nco2Ftw_Set(adi_adrv910x_Device_t        *adrv910x,
                                    adi_common_ChannelNumber_e  channel,
                                    uint32_t                       ncoControlWord);

/**
 * \brief Sets the resampler phase for the specified Rx channel
 *
 * This function enables the resampler and programs the I and Q phase
 * values for the specified Rx channel (RX1 or RXNB).
 *
 * \note Message type: \ref timing_direct "Direct register access"
 * \pre Channel state is any of #ADI_ADRV910X_CHANNEL_CALIBRATED, #ADI_ADRV910X_CHANNEL_PRIMED, or #ADI_ADRV910X_CHANNEL_RF_ENABLED
 *
 * \param[in] adrv910x        Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in] channel         The channel to configure
 * \param[in] resampleInputI  Resampler phase value for I path
 * \param[in] resampleInputQ  Resampler phase value for Q path
 *
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_FULL Recovery action for SPI error
 */
int32_t adi_adrv910x_Rx_ResampPhase_Set(adi_adrv910x_Device_t        *adrv910x,
                                       adi_common_ChannelNumber_e  channel,
                                       int16_t                        resampleInputI,
                                       int16_t                        resampleInputQ);

/**
 * \brief Gets the resampler phase for the specified Rx channel
 *
 * This function reads back the I and Q phase values from the resampler
 * for the specified Rx channel (RX1 or RXNB).
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in]  adrv910x        Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in]  channel         The channel to read back the resampler phase for
 * \param[out] resampleInputI  Pointer to store resampler phase value for I path
 * \param[out] resampleInputQ  Pointer to store resampler phase value for Q path
 *
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_FULL Recovery action for SPI error
 */
int32_t adi_adrv910x_Rx_ResampPhase_Get(adi_adrv910x_Device_t        *adrv910x,
                                       adi_common_ChannelNumber_e  channel,
                                       int16_t                       *resampleInputI,
                                       int16_t                       *resampleInputQ);

/**
 * \brief Sets the NCO frequency offset for the specified Rx channel
 *
 * This function converts the desired frequency offset in Hz to a 32-bit NCO
 * frequency tuning word (FTW) and programs it to the specified Rx channel.
 * The frequency offset is validated against the RX interface rate: the maximum
 * allowed offset is half the channel's output sample rate.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre Channel state is #ADI_ADRV910X_CHANNEL_RF_ENABLED
 * \param[in] adrv910x           Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in] channel            The channel to configure
 * \param[in] frequencyOffset_Hz Signed frequency offset in Hz. Range: +/-(rxOutputRate_Hz / 2)
 *
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_FULL Recovery action for SPI error
 */
int32_t adi_adrv910x_Rx_NcoFrequency_Set(adi_adrv910x_Device_t        *adrv910x,
                                                adi_common_ChannelNumber_e  channel,
                                                int32_t                        frequencyOffset_Hz);

/**
 * \brief Gets the current NCO frequency offset for the specified Rx channel
 *
 * This function reads the current 32-bit NCO frequency tuning word (FTW)
 * from the specified Rx channel registers and converts it back to a
 * frequency offset in Hz.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in]  adrv910x           Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in]  channel            The channel to read back the NCO frequency offset for
 * \param[out] frequencyOffset_Hz Pointer to store the current frequency offset in Hz
 *
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
 * \retval ADI_COMMON_ACT_ERR_RESET_FULL Recovery action for SPI error
 */
int32_t adi_adrv910x_Rx_NcoFrequency_Get(adi_adrv910x_Device_t        *adrv910x,
                                                adi_common_ChannelNumber_e  channel,
                                                int32_t                       *frequencyOffset_Hz);

/**
 * \brief Switches the active RX PFIR coefficient bank for the specified channel
 *
 * Selects which pre-loaded PFIR coefficient bank (A through F) is active on the
 * specified RX channel. The desired banks must be loaded at initialization time.
 * The bank selection is written to both the PFIR bank select register and the
 * stream processor software spare register so it persists across RF_ON transitions.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 * \pre Channel state any of #ADI_ADRV910X_CHANNEL_CALIBRATED, #ADI_ADRV910X_CHANNEL_PRIMED, #ADI_ADRV910X_CHANNEL_RF_ENABLED
 *
 * \param[in] adrv910x  Context variable - Pointer to the ADRV910X device data structure
 * \param[in] channel   Rx channel (ADI_ADRV910X_RX1 or ADI_ADRV910X_RXNB)
 * \param[in] bankSel   PFIR bank to activate (ADI_ADRV910X_RX_PFIR_BANK_A through ADI_ADRV910X_RX_PFIR_BANK_F)
 *
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid channel or bank parameter
 * \retval ADI_COMMON_ACT_ERR_RESET_FULL Recovery action for SPI error
 */
int32_t adi_adrv910x_Rx_PfirBankSelect_Set(adi_adrv910x_Device_t        *adrv910x,
                                           adi_adrv910x_MailboxChannel_e  channel,
                                           adi_adrv910x_Rx_PfirBank_e     bankSel);

/**
 * \brief Get the currently active RX PFIR coefficient bank
 *
 * Reads the bank select register to determine which PFIR coefficient bank
 * is currently active for the specified RX channel.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 * \pre Channel state any of CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv910x  Context variable - Pointer to the ADRV910X device data structure
 * \param[in]  channel   Rx channel (ADI_ADRV910X_RX1 or ADI_ADRV910X_RXNB)
 * \param[out] bankSel   Pointer to store the currently active PFIR bank
 *
 * \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Invalid channel or null pointer
 * \retval ADI_COMMON_ACT_ERR_RESET_FULL Recovery action for SPI error
 */
int32_t adi_adrv910x_Rx_PfirBankSelect_Get(adi_adrv910x_Device_t        *adrv910x,
                                           adi_adrv910x_MailboxChannel_e  channel,
                                           adi_adrv910x_Rx_PfirBank_e    *bankSel);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV910X_RX_H_ */
