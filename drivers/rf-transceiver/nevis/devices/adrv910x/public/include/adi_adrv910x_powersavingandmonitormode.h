/**
 * \file
 * \brief Contains ADRV910X Power saving and Monitor mode related function prototypes for
 *        adi_adrv910x_PowerSavingAndMonitorMode.c
 *
 * Copyright (c) 2021 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_ADRV910X_POWERSAVINGANDMONITORMODE_H_
#define _ADI_ADRV910X_POWERSAVINGANDMONITORMODE_H_

#include "adi_adrv910x_powersavingandmonitormode_types.h"
#include "adi_adrv910x_common_types.h"
#include "adi_adrv910x_error.h"
#include "adi_adrv910x_gpio.h"
#include "adi_adrv910x_syncdetect_types.h"
#include "adi_adrv910x_types.h"
#include "adi_common_error_types.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Configure the power saving settings for the specified channel
 *
 * \note Message type: \ref timing_prioritymailbox "High-priority mailbox command"
 * \pre Both Rx and Tx channels must be in any of #ADI_ADRV910X_CHANNEL_CALIBRATED, #ADI_ADRV910X_CHANNEL_PRIMED, #ADI_ADRV910X_CHANNEL_RF_ENABLED states
 *
 * \param[in] adrv910x        Context variable - Pointer to the ADRV910X device data structure
 * \param[in] channel         The Channel for which to configure the power saving settings
 * \param[in] powerSavingCfg  The desired power saving configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_ChannelPowerSaving_Configure(adi_adrv910x_Device_t *adrv910x,
                                                                            adi_common_ChannelNumber_e channel,
                                                                            adi_adrv910x_PowerSavingAndMonitorMode_ChannelPowerSavingCfg_t *powerSavingCfg);

/**
 * \brief Inspect the power saving settings for the specified channel
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * \pre Both Rx and Tx channels must be in any of #ADI_ADRV910X_CHANNEL_CALIBRATED, #ADI_ADRV910X_CHANNEL_PRIMED, #ADI_ADRV910X_CHANNEL_RF_ENABLED states
 *
 * \param[in]  adrv910x        Context variable - Pointer to the ADRV910X device data structure
 * \param[in]  channel         The Channel for which to inspect the power saving settings
 * \param[out] powerSavingCfg  The current power saving configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_ChannelPowerSaving_Inspect(adi_adrv910x_Device_t *adrv910x,
                                                                          adi_common_ChannelNumber_e channel,
                                                                          adi_adrv910x_PowerSavingAndMonitorMode_ChannelPowerSavingCfg_t *powerSavingCfg);

/**
 * \brief Configure the monitor mode settings for all channels
 *
 * \note Message type: \ref timing_prioritymailbox "High-priority mailbox command"
 * \pre The ADI_ADRV910X_GPIO_SIGNAL_MON_ENABLE_SPS signal must be assigned to a pin with adi_adrv910x_gpio_Ps1_Configure
 * \pre All channels in any of #ADI_ADRV910X_CHANNEL_STANDBY, #ADI_ADRV910X_CHANNEL_CALIBRATED, #ADI_ADRV910X_CHANNEL_PRIMED, #ADI_ADRV910X_CHANNEL_RF_ENABLED states
 *
 * \param[in] adrv910x        Context variable - Pointer to the ADRV910X device data structure
 * \param[in] monitorModeCfg  The desired monitor mode configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_SystemPowerSaving_Configure(adi_adrv910x_Device_t *adrv910x,
                                                                           adi_adrv910x_PowerSavingAndMonitorMode_SystemPowerSavingCfg_t *monitorModeCfg);

/**
 * \brief Inspect the monitor mode settings for all channels
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * \pre All channels in any of #ADI_ADRV910X_CHANNEL_STANDBY, #ADI_ADRV910X_CHANNEL_CALIBRATED, #ADI_ADRV910X_CHANNEL_PRIMED, #ADI_ADRV910X_CHANNEL_RF_ENABLED states
 *
 * \param[in]  adrv910x        Context variable - Pointer to the ADRV910X device data structure
 * \param[out] monitorModeCfg  The current monitor mode configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_SystemPowerSaving_Inspect(adi_adrv910x_Device_t *adrv910x,
                                                                         adi_adrv910x_PowerSavingAndMonitorMode_SystemPowerSavingCfg_t *monitorModeCfg);

/**
 * \brief Set the System Power Saving mode
 *
 * \note Message type: \ref timing_prioritymailbox "High-priority mailbox command"
 *
 * \param[in] adrv910x  Context variable - Pointer to the ADRV910X device data structure
 * \param[in] mode      The desired system power saving power down mode
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
 * \param[in]  adrv910x  Context variable - Pointer to the ADRV910X device data structure
 * \param[out] mode      The current system power saving power down mode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_SystemPowerSavingMode_Get(adi_adrv910x_Device_t *adrv910x,
                                                                         adi_adrv910x_PowerSavingAndMonitorMode_SystemPowerDownMode_e *mode);

/**
 * \brief Configure monitor mode pattern and inspects each pattern by reading back
 *
 * FIFO. More than would cause the FIFO to discard the first sample(s)
 * loaded.
 *
 * This function obtains the current value of dpinfifo_rd_addr_offset (for
 * range checking) by reading it from the ADRV910X device.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 * \pre Call adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Prepare first
 *
 * \param[in] adrv910x            Context variable - Pointer to the ADRV910X device data structure
 * \param[in] monitorModePattern  The desired monitor mode pattern
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Pattern_Configure(adi_adrv910x_Device_t *adrv910x,
                                                                             adi_adrv910x_PowerSavingAndMonitorMode_MonitorModePatternCfg_t *monitorModePattern);

/**
 * \brief Set the monitor mode RSSI configuration settings
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * \pre Channel state is #ADI_ADRV910X_CHANNEL_STANDBY, #ADI_ADRV910X_CHANNEL_CALIBRATED or #ADI_ADRV910X_CHANNEL_PRIMED
 *
 * \param[in] adrv910x            Context variable - Pointer to the ADRV910X device data structure
 * \param[in] monitorModeRssiCfg  The desired monitor mode RSSI configuration settings
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Rssi_Configure(adi_adrv910x_Device_t *adrv910x,
                                                                          adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeRssiCfg_t *monitorModeRssiCfg);

/**
 * \brief Get the monitor mode RSSI configuration settings
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * \pre Channel state any of #ADI_ADRV910X_CHANNEL_STANDBY, #ADI_ADRV910X_CHANNEL_CALIBRATED, #ADI_ADRV910X_CHANNEL_PRIMED, #ADI_ADRV910X_CHANNEL_RF_ENABLED
 *
 * \param[in]  adrv910x            Context variable - Pointer to the ADRV910X device data structure
 * \param[out] monitorModeRssiCfg  The current monitor mode RSSI configuration settings
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Rssi_Inspect(adi_adrv910x_Device_t *adrv910x,
                                                                        adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeRssiCfg_t *monitorModeRssiCfg);

/**
 * \brief Begin preparing to run the Rx DMR path delay calibration.
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * \pre Channel state is #ADI_ADRV910X_CHANNEL_CALIBRATED
 *
 * \param[in] adrv910x  Context variable - Pointer to the ADRV910X device data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Prepare(adi_adrv910x_Device_t *adrv910x);

/**
 * \brief Run the Rx DMR Path Delay calibration.
 *
 * correlation between the waveform pattern and the sync vector.
 * To fetch the result, call adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Get.
 *
 * \note Always operates on Rx channel 1
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * \pre Channel state is #ADI_ADRV910X_CHANNEL_CALIBRATED, pattern and vector are already loaded
 *
 * \param[in]  adrv910x       Context variable - Pointer to the ADRV910X device data structure
 * \param[in]  timeout_ms     A timeout value in milliseconds to wait for the calibrations to complete. The timeout applies if the algorithm fails to find a sync in the input data. A value of 1000 ms may be suitable. Nominal execution time is: number of samples until sync is detected (maximum possible is ~2048) divided by the data path rate (typically 24 kHz or 48 kHz, but higher if burst mode is configured).
 * \param[out] initCalsError  A 3-bit error flag that helps identify any errors during Initial calibrations. '0' indicates that there was no error.
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Run(adi_adrv910x_Device_t *adrv910x,
                                                                       uint32_t timeout_ms,
                                                                       uint8_t *initCalsError);

/**
 * \brief Measure the Rx DMR Path Delay. Fetches the Rx DMR path delay, as previously measured by sync symbol as defined
 *        by the Digital Mobile Radio (DMR) standard.
 *
 * This loads a user-supplied waveform and sync vector, and determines
 * the delay until the vector is detected, relative to start-of-frame.
 *
 * \note Always operates on Rx channel 1
 * \note Message type: Sequence of mailbox commands and direct register access
 * \pre Channel state is #ADI_ADRV910X_CHANNEL_CALIBRATED
 *
 * \param[in]  adrv910x            Context variable - Pointer to the ADRV910X device data structure
 * \param[in]  monitorModePattern  The desired monitor mode pattern
 * \param[in]  syncDetectVector   The desired sync detect vector
 * \param[in]  timeout_ms          A timeout value in milliseconds to wait for the initcal to complete. This does not include configuring the Pattern and Vector.
 * \param[out] initCalsError       A 3-bit error flag that helps identify any errors during Initial calibrations. '0' indicates that there was no error.
 * \param[out] pathDelay           Result from DMR path delay calibration, 0..2047 samples. Use to config Monitor DMR Search
 * \param[out] syncIndex           Index of sync vector detected, 0-14 vector indices
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Calibrate(adi_adrv910x_Device_t *adrv910x,
                                                                             adi_adrv910x_PowerSavingAndMonitorMode_MonitorModePatternCfg_t *monitorModePattern,
                                                                             adi_adrv910x_SyncDetect_SyncDetectVectorCfg_t *syncDetectVector,
                                                                             uint32_t timeout_ms,
                                                                             uint8_t *initCalsError,
                                                                             uint16_t *pathDelay,
                                                                             uint8_t *syncIndex);

/**
 * \brief Get the Rx DMR Path Delay.
 *
 * adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Run. See that function for more information.
 *
 * \note Always operates on Rx channel 1
 * \note Message type: \ref timing_mailbox "Mailbox command"
 * \pre adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Run completed without error
 *
 * \param[in]  adrv910x   Context variable - Pointer to the ADRV910X device data structure
 * \param[out] pathDelay  Result from DMR path delay calibration, 0..2047 samples. Use to config Monitor DMR Search
 * \param[out] syncIndex  Index of sync vector detected, 0-14 vector indices
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Get(adi_adrv910x_Device_t *adrv910x,
                                                                       uint16_t *pathDelay,
                                                                       uint8_t *syncIndex);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV910X_POWERSAVINGANDMONITORMODE_H_ */
