/**
 * \file
 * \brief Contains ADRV9001 calibration related function prototypes for
 *        adi_adrv9001_cals.c
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2015 - 2018 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_CALS_H_
#define _ADI_ADRV9001_CALS_H_

#include "adi_adrv9001_types.h"
#include "adi_adrv9001_cals_types.h"
#include "adi_adrv9001_tx_types.h"
#include "adi_adrv9001_rx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * Initialization functions
 ****************************************************************************
 */
/**
 * \brief Runs the ADRV9001 initialization calibrations
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre This function is called after the device has been initialized, and the RF PLL has been
 * verified to be locked
 *
 *  calMask Bit | Calibration
 *  ------------|----------------------
 *       0      | TX QEC Init Cal
 *       1      | TX LOL Init Cal
 *       2      | TX Loopback path delay Init Cal
 *       3      | TX LO Delay Init Cal
 *       4      | TX BBAF Init Cal
 *       5      | TX BBAF Gain Delay Init Cal
 *       6      | TX Attenuation Delay Init Cal
 *       7      | TX DAC Init Cal
 *       8      | TX Path Delay Init Cal
 *       9      | RX HP ADC Init Cal
 *       10     | RX HP ADC Flash Init Cal
 *       11     | RX HP ADC DAC Init Cal
 *       12     | RX HP ADC Stability Init Cal
 *       13     | RX LP ADC Init Cal
 *       14     | RX TIA Cutoff Init Cal
 *       15     | RX TIA Fine Init Cal
 *       16     | RX T - Cal Init Cal
 *       17     | RX FIIC Init Cal
 *       18     | RX Internal Loopback LO Delay Init Cal
 *       19     | RX RFDC Init Cal
 *       20     | RX Gain Path Delay Init Cal
 *       21     | PLL Init Cal
 *       22     | AUX PLL Init Cal
 *      [23]    | Reserved

 * \param[in]  adrv9001	        Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  initCals         Pointer to the InitCals structure which calibrations to run;
 *                              'calMode' must not be ADI_ADRV9001_INIT_CAL_MODE_ELB_ONLY
 * \param[in]  timeout_ms       A timeout value in milliseconds to wait for the calibrations to complete
 * \param[out] errorFlag        A 3-bit error flag that helps identify any errors during Initial calibrations.
 *                              '0' indicates that there was no error.
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_cals_InitCals_Run(adi_adrv9001_Device_t *adrv9001,
                                       adi_adrv9001_InitCals_t *initCals,
                                       uint32_t timeout_ms,
                                       uint8_t *errorFlag);

/**
 * \brief Builds a default instance of adi_adrv9001_InitCals_t with all Init Cals turned ON
 *
 * \param[out] initCals Pointer to an instance of adi_adrv9001_InitCals_t which will be populated with default values
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_cals_InitCalsBuildDefault(adi_adrv9001_InitCals_t *initCals);

/**
 * \brief Sets which tracking calibrations the ADRV9001 should continuously run for the specified channels
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED
 *
 * \ref adi_adrv9001_TrackingCalibrations_e
 *
 * \param[in] adrv9001	     Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] trackingCals   Tracking calibrations to enable. 
 *                           Each mask is an OR'd combination of 'adi_adrv9001_TrackingCalibrations_e' enum values
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_cals_Tracking_Set(adi_adrv9001_Device_t *adrv9001,
                                       adi_adrv9001_TrackingCals_t *trackingCals);

/**
 * \brief Gets which tracking calibrations the ADRV9001 is running
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv9001	      Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[out] trackingCals   The current tracking calibration bitmask information from ADRV9001 device
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_cals_Tracking_Get(adi_adrv9001_Device_t *adrv9001,
                                       adi_adrv9001_TrackingCals_t *trackingCals);

/**
 * \brief Runs the ADRV9001 external path delay calibrations
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state must be in CALIBRATED;
 *              Also in order to run external path delay calibrations, 'ADI_ADRV9001_INIT_CAL_TX_LB_PD' must have been enabled
 *              when Initial calibrations "adi_adrv9001_InitCals_Run()" is run
 *
 * \param[in]  adrv9001	       Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  channel         The Channel for which to run external path delay calibrations
 * \param[in]  timeout_ms      A timeout value in milliseconds to wait for the calibrations to complete
 * \param[out] initCalsError   A 3-bit error flag that helps identify any errors during Initial calibrations.
 *                             '0' indicates that there was no error.
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_cals_ExternalPathDelay_Run(adi_adrv9001_Device_t *adrv9001,
                                                adi_common_ChannelNumber_e channel,
                                                uint32_t timeout_ms,
                                                uint8_t *initCalsError);

/**
 * \brief Measures and gets the result of the difference in path delays between ILB and ELB and calculate the delay in tenths of a nanosecond
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state must be in CALIBRATED
 *
 * \param[in]  adrv9001	              Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  channel                The Channel for which to run external path delay calibrations
 * \param[out] externalPathDelay_ps   Measurement result of the difference in path delays (LSB is 100ps) between ILB and ELB
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_cals_ExternalMinusInternalPathDelay_Measure(adi_adrv9001_Device_t *adrv9001,
                                                                 adi_common_ChannelNumber_e channel,
                                                                 uint32_t *externalPathDelay_ps);

/**
 * \brief Runs the ADRV9001 external path delay calibrations and gets the measurement result of the difference 
 *        in path delays between ILB and ELB
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state must be in CALIBRATED
 *
 * \param[in]  adrv9001	              Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  channel                The Channel for which to run external path delay calibrations
 * \param[in]  timeout_ms             A timeout value in milliseconds to wait for the calibrations to complete
 * \param[out] initCalsError          A 3-bit error flag that helps identify any errors during Initial calibrations.
 *                                    '0' indicates that there was no error.
 * \param[out] externalPathDelay_ps   Measurement result of the difference in path delays (LSB is 100ps) between ILB and ELB
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_cals_ExternalPathDelay_Calibrate(adi_adrv9001_Device_t *adrv9001,
                                                      adi_common_ChannelNumber_e channel,
                                                      uint32_t timeout_ms,
                                                      uint8_t *initCalsError,
                                                      uint32_t *externalPathDelay_ps);

/**
 * \brief Sets the external path delay value measured 'adi_adrv9001_cals_External_Path_Delay_Calibrate' for the specified channels
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state in STANDBY and CALIBRATED only
 *
 * \param[in] adrv9001	             Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] channel                The Channel for which to run external path delay calibrations
 * \param[in] externalPathDelay_ps   Desired external path delay (LSB is 100ps; valid 0-6553500 ps)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_cals_ExternalPathDelay_Set(adi_adrv9001_Device_t *adrv9001,
                                                adi_common_ChannelNumber_e channel,
                                                uint32_t externalPathDelay_ps);

/**
 * \brief Gets the external path delay value for the specified channels
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv9001	              Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  channel                The Channel for which to run external path delay calibrations
 * \param[out] externalPathDelay_ps   Current external path delay (LSB is 100ps)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_cals_ExternalPathDelay_Get(adi_adrv9001_Device_t *adrv9001,
                                                adi_common_ChannelNumber_e channel,
                                                uint32_t *externalPathDelay_ps);

/**
 * \brief Get the internal path delay calculated for the given port and channel
 * 
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * 
 * \param[in]  adrv9001                 Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  port                     The port that the channel refers to
 * \param[in]  channel                  The channel of the specified port
 * \param[out] internalPathDelays_ns    An array of internal path delays in nanoseconds calculated for the given port/channel.
 *                                      internalPathDelays_ns[0] contains the value for the main profile
 *                                      internalPathDelays_ns[1:5] will return as 0x0 until Dynamic Profile Switching is supported
 * \param[in]  length                   Length of the array with '6 'is the maximum (1 main profile and 5 dynamic profile)
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_cals_InternalPathDelay_Get(adi_adrv9001_Device_t *adrv9001,
                                                adi_common_Port_e port,
                                                adi_common_ChannelNumber_e channel,
                                                uint32_t internalPathDelays_ns[],
                                                uint32_t length);

/**
 * \brief Get the carrier frequencies of all channels used in the previous successful call to the init cal
 * 
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * 
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 * 
 * \param[in]  adrv9001                 Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[out] carrierFrequencies_Hz    An array of carrier frequencies of all channels used in the previous successful call to the init cal in Hertz.
 * \param[in]  length                   Length of the array with '4 'is the maximum (In this specific order: Rx1, Rx2, Tx1, Tx2)
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_cals_LastInitCal_CarrierFrequency_Get(adi_adrv9001_Device_t *adrv9001,
                                                           uint64_t carrierFrequencies_Hz[],
                                                           uint32_t length);

/**
 * \brief Runs the ADRV9001 dynamic profiles calibrations
 * 
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * 
 * \pre Channel state is either STANDBY or CALIBRATED only
 * 
 * \param[in]  adrv9001          Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  initCals          Pointer to the InitCals structure which calibrations to run;
 *                               'calMode' must not be ADI_ADRV9001_INIT_CAL_MODE_ELB_ONLY
 * \param[in]  timeout_ms        A timeout value in milliseconds to wait for the calibrations to complete
 * \param[out] errorFlag         A 3-bit error flag that helps identify any errors during Initial calibrations.
 *                               '0' indicates that there was no error.
 * \param[in]  dynamicProfile    An array of dynamic profile parameters.
 * \param[in]  length            Length of the array with '6 'is the maximum
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_cals_Dynamic_profiles_calibrate(adi_adrv9001_Device_t *adrv9001,
                                                     adi_adrv9001_InitCals_t *initCals,
                                                     uint32_t timeout_ms,
                                                     uint8_t *errorFlag,
                                                     adi_adrv9000_DynamicProfile_t dynamicProfile[],
                                                     uint32_t length);

 /**
 * \brief Read the InitCal coefficients needed for Warmboot
 * 
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * 
 * \pre Channel state is CALIBRATED
 * 
 * \param[in]  device          Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[out] savedCals       Coefficients for enabled cals
 * \param[in]  maskChannel1    Calibration bit mask for channel 1
 * \param[in]  maskChannel2    Calibration bit mask for channel 2
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
 int32_t adi_adrv9001_cals_InitCals_WarmBoot_Coefficients_Get(adi_adrv9001_Device_t *device,
																adi_adrv9001_Warmboot_Coeff_t *savedCals,
																uint32_t maskChannel1,
																uint32_t maskChannel2);

 /**
 * \brief Store the InitCal coefficients needed for Warmboot
 * 
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * 
 * \pre Channel state is STANDBY
 * 
 * \param[in]  device          Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] savedCals        Coefficients for enabled cals
 * \param[in]  maskChannel1    Calibration bit mask for channel 1
 * \param[in]  maskChannel2    Calibration bit mask for channel 2
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
 int32_t adi_adrv9001_cals_InitCals_WarmBoot_Coefficients_Set(adi_adrv9001_Device_t *device,
																adi_adrv9001_Warmboot_Coeff_t *savedCals,
																uint32_t maskChannel1,
																uint32_t maskChannel2);
#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_CALS_H_ */

