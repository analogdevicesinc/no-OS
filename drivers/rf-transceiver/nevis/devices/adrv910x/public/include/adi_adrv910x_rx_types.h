/**
* \file
* \brief Contains ADRV910X API Rx datapath data types
*
* ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
*/

/**
* Copyright 2022 Analog Devices Inc.
* Released under the ADRV910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_ADRV910X_RX_TYPES_H_
#define _ADI_ADRV910X_RX_TYPES_H_

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#include <stdbool.h>
#endif
	
#include "adi_adrv910x_types.h"
#include "adi_adrv910x_gpio_types.h"

#define ADI_ADRV910X_RX_GAIN_INDEX_MIN 187
#define ADI_ADRV910X_RX_GAIN_INDEX_MAX 255

/**
*  \brief Enum to select the type of gain table loaded during ADRV910X initialization
*/
typedef enum adi_adrv910x_RxGainTableType
{
    ADI_ADRV910X_RX_GAIN_CORRECTION_TABLE   = 0,   /*!< Gain table to use digital gain to adjust for coarse analog gain steps and maintain a constant gain */
    ADI_ADRV910X_RX_GAIN_COMPENSATION_TABLE = 1    /*!< Gain table to adjust digital gain when analog gain changes to maintain a constant gain*/
} adi_adrv910x_RxGainTableType_e;
	
/**
*  \brief Rx gain table SRAM base addresses
*/
typedef enum adi_adrv910x_RxGainTableBaseAddr_e
{
    ADI_ADRV910X_RX_GAIN_TABLE_BASE_ADDR_1 = 0x73300000,  /*!< Select gain table SRAM base address for Rx1 Channel */
    ADI_ADRV910X_RX_GAIN_TABLE_BASE_ADDR_2 = 0x73400000,  /*!< Select gain table SRAM base address for Rx2 Channel */
} adi_adrv910x_RxGainTableBaseAddr_e;

/**
 * @brief Enum to select manual gain control modes
 * 
 */
typedef enum adi_adrv910x_RxGainControlMode
{
    ADI_ADRV910X_RX_GAIN_CONTROL_MODE_SPI, /*!< Control gain index via SPI */
    ADI_ADRV910X_RX_GAIN_CONTROL_MODE_PIN, /*!< Control gain index via GPIO pins */
    ADI_ADRV910X_RX_GAIN_CONTROL_MODE_AUTO  /*!< Control gain index based on thresholds in ADRV910X Gain Control block */
} adi_adrv910x_RxGainControlMode_e;
	
/**
*  \brief Enum to determine Rx interface gain control type
*/
typedef enum adi_adrv910x_RxInterfaceGainCtrlMode
{
    ADI_ADRV910X_RX_INTERFACE_GAIN_CONTROL_AUTOMATIC = 0x00,
    /*!< Use internal Rx interface gain value. */
    ADI_ADRV910X_RX_INTERFACE_GAIN_CONTROL_MANUAL = 0x01,
    /*!< Use external Rx interface gain value. Gain value has to be provided in this case. */
} adi_adrv910x_RxInterfaceGainCtrlMode_e;

/**
*  \brief Enum to determine Rx interface gain value
*/
typedef enum adi_adrv910x_RxInterfaceGain
{
    ADI_ADRV910X_RX_INTERFACE_GAIN_18_DB = 0,       /*!<  18 dB */
    ADI_ADRV910X_RX_INTERFACE_GAIN_12_DB,           /*!<  12 dB */
    ADI_ADRV910X_RX_INTERFACE_GAIN_6_DB,            /*!<   6 dB */
    ADI_ADRV910X_RX_INTERFACE_GAIN_0_DB,            /*!<   0 dB */
    ADI_ADRV910X_RX_INTERFACE_GAIN_NEGATIVE_6_DB,   /*!<  -6 dB */
    ADI_ADRV910X_RX_INTERFACE_GAIN_NEGATIVE_12_DB,  /*!< -12 dB */
    ADI_ADRV910X_RX_INTERFACE_GAIN_NEGATIVE_18_DB,  /*!< -18 dB */
    ADI_ADRV910X_RX_INTERFACE_GAIN_NEGATIVE_24_DB,  /*!< -24 dB */
    ADI_ADRV910X_RX_INTERFACE_GAIN_NEGATIVE_30_DB,  /*!< -30 dB */
    ADI_ADRV910X_RX_INTERFACE_GAIN_NEGATIVE_36_DB,  /*!< -36 dB */
} adi_adrv910x_RxInterfaceGain_e;
	
