/**
 * \file
 * \brief Contains ADRV9001 ARM data types
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2015 - 2018 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_ARM_TYPES_H_
#define _ADI_ADRV9001_ARM_TYPES_H_

#include "adi_adrv9001_stream_types.h"
#include "adi_adrv9001_gpio_types.h"

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

/**
* \brief Enumerated list of ARM System States.
*/
typedef enum adi_adrv9001_ArmSystemStates
{
    ADI_ADRV9001_ARM_SYSTEM_POWERUP,         /*!< Powerup State */
    ADI_ADRV9001_ARM_SYSTEM_NORMALMODE,      /*!< Normal TDD/FDD State */
    ADI_ADRV9001_ARM_SYSTEM_POWERSAVINGMODE, /*!< System Power Saving Mode */
    ADI_ADRV9001_ARM_SYSTEM_MCS              /*!< MCS State */
} adi_adrv9001_ArmSystemStates_e;

/**
* \brief Enumerated list of ARM Monitor Mode States.
*
* Only valid when System State is in monitor mode
*/
typedef enum adi_adrv9001_ArmMonitorModeStates
{
    ADI_ADRV9001_ARM_MONITORMODE_SLEEP,     /*!< Sleep state in monitor mode */
    ADI_ADRV9001_ARM_MONITORMODE_DETECTING, /*!< Detecting state in monitor mode */
    ADI_ADRV9001_ARM_MONITORMODE_DETECTED,  /*!< Detected state in monitor mode */
    ADI_ADRV9001_ARM_MONITORMODE_UNDEFINED
} adi_adrv9001_ArmMonitorModeStates_e;

/**
 * \brief The MCS substates
 */
typedef enum adi_adrv9001_ArmMcsStates
{
    ADI_ADRV9001_ARMMCSSTATES_READY,      /*!< MCS Ready state */
    ADI_ADRV9001_ARMMCSSTATES_TRANSITION, /*!< MCS Transition state */
    ADI_ADRV9001_ARMMCSSTATES_DONE,       /*!< MCS Done state */
} adi_adrv9001_ArmMcsStates_e;

/**
* \brief Enumerated list of ARM bootup states.
*/
typedef enum adi_adrv9001_ArmBootStates
{
    ADI_ADRV9001_ARM_BOOT_POWERUP                           = 0,    /*!< Used to put API in wait for ARM state */
    ADI_ADRV9001_ARM_BOOT_READY                             = 1,    /*!< ARM booted with no failure */
    ADI_ADRV9001_ARM_BOOT_FW_CHECKSUM_ERR                   = 2,    /*!< ARM firmware checksum error */
    ADI_ADRV9001_ARM_BOOT_EFUSE_DATA_ERR                    = 3,    /*!< Efuse data error */
    ADI_ADRV9001_ARM_BOOT_STREAM_IMG_CHECKSUM_ERR           = 4,    /*!< Stream image checksum error */
    ADI_ADRV9001_ARM_BOOT_DEVICE_PROFILE_CHECKSUM_ERR       = 5,    /*!< Device profile checksum error */
    ADI_ADRV9001_ARM_BOOT_CLKGEN_ERR                        = 6,    /*!< Bootup clkgen setup error */
    ADI_ADRV9001_ARM_BOOT_CLKSSI_ERR                        = 7,    /*!< Bootup SSI setup error */
    ADI_ADRV9001_ARM_BOOT_POWER_INIT_ERR                    = 8,    /*!< Power init setup error */
    ADI_ADRV9001_ARM_BOOT_JTAG_BUILD_STATUS_READY           = 9,    /*!< JTAG build status ready indication */
    ADI_ADRV9001_ARM_BOOT_CLKLOGEN_ERR                      = 10,   /*!< Bootup clock LOGEN error */
    ADI_ADRV9001_ARM_BOOT_RXQECHW_ERR                       = 11,   /*!< Error initializing RxQEC hardware */
    ADI_ADRV9001_ARM_BOOT_HM_TIMER_ERR                      = 12,   /*!< Failed to create health monitor timers */
    ADI_ADRV9001_ARM_BOOT_ADC_RCAL_ERR                      = 13,   /*!< ADC RCAL error */
    ADI_ADRV9001_ARM_BOOT_STREAM_RUNTIME_ERR                = 14,   /*!< Stream runtime error */
    ADI_ADRV9001_ARM_BOOT_CLKGEN_RCAL_ERR                   = 15,   /*!< Bootup clkgen setup after RCAL error */
} adi_adrv9001_ArmBootStates_e;

/**
* \brief Enumerated list of ARM image build options.
*/
typedef enum adi_adrv9001_ArmBuildType
{
    ADI_ADRV9001_ARMBUILD_RELEASE = 0,
    ADI_ADRV9001_ARMBUILD_DEBUG = 1,
    ADI_ADRV9001_ARMBUILD_TESTOBJ = 2
} adi_adrv9001_ArmBuildType_e;

