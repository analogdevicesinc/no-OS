/**
 * \file
 * \brief Contains ADRV910X calibration related function prototypes for
 *        adi_adrv910x_cals.c
 *
 * ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
 */

 /**
 * Copyright 2022 Analog Devices Inc.
 * Released under the ADRV910X API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV910X_CALS_H_
#define _ADI_ADRV910X_CALS_H_

#include "adi_adrv910x_types.h"
#include "adi_adrv910x_cals_types.h"
#include "adi_adrv910x_tx_types.h"
#include "adi_adrv910x_rx_types.h"
#include "device_profile_init_cals_t.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 * Initialization functions
 ****************************************************************************
 */
/**
 * \brief Runs the ADRV910X initialization calibrations
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre All channels in the STANDBY or CALIBRATED states
 *
 * \param[in]  adrv910x	        Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in]  initCals         Pointer to the InitCals structure (from Profile Types) which calibrations to run;
 *                              'calMode' must not be ADI_ADRV910X_INIT_CAL_MODE_ELB_ONLY
 * \param[in]  timeout_ms       A timeout value in milliseconds to wait for the calibrations to complete
 * \param[out] errorFlag        A 3-bit error flag that helps identify any errors during Initial calibrations.
 *                              '0' indicates that there was no error.
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_cals_InitCals_Run(adi_adrv910x_Device_t *adrv910x,
                                       initCals_t *initCals,
                                       uint32_t timeout_ms,
                                       uint8_t *errorFlag);

/**
 * \brief Builds a default instance of adi_adrv910x_InitCals_t with all Init Cals turned ON
 *
 * \param[out] initCals Pointer to an instance of adi_adrv910x_InitCals_t which will be populated with default values
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_cals_InitCalsBuildDefault(initCals_t *initCals);

/**
 * \brief Sets which tracking calibrations the ADRV910X should continuously run for the specified channels
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED
 *
 * \ref adi_adrv910x_TrackingCalibrations_e
 *
 * \param[in] adrv910x	     Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in] trackingCals   Tracking calibrations to enable. 
 *                           Each mask is an OR'd combination of 'adi_adrv910x_TrackingCalibrations_e' enum values
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_cals_Tracking_Set(adi_adrv910x_Device_t *adrv910x,
                                       trackingCals_t *trackingCals);

/**
 * \brief Gets which tracking calibrations the ADRV910X is running
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv910x	      Context variable - Pointer to the ADRV910X device settings data structure
 * \param[out] trackingCals   The current tracking calibration bitmask information from ADRV910X device
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_cals_Tracking_Get(adi_adrv910x_Device_t *adrv910x,
                                       trackingCals_t *trackingCals);

/**
 * \brief Sets the External LO or VCO circuitry settings used for calibration
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv910x	          Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in]  port               The port that the channel refers to
 * \param[in]  channel            The channel of the specified port for which the LO parameters are applied to
 * \param[in]  loGenCalSettings   The circuitry settings of the LO generator used for calibration
 * 
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_cals_InitCalsLoGenParams_Configure(adi_adrv910x_Device_t *adrv910x,
                                                        adi_common_Port_e port,
                                                        adi_common_ChannelNumber_e channel,
                                                        adi_adrv910x_LoGenCalSettings_t *loGenCalSettings);
/**
 * \brief Gets the External LO or VCO circuitry settings used for calibration
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv910x	          Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in]  port               The port that the channel refers to
 * \param[in]  channel            The channel of the specified port for which the LO parameters are applied to
 * \param[out] loGenCalSettings   The circuitry settings of the LO generator used for calibration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_cals_InitCalsLoGenParams_Inspect(adi_adrv910x_Device_t *adrv910x,
                                                        adi_common_Port_e port,
                                                        adi_common_ChannelNumber_e channel,
                                                        adi_adrv910x_LoGenCalSettings_t *loGenCalSettings);

/**
 * \brief Runs the ADRV910x external path delay calibrations
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state must be in CALIBRATED;
 *              Also in order to run external path delay calibrations, 'ADI_ADRV910x_INIT_CAL_TX_LB_PD' must have been enabled
 *              when Initial calibrations "adi_adrv910x_InitCals_Run()" is run
 *
 * \param[in]  adrv910x	       Context variable - Pointer to the ADRV910x device settings data structure
 * \param[in]  channel         The Channel for which to run external path delay calibrations
 * \param[in]  timeout_ms      A timeout value in milliseconds to wait for the calibrations to complete
 * \param[out] initCalsError   A 3-bit error flag that helps identify any errors during Initial calibrations.
 *                             '0' indicates that there was no error.
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_cals_ExternalPathDelay_Run(adi_adrv910x_Device_t *adrv910x,
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
 * \param[in]  adrv910x	              Context variable - Pointer to the ADRV910x device settings data structure
 * \param[in]  channel                The Channel for which to run external path delay calibrations
 * \param[out] externalPathDelay_ps   Measurement result of the difference in path delays (LSB is 100ps) between ILB and ELB
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_cals_ExternalMinusInternalPathDelay_Measure(adi_adrv910x_Device_t *adrv910x,
																 adi_common_ChannelNumber_e channel,
																 uint32_t *externalPathDelay_ps);

/**
 * \brief Runs the ADRV910x external path delay calibrations and gets the measurement result of the difference
 *        in path delays between ILB and ELB
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state must be in CALIBRATED
 *
 * \param[in]  adrv910x	              Context variable - Pointer to the ADRV910x device settings data structure
 * \param[in]  channel                The Channel for which to run external path delay calibrations
 * \param[in]  timeout_ms             A timeout value in milliseconds to wait for the calibrations to complete
 * \param[out] initCalsError          A 3-bit error flag that helps identify any errors during Initial calibrations.
 *                                    '0' indicates that there was no error.
 * \param[out] externalPathDelay_ps   Measurement result of the difference in path delays (LSB is 100ps) between ILB and ELB
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_cals_ExternalPathDelay_Calibrate(adi_adrv910x_Device_t *adrv910x,
													  adi_common_ChannelNumber_e channel,
													  uint32_t timeout_ms,
													  uint8_t *initCalsError,
													  uint32_t *externalPathDelay_ps);

/**
 * \brief Sets the external path delay value measured 'adi_adrv910x_cals_External_Path_Delay_Calibrate' for the specified channels
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state in STANDBY and CALIBRATED only
 *
 * \param[in] adrv910x	             Context variable - Pointer to the ADRV910x device settings data structure
 * \param[in] channel                The Channel for which to run external path delay calibrations
 * \param[in] externalPathDelay_ps   Desired external path delay (LSB is 100ps; valid 0-6553500 ps)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_cals_ExternalPathDelay_Set(adi_adrv910x_Device_t *adrv910x,
												adi_common_ChannelNumber_e channel,
												uint32_t externalPathDelay_ps);

/**
 * \brief Gets the external path delay value for the specified channels
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv910x	              Context variable - Pointer to the ADRV910x device settings data structure
 * \param[in]  channel                The Channel for which to run external path delay calibrations
 * \param[out] externalPathDelay_ps   Current external path delay (LSB is 100ps)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_cals_ExternalPathDelay_Get(adi_adrv910x_Device_t *adrv910x,
												adi_common_ChannelNumber_e channel,
												uint32_t *externalPathDelay_ps);

/**
 * \brief Get the internal path delay calculated for the given port and channel
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \param[in]  adrv910x                 Context variable - Pointer to the ADRV910x device settings data structure
 * \param[in]  port                     The port that the channel refers to
 * \param[in]  channel                  The channel of the specified port
 * \param[out] internalPathDelays_ns    An array of internal path delays in nanoseconds calculated for the given port/channel.
 *                                      internalPathDelays_ns[0] contains the value for the main profile
 *                                      internalPathDelays_ns[1:5] will return as 0x0 until Dynamic Profile Switching is supported
 * \param[in]  length                   Length of the array with '6 'is the maximum (1 main profile and 5 dynamic profile)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_cals_InternalPathDelay_Get(adi_adrv910x_Device_t *adrv910x,
												adi_common_Port_e port,
												adi_common_ChannelNumber_e channel,
												uint32_t internalPathDelays_ns[],
												uint32_t length);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV910X_CALS_H_ */
