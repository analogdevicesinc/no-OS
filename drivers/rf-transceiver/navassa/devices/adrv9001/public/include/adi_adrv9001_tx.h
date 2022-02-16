/**
* \file
* \brief Contains ADRV9001 transmit related function prototypes for
*        adi_adrv9001_tx.c
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the FPGA9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_ADRV9001_TX_H_
#define _ADI_ADRV9001_TX_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

/* System header files */

/* ADI specific header files */
#include "adi_adrv9001_tx_types.h"
#include "adi_adrv9001_types.h"

/**
 * \brief Configure the Tx attenuation for the specified channel
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \pre Channel state any of STANDBY, CALIBRATED
 * \pre Attenuation mode is not ADI_ADRV9001_TX_ATTENUATION_CONTROL_MODE_CLGC
 *
 * \param[in] adrv9001  Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel   The Tx channel for which to configure the attenuation
 * \param[in] config    The Tx attenuation configuration to apply
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Tx_Attenuation_Configure(adi_adrv9001_Device_t *adrv9001,
                                              adi_common_ChannelNumber_e channel,
                                              adi_adrv9001_TxAttenuationConfig_t *config);

/**
 * \brief Inspect the Tx attenuation configuration for the specified channel
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \pre Channel state any of STANDBY, CALIBRATED
 *
 * \param[in]  adrv9001     Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel      The Tx channel for which to inspect the attenuation configuration
 * \param[out] config       The current Tx attenuation configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Tx_Attenuation_Inspect(adi_adrv9001_Device_t *adrv9001,
                                            adi_common_ChannelNumber_e channel,
                                            adi_adrv9001_TxAttenuationConfig_t *config);

/**
 * \brief Set the attenuation control mode
 *
 * The relationship between Tx attenuation control mode and channel state is shown in the following timing diagram
 * 
 * \image html "ADRV9001_TxAtten_State_Transitions.jpg" "Tx Attenuation control modes and channel state diagram"
 * 
 * \note Message type: \ref timing_direct "Direct register acccess"
 * \note Tx attenuation control via GPIO pins is possible only in RF_ENABLED state
 * 
 * \pre Channel state any of STANDBY, CALIBRATED
 *
 * \param[in] adrv9001  Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel   The Tx channel for which to set the attenuation control mode
 * \param[in] mode      The desired Tx attenuation mode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Tx_AttenuationMode_Set(adi_adrv9001_Device_t *adrv9001,
                                            adi_common_ChannelNumber_e channel,
                                            adi_adrv9001_TxAttenuationControlMode_e mode);

/**
 * \brief Get the current attenuation control mode
 *
 * \note If channel state is PRIMED, will always read back as ADI_ADRV9001_TX_ATTENUATION_CONTROL_MODE_SPI
 *
 * \param[in]  adrv9001     Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel      The Tx channel for which to get the attenuation control mode
 * \param[out] mode         The current Tx attenuation mode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Tx_AttenuationMode_Get(adi_adrv9001_Device_t *adrv9001,
                                            adi_common_ChannelNumber_e channel,
                                            adi_adrv9001_TxAttenuationControlMode_e *mode);

/**
 * \brief Set the Tx attenuation for the specified channel
 *
 * \pre If attenuation mode is ADI_ADRV9001_TX_ATTENUATION_CONTROL_MODE_PIN: 
 *      channel state must be any of STANDBY, CALIBRATED, PRIMED.
 * \pre If attenuation mode is ADI_ADRV9001_TX_ATTENUATION_CONTROL_MODE_SPI:
 *      channel state must be any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 * \pre Attenuation mode is ADI_ADRV9001_TX_ATTENUATION_CONTROL_MODE_CLGC
 *      channel state must be any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *      clgcLoopOpen must be 0
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 * \note The new attenuation only takes effect in the RF_ENABLED state - may read back incorrect value otherwise
 *
 * \param[in] adrv9001          Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel           The Tx channel for which to set the attenuation
 * \param[in] attenuation_mdB
 * \parblock
 * The desired attenuation in milli-dB.
 *
 * Constraints:
 * | adi_adrv9001_TxAttenuationConfig_t.outputSignaling | Maximum Attenuation (mdB) | Resolution (mdB)  |
 * | :------------------------------------------------: | :-----------------------: | :--------------:  |
 * |                ADI_ADRV9001_FM_DM                  |           12000           |       500         |
 * |                      Other                         |           41950           |        50         |
 *
 * Value must be a multiple of the Resolution (above) between 0 and the Maximum Attenuation (above).
 *
 * The ADRV9001 attenuator block consists of an analog portion (coarse adjustments) and a digital portion (fine
 * adjustments). Normally, both analog and digital attenuation are used. In FM_DM mode only the analog attenuation is
 * used. As a result, the resolution is decreased. The maximum attenuation is decreased because the resolution can't be
 * satisfied below that threshold.
 * \endparblock
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Tx_Attenuation_Set(adi_adrv9001_Device_t *adrv9001,
                                        adi_common_ChannelNumber_e channel,
                                        uint16_t attenuation_mdB);

/**
 * \brief Get the Tx attenuation for the specified channel
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \pre This feature requires the initialization to be complete and the attenuation table to be loaded.
 *  
 * \note During the transition from RF_ENABLED to PRIMED, attenuation will be ramped up to 40dB to protect the analog
 * front-end. During the reverse transition, the attenuation will be ramped to the value last set by the user. As a
 * result, unexpected values may be returned during TDD operation.
 *
 * \param[in]  adrv9001         Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel          The Tx channel for which to get the attenuation
 * \param[out] attenuation_mdB  The current attenuation in milli-dB (Range: 0 to 41950 mdB)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Tx_Attenuation_Get(adi_adrv9001_Device_t *adrv9001,
                                        adi_common_ChannelNumber_e channel,
                                        uint16_t *attenuation_mdB);

/**
 * \brief Enable or disable the Tx output power boost
 *
 * Enable or disable the Tx output power boost. Enabling the boost increases the output power by 3dB. Linearity will be
 * degraded. Boost is disabled by default.
 *
 * \pre Channel state is STANDBY
 * \pre Attenuation mode is not ADI_ADRV9001_TX_ATTENUATION_CONTROL_MODE_CLGC
 * \note Enabling power boost after calibrations requires running calibrations again or performance will be degraded
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in] adrv9001          Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel           The Tx channel of interest
 * \param[in] boostEnable       Whether or not to enable the Tx output power boost
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Tx_OutputPowerBoost_Set(adi_adrv9001_Device_t *adrv9001,
                                             adi_common_ChannelNumber_e channel,
                                             bool boostEnable);

/**
 * \brief Get the current Tx output power boost enable status
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in]  adrv9001         Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel          The Tx channel of interest
 * \param[out] boostEnabled     Whether or not the Tx output power boost is currently enabled
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Tx_OutputPowerBoost_Get(adi_adrv9001_Device_t *adrv9001,
                                             adi_common_ChannelNumber_e channel,
                                             bool *boostEnabled);

/**
 * \brief Write the attenuation table for the specified Tx channels
 *
 * The full attenuation table can be loaded in a single call or the table can be loaded in several calls,
 * loading a subset of the table with each function call using the indexOffset parameter.
 *
 * \pre This function may be called any time after device initialization
 *
 * \todo Automatically set unused entries to max attenuation
 * \note All unused attenuation table entries should be written to max attenuation so that if an
 * index that doesn't have an entry is selected, the output is known and deterministic.
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in] adrv9001              Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channelMask           An OR'd combination of adi_common_ChannelNumber_e
 *                                  specifying the Tx Channels for which to write the atten table
 * \param[in] indexOffset           The attenuation table index at which to start writing (0 - 839).
 * \param[in] attenTableRows        Attenuation table entries to write
 * \param[in] arraySize             The number of attenuation table rows to write (1 - 840).
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Tx_AttenuationTable_Write(adi_adrv9001_Device_t *adrv9001,
                                               uint32_t channelMask,
                                               uint32_t indexOffset,
                                               adi_adrv9001_TxAttenTableRow_t attenTableRows[],
                                               uint32_t arraySize);

/**
 * \brief Read the atten table entries for the specified Tx channel
 *
 * The full TxAtten table can be read in a single call or the table can be read in several calls,
 * reading a subset of the table with each function call using the txAttenIndexOffset parameter.
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \pre This function may be called any time after device initialization
 *
 * \param[in]  adrv9001             Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel              The Tx channel from which to read the attenuation table
 * \param[in]  indexOffset          The attenuation table index at which to start reading (0 - 839).
 * \param[out] attenTableRows       Attenuation table entries to write
 * \param[in]  arraySize            The size of the attenTableRows array; the max number of atten table rows to read
 * \param[out] numAttenIndicesRead  The actual no. of atten indices read. Pass NULL if this info is not needed
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Tx_AttenuationTable_Read(adi_adrv9001_Device_t *adrv9001,
                                              adi_common_ChannelNumber_e channel,
                                              uint32_t indexOffset,
                                              adi_adrv9001_TxAttenTableRow_t attenTableRows[],
                                              uint32_t arraySize,
                                              uint16_t *numAttenIndicesRead);

/**
 * \brief Configure PA Protection for the specified Tx channel
 *
 * This function sets up the PA Protection functionality and enables Tx sample
 * power measurements. It does not enable the ability to change Tx Attenuation
 * automatically.
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \pre Complete normal ADRV9001 initialization and init cals before running this function.
 *
 * \param[in] adrv9001      Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel       The Tx channel for which to configure PA protection
 * \param[in] config        The desired PA protection configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Tx_PaProtection_Configure(adi_adrv9001_Device_t *adrv9001,
                                               adi_common_ChannelNumber_e channel,
                                               adi_adrv9001_TxPaProtectCfg_t *config);

/**
 * \brief Inspect the PA protection configuration for the specified Tx channel
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \pre Complete normal ADRV9001 initialization and init cals before running this function.
 *
 * \param[in]  adrv9001     Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel      The Tx channel for which to inspect the PA protection configuration
 * \param[out] config       The current PA protection configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Tx_PaProtection_Inspect(adi_adrv9001_Device_t *adrv9001,
                                             adi_common_ChannelNumber_e channel,
                                             adi_adrv9001_TxPaProtectCfg_t *config);

/**
 * \brief Configure the Tx NCO test tone for the specified Tx channel
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is CALIBRATED
 *
 * \param[in] adrv9001      Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel       The Tx channel for which to set the NCO test tone frequency
 * \param[in] tone          The desired NCO tone
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Tx_InternalToneGeneration_Configure(adi_adrv9001_Device_t *adrv9001,
                                                         adi_common_ChannelNumber_e channel,
                                                         adi_adrv9001_TxInternalToneGeneration_t *tone);

/**
 * \brief Inspect the Tx NCO test tone for the specified Tx channel
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv9001     Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel      The Tx channel for which to get the NCO test tone frequency
 * \param[out] tone         The current NCO tone
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Tx_InternalToneGeneration_Inspect(adi_adrv9001_Device_t *adrv9001,
                                                       adi_common_ChannelNumber_e channel,
                                                       adi_adrv9001_TxInternalToneGeneration_t *tone);


/**
 * \brief Configure the slew rate limiter for the specified Tx channel
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is STANDBY
 *
 * \param[in] adrv9001          Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel           The Tx channel for which to configure the slew rate limiter
 * \param[in] config            The desired slew rate limiter configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Tx_SlewRateLimiter_Configure(adi_adrv9001_Device_t *adrv9001,
                                               adi_common_ChannelNumber_e channel,
                                               adi_adrv9001_SlewRateLimiterCfg_t *config);

/**
 * \brief Inspect the slew rate limiter configuration for the specified Tx channel
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv9001         Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel          The Tx channel for which to configure the slew rate limiter
 * \param[out] config           The desired slew rate limiter configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Tx_SlewRateLimiter_Inspect(adi_adrv9001_Device_t *adrv9001,
                                             adi_common_ChannelNumber_e channel,
                                             adi_adrv9001_SlewRateLimiterCfg_t *config);

/**
 * \brief This function configures the PA ramp for the specified Tx channel
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 * 
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED
 *
 * \param[in] adrv9001      Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel       The Tx channel for which to configure the PA ramp
 * \param[in] paRampCfg     PA ramp configuration to be written, gpioSource can not equal ADI_ADRV9001_GPIO_UNASSIGNED even if triggerSelect != ADI_ADRV9001_TX_PA_RAMP_TRIGGER_GPIO
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Tx_PaRamp_Configure(adi_adrv9001_Device_t *adrv9001,
                                         adi_common_ChannelNumber_e channel,
                                         adi_adrv9001_PaRampCfg_t *paRampCfg);

/**
 * \brief This function reads back the PA ramp configuration for the specified Tx channel
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 * 
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED
 *
 * \param[in]  adrv9001     Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel      The Tx channel for which to configure the PA ramp
 * \param[out] paRampCfg    PA ramp configuration read back, _Inspect will always return auxDacChannelSelect == ADI_ADRV9001_AUXDAC0
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Tx_PaRamp_Inspect(adi_adrv9001_Device_t *adrv9001,
                                       adi_common_ChannelNumber_e channel,
                                       adi_adrv9001_PaRampCfg_t *paRampCfg);

/**
 * \brief Configure the Tx attenuation through GPIO pins for the specified Tx channel
 *
 * The relationship between Tx attenuation control mode and channel state is shown in the following timing diagram
 * 
 * \image html "ADRV9001_TxAtten_State_Transitions.png" "Tx Attenuation control modes and channel state diagram"
 * 
 * \note Message type: \ref timing_direct "Direct register acccess"
 * \note Tx attenuation pin control configure is not supported in TX_DIRECT_FM_FSK mode
 * \note Tx attenuation control via GPIO pins is possible only in RF_ENABLED state
 *
 * \pre  Channel state must be CALIBRATED
 * \pre  Attenuation mode is not ADI_ADRV9001_TX_ATTENUATION_CONTROL_MODE_CLGC
 *
 * \param[in]  adrv9001    Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel     The Tx channel for which to configure the Tx attenuation
 * \param[in]  config      The desired Tx attenuation pin control configuration to be written
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Tx_Attenuation_PinControl_Configure(adi_adrv9001_Device_t *adrv9001,
                                                         adi_common_ChannelNumber_e channel,
                                                         adi_adrv9001_TxAttenuationPinControlCfg_t *config);

/**
 * \brief Inspect the Tx attenuation through GPIO pins for the specified Tx channel
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 * \note Tx attenuation pin control inspect is not supported in TX_DIRECT_FM_FSK mode
 *
 * \param[in]  adrv9001    Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  channel     The Tx channel for which to inspect the Tx attenuation
 * \param[out] config      The current Tx attenuation pin control configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Tx_Attenuation_PinControl_Inspect(adi_adrv9001_Device_t *adrv9001,
                                                       adi_common_ChannelNumber_e channel,
                                                       adi_adrv9001_TxAttenuationPinControlCfg_t *config);

/**
 * \brief Set the NCO frequency to correct for small deviations in Tx LO frequency
 *
 * \note Message type: \ref timing_prioritymailbox "High-priority mailbox command"
 *
 * \pre Channel state any of CALIBRATED, PRIMED, RF_ENABLED
 * \pre adi_adrv9001_Init_t.tx.txProfile.txDpProfile.txIqdmDuc.iqdmDucMode must be ADI_ADRV9001_TX_DP_IQDMDUC_MODE2
 *
 * \param[in] adrv9001              Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel               The Tx channel for which to set the NCO frequency
 * \param[in] frequencyOffset_Hz    The desired offset frequency, denoted in Hz
 * \param[in] immediate             Whether to change the frequency immediately (true), or
 *                                  update it at the start of the next available frame (false)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Tx_FrequencyCorrection_Set(adi_adrv9001_Device_t *adrv9001,
                                                adi_common_ChannelNumber_e channel,
                                                int32_t frequencyOffset_Hz,
                                                bool immediate);

/**
 * \brief Set the Rx datapath to Tx datapath loopback. 
 *		  This function loops back Rx port A/B datapath to Tx datapath for hardware debug and SSI is bypassed. 
 *		  Allows verification of the datapath using an RF signal applied to the Rx Port and observed at the Tx Port 
 *		  without the need for SSI configuration.
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in] adrv9001              Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel               The channel for which to set datapath loopback
 * \param[in] loopbackEnable        A boolean flag to enable or disable ADRV9001 Tx datapath to Rx datapath loopback
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Tx_DataPath_Loopback_Set(adi_adrv9001_Device_t *adrv9001,
                                              adi_common_ChannelNumber_e channel,
                                              bool loopbackEnable);


#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_TX_H_ */
