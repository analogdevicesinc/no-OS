/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adi_adrv904x_agc_types.h
 * \brief Contains ADRV904X API AGC data types
 *
* ADRV904X API Version: 2.10.0.4
 */

#ifndef _ADI_ADRV904X_AGC_TYPES_H_
#define _ADI_ADRV904X_AGC_TYPES_H_

#include "adi_library_types.h"
#include "adi_adrv904x_gpio_types.h"

/**
*  \brief Data structure to hold AGC peak settings
*/
typedef struct adi_adrv904x_AgcPeak
{
    uint8_t     adcOvldUpperThreshPeakExceededCnt; /*!< Number of peak events needed in excess of ADC Peak Detector's high threshold to trigger an over ranging condition. Valid range is 2 to 255 */
    uint8_t     adcOvldGainStepAttack;             /*!< AGC ADC overload gain index step size for attack. Valid range is 0 to 31 */
    
    uint8_t     adcOvldLowThresh;                  /*!< AGC ADC overload low threshold. Valid range is 0 to 8, where 8 means full scale of ADC. Threshold in dBFS = 20log(adcOvldLowThresh/8) */
    uint8_t     adcOvldLowerThreshPeakExceededCnt; /*!< Number of peak events needed in excess of adcOvldLowThresh to prevent an under ranging condition. Valid range is 2 to 255 */
    uint8_t     adcOvldGainStepRecovery;           /*!< AGC ADC overload gain index step size for recovery. Valid range is 0 to 31 */

    uint8_t     hb2OverloadDurationCnt; /*!< If hb2OverloadThreshCnt number of samples exceed HB2 high/low thresholds within hb2OverloadDurationCnt sample group,
                                             threshold exceeded signal is asserted and threshold exceeded counter is incremented. If this counter exceeds HB2 high/low
                                             threshold exceeded counts then AGC makes the final gain adjustment. Valid range is 0 to 63.*/
    uint8_t     hb2OverloadThreshCnt;   /*!< If hb2OverloadThreshCnt number of samples exceed HB2 high/low thresholds within hb2OverloadDurationCnt sample group,
                                             threshold exceeded signal is asserted and threshold exceeded counter is incremented. If this counter exceeds HB2 high/low
                                             threshold exceeded counts then AGC makes the final gain adjustment. Valid range is 0 to 7. */
    
    uint16_t    hb2HighThresh;                 /*!< AGC HB2 output high threshold. Valid range from  0 to 16383. */
    uint8_t     hb2UpperThreshPeakExceededCnt; /*!< Number of peak events needed in excess of hb2HighThresh to trigger an over ranging condition. Valid range from  1 to 255 */
    uint8_t     hb2GainStepAttack;             /*!< AGC HB2 output attack gain step. Valid range from  0 to 31 */

    uint16_t    hb2UnderRangeHighThresh;            /*!< AGC HB2 output low threshold for 3rd interval (hb2UnderRangeHighInterval) for multiple time constant AGC mode. Valid range from  0 to 16383. */
    uint8_t     hb2UnderRangeHighThreshExceededCnt; /*!< Number of peak events needed in excess of hb2UnderRangeHighThresh to prevent an under ranging condition. Valid range from  1 to 255 */
    uint8_t     hb2GainStepHighRecovery;            /*!< AGC HB2 gain index step size. Valid range from  0 to 31 */
    
    /* Only used when fast recovery is enabled */
    uint16_t    hb2UnderRangeMidThresh;            /*!< AGC HB2 output low threshold for 2nd interval (hb2UnderRangeMidInterval) for multiple time constant AGC mode. Valid range from  0 to 16383 */
    uint8_t     hb2UnderRangeMidThreshExceededCnt; /*!< Number of peak events needed in excess of hb2UnderRangeMidThresh to prevent an under ranging condition. Valid range from  0 to 255 */
    uint8_t     hb2GainStepMidRecovery;            /*!< AGC HB2 gain index step size, when the HB2 Low Overrange interval 3 triggers (under ranges). Valid range from  0 to 31 */
    
    uint16_t    hb2UnderRangeLowThresh;             /*!< AGC HB2 output low threshold for hb2UnderRangeLowInterval. Valid range from  0 to 16383 */
    uint8_t     hb2UnderRangeLowThreshExceededCnt; /*!< Number of peak events needed in excess of hb2UnderRangeLowThesh to prevent an under ranging condition. Valid range from  0 to 255 */
    uint8_t     hb2GainStepLowRecovery;            /*!< AGC HB2 gain index step size, when the HB2 Low Overrange interval 2 triggers (under ranges). Valid range from  0 to 31 */
    
    uint16_t    hb2UnderRangeLowInterval;          /*!< Update interval for AGC fast recovery loop mode in AGC clock cycles. Valid range is 0 to 65535. 
                                                        This update interval is used as gain update counter period when fast recovery is enabled */
    uint8_t     hb2UnderRangeMidInterval;          /*!< 2nd update interval for multiple time constant AGC mode. 
                                                        Calculated as (hb2UnderRangeMidInterval+1)*hb2UnderRangeLowInterval_ns. Valid range is 0 to 63 */
    uint8_t     hb2UnderRangeHighInterval;         /*!< 3rd update interval for multiple time constant AGC mode. 
                                                        Calculated as (hb2UnderRangeHighInterval+1)*2nd update interval. Valid range is 0 to 63 */
    /* Only used when fast recovery is enabled */
    
    uint8_t     enableHb2Overload;    /*!< Enable or disables the HB2 overload peak detector. */
    uint8_t     hb2OverloadPowerMode; /*!< Selects HB2 sample type 1:I^2+Q^2  // 0:max(|I|, |Q|)
                                           1:For this mode, HB2 thresholds can be calculated as : Threshold = 2^14 * 10^(-x dBFS / 10) where x is the desired threshold in dBFS
                                           0:For this mode, HB2 thresholds can be calculated as : Threshold = 2^14 * 10^(-x dBFS / 20) where x is the desired threshold in dBFS*/
    uint8_t     hb2OverloadSignalSelection; /*!< HB2 peak detector signal source selection. 1:Select input of HB2 . 0:Select output of HB2*/
} adi_adrv904x_AgcPeak_t;

