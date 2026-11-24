/**
 * \file
 * \brief Contains ADRV910X Power saving and Monitor mode data types
 *
 * Copyright (c) 2021 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_ADRV910X_POWERSAVINGANDMONITORMODE_TYPES_H_
#define _ADI_ADRV910X_POWERSAVINGANDMONITORMODE_TYPES_H_

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "adi_adrv910x_syncdetect_types.h"
#include "adi_adrv910x_syncdetect.h"
#include "adi_adrv910x_user.h"

/**
 * \brief Enumerated list of ARM Monitor Mode States. Only valid when System State is in monitor mode.
 */
typedef enum adi_adrv910x_PowerSavingAndMonitorMode_ArmMonitorModeStates
{
    ADI_ADRV910X_POWERSAVINGANDMONITORMODE_ARM_MONITORMODE_SLEEP,             /*!< Sleep state in monitor mode */
    ADI_ADRV910X_POWERSAVINGANDMONITORMODE_ARM_MONITORMODE_DETECTING,         /*!< Detecting state in monitor mode */
    ADI_ADRV910X_POWERSAVINGANDMONITORMODE_ARM_MONITORMODE_DETECTED,          /*!< Detected state in monitor mode */
    ADI_ADRV910X_POWERSAVINGANDMONITORMODE_ARM_MONITORMODE_STREAMING_STREAMED /*!< Streaming or Streamed state in monitor mode */
} adi_adrv910x_PowerSavingAndMonitorMode_ArmMonitorModeStates_e;

/**
 * \brief Channel level power down modes
 */
typedef enum adi_adrv910x_PowerSavingAndMonitorMode_ChannelPowerDownMode
{
    ADI_ADRV910X_POWERSAVINGANDMONITORMODE_CHANNEL_MODE_DEFAULT = 0,      /*!< Default radio operation, no extra power-down */
    ADI_ADRV910X_POWERSAVINGANDMONITORMODE_CHANNEL_MODE_RFPLL_AND_LDO = 1 /*!< RF PLL and LDO power-down */
} adi_adrv910x_PowerSavingAndMonitorMode_ChannelPowerDownMode_e;

/**
 * \brief System level power down modes
 */
typedef enum adi_adrv910x_PowerSavingAndMonitorMode_SystemPowerDownMode
{
    ADI_ADRV910X_POWERSAVINGANDMONITORMODE_SYSTEM_MODE_CLKPLL = 2, /*!< CLK PLL power down */
    ADI_ADRV910X_POWERSAVINGANDMONITORMODE_SYSTEM_MODE_ARM = 3,    /*!< ARM power down using GPIO */
    ADI_ADRV910X_POWERSAVINGANDMONITORMODE_SYSTEM_MODE_ARM_PS2 = 4 /*!< ARM power down using PS2 */
} adi_adrv910x_PowerSavingAndMonitorMode_SystemPowerDownMode_e;

/**
 * \brief Monitor modes.
 */
typedef enum adi_adrv910x_PowerSavingAndMonitorMode_MonitorDetectionMode
{
    ADI_ADRV910X_POWERSAVINGANDMONITORMODE_MONITOR_DETECTION_MODE_RSSI,      /*!< RSSI detection only */
    ADI_ADRV910X_POWERSAVINGANDMONITORMODE_MONITOR_DETECTION_MODE_SYNC,      /*!< Sync detection only */
    ADI_ADRV910X_POWERSAVINGANDMONITORMODE_MONITOR_DETECTION_MODE_FFT,       /*!< FFT detection only */
    ADI_ADRV910X_POWERSAVINGANDMONITORMODE_MONITOR_DETECTION_MODE_RSSI_SYNC, /*!< RSSI and Sync detection */
    ADI_ADRV910X_POWERSAVINGANDMONITORMODE_MONITOR_DETECTION_MODE_RSSI_FFT   /*!< RSSI and FFT detection */
} adi_adrv910x_PowerSavingAndMonitorMode_MonitorDetectionMode_e;

/**
 * \brief Data structure to hold monitor mode configuration parameters for Channel power saving
 */
typedef struct adi_adrv910x_PowerSavingAndMonitorMode_ChannelPowerSavingCfg
{
    adi_adrv910x_PowerSavingAndMonitorMode_ChannelPowerDownMode_e channelDisabledPowerDownMode; /*!< Power down mode to apply when leaving the RF enabled state */
} adi_adrv910x_PowerSavingAndMonitorMode_ChannelPowerSavingCfg_t;

/**
 * \brief Data structure to hold monitor mode configuration parameters
 */