/**
*  \brief Enum of Rx interface gain control update instance
*/
typedef enum adi_adrv910x_RxInterfaceGainUpdateTiming
{
    ADI_ADRV910X_RX_INTERFACE_GAIN_UPDATE_TIMING_NEXT_FRAME = 0x00,
    /*!< Update Rx interface gain control at start of next frame */
    ADI_ADRV910X_RX_INTERFACE_GAIN_UPDATE_TIMING_NOW = 0x01,
    /*!< Update Rx interface gain control immediately  */
} adi_adrv910x_RxInterfaceGainUpdateTiming_e;

/**
*  \brief Available gain control detect modes
*/
typedef enum adi_adrv910x_RxGainControlDetectionMode
{
    ADI_ADRV910X_RX_GAIN_CONTROL_DETECTION_MODE_PEAK_AND_POWER,
    ADI_ADRV910X_RX_GAIN_CONTROL_DETECTION_MODE_PEAK
} adi_adrv910x_RxGainControlDetectionMode_e ;

/**
 * \brief Data structure to hold ADRV910X Rx gain table row entry
 */
typedef struct adi_adrv910x_RxGainTableRow
{
    uint8_t rxFeGain;     /*!< Rx Front End gain for a given gain index */
    uint8_t extControl;   /*!< External LNA control word */
    uint8_t adcTiaGain;   /*!< ADC and TIA control for a given gain index */
    int16_t digGain;      /*!< Digital gain ranging from -18dB to 50dB (68dB total range) */
    uint16_t phaseOffset; /*!< 16 bit phase offset from 0 - 2pi in resolution of 0.005 degrees */
} adi_adrv910x_RxGainTableRow_t;
	
/**
* \brief Data structure to set/get the Rx interface gain control parameters for the given Rx channel.
*/
typedef struct adi_adrv910x_RxInterfaceGainCtrl
{
    adi_adrv910x_RxInterfaceGainUpdateTiming_e  updateInstance;
    adi_adrv910x_RxInterfaceGainCtrlMode_e      controlMode;
    adi_adrv910x_RxInterfaceGain_e              gain;
    uint8_t rssiDuration;                                           /* Duration of RSSI measurement (unit = 1ms/255 ) */
    uint8_t rssiMovingAverageDuration;                              /* Number of measurements in RSSI Moving-Average window */
    int8_t gainControlAutomaticThreshold_dBFS;                      /* Max signal level target in dBFS */
	uint8_t signalPAR;                                              /* Peak to Average Ratio of applied signal */
	bool enableFastAttack;                                          /* false: fastAttack and tracking use same configuration of MovingAveragefilter, true: No MovingAveragefilter in fastAttack region */

} adi_adrv910x_RxInterfaceGainCtrl_t;