/**
* \brief Data structure to hold AGC power settings
*  The evaluation software GUI for the product can be used to generate a structure with suggested settings.
*/
typedef struct adi_adrv904x_AgcPower
{	
    uint8_t     underRangeHighPowerThresh;           /*!< AGC power measurement detect lower 0 threshold. Valid Range from 0 to 127. Threshold_in_dBFS = (-1 * underRangeHighPowerThresh) */
    uint8_t     underRangeHighPowerGainStepRecovery; /*!< AGC power measurement detect lower 0 recovery gain step. Valid range from  0 to 31 */
    
    uint8_t     underRangeLowPowerThresh;           /*!< AGC power measurement detect lower 1 threshold. Valid offset from 0 to 31. This configures an offset from underRangeHighPowerThresh.
                                                         Offset_in_dBFS = (-1 * underRangeLowPowerThresh)*/
    uint8_t     underRangeLowPowerGainStepRecovery; /*!< AGC power measurement detect lower 1 recovery gain step. Valid range from  0 to 31 */

    uint8_t     overRangeLowPowerThresh;         /*!< AGC upper 0 threshold for power measurement. Valid Range from 0 to 127. Threshold_in_dBFS = (-1 * overRangeLowPowerThresh)*/
    uint8_t     overRangeLowPowerGainStepAttack; /*!< AGC power measurement detect lower 0 attack gain step. Valid range from  0 to 31 */
    
    uint8_t     overRangeHighPowerThresh;         /*!< AGC upper 1 threshold for power measurement. Valid offset from 0 to 15. This configures an offset from overRangeLowPowerThresh.
                                                       Offset_in_dBFS = (-1 * overRangeHighPowerThresh) */
    uint8_t     overRangeHighPowerGainStepAttack; /*!< AGC power measurement detect lower 1 attack gain step. Valid range from  0 to 31 */

    uint8_t     powerEnableMeasurement;   /*!< Enable the Rx power measurement block. (0/1) */
    uint8_t     powerInputSelect;         /*!< Use output of Rx for power measurement. 0: DC Offset output, 1: RFIR output, 2: QFIR output, 3:HB2 output. Valid Range from 0 to 3 */
    uint8_t     powerMeasurementDuration; /*!< Average power measurement duration = 8*2^powerMeasurementDuration. Valid range from 0 to 31 */

} adi_adrv904x_AgcPower_t;

