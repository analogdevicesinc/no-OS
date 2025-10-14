/**
 * \file
 * \brief Contains ADRV910X Power saving and Monitor mode related function prototypes for adi_adrv910x_PowerSavingAndMonitorMode.c
 *
 * ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
 */

 /**
 * Copyright 2021 Analog Devices Inc.
 * Released under the ADRV910X API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV910X_POWERSAVINGANDMONITORMODE_H_
#define _ADI_ADRV910X_POWERSAVINGANDMONITORMODE_H_

#include "adi_adrv910x_powersavingandmonitormode_types.h"
#include "adi_common_error_types.h"
#include "adi_adrv910x_types.h"
#include "adi_adrv910x_error.h"
#include "adi_adrv910x_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Configure the monitor mode settings for all channels
 *
 * \note Message type: \ref timing_prioritymailbox "High-priority mailbox command"
 *
 * \pre The ADI_ADRV910X_GPIO_SIGNAL_MON_ENABLE_SPS signal must be assigned to a pin with adi_adrv910x_gpio_Ps1_Configure
 * \pre All channels in any of STANDBY, CALIBRATED, PRIMED, RF_ENABLE states
 *
 * \param[in] adrv910x		    Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in] monitorModeCfg    The desired monitor mode configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_SystemPowerSavingAndMonitorMode_Configure(adi_adrv910x_Device_t *adrv910x,
                                                                                         adi_adrv910x_PowerSavingAndMonitorMode_SystemPowerSavingAndMonitorModeCfg_t *monitorModeCfg);

/**
 * \brief Inspect the monitor mode settings for all channels
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre All channels in any of STANDBY, CALIBRATED, PRIMED, RF_ENABLE states
 *
 * \param[in] adrv910x		    Context variable - Pointer to the ADRV910X device settings data structure
 * \param[out] monitorModeCfg   The current monitor mode configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_SystemPowerSavingAndMonitorMode_Inspect(adi_adrv910x_Device_t *adrv910x,
                                                                                       adi_adrv910x_PowerSavingAndMonitorMode_SystemPowerSavingAndMonitorModeCfg_t *monitorModeCfg);

/**
 * \brief Set the System Power Saving mode
 *
 * \note Message type: \ref timing_prioritymailbox "High-priority mailbox command"
 *
 * \param[in] adrv910x		Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in] mode          The desired system power saving power down mode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_SystemPowerSavingMode_Set(adi_adrv910x_Device_t *adrv910x,
                                                                         adi_adrv910x_PowerSavingAndMonitorMode_SystemPowerDownMode_e mode);

/**
 * \brief Get the System Power Saving mode
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \param[in]  adrv910x	    Context variable - Pointer to the ADRV910X device settings data structure
 * \param[out] mode         The current system power saving power down mode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_SystemPowerSavingMode_Get(adi_adrv910x_Device_t *adrv910x,
                                                                         adi_adrv910x_PowerSavingAndMonitorMode_SystemPowerDownMode_e *mode);

/**
 * \brief Configure monitor mode pattern and inspects each pattern by reading back
 *
 * \details Loads up to `2048 - dpinfifo_rd_addr_offset` IQ samples into the input
 *          FIFO. More than would cause the FIFO to discard the first sample(s)
 *          loaded.
 *
 *          This function obtains the current value of dpinfifo_rd_addr_offset (for
 *          range checking) by reading it from the ADRV910X device.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre Call adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Prepare first
 *
 * \param[in]  adrv910x			    Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in]  monitorModePattern   The desired monitor mode pattern
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Pattern_Configure(adi_adrv910x_Device_t *adrv910x,
                                                                             adi_adrv910x_PowerSavingAndMonitorMode_MonitorModePatternCfg_t *monitorModePattern);

/**
 * \brief Configure monitor mode vector
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in]  adrv910x			   Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in]  monitorModeVector   The desired monitor mode vector
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Vector_Configure(adi_adrv910x_Device_t *adrv910x,
                                                                            adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeVectorCfg_t *monitorModeVector);

/**
 * \brief Get the monitor mode vector
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in]  adrv910x 		   Context variable - Pointer to the ADRV910X device settings data structure
 * \param[out] monitorModeVector   The current monitor mode vector
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Vector_Inspect(adi_adrv910x_Device_t *adrv910x,
                                                                          adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeVectorCfg_t *monitorModeVector);

/**
 * \brief Set the monitor mode RSSI configuration settings
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is STANDBY, CALIBRATED or PRIMED
 *
 * \param[in] adrv910x	           Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in] monitorModeRssiCfg   The desired monitor mode RSSI configuration settings
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Rssi_Configure(adi_adrv910x_Device_t *adrv910x,
                                                                          adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeRssiCfg_t *monitorModeRssiCfg);

/**
 * \brief Get the monitor mode RSSI configuration settings
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv910x	            Context variable - Pointer to the ADRV910X device settings data structure
 * \param[out] monitorModeRssiCfg   The current monitor mode RSSI configuration settings
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Rssi_Inspect(adi_adrv910x_Device_t *adrv910x,
                                                                        adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeRssiCfg_t *monitorModeRssiCfg);

/**
 * \brief Begin preparing to run the Rx DMR path delay calibration.
 *
 * \details Call this before adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Pattern_Configure
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is CALIBRATED
 *
 * \param[in] adrv910x	        Context variable - Pointer to the ADRV910X device settings data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Prepare(adi_adrv910x_Device_t *adrv910x);

/**
 * \brief Run the Rx DMR Path Delay calibration.
 *
 * \details This triggers the calibration function, which searches for the peak
 * correlation between the waveform pattern and the sync vector.
 * To fetch the result, call adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Get.
 *
 * \note Always operates on Rx channel 1
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is CALIBRATED, pattern and vector are already loaded
 *
 * \param[in]  adrv910x         Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in]  timeout_ms       A timeout value in milliseconds to wait for the calibrations to complete.
 *                              The timeout applies if the algorithm fails to find a sync in the input data.
 *                              A value of 1000 ms may be suitable. Nominal execution time is: number of
 *                              samples until sync is detected (maximum possible is ~2048) divided by the
 *                              data path rate (typically 24 kHz or 48 kHz, but higher if burst mode is
 *                              configured).
 * \param[out] initCalsError    A 3-bit error flag that helps identify any errors during Initial calibrations.
 *                             '0' indicates that there was no error.
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Run(adi_adrv910x_Device_t *adrv910x,
                                                                       uint32_t timeout_ms,
                                                                       uint8_t *initCalsError);

/**
 * \brief Measure the Rx DMR Path Delay.
 *
 * \details Measures the latency of the ADRV910X receiver when it detects a
 * sync symbol as defined by the Digital Mobile Radio (DMR) standard.
 *
 * This loads a user-supplied waveform and sync vector, and determines
 * the delay until the vector is detected, relative to start-of-frame.
 *
 * \note Always operates on Rx channel 1
 *
 * \note Message type: Sequence of mailbox commands and direct register access
 *
 * \pre Channel state is CALIBRATED
 *
 * \param[in] adrv910x            Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in] monitorModePattern  The desired monitor mode pattern
 * \param[in] monitorModeVector   The desired monitor mode vector
 * \param[in] timeout_ms          A timeout value in milliseconds to wait for the initcal to complete. This
 *                                does not include configuring the Pattern and Vector.
 * \param[out] initCalsError      A 3-bit error flag that helps identify any errors during Initial calibrations.
 *                               '0' indicates that there was no error.
 * \param[out] pathDelay          Result from DMR path delay calibration, 0..2047 samples. Use to config Monitor DMR Search
 * \param[out] syncIndex          Index of sync vector detected, 0-14 vector indices
 * 
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Calibrate(adi_adrv910x_Device_t *adrv910x,
                                                                             adi_adrv910x_PowerSavingAndMonitorMode_MonitorModePatternCfg_t *monitorModePattern,
                                                                             adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeVectorCfg_t *monitorModeVector,
                                                                             uint32_t timeout_ms,
                                                                             uint8_t *initCalsError,
                                                                             uint16_t *pathDelay,
                                                                             uint8_t *syncIndex);

/**
 * \brief Get the Rx DMR Path Delay.
 *
 * \details Fetches the Rx DMR path delay, as previously measured by
 * adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Run. See that function for more information.
 *
 * \note Always operates on Rx channel 1
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Run completed without error
 *
 * \param[in] adrv910x	        Context variable - Pointer to the ADRV910X device settings data structure
 * \param[out] pathDelay        Result from DMR path delay calibration, 0..2047 samples. Use to config Monitor DMR Search
 * \param[out] syncIndex        Index of sync vector detected, 0-14 vector indices
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Get(adi_adrv910x_Device_t *adrv910x,
                                                                       uint16_t *pathDelay, uint8_t *syncIndex);

/**
 * \brief Configure the monitor mode DMR sync search
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is CALIBRATED or PRIMED or RF_ENABLED
 *
 * \param[in] adrv910x	        Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in] dmrSearchCfg      The desired DMR search settings
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_DmrSearch_Configure(adi_adrv910x_Device_t *adrv910x,
                                                                               adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeDmrSearchCfg_t *dmrSearchCfg);
	
/**
 * \brief Inspect the monitor mode DMR sync search
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is CALIBRATED or PRIMED or RF_ENABLED
 *
 * \param[in] adrv910x	        Context variable - Pointer to the ADRV910X device settings data structure
 * \param[out] dmrSearchCfg     The desired DMR search settings
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_DmrSearch_Inspect(adi_adrv910x_Device_t *adrv910x,
		                                                            adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeDmrSearchCfg_t *dmrSearchCfg);

/**
 * \brief Set continuous sync detection
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \note detected must be UNASSIGNED or a valid digital GPIO pin, Analog GPIO pins are not supported.
 * 
 * \pre Channel state is CALIBRATED or PRIMED or RF_ENABLED
 *
 * \param[in] adrv910x	            Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in] syncDetectionEnable   Continuous sync detection Enable, 0-disable 1-enable
 * \param[in] detectedPin           Pin used to indicate the sync pattern was detected
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_ContinuousSyncDetect_Set(adi_adrv910x_Device_t *adrv910x,
																					bool syncDetectionEnable,
																					adi_adrv910x_GpioPin_e detectedPin);
/**
 * \brief Get continuous sync detection
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is CALIBRATED or PRIMED or RF_ENABLED
 *
 * \param[in]  adrv910x	             Context variable - Pointer to the ADRV910X device settings data structure
 * \param[out] syncDetectionEnable   Continuous sync detection Enable, 0-disable 1-enable
 * \param[out] detectedPin           Pin used to indicate the sync pattern was detected
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_ContinuousSyncDetect_Get(adi_adrv910x_Device_t *adrv910x,
																						bool *syncDetectionEnable,
																						adi_adrv910x_GpioPin_e *detectedPin);
/**
 * \brief Get continuous sync detection status and information
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is CALIBRATED or PRIMED or RF_ENABLED
 *
 * \param[in]  adrv910x	             Context variable - Pointer to the ADRV910X device settings data structure
 * \param[out] syncDetectionResult   Information about the sync detection event
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_ContinuousSyncDetect_Status_Get(adi_adrv910x_Device_t *adrv910x,
																							adi_adrv910x_PowerSavingAndMonitorMode_SyncDetectionResult_t *syncDetectionResult);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV910X_POWERSAVINGANDMONITORMODE_H_ */