/**
*  \brief Gain control peak detection settings
*  The Transceiver Evaluation Software can be used to generate a structure with suggested settings.
*/
typedef struct adi_adrv910x_PeakDetector
{
    uint16_t    agcUnderRangeLowInterval;           /*!< Update interval for AGC loop mode in AGC clock cycles. Valid range is 0 to 65535 */
    uint8_t     agcUnderRangeMidInterval;           /*!< 2nd update interval for multiple time constant AGC mode. Calculated as (agcUnderRangeMidInterval+1)*agcUnderRangeLowInterval_ns. Valid range is 0 to 63 */
    uint8_t     agcUnderRangeHighInterval;          /*!< 3rd update interval for multiple time constant AGC mode. Calculated as (agcUnderRangeHighInterval+1)*2nd update interval. Valid range is 0 to 63 */
    uint8_t     apdHighThresh;                      /*!< AGC APD high threshold. Valid range is 0 to 63 */
    uint8_t     apdLowThresh;                       /*!< AGC APD low threshold. Valid range is 0 to 63. Recommended to be 3dB below apdHighThresh */
    uint8_t     apdUpperThreshPeakExceededCount;    /*!< AGC APD peak detect upper threshold count. Valid range is 0 to 255 */
    uint8_t     apdLowerThreshPeakExceededCount;    /*!< AGC APD peak detect lower threshold count. Valid range is 0 to 255 */
    uint8_t     apdGainStepAttack;                  /*!< AGC APD peak detect attack gain step. Valid range is 0 to 31 */
    uint8_t     apdGainStepRecovery;                /*!< AGC APD gain index step size for recovery. Valid range is 0 to 31 */
    bool        enableHbOverload;                   /*!< Enable or disables the HB overload detector. */
    uint8_t     hbOverloadDurationCount;            /*!< Sets the window of clock cycles (at the HB output rate) to meet the overload count. (0 = 2 cycles, 1 = 4 cycles, 2 = 8 cycles, 3 = 12 cycles, 4 = 16 cycles, 5 = 24 cycles, 6 = 32 cycles) */
    uint8_t     hbOverloadThreshCount;              /*!< Sets the number of actual overloads required to trigger the overload signal. Valid range from 1 to 15 */
    uint16_t    hbHighThresh;                       /*!< AGC HB output high threshold. Valid range from  0 to 16383 */
    uint16_t    hbUnderRangeLowThresh;              /*!< AGC HB output low threshold. Valid range from  0 to 16383 */
    uint16_t    hbUnderRangeMidThresh;              /*!< AGC HB output low threshold for 2nd interval for multiple time constant AGC mode. Valid range from  0 to (A0: 255, B0: 16383) */
    uint16_t    hbUnderRangeHighThresh;             /*!< AGC HB output low threshold for 3rd interval for multiple time constant AGC mode. Valid range from  0 to (A0: 255, B0: 16383) */
    uint8_t     hbUpperThreshPeakExceededCount;     /*!< AGC HB output upper threshold count. Valid range from  0 to 255 */
    uint8_t     hbUnderRangeHighThreshExceededCount;/*!< AGC HB output lower threshold count. Valid range from  0 to 255 */
    uint8_t     hbGainStepHighRecovery;             /*!< AGC HB gain index step size. Valid range from  0 to 31 */
    uint8_t     hbGainStepLowRecovery;              /*!< AGC HB gain index step size, when the HB Low Overrange interval 2 triggers. Valid range from  0 to 31 */
    uint8_t     hbGainStepMidRecovery;              /*!< AGC HB gain index step size, when the HB Low Overrange interval 3 triggers. Valid range from  0 to 31 */
    uint8_t     hbGainStepAttack;                   /*!< AGC HB output attack gain step. Valid range from  0 to 31 */
    uint8_t     hbOverloadPowerMode;                /*!< Select magnitude measurements or power mearurements. 0 = enable peak mode 1 = enable I2 + Q2 power mode */
    uint8_t     hbUnderRangeMidThreshExceededCount; /*!< AGC HB output upper threshold count. Valid range from  0 to 255 */
    uint8_t     hbUnderRangeLowThreshExceededCount; /*!< AGC HB output lower threshold count. Valid range from  0 to 255 */

    /** A pair of DGPIO pins - ADI_ADRV910X_GPIO_PIN_CRUMB_XX_YY - where
     *  pin XX is the apd low threshold counter exceeded status and
     *  pin YY is the hb low threshold counter exceeded status
     */
    adi_adrv910x_GpioPinCrumbSel_e feedback_apd_low_hb_low;

    /** A pair of DGPIO pins - ADI_ADRV910X_GPIO_PIN_CRUMB_XX_YY - where
     *  pin XX is the apd high threshold counter exceeded status and
     *  pin YY is the hb high threshold counter exceeded status
     */
    adi_adrv910x_GpioPinCrumbSel_e feedback_apd_high_hb_high;
} adi_adrv910x_PeakDetector_t;

