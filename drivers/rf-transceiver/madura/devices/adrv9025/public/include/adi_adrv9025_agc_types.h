/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adi_adrv9025_agc_types.h
* \brief Contains ADRV9025 API AGC data types
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_AGC_TYPES_H_
#define _ADI_ADRV9025_AGC_TYPES_H_

#ifdef __cplusplus
extern
"C"
 {
#endif

#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdint.h>
#endif

/**
*  \brief Data structure to hold AGC peak settings
*  The evaluation software GUI for the product can be used to generate a structure with suggested settings.
*/
typedef struct adi_adrv9025_AgcPeak
{
    uint16_t    agcUnderRangeLowInterval;           /*!< Update interval for AGC loop mode in AGC clock cycles. Valid range is 0 to 65535 */
    uint8_t     agcUnderRangeMidInterval;           /*!< 2nd update interval for multiple time constant AGC mode. Calculated as (agcUnderRangeMidInterval+1)*agcUnderRangeLowInterval_ns. Valid range is 0 to 63 */
    uint8_t     agcUnderRangeHighInterval;          /*!< 3rd update interval for multiple time constant AGC mode. Calculated as (agcUnderRangeHighInterval+1)*2nd update interval. Valid range is 0 to 63 */
    uint8_t     apdHighThresh;                      /*!< AGC APD high threshold. Valid range is 0 to 63 */
    uint8_t     apdLowGainModeHighThresh;           /*!< AGC APD high threshold in low gain mode. Valid range is 0 to 63. Recommended to be 3dB above apdHighThresh */
    uint8_t     apdLowThresh;                       /*!< AGC APD low threshold. Valid range is 0 to 63. Recommended to be 3dB below apdHighThresh */
    uint8_t     apdLowGainModeLowThresh;            /*!< AGC APD low threshold in low gain mode. Valid range is 0 to 63. Recommended to be 3dB above apdLowThresh */
    uint8_t     apdUpperThreshPeakExceededCnt;      /*!< AGC APD peak detect upper threshold count. Valid range is 0 to 255 */
    uint8_t     apdLowerThreshPeakExceededCnt;      /*!< AGC APD peak detect lower threshold count. Valid range is 0 to 255 */
    uint8_t     apdGainStepAttack;                  /*!< AGC APD peak detect attack gain step. Valid range is 0 to 31 */
    uint8_t     apdGainStepRecovery;                /*!< AGC APD gain index step size for recovery. Valid range is 0 to 31 */
    uint8_t     enableHb2Overload;                  /*!< Enable or disables the HB2 overload detector. */
    uint8_t     hb2OverloadDurationCnt;             /*!< Sets the window of clock cycles (at the HB2 output rate) to meet the overload count. (0 = 2 cycles, 1 = 4 cycles, 2 = 8 cycles, 3 = 12 cycles, 4 = 16 cycles, 5 = 24 cycles, 6 = 32 cycles) */
    uint8_t     hb2OverloadThreshCnt;               /*!< Sets the number of actual overloads required to trigger the overload signal. Valid range from 1 to 15 */
    uint16_t    hb2HighThresh;                      /*!< AGC HB2 output high threshold. Valid range from  0 to 16383 */
    uint16_t    hb2UnderRangeLowThresh;             /*!< AGC HB2 output low threshold. Valid range from  0 to 16383 */
    uint16_t    hb2UnderRangeMidThresh;             /*!< AGC HB2 output low threshold for 2nd interval for multiple time constant AGC mode. Valid range from  0 to 16383 */
    uint16_t    hb2UnderRangeHighThresh;            /*!< AGC HB2 output low threshold for 3rd interval for multiple time constant AGC mode. Valid range from  0 to 16383 */
    uint8_t     hb2UpperThreshPeakExceededCnt;      /*!< AGC HB2 output upper threshold count. Valid range from  0 to 255 */
    uint8_t     hb2UnderRangeHighThreshExceededCnt; /*!< AGC HB2 output lower threshold count. Valid range from  0 to 255 */
    uint8_t     hb2GainStepHighRecovery;            /*!< AGC HB2 gain index step size. Valid range from  0 to 31 */
    uint8_t     hb2GainStepLowRecovery;             /*!< AGC HB2 gain index step size, when the HB2 Low Overrange interval 2 triggers. Valid range from  0 to 31 */
    uint8_t     hb2GainStepMidRecovery;             /*!< AGC HB2 gain index step size, when the HB2 Low Overrange interval 3 triggers. Valid range from  0 to 31 */
    uint8_t     hb2GainStepAttack;                  /*!< AGC HB2 output attack gain step. Valid range from  0 to 31 */
    uint8_t     hb2OverloadPowerMode;               /*!< When this bit is set, the dynamic range of the power measurement increases from -40dB to ~-60dB (that is, all signal levels from 0dBFS to -60dBFS are accurately detected */
    uint8_t     hb2ThreshConfig;                    /*!< Not User Modifiable   Initialized to 0x03 */
    uint8_t     hb2UnderRangeMidThreshExceededCnt;  /*!< AGC HB2 output upper threshold count. Valid range from  0 to 255 */
    uint8_t     hb2UnderRangeLowThreshExceededCnt;  /*!< AGC HB2 output lower threshold count. Valid range from  0 to 255 */
} adi_adrv9025_AgcPeak_t;

/**
* \brief Data structure to hold AGC power settings
*  The evaluation software GUI for the product can be used to generate a structure with suggested settings.
*/
typedef struct adi_adrv9025_AgcPower
{
    uint8_t     powerEnableMeasurement;                 /*!< Enable the Rx power measurement block. (0/1) */
    uint8_t     powerInputSelect;                       /*!< Use output of Rx for power measurement. 0: RFIR, 1: HB1, 2: HB2, 3:HB2. Valid Range from 0 to 3) */
    uint8_t     underRangeHighPowerThresh;              /*!< AGC power measurement detect lower 0 threshold. Valid Range from 0 to 127. */
    uint8_t     underRangeLowPowerThresh;               /*!< AGC power measurement detect lower 1 threshold. Valid offset from 0 to 31 */
    uint8_t     underRangeHighPowerGainStepRecovery;    /*!< AGC power measurement detect lower 0 recovery gain step. Valid range from  0 to 31 */
    uint8_t     underRangeLowPowerGainStepRecovery;     /*!< AGC power measurement detect lower 1 recovery gain step. Valid range from  0 to 31 */
    uint8_t     powerMeasurementDuration;               /*!< Average power measurement duration = 8*2^powerMeasurementDuration. Valid range from 0 to 31 */
    uint16_t    rxTddPowerMeasDuration;                 /*!< Measurement duration to detect power for specific slice of the gain update counter. */
    uint16_t    rxTddPowerMeasDelay;                    /*!< Measurement delay to detect power for specific slice of the gain update counter. */
    uint8_t     overRangeHighPowerThresh;               /*!< AGC upper 0 (overRangeHighPowerThreshold) threshold for power measurement. Valid Range from 0 to 127.*/
    uint8_t     overRangeLowPowerThresh;                /*!< AGC upper 1 (overRangeLowPowerThreshold)  threshold for power measurement. Valid offset from 0 to 15 */
    uint8_t     powerLogShift;                          /*!< Not User Modifiable   Initialized to 0x01. Enable Increase in dynamic range of the power measurement from 40dB to ~60dB. Provides higher accuracy. */

    uint8_t     overRangeHighPowerGainStepAttack;       /*!< AGC power measurement detect lower 0 attack gain step. Valid range from  0 to 31 */
    uint8_t     overRangeLowPowerGainStepAttack;        /*!< AGC power measurement detect lower 1 attack gain step. Valid range from  0 to 31 */
} adi_adrv9025_AgcPower_t;

/**
* \brief Data structure to hold all AGC configuration settings for initialization
*  The evaluation software GUI for the product can be used to generate a structure with suggested settings.
*/
typedef struct adi_adrv9025_AgcCfg
{
    uint32_t    rxChannelMask;                        /*!< Channel mask of Rx channels to apply these settings to, or that settings were read from */
    uint8_t     agcPeakWaitTime;                      /*!< AGC peak wait time. Valid range is from 0 to 31 */
    uint8_t     agcRxMaxGainIndex;                    /*!< AGC Rx max gain index. Valid range is from 0 to 255 */
    uint8_t     agcRxMinGainIndex;                    /*!< AGC Rx min gain index. Valid range is from 0 to 255 */
    uint32_t    agcGainUpdateCounter;                 /*!< AGC gain update time in AGC clock cycles. Valid range is from 0 to 4194303*/
    uint8_t     agcRxAttackDelay;                     /*!< On entering Rx, the Rx AGC is kept inactive for a period = agcRxAttackDelay*1us */
    uint8_t     agcSlowLoopSettlingDelay;             /*!< On any gain change, the AGC waits for the time (range 0 to 127) specified in AGC clock cycles to allow gain transients to flow through the Rx path before starting any measurements. */
    uint8_t     agcLowThreshPreventGainInc;           /*!< Prevent gain index from incrementing if peak thresholds are being exceeded */
    uint8_t     agcChangeGainIfThreshHigh;            /*!< Enable immediate gain change if high threshold counter is exceeded. Bit 0 enables ULB high threshold, Bit 1 enables HB2 high threshold */
    uint8_t     agcPeakThreshGainControlMode;         /*!< Enable gain change based only on the signal peak threshold over-ranges. Power based AGC changes are disabled in this mode. */
    uint8_t     agcResetOnRxon;                       /*!< Reset the AGC slow loop state machine to max gain when the Rx Enable is taken low */
    uint8_t     agcEnableSyncPulseForGainCounter;     /*!< Enable the AGC gain update counter to be sync'ed to a time-slot boundary. */
    uint8_t     agcEnableFastRecoveryLoop;            /*!< Enable multiple time constants in AGC loop for fast attack and fast recovery. */
    uint8_t     agcAdcResetGainStep;                  /*!< Indicates how much gain steps to reduce when a reset happens in the ADC. Typically larger (1-2dB) than the agc_ovrg_gain_step */
    uint8_t     agcSlowloopFastGainChangeBlockEnable; /*!< Block change_gain_immediately for settling_delay cycles amount of time before gainupdate counter expired */
    adi_adrv9025_AgcPower_t agcPower;
    adi_adrv9025_AgcPeak_t agcPeak;
} adi_adrv9025_AgcCfg_t;

/**
* \brief Data structure to hold all AGC Dualband configuration.
* Used along with adi_adrv9025_RxChannels_e structure to setup AGC for Rx1 and/or Rx2
*/
typedef struct adi_adrv9025_AgcDualBandCfg
{
    uint32_t rxChannelMask;                     /*!< Channel mask of Rx channels to apply these settings to, or that settings were read from */
    uint8_t agcDualBandEnable;                  /*!< Enable AGC operation for dualband receiver  */
    uint8_t agcRxDualbandExtTableUpperIndex;    /*!< Rx1/2 AGC dual band operation - Indicates the gain table index below which the AGC prioritizes
                                                        decreasing gain through external LNA control over the Front-end gain  */
    uint8_t agcRxDualbandExtTableLowerIndex;    /*!< Rx1/2 AGC dual band operation - Indicates the gain table index above which the AGC prioritizes
                                                        increasing gain through external LNA control over the Front-end gain  */
    uint8_t agcDualbandPwrMargin;               /*!< Margin for comparing total power against power of individual bands. If
                                                        Total power > Upper Band Power + Lower Band Power +margin, the signal contains other components
                                                        than the two bands, and AGC should behave like a single band system. Margin is in 0.5db steps. */
    uint8_t agcDualbandLnaStep;                 /*!< Margin to compare Upper band power versus Lower band power
                                                        (for Upper Band Power > Lower Band Power + margin, and Lower Band Power > Upper Band Power + margin checks).
                                                        The margin compares the powers of the bands to change the LNA of one band so that powers of the bands match.
                                                        Value is in 0.5db resolution  */
    uint8_t agcDualbandHighLnaThreshold;        /*!< High threshold for Upper band or Lower band power above which the LNA index is decreased.
                                                        Value is in 0.5db */
    uint8_t agcDualbandLowLnaThreshold;         /*!< Low threshold for Upper band or Lower band power below which the LNA index is increased.
                                                        Value is in 0.5db */
    uint8_t dualBandGpioEnable;                 /*!< Enable the 3.3V GPIO's that would be used to drive the external LNA's.
                                                        Rx1 : GPIO3.3[1:0] controls Lower band LNA, GPIO3.3[3:2] controls Upper band LNA
                                                        Rx2 : GPIO3.3[5:4] controls Lower band LNA, GPIO3.3[7:6] controls Upper band LNA
                                                        If disabled, user needs to use API function to read back the LNA control
                                                        value through the SPI. */
    uint8_t decPowerDdcMeasurementDuration;     /*!< Power measurement duration for measuring the power of the individual bands. This variable has
                                                        a range of 0 to 31. The sampling period is calculated as = 8 x 2^decPowerDdcMeasurementDuration. */
} adi_adrv9025_AgcDualBandCfg_t;

/**
* \brief Data structure to hold AGC Dualband LNA controls for the external LNA's.
* Used along with adi_adrv9025_RxChannels_e structure to read back LNA controls for Rx1 or Rx2
*/
typedef struct adi_adrv9025_DualBandLnaCfg
{
    uint32_t rxChannelMask;        /*!< Rx Channel that these settings were read from */
    uint8_t rxLowerBandLnaControl; /* The control value for the Rx1/2 Lower band LNA (Values 0-3) */
    uint8_t rxUpperBandLnaControl; /* The control value for the Rx1/2 Upper band LNA (Values 0-3) */
} adi_adrv9025_DualBandLnaCfg_t;

/**
* \brief Data structure to set/get the AGC min/max gain index for one or more Rx channels.
*/
typedef struct adi_adrv9025_AgcGainRange
{
    uint32_t rxChannelMask; /*!< Channel mask of Rx channels to apply these settings to, or that settings were read from */
    uint8_t maxGainIndex;   /*!< Max gain index the AGC can use */
    uint8_t minGainIndex;   /*!< Min gain index the AGC can use */
} adi_adrv9025_AgcGainRange_t;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_AGC_TYPES_H_ */