/**
* \brief Data structure to hold all AGC configuration settings for initialization
*  The evaluation software GUI for the product can be used to generate a structure with suggested settings.
*/
typedef struct adi_adrv904x_AgcCfg
{
    uint32_t    rxChannelMask;                        /*!< Channel mask of Rx channels to apply these settings to, or that settings were read from */
    uint8_t     agcRxMaxGainIndex;                    /*!< AGC Rx max gain index. Valid range is from 0 to 255 */
    uint8_t     agcRxMinGainIndex;                    /*!< AGC Rx min gain index. Valid range is from 0 to 255 */
    uint32_t    agcGainUpdateCounter;                 /*!< AGC gain update time in AGC clock cycles. Valid range is from 0 to 4194303*/
    uint8_t     agcChangeGainIfThreshHigh;            /*!< Enable immediate gain change if high threshold counter is exceeded. 
                                                           Bit 0 enables ADC high threshold, Bit 1 enables HB2 high threshold */
    uint8_t     agcSlowLoopSettlingDelay;             /*!< On any gain change, the AGC waits for twice this time (specified in AGC clock cycles) to allow
                                                           gain transients to flow through the Rx path before starting any measurements. Valid range is from 0 to 255 */
    uint8_t     agcEnableFastRecoveryLoop;            /*!< 1:Enable fast recovery. 0:Disable fast recovery */
    uint8_t     agcPeakThreshGainControlMode;         /*!< 1:Enable gain change based only on the signal peak threshold over-ranges. 
                                                           Power based AGC changes are disabled in this mode. 0:Disable*/
    uint8_t     agcLowThreshPreventGainInc;           /*!< 1:Prevent gain index from incrementing (by power measurements) if peak thresholds are being exceeded 0:Don't prevent*/
    uint8_t     agcResetOnRxon;                       /*!< 1: Reset the AGC slow loop state machine to max gain when the Rx Enable is taken low 0:Don't reset */
    uint8_t     agcRxAttackDelay;                     /*!< On entering Rx, the Rx AGC is kept inactive for a period = agcRxAttackDelay*1us. Valid range is from 0 to 63 */
    uint8_t     agcAdcResetGainStep;                  /*!< Indicates how much gain steps to reduce when a reset happens in the ADC. 
                                                           Typically larger (1-2dB) than the agc_ovrg_gain_step. Valid range is from 0 to 31 */
    uint8_t     agcPeakWaitTime;                      /*!< AGC peak wait time in AGC clock cycles. Valid range is from 0 to 3. Configure the duration that
                                                           the gain control algorithms wait before enabling regular operation of the peak detectors after a peak is detected*/
    adi_adrv904x_AgcPower_t agcPower;
    adi_adrv904x_AgcPeak_t agcPeak;
} adi_adrv904x_AgcCfg_t;

/**
* \brief Data structure to set/get the AGC min/max gain index for one or more Rx channels.
*/
typedef struct adi_adrv904x_AgcGainRange
{
    uint32_t rxChannelMask; /*!< Channel mask of Rx channels to apply these settings to, or that settings were read from */
    uint8_t maxGainIndex;   /*!< Max gain index the AGC can use */
    uint8_t minGainIndex;   /*!< Min gain index the AGC can use */
} adi_adrv904x_AgcGainRange_t;


