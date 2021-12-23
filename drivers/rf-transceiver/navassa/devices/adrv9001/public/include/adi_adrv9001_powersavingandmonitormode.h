/**
 * \file
 * \brief Contains ADRV9001 Power saving and Monitor mode related function prototypes for adi_adrv9001_PowerSavingAndMonitorMode.c
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2021 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_POWERSAVINGANDMONITORMODE_H_
#define _ADI_ADRV9001_POWERSAVINGANDMONITORMODE_H_

#include "adi_adrv9001_powersavingandmonitormode_types.h"
#include "adi_common_error_types.h"
#include "adi_adrv9001_types.h"
#include "adi_adrv9001_error.h"
#include "adrv9001_arm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Configure the power saving settings for the specified channel
 *
 * \note Message type: \ref timing_prioritymailbox "High-priority mailbox command"
 *
 * \pre Both Rx and Tx channels must be in any of CALIBRATED, PRIMED, RF_ENABLE states
 *
 * \param[in] adrv9001		    Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] channel           The Channel for which to configure the power saving settings
 * \param[in] powerSavingCfg    The desired power saving configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_powerSavingAndMonitorMode_ChannelPowerSaving_Configure(adi_adrv9001_Device_t *adrv9001,
                                                                            adi_common_ChannelNumber_e channel,
                                                                            adi_adrv9001_PowerSavingAndMonitorMode_ChannelPowerSavingCfg_t *powerSavingCfg);

/**
 * \brief Inspect the power saving settings for the specified channel
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Both Rx and Tx channels must be in any of CALIBRATED, PRIMED, RF_ENABLE states
 *
 * \param[in] adrv9001		    Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] channel           The Channel for which to inspect the power saving settings
 * \param[out] powerSavingCfg   The current power saving configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_powerSavingAndMonitorMode_ChannelPowerSaving_Inspect(adi_adrv9001_Device_t *adrv9001,
                                                                          adi_common_ChannelNumber_e channel,
                                                                          adi_adrv9001_PowerSavingAndMonitorMode_ChannelPowerSavingCfg_t *powerSavingCfg);

/**
 * \brief Configure the monitor mode settings for all channels
 *
 * \note Message type: \ref timing_prioritymailbox "High-priority mailbox command"
 *
 * \pre The ADI_ADRV9001_GPIO_SIGNAL_MON_ENABLE_SPS signal must be assigned to a pin with adi_adrv9001_gpio_Configure
 * \pre All channels in any of STANDBY, CALIBRATED, PRIMED, RF_ENABLE states
 *
 * \param[in] adrv9001		    Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] monitorModeCfg    The desired monitor mode configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_powerSavingAndMonitorMode_SystemPowerSavingAndMonitorMode_Configure(adi_adrv9001_Device_t *adrv9001,
                                                                                         adi_adrv9001_PowerSavingAndMonitorMode_SystemPowerSavingAndMonitorModeCfg_t *monitorModeCfg);

/**
 * \brief Inspect the monitor mode settings for all channels
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre All channels in any of STANDBY, CALIBRATED, PRIMED, RF_ENABLE states
 *
 * \param[in] adrv9001		    Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[out] monitorModeCfg   The current monitor mode configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_powerSavingAndMonitorMode_SystemPowerSavingAndMonitorMode_Inspect(adi_adrv9001_Device_t *adrv9001,
                                                                                       adi_adrv9001_PowerSavingAndMonitorMode_SystemPowerSavingAndMonitorModeCfg_t *monitorModeCfg);

/**
 * \brief Set the System Power Saving mode
 *
 * \note Message type: \ref timing_prioritymailbox "High-priority mailbox command"
 *
 * \param[in] adrv9001		Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] mode          The desired system power saving power down mode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_powerSavingAndMonitorMode_SystemPowerSavingMode_Set(adi_adrv9001_Device_t *adrv9001,
                                                                         adi_adrv9001_PowerSavingAndMonitorMode_SystemPowerDownMode_e mode);

/**
 * \brief Get the System Power Saving mode
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \param[in]  adrv9001	    Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[out] mode         The current system power saving power down mode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_powerSavingAndMonitorMode_SystemPowerSavingMode_Get(adi_adrv9001_Device_t *adrv9001,
                                                                         adi_adrv9001_PowerSavingAndMonitorMode_SystemPowerDownMode_e *mode);

/**
 * \brief Configure monitor mode pattern and inspects each pattern by reading back
 *
 * \details Loads up to `2048 - dpinfifo_rd_addr_offset` IQ samples into the input
 *          FIFO. More than would cause the FIFO to discard the first sample(s)
 *          loaded.
 *
 *          This function obtains the current value of dpinfifo_rd_addr_offset (for
 *          range checking) by reading it from the ADRV9001 device.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre Call adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Prepare first
 *
 * \param[in]  adrv9001			    Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  monitorModePattern   The desired monitor mode pattern
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Pattern_Configure(adi_adrv9001_Device_t *adrv9001,
                                                                             adi_adrv9001_PowerSavingAndMonitorMode_MonitorModePatternCfg_t *monitorModePattern);

/**
 * \brief Configure monitor mode vector
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in]  adrv9001			   Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  monitorModeVector   The desired monitor mode vector
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Vector_Configure(adi_adrv9001_Device_t *adrv9001,
                                                                            adi_adrv9001_PowerSavingAndMonitorMode_MonitorModeVectorCfg_t *monitorModeVector);

/**
 * \brief Get the monitor mode vector
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \param[in]  adrv9001 		   Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[out] monitorModeVector   The current monitor mode vector
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Vector_Inspect(adi_adrv9001_Device_t *adrv9001,
                                                                          adi_adrv9001_PowerSavingAndMonitorMode_MonitorModeVectorCfg_t *monitorModeVector);

/**
 * \brief Set the monitor mode RSSI configuration settings
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is STANDBY, CALIBRATED or PRIMED
 *
 * \param[in] adrv9001	           Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] monitorModeRssiCfg   The desired monitor mode RSSI configuration settings
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Rssi_Configure(adi_adrv9001_Device_t *adrv9001,
                                                                          adi_adrv9001_PowerSavingAndMonitorMode_MonitorModeRssiCfg_t *monitorModeRssiCfg);

/**
 * \brief Get the monitor mode RSSI configuration settings
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv9001	            Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[out] monitorModeRssiCfg   The current monitor mode RSSI configuration settings
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Rssi_Inspect(adi_adrv9001_Device_t *adrv9001,
                                                                        adi_adrv9001_PowerSavingAndMonitorMode_MonitorModeRssiCfg_t *monitorModeRssiCfg);

/**
 * \brief Begin preparing to run the Rx DMR path delay calibration.
 *
 * \details Call this before adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Pattern_Configure
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is CALIBRATED
 *
 * \param[in] adrv9001	        Context variable - Pointer to the ADRV9001 device settings data structure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Prepare(adi_adrv9001_Device_t *adrv9001);

/**
 * \brief Run the Rx DMR Path Delay calibration.
 *
 * \details This triggers the calibration function, which searches for the peak
 * correlation between the waveform pattern and the sync vector.
 * To fetch the result, call adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Get.
 *
 * \note Always operates on Rx channel 1
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is CALIBRATED, pattern and vector are already loaded
 *
 * \param[in]  adrv9001         Context variable - Pointer to the ADRV9001 device settings data structure
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
int32_t adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Run(adi_adrv9001_Device_t *adrv9001,
                                                                       uint32_t timeout_ms,
                                                                       uint8_t *initCalsError);

/**
 * \brief Measure the Rx DMR Path Delay.
 *
 * \details Measures the latency of the ADRV9001 receiver when it detects a
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
 * \param[in] adrv9001            Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] monitorModePattern  The desired monitor mode pattern
 * \param[in] monitorModeVector   The desired monitor mode vector
 * \param[in] timeout_ms          A timeout value in milliseconds to wait for the initcal to complete. This
 *                                does not include configuring the Pattern and Vector.
 * \param[out] initCalsError      A 3-bit error flag that helps identify any errors during Initial calibrations.
 *                               '0' indicates that there was no error.
 * \param[out] pathDelay          Result from DMR path delay calibration, 0..2047 samples. Use to config Monitor DMR Search
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Calibrate(adi_adrv9001_Device_t *adrv9001,
                                                                             adi_adrv9001_PowerSavingAndMonitorMode_MonitorModePatternCfg_t *monitorModePattern,
                                                                             adi_adrv9001_PowerSavingAndMonitorMode_MonitorModeVectorCfg_t *monitorModeVector,
                                                                             uint32_t timeout_ms,
                                                                             uint8_t *initCalsError,
                                                                             uint32_t *pathDelay);

/**
 * \brief Get the Rx DMR Path Delay.
 *
 * \details Fetches the Rx DMR path delay, as previously measured by
 * adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Run. See that function for more information.
 *
 * \note Always operates on Rx channel 1
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Run completed without error
 *
 * \param[in] adrv9001	        Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[out] pathDelay        Result from DMR path delay calibration, 0..2047 samples. Use to config Monitor DMR Search
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Get(adi_adrv9001_Device_t *adrv9001,
                                                                       uint32_t *pathDelay);

/**
 * \brief Configure the monitor mode DMR sync search
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is STANDBY or CALIBRATED
 *
 * \param[in] adrv9001	        Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] dmrSearchCfg      The desired DMR search settings
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_DmrSearch_Configure(adi_adrv9001_Device_t *adrv9001,
                                                                               adi_adrv9001_PowerSavingAndMonitorMode_MonitorModeDmrSearchCfg_t *dmrSearchCfg);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_POWERSAVINGANDMONITORMODE_H_ */