typedef struct adi_adrv910x_PowerSavingAndMonitorMode_SystemPowerSavingCfg
{
    adi_adrv910x_PowerSavingAndMonitorMode_SystemPowerDownMode_e powerDownMode;  /*!< Power down mode for Monitor */
    uint32_t initialBatterySaverDelay_us;                                        /*!< Delay Timer (us) for detection before entering Monitor Mode */
    uint32_t detectionTime_us;                                                   /*!< Timer for detection state (us) */
    uint32_t sleepTime_us;                                                       /*!< Timer for sleep state (us) */
    uint8_t detectionFirst;                                                      /*!< Select first state when entering Monitor Mode, 0-sleep first, 1-detection first */
    adi_adrv910x_PowerSavingAndMonitorMode_MonitorDetectionMode_e detectionMode; /*!< Mode of detection in the detect state */
    bool bbicWakeupLevelEnable;                                                  /*!< Enable ADI_ADRV910X_GPIO_SIGNAL_MON_BBIC_WAKEUP as a Level instead of Pulse (0 = Pulse, 1 = Level) */
    bool externalPllEnable;                                                      /*!< External PLL Enable, 0-disable, 1-enable */
} adi_adrv910x_PowerSavingAndMonitorMode_SystemPowerSavingCfg_t;

/**
 * \brief Data structure to hold monitor mode correlator pattern
 */
typedef struct adi_adrv910x_PowerSavingAndMonitorMode_MonitorModePatternCfg
{
    uint32_t patternLength;  /*!< Length of pattern in I/Q sample pairs */
    uint32_t patternI[2048]; /*!< 31-bit Monitor Mode I Detection Pattern */
    uint32_t patternQ[2048]; /*!< 31-bit Monitor Mode Q Detection Pattern */
} adi_adrv910x_PowerSavingAndMonitorMode_MonitorModePatternCfg_t;

/**
 * \brief RSSI measurement settings for monitor mode
 *
 * In this monitor mode configuration the ADRV910X will measure the incoming signal strength, and based on its power
 * decide either to notify the BBIC that a signal has been detected or to continue monitoring.  Measuring signal strength
 * starts by taking a single RSSI measurement that lasts measurementDuration_us.  The interval of such measurements
 * is in measurementsStartPeriod_ms.  Furthermore, the signal strength may be composed of an average of the
 * RSSI measurements taken.  The total time allotted to measuring the signal strength must be less than
 * adi_adrv910x_PowerSavingAndMonitorMode_SystemPowerSavingCfg_t.detectionTime_us.
 *
 * For example, given:
 *   numberOfMeasurementsToAverage = 4
 *   measurementsStartPeriod_ms = 7
 *   detectionThreshold_mdBFS = 100
 *   measurementDuration_samples = 24
 *   adi_adrv910x_PowerSavingAndMonitorMode_SystemPowerSavingCfg_t.detectionTime_us = 30000
 * In the 30ms detection frame, 4 RSSI measurements will be performed, one every 7ms. Each RSSI measurement is 1ms in
 * duration with approximately 6ms gap (end of one to start of next) between consecutive measurements.
 */
typedef struct adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeRssiCfg
{
    uint8_t numberOfMeasurementsToAverage; /*!< Number of RSSI measurements(1 to 255) to average */
    /**
    * \brief Period of consecutive RSSI measurements, denoted in milliseconds (valid: 0 to 255)
    *
    * \note 0: Continuous RSSI measurements (currently not supported by FW).
    * \note 1 to 255: Start a new RSSI measurement at regular intervals of measurementsStartPeriod_ms or as fast as the hardware is able to.
    * \note Irrelevant if numberOfMeasurementsToAverage is 1
    */
    uint8_t measurementsStartPeriod_ms;
    /**
    * \brief Threshold at which to notify BBIC (if exceeded by average RSSI)
    *
    * \note Step size for detection threshold is 100 mdBFS, ranging from -140000 to 0.
    */
    int32_t detectionThreshold_mdBFS;
    uint32_t measurementDuration_samples;  /*!< Duration of a single RSSI measurement denoted in samples ranging from 1 to (2^21 - 1) */
} adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeRssiCfg_t;

/**
 * \brief Data structure to hold monitor mode configuration at initialization time
 */
typedef struct adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeInitCfg
{
    adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeRssiCfg_t monitorModeRssiCfg; /*!< Structure to hold RSSI configuration at initialization time */
    adi_adrv910x_SyncDetect_SyncSearchCfg_t syncSearchCfg;                          /*!< structure to hold Sync search configuration used in monitor mode */
} adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeInitCfg_t;

#endif /* _ADI_ADRV910X_POWERSAVINGANDMONITORMODE_TYPES_H_ */