/**
* \brief Gain control power detection settings
*  The Transceiver Evaluation Software can be used to generate a structure with suggested settings.
*/
typedef struct adi_adrv910x_PowerDetector
{
    bool        powerEnableMeasurement;                 /*!< Enable the Rx power measurement block */
    uint8_t     underRangeHighPowerThresh;              /*!< AGC power measurement detect lower 0 threshold. Valid Range from 0 to 127. */
    uint8_t     underRangeLowPowerThresh;               /*!< AGC power measurement detect lower 1 threshold. Valid offset from 0 to 15 */
    uint8_t     underRangeHighPowerGainStepRecovery;    /*!< AGC power measurement detect lower 0 recovery gain step. Valid range from  0 to 31 */
    uint8_t     underRangeLowPowerGainStepRecovery;     /*!< AGC power measurement detect lower 1 recovery gain step. Valid range from  0 to 31 */
    uint8_t     powerMeasurementDuration;               /*!< Average power measurement duration = 8*2^powerMeasurementDuration. Valid range from 0 to 31 */
    uint8_t     powerMeasurementDelay;                  /*!< Average power measurement delay between sucessive measurement. Valid range from 0 to 255 */
    uint16_t    rxTddPowerMeasDuration;                 /*!< Measurement duration to detect power for specific slice of the gain update counter. */
    uint16_t    rxTddPowerMeasDelay;                    /*!< Measurement delay to detect power for specific slice of the gain update counter. */
    uint8_t     overRangeHighPowerThresh;               /*!< AGC upper 0 (overRangeHighPowerThreshold) threshold for power measurement. Valid Range from 0 to 15.*/
    uint8_t     overRangeLowPowerThresh;                /*!< AGC upper 1 (overRangeLowPowerThreshold)  threshold for power measurement. Valid offset from 0 to 127 */
    uint8_t     overRangeHighPowerGainStepAttack;       /*!< AGC power measurement detect lower 0 attack gain step. Valid range from  0 to 31 */
    uint8_t     overRangeLowPowerGainStepAttack;        /*!< AGC power measurement detect lower 1 attack gain step. Valid range from  0 to 31 */

    /** A pair of DGPIO pins - ADI_ADRV910X_GPIO_PIN_CRUMB_XX_YY - where
     *  pin XX is the power detector inner high threshold exceeded status and
     *  pin YY is the power detector inner low threshold exceeded status
     */
    adi_adrv910x_GpioPinCrumbSel_e feedback_inner_high_inner_low;

    /** A pair of DGPIO pins - ADI_ADRV910X_GPIO_PIN_CRUMB_XX_YY - where
     *  pin XX is the apd high threshold counter exceeded status and
     *  pin YY is the apd low threshold counter exceeded status
     */
    adi_adrv910x_GpioPinCrumbSel_e feedback_apd_high_apd_low;
} adi_adrv910x_PowerDetector_t;

/**
* \brief Ext LNA settings
*  The Transceiver Evaluation Software can be used to generate a structure with suggested settings.
*/
typedef struct adi_adrv910x_ExtLna
{
    uint8_t     settlingDelay;                      /*!< External LNA Settling Delay. Valid range is from 0 to 255 */
} adi_adrv910x_ExtLna_t;