/**
 * \brief Enumerated list of ARM Clock Type options.
 */
typedef enum adi_adrv9001_ArmClockType
{
    ADI_ADRV9001_ARMCLOCK_DATAPATH = 0,
    ADI_ADRV9001_ARMCLOCK_MAX
} adi_adrv9001_ArmClockType_e;

/**
 * \brief Per-channel power down modes
 */
typedef enum adi_adrv9001_ChannelPowerDownMode
{
    ADI_ADRV9001_CHANNEL_POWER_DOWN_MODE_DISABLED = 0,  /*!< Default radio operation, no extra power down */
    ADI_ADRV9001_CHANNEL_POWER_DOWN_MODE_RFPLL = 1,     /*!< RF PLL power down */
    ADI_ADRV9001_CHANNEL_POWER_DOWN_MODE_LDO = 2,       /*!< Channel LDO power down */
} adi_adrv9001_ChannelPowerDownMode_e;

/**
 * \brief System level power down modes
 */
typedef enum adi_adrv9001_SystemPowerDownMode
{
    ADI_ADRV9001_SYSTEM_POWER_DOWN_MODE_CLKPLL = 3,     /*!< CLK PLL power down */
    ADI_ADRV9001_SYSTEM_POWER_DOWN_MODE_LDO = 4,        /*!< LDO power down */
    ADI_ADRV9001_SYSTEM_POWER_DOWN_MODE_ARM = 5         /*!< ARM power down */
} adi_adrv9001_SystemPowerDownMode_e;

/**
 * \brief Monitor modes.
 */
typedef enum adi_adrv9001_MonitorDetectionMode
{
    ADI_ADRV9001_MONITOR_DETECTION_MODE_RSSI,        /*!< RSSI detection only */
    ADI_ADRV9001_MONITOR_DETECTION_MODE_SYNC,        /*!< Sync detection only */
    ADI_ADRV9001_MONITOR_DETECTION_MODE_FFT,         /*!< FFT detection only */
    ADI_ADRV9001_MONITOR_DETECTION_MODE_RSSI_SYNC,   /*!< RSSI and Sync detection */
    ADI_ADRV9001_MONITOR_DETECTION_MODE_RSSI_FFT,    /*!< RSSI and FFT detection */
} adi_adrv9001_MonitorDetectionMode_e;

/**
 * \brief Monitor mode correlator pattern length.
 */
typedef enum adi_adrv9001_MonitorModePatternLength
{
    ADI_ADRV9001_MONITOR_MODE_PATTERN_LENGTH_240,   /*!< 240 I/Q sample pairs */
    ADI_ADRV9001_MONITOR_MODE_PATTERN_LENGTH_480,    /*!< 480 I/Q sample pairs */
}adi_adrv9001_MonitorModePatternLength_e;

/**
 * \brief ARM memory SPI write mode
 */
typedef enum adi_adrv9001_ArmSingleSpiWriteMode
{
    ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4,    /*!< 4 bytes in a single SPI write in standard mode */
                                                                /*!< Only one write operation (4 bytes per write operation) is performed regardless of the state of chip select */
    ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_252,  /*!< 252 bytes in a single SPI write in standard mode */
                                                                /*!< Only one write operation (up to 252 bytes per write operation) is performed regardless of the state of chip select */
    ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STREAMING_BYTES_4,   /*!< 4 bytes in a single SPI write in streaming mode */
                                                                /*!< Four write operation (but only 4 bytes per write operation) is performed in one chip select */
}adi_adrv9001_ArmSingleSpiWriteMode_e;

/**
* \brief Data structure to hold ARM version information
*/
typedef struct adi_adrv9001_ArmVersion
{
    uint8_t majorVer;
    uint8_t minorVer;
    uint8_t maintVer;
    uint8_t rcVer;
    adi_adrv9001_ArmBuildType_e armBuildType;
} adi_adrv9001_ArmVersion_t;

/**
* \brief Data structure to hold ARM image build and runtime calculated checksums.
*/
typedef struct adi_adrv9001_Checksum
{
    uint32_t buildChecksum;
    uint32_t runChecksum;
} adi_adrv9001_Checksum_t;

/**
* \brief Data structure to hold a table of additional ARM checksums
*/
typedef struct adi_adrv9001_ChecksumTable
{
    adi_adrv9001_Checksum_t fwCheckSums;
    adi_adrv9001_Checksum_t streamsCheckSum[ADRV9001_MAX_NUM_STREAM];
    adi_adrv9001_Checksum_t deviceProfileCheckSum;
    adi_adrv9001_Checksum_t adcProfilefwCheckSum;
    uint32_t fwError;
    uint32_t fwErrorCode;
} adi_adrv9001_ChecksumTable_t;