/**
* \brief Data structure to hold AGC Dualband configuration.
*/
typedef struct adi_adrv904x_AgcDualBandCfg
{
    uint32_t rxChannelMask;                         /*!< Channel mask of Rx channels to apply these settings to, or that settings were read from */
    uint8_t agcDualBandEnable;                      /*!< Enable AGC operation for dualband receiver */
    uint8_t agcRxDualbandExtTableUpperIndex;        /*!<Indicates the gain table index below which the AGC prioritizes
                                                        decreasing gain through external LNA control over the Front-end gain.
                                                        AGC decreases the LNA gain if the gain index is less than this value */
    uint8_t agcRxDualbandExtTableLowerIndex;        /*!<Indicates the gain table index above which the AGC prioritizes
                                                        increasing gain through external LNA control over the Front-end gain.
                                                        AGC increases the LNA gain if the gain index is greater than this value.*/
    uint8_t agcDualbandPwrMargin;                   /*!<Margin for comparing total power against power of individual bands. If
                                                        Total power(from main dec power meas block) > Upper Band Power + Lower Band Power + margin, the signal contains other components
                                                        than the two bands, and AGC should behave like a single band system. Margin is in 0.5dBFS steps, i.e. margin[dBFS] = 0.5 * agcDualbandPwrMargin
                                                        Range for agcDualbandPwrMargin: [0-31] */
    uint8_t agcDualbandLnaStep;                     /*!<Margin to compare Upper band power versus Lower band power
                                                        (for Upper Band Power > Lower Band Power + margin, and Lower Band Power > Upper Band Power + margin checks).
                                                        The margin compares the powers of the bands to change the LNA of one band so that powers of the bands match.
                                                        Value is in 0.5dBFS resolution, i.e. margin[dBFS] = 0.5 * agcDualbandLnaStep */
    uint8_t agcDualbandHighLnaThreshold;            /*!<High threshold for Upper band or Lower band power above which the LNA index is decreased.
                                                        Value is in 0.5dBFS resolution, i.e. threshold[dBFS] = -0.5 * agcDualbandHighLnaThreshold */
    uint8_t agcDualbandLowLnaThreshold;             /*!<Low threshold for Upper band or Lower band power below which the LNA index is increased.
                                                        Value is in 0.5dBFS resolution, i.e. threshold[dBFS] = -0.5 * agcDualbandLowLnaThreshold */
    uint8_t decPowerDdcMeasurementDuration;         /*!<Power measurement duration for measuring the power of the individual bands. This variable has
                                                        a range of 0 to 31. The sampling period is calculated as = 8 x 2^decPowerDdcMeasurementDuration. */
    uint8_t agcDualBandMaxGainIndex;                /*!<Maximum gain index to be selected by dual band AGC. Range[0-3]. When 1 GPIO is being used (per band), this value needs to be 
                                                        configured to 1. If 2 GPIOs are being used (per band), this index should be configured either 2 or 3. Please check 
                                                        adi_adrv904x_RxGainTableDualBandLnaWrite function for more details. */
    uint8_t enableGainCompensationForExtLna;        /*!<1:Slicer gain compensation should also consider the compensation for dualband external LNA gain
                                                        0:Slicer gain compensation ignores the dualband external LNA gain */
} adi_adrv904x_AgcDualBandCfg_t;

/**
  * \brief Data structure to hold ADRV904X Rx gain table row entry for dualband external LNA control
  */
typedef struct adi_adrv904x_RxDualBandLnaGainTableRow
{
    uint8_t gainCompensation; /*!< Digital gain word to compensate the external LNA output. Resolution is 0.5dBFS for each LSB.
                                   This represents the equivalent dualband external LNA gain in digital. It's only being used. Range [0-63]. This
                                   setting is being used by slicer block when adi_adrv904x_AgcDualBandCfg_t->enableGainCompensationForExtLna is set to 1 */
    uint8_t externalControlWord; /*!< 2 bit external LNA control word. Range:[0-3] */

} adi_adrv904x_RxDualBandLnaGainTableRow_t;

/**
  * \brief Data structure to hold ADRV904X Rx dual band external LNA analog gpio output mapping settings
  */
typedef struct adi_adrv904x_AgcDualBandGpioCfg
{
    uint16_t gpioSelectionMask; /* User can select one or multiple analog GPIOs(out of 16 analog GPIO pins) to configure */
	adi_adrv904x_GpioSignal_e analogGpioMapping[ADI_ADRV904X_GPIO_ANALOG_COUNT]; /* Gpio signal assignments for GPIOs selected with gpioSelectionMask. gpioSelectionMask Bit0
                                                                                                          * selects analogGpioMapping[0], Bit1 selects analogGpioMapping[1] and so on. For unselected
                                                                                                          * GPIOs the value of analogGpioMapping[*] is don't care. */

} adi_adrv904x_AgcDualBandGpioCfg_t;
#endif /* _ADI_ADRV904X_AGC_TYPES_H_ */