/**
* \brief Gain control configuration settings for initialization
*  The Transceiver Evaluation Software can be used to generate a structure with suggested settings.
*/
typedef struct adi_adrv910x_GainControlCfg
{
    uint8_t peakWaitTime;           /*!< AGC peak wait time. Valid range is from 0 to 31 */
    uint8_t maxGainIndex;           /*!< AGC Rx max gain index. Valid range is from minGainIndex to 255 */
    uint8_t minGainIndex;           /*!< AGC Rx min gain index. Valid range is from 0 to maxGainIndex */
    /** AGC gain update time denominated in AGC clock cycles; Valid range is from 0 to 4194303. */
    uint32_t gainUpdateCounter;
    uint8_t attackDelay_us;         /*!< Delay time, denominated in microseconds, before starting AGC, after starting Rx. Valid range is from 0 to 63 */
    /** On any gain change, the AGC waits for the time (range 0 to 127) specified in AGC clock cycles to allow gain
     *  transients to flow through the Rx path before starting any measurements. */
    uint8_t slowLoopSettlingDelay;
    bool lowThreshPreventGainInc;   /*!< Prevent gain index from incrementing while peak thresholds are exceeded */
    /** Enable immediate gain change if high threshold counter is exceeded.
     *  Bit 0 enables ULB high threshold, Bit 1 enables HB high threshold */
    uint8_t changeGainIfThreshHigh;
    /** Enable gain change based only on the signal peak threshold over-ranges.
     *  Power-based AGC changes are disabled in this mode. */
    adi_adrv910x_RxGainControlDetectionMode_e agcMode;
    bool resetOnRxon;       /*!< Reset the AGC slow loop state machine to max gain when the Rx Enable is taken low */
    uint8_t resetOnRxonGainIndex;       /*!< AGC Reset On gain index. Valid range is from minGainIndex to maxGainIndex */
    bool enableSyncPulseForGainCounter; /*!< Enable the AGC gain update counter to be sync'ed to a time-slot boundary. */
    bool enableFastRecoveryLoop;        /*!< Enable multiple time constants in AGC loop for fast attack and fast recovery. */
    adi_adrv910x_PowerDetector_t power;
    adi_adrv910x_PeakDetector_t peak;
    adi_adrv910x_ExtLna_t extLna;
	bool rxQecFreezeEnable;		/*!< RXQEC Freeze Enable/Disable, only applies in AGC mode*/
	adi_adrv910x_GpioPin_e gpioFreezePin; /*!< GPIO pin to activate to freeze AGC - set to 0/UNASSIGNED if unused */
} adi_adrv910x_GainControlCfg_t;

typedef struct adi_adrv910x_RxGainControlPinCfg
{
    uint8_t minGainIndex;  //!< Minimum gain index. Must be >= gainTableMinGainIndex and < maxGainIndex
    uint8_t maxGainIndex;  //!< Maximum gain index. Must be > minGainIndex and <= gainTableMaxGainIndex
    uint8_t incrementStepSize;  //!< Number of indices to increase gain on rising edge on incrementPin (Range: 0 to 7)
    uint8_t decrementStepSize;  //!< Number of indices to decrease gain on rising edge on decrementPin (Range: 0 to 7)
    /** A rising edge on this pin will increment gain by incrementStepSize.
     *  \note Once gainIndex has reached maxGainIndex subsequent rising edges will not change the gainIndex */
    adi_adrv910x_GpioPin_e incrementPin;
    /** A rising edge on this pin will decrement gain by decrementStepSize.
     *  \note Once gainIndex has reached minGainIndex subsequent rising edges will not change the gainIndex */
    adi_adrv910x_GpioPin_e decrementPin;
} adi_adrv910x_RxGainControlPinCfg_t;

/**
 * \brief Structure which holds the LOID configuration parameters
 */
typedef struct adi_adrv910x_RxrfdcLoidCfg
{
	bool loidEnable;								   /* LOID enable flag for RX1 and RX2 */
    uint8_t loidDisableTrackingHighThreshold_negdBFS;  /* Threshold for LO detection (in -dBFS) */ 
    uint8_t loidReenableTrackingLowThreshold_negdBFS;  /* Threshold for LO detection release (in -dBFS) */
} adi_adrv910x_RxrfdcLoidCfg_t;

/**
 * \brief Data structure to hold rx High Blocker Mode settings for LO reconfiguration
 */
typedef struct adi_adrv910x_HighBlockerModeCfg
{
	bool enableHighBlockerMode; /*!< Enable/Disable RXQEC enhancements for high blocker mode */
	uint16_t highBlockerModeThresholdOne_negQuartdBFS; /*!< Blocker power in quarter dBFS steps. (Valid range 0 to 64 quarter dBFS steps) Values are interpreted as negative. */
	uint16_t highBlockerModeThresholdTwo_QuartdB; /*!< Blocker-to-Signal ratio in quarter dB steps (Typical range 0 to 640 quarter dB steps) */
} adi_adrv910x_HighBlockerModeCfg_t;

#endif /* _ADI_ADRV910X_RX_TYPES_H_ */ 