/**
 * \brief Data structure to hold power saving configuration parameters
 */
typedef struct adi_adrv9001_ChannelPowerSavingCfg
{
    adi_adrv9001_ChannelPowerDownMode_e channelDisabledPowerDownMode;   /*!< Pin power down mode */
    /** Additional GPIO pin power down mode; only valid if greater than channelDisabledPowerDownMode */
    adi_adrv9001_ChannelPowerDownMode_e gpioPinPowerDownMode;
} adi_adrv9001_ChannelPowerSavingCfg_t;

/**
 * \brief Data structure to hold monitor mode configuration parameters
 */
typedef struct adi_adrv9001_SystemPowerSavingAndMonitorModeCfg
{
    adi_adrv9001_SystemPowerDownMode_e powerDownMode;   /*!< Power down mode for Monitor */
    uint32_t initialBatterySaverDelay_us;               /*!< Delay Timer (us) for detection before entering Monitor Mode  */
    uint32_t detectionTime_us;                          /*!< Timer for detection state (us) */
    uint32_t sleepTime_us;                              /*!< Timer for sleep state (us) */
    uint8_t detectionFirst;                             /*!< Select first state when entering Monitor Mode, 0-sleep first, 1-detection first */
    adi_adrv9001_MonitorDetectionMode_e detectionMode;  /*!< Mode of detection in the detect state */
    bool detectionDataBufferEnable;                     /*!< Enable data buffer in detect mode, 0-disable, 1-enable */
    bool externalPllEnable;                             /*!< External PLL Enable, 0-disable, 1-enable */
} adi_adrv9001_SystemPowerSavingAndMonitorModeCfg_t;

/**
 * \brief Data structure to hold monitor mode correlator pattern
 */
typedef struct adi_adrv9001_MonitorModePatternCfg
{
    adi_adrv9001_MonitorModePatternLength_e patternLength;   /*!< Length of pattern (240 or 480) I/Q sample pairs */
    uint32_t patternI[480];                                  /*!< 31-bit Monitor Mode I Detection Pattern */
    uint32_t patternQ[480];                                  /*!< 31-bit Monitor Mode Q Detection Pattern */
} adi_adrv9001_MonitorModePatternCfg_t;

/**
 * \brief Data structure to hold monitor mode correlator vector
 */
typedef struct adi_adrv9001_MonitorModeVectorCfg
{
    uint16_t vectorMask;   /*!< 14-bit Monitor Mode Correlator Vector Mask */
    uint64_t vector[14];   /*!< 12-HEX Character(48 bits) Monitor Mode Correlator Vectors */
} adi_adrv9001_MonitorModeVectorCfg_t;

/**
 * \brief RSSI measurement settings for monitor mode
 *
 * In this monitor mode configuration the ADRV9001 will measure the incoming signal strength, and based on its power
 * decide either to notify the BBIC that a signal has been detected or to continue monitoring.  Measuring signal strength
 * starts by taking a single RSSI measurement that lasts measurementDuration_us.  The interval of such measurements
 * is in measurementsStartPeriod_ms.  Furthermore, the signal strength may be composed of an average of the
 * RSSI measurements taken.  The total time allotted to measuring the signal strength must be less than 
 * adi_adrv9001_SystemPowerSavingAndMonitorModeCfg_t.detectionTime_us.
 * 
 * For example, given:
 *   numberOfMeasurementsToAverage = 4
 *   measurementsStartPeriod_ms = 7
 *   detectionThreshold_mdBFS = 100
 *   measurementDuration_samples = 24
 *   adi_adrv9001_SystemPowerSavingAndMonitorModeCfg_t.detectionTime_us = 30000
 * In the 30ms detection frame, 4 RSSI measurements will be performed, one every 7ms. Each RSSI measurement is 1ms in
 * duration with approximately 6ms gap (end of one to start of next) between consecutive measurements.
 */
typedef struct adi_adrv9001_arm_MonitorModeRssiCfg
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
    uint32_t measurementDuration_samples; /*!< Duration of a single RSSI measurement denoted in samples ranging from 1 to (2^21 - 1)*/
} adi_adrv9001_arm_MonitorModeRssiCfg_t;

/**
 * \brief Data structure to hold monitor mode configuration at initialization time
 */	
typedef struct adi_adrv9001_arm_MonitorModeInitCfg
{
    uint16_t numberOfPathDelaySamples;                          /* BBIC will be notified if average RSSI is above this threshold */
    adi_adrv9001_arm_MonitorModeRssiCfg_t monitorModeRssiCfg;   /* Struct to hold RSSI configuration at initialization time */
} adi_adrv9001_arm_MonitorModeInitCfg_t;

#endif /* _ADI_ADRV9001_ARM_TYPES_H_ */
