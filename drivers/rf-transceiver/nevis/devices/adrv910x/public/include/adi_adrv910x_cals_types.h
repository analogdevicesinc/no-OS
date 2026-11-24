/**
 * \file
 * \brief Contains ADRV910X API Calibration data types
 *
 * Copyright (c) 2024 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_ADRV910X_CALS_TYPES_H_
#define _ADI_ADRV910X_CALS_TYPES_H_

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "adi_adrv910x_types.h"

/**
 * \brief Enum to select single or differential mode settings for Lo interface calibration
 */
typedef enum adi_adrv910x_LoGenDifferentialMode
{
    ADI_ADRV910X_CAL_LOGEN_DIFFERENTIAL,                /*!< Positive Input to Positive Output, Negative Input to Negative Output */
    ADI_ADRV910X_CAL_LOGEN_SINGLE_ENDED_NEGATIVE_INPUT, /*!< Inverted Negative Input to Positive Output, Negative Input to Negative Output */
    ADI_ADRV910X_CAL_LOGEN_SINGLE_ENDED_POSITIVE_INPUT, /*!< Positive Input to Positive Output, Inverted Positive Input to Negative Output */
    ADI_ADRV910X_CAL_LOGEN_DIFFERENTIAL_INVERTED        /*!< Inverted Positive Input to Negative Output, Inverted Negative Input to Positive Output */
} adi_adrv910x_LoGenDifferentialMode_e;

/**
 * \brief Structure which holds the LO interface Calibration parameters
 */
typedef struct adi_adrv910x_LoGenCalSettings
{
    uint8_t termRes;                              /*!< Set termination resistance of LO interface (Range: 0-15, 0: No resistance, 1: max resistance) */
    bool vcmBiasEnable;                           /*!< Enable Common mode bias voltage to set to VDD/2 */
    adi_adrv910x_LoGenDifferentialMode_e s2dMode; /*!< Set single or differential mode */
} adi_adrv910x_LoGenCalSettings_t;

/**
 * \brief Structure which holds the calibration bit masks for ADRV910X initialization calibrations
 */
typedef struct adi_adrv910x_initCalMask
{
    uint32_t sysInitCalMask;     /*!< Calibration bit mask for non-channel related init cals */
    uint32_t chanInitCalMask[2]; /*!< Calibration bit mask for for channel related init cals. It contains two masks, i.e. CH_1 for masks on RX1/TX1 channels, CH_2 for masks on RX2/TX2 channels */
} adi_adrv910x_initCalMask_t;

/**
 * \brief Common calibration status header - mirrors ARM-side CalStatusHdr_t (20 bytes)
 *
 * This structure is the first member of most calibration status structures on the ARM.
 * It contains the common metrics applicable to all calibrations.
 */
typedef struct adi_adrv910x_CalStatusHdr
{
    uint32_t errorCode;          /*!< Cal error code (0 = no error) */
    uint32_t percentComplete;    /*!< Percentage of data collected (0-100) */
    uint32_t performanceMetric;  /*!< Cal-specific performance metric */
    uint32_t iterCount;          /*!< Number of times cal algorithm has run (scheduled iterations) */
    uint32_t updateCount;        /*!< Number of times actuator/coefficients were updated */
} adi_adrv910x_CalStatusHdr_t;

#define ADI_ADRV910X_CAL_STATUS_MAX_SIZE_BYTES  (5120u)

/*============================================================================
 * Calibration status selector
 *===========================================================================*/

/**
 * \brief Selects which calibration's status is read by adi_adrv910x_cals_CalStatus_Get()
 *
 * These values are an API-level selector only. The mapping onto firmware object
 * IDs is private to the API and may change without affecting this enum.
 */
typedef enum adi_adrv910x_CalStatusSel
{
    ADI_ADRV910X_CAL_STATUS_TX_QEC_INIT = 0,        /*!< Tx quadrature error correction init cal */
    ADI_ADRV910X_CAL_STATUS_TX_LOL_INIT,            /*!< Tx local oscillator leakage init cal */
    ADI_ADRV910X_CAL_STATUS_TX_BBAF_INIT,           /*!< Tx baseband analog filter init cal */
    ADI_ADRV910X_CAL_STATUS_TX_BBAF_GD_INIT,        /*!< Tx baseband analog filter group delay init cal */
    ADI_ADRV910X_CAL_STATUS_RX_HPADC_RC_INIT,       /*!< Rx high power ADC RC tuner init cal */
    ADI_ADRV910X_CAL_STATUS_RX_HPADC_FLASH_INIT,    /*!< Rx high power ADC flash init cal */
    ADI_ADRV910X_CAL_STATUS_RX_TIA_CUTOFF_INIT,     /*!< Rx TIA cutoff init cal */
    ADI_ADRV910X_CAL_STATUS_RX_TCAL_INIT,           /*!< Rx tone calibration init cal */
    ADI_ADRV910X_CAL_STATUS_RX_RFDC_OFFSET_INIT,    /*!< Rx RF DC offset init cal */
    ADI_ADRV910X_CAL_STATUS_RX_DMR_PATH_DELAY_INIT, /*!< DMR path delay init cal */
    ADI_ADRV910X_CAL_STATUS_TX_DPD_TRACKING,        /*!< Tx digital pre-distortion tracking cal */
    ADI_ADRV910X_CAL_STATUS_RX_HD2_TRACKING,        /*!< Rx second harmonic distortion tracking cal */
    ADI_ADRV910X_CAL_STATUS_RX_WB_POLY_TRACKING,    /*!< Rx wideband poly tracking cal */
    ADI_ADRV910X_CAL_STATUS_TX_QEC_TRACKING,        /*!< Tx quadratur error correction tracking cal */
    ADI_ADRV910X_CAL_STATUS_NUM_SEL                 /*!< Number of selectable calibration statuses */
} adi_adrv910x_CalStatusSel_e;

/*============================================================================
 * Per-calibration status geometry
 *===========================================================================*/

#define ADI_ADRV910X_TXQEC_ATTEN_POINTS             (8u)   /*!< Tx QEC attenuation points reported in estCnt[] */
#define ADI_ADRV910X_TXBBAF_GD_MEAS_TYPES           (3u)   /*!< Tx BBAF group delay measurement types */
#define ADI_ADRV910X_TXBBAF_GD_ESTIMATES            (2u)   /*!< Tx BBAF group delay estimates per measurement */
#define ADI_ADRV910X_FLASHCAL_MAX_NUM_ADC           (4u)   /*!< RxADC0 (I,Q) and RxADC1 (I,Q) */
#define ADI_ADRV910X_TIA_CUTOFF_NUM_LPF_VALS        (1u)   /*!< Must equal firmware TIA_CUTOFF_CAL_NUM_LPF_VAL_MAX */
#define ADI_ADRV910X_RX_LODELAY_NUM_PLL             (3u)   /*!< Rx LO delay PLL count */
#define ADI_ADRV910X_RX_LODELAY_LO_TABLE_SIZE       (2u)   /*!< Rx LO delay LO table size */
#define ADI_ADRV910X_RX_LODELAY_GAIN_TABLE_SIZE     (1u)   /*!< Rx LO delay gain table size */
#define ADI_ADRV910X_LO_DELAY_NUM                   (2u)   /*!< I and Q LO delay entries */
#define ADI_ADRV910X_RXQEC_CFIR_NTAPS               (13u)  /*!< Rx QEC correction FIR tap count */
#define ADI_ADRV910X_TCAL_HD_LEN                    (50u)  /*!< RXQEC_NFFT (48) + 2 scratch words for the in-place IFFT */
#define ADI_ADRV910X_RFDC_NUM_RX_GAINS              (64u)  /*!< Rx gain entries in the RFDC correction table */
#define ADI_ADRV910X_RFDC_NUM_ORX_FE_GAINS          (7u)   /*!< ORx front end gain entries in the RFDC correction table */
#define ADI_ADRV910X_RFDC_NUM_LPBK_GAINS            (3u)   /*!< Loopback gain entries in the RFDC correction table */
#define ADI_ADRV910X_RFDC_NUM_FREQ_REGIONS          (2u)   /*!< RFDC frequency regions */
#define ADI_ADRV910X_RFDC_NUM_LOS                   (2u)   /*!< RFDC local oscillators */
#define ADI_ADRV910X_CAL_STATUS_NUM_CHANNELS        (2u)   /*!< Channels reported per calibration */

/*============================================================================
 * Per-calibration status structures
 *
 * Field layouts mirror the firmware status structures. The API deserialises the
 * raw ARM bytes into these structures so that every consumer shares one
 * definition of the layout.
 *===========================================================================*/

/**
 * \brief Tx QEC init calibration status
 */
typedef struct adi_adrv910x_TxQecInitStatus
{
    adi_adrv910x_CalStatusHdr_t hdr;                            /*!< Common calibration status header */
    uint16_t batchCnt;                                          /*!< Number of capture batches processed */
    uint16_t numEsts;                                           /*!< Number of estimates performed */
    uint8_t estCnt[ADI_ADRV910X_TXQEC_ATTEN_POINTS];            /*!< Estimate count per attenuation point */
    uint8_t converged;                                          /*!< Per-attenuation-point convergence bit field */
    uint16_t maxEstLoops;                                       /*!< Maximum estimation loops allowed */
} adi_adrv910x_TxQecInitStatus_t;

/**
 * \brief Tx LOL init calibration status
 */
typedef struct adi_adrv910x_TxLolInitStatus
{
    adi_adrv910x_CalStatusHdr_t hdr;                            /*!< Common calibration status header */
    uint32_t saturationCount;                                   /*!< Number of saturated captures */
    float gain;                                                 /*!< Estimated loopback gain */
    float phase;                                                /*!< Estimated loopback phase */
    float lolStatMean;                                          /*!< Running mean of the LOL statistic */
    float lolStatM2;                                            /*!< Running sum of squared deviations */
    float lolStatCount;                                         /*!< Number of samples in the running statistic */
    float lolStatVariance;                                      /*!< Variance of the LOL statistic */
    int32_t perturbationScaler;                                 /*!< Perturbation scaling applied by the algorithm */
    uint32_t batchSetCount;                                     /*!< Number of batch sets processed */
    uint16_t startCnt;                                          /*!< Cal start count */
    uint16_t stopCnt;                                           /*!< Cal stop count */
    uint16_t saveCnt;                                           /*!< Cal state save count */
    uint16_t completeCnt;                                       /*!< Cal completion count */
    uint16_t calExitCnt;                                        /*!< Early cal exit count */
    uint16_t calEmptyCnt;                                       /*!< Empty capture count */
    uint16_t capPartialCnt;                                     /*!< Partial capture count */
    bool updated;                                               /*!< Actuator was updated on the last run */
} adi_adrv910x_TxLolInitStatus_t;

/**
 * \brief Tx BBAF init calibration status
 */
typedef struct adi_adrv910x_TxBbafInitStatus
{
    adi_adrv910x_CalStatusHdr_t hdr;                            /*!< Common calibration status header */
    uint16_t c1;                                                /*!< Selected C1 capacitor code */
    int16_t trimQ;                                              /*!< Applied Q trim */
    uint8_t freqCalOffset;                                      /*!< Frequency calibration offset */
} adi_adrv910x_TxBbafInitStatus_t;

/**
 * \brief Tx BBAF group delay init calibration status
 */
typedef struct adi_adrv910x_TxBbafGdInitStatus
{
    adi_adrv910x_CalStatusHdr_t hdr;                                                                    /*!< Common calibration status header */
    int16_t gdNeg[ADI_ADRV910X_TXBBAF_GD_ESTIMATES];                                                    /*!< Group delay estimates from the negative tone */
    int16_t gdPos[ADI_ADRV910X_TXBBAF_GD_ESTIMATES];                                                    /*!< Group delay estimates from the positive tone */
    int16_t gdAvg[ADI_ADRV910X_TXBBAF_GD_ESTIMATES];                                                    /*!< Averaged group delay estimates */
    float gdImageMeas[ADI_ADRV910X_TXBBAF_GD_ESTIMATES][ADI_ADRV910X_TXBBAF_GD_MEAS_TYPES];             /*!< Image measurements per estimate and measurement type */
    int16_t gd[ADI_ADRV910X_TXBBAF_GD_ESTIMATES];                                                       /*!< Final gd1 and gd2 estimates */
    uint32_t gdSelected;                                                                                /*!< Measurement type selected by the algorithm */
    uint16_t gdMeasCnt[ADI_ADRV910X_TXBBAF_GD_MEAS_TYPES];                                              /*!< Measurement count per measurement type */
    uint16_t gdCurveFitCnt;                                                                             /*!< Curve fits performed */
    uint16_t gdCurveFitUnusedCnt;                                                                       /*!< Curve fits discarded */
} adi_adrv910x_TxBbafGdInitStatus_t;

/**
 * \brief Rx high power ADC RC tuner init calibration status
 */
typedef struct adi_adrv910x_RxHpAdcRcInitStatus
{
    adi_adrv910x_CalStatusHdr_t hdr;                            /*!< Common calibration status header */
    uint16_t sc;                                                /*!< Capacitor tuning code */
    uint16_t sg;                                                /*!< Resistor tuning code */
    bool rCalPerformed;                                         /*!< Resistor calibration was performed */
    bool cCalPerformed;                                         /*!< Capacitor calibration was performed */
    uint8_t adcProfileScaling;                                  /*!< ADC profile scaling applied */
} adi_adrv910x_RxHpAdcRcInitStatus_t;

/**
 * \brief Rx high power ADC flash init calibration status
 */
typedef struct adi_adrv910x_RxHpAdcFlashInitStatus
{
    adi_adrv910x_CalStatusHdr_t hdr;                                        /*!< Common calibration status header */
    uint32_t minimumRssi[ADI_ADRV910X_FLASHCAL_MAX_NUM_ADC];                /*!< Lowest RSSI observed per ADC */
    uint32_t successCount[ADI_ADRV910X_FLASHCAL_MAX_NUM_ADC];               /*!< Successful calibration count per ADC */
    uint64_t minimumCalCodes[ADI_ADRV910X_FLASHCAL_MAX_NUM_ADC];            /*!< Calibration codes at the lowest RSSI per ADC */
    uint64_t unNerfMinimumCalCodes[ADI_ADRV910X_FLASHCAL_MAX_NUM_ADC];      /*!< Un-nerfed calibration codes at the lowest RSSI per ADC */
} adi_adrv910x_RxHpAdcFlashInitStatus_t;

/**
 * \brief Rx TIA cutoff init calibration status
 *
 * Field order mirrors the firmware tiaCutoffCal_Status_t; the C1 and power
 * difference members are arrays indexed by LPF gain.
 */
typedef struct adi_adrv910x_RxTiaCutoffInitStatus
{
    uint32_t errorCode;                                                     /*!< Cal error code (0 = no error) */
    uint32_t corner3dB;                                                     /*!< Measured 3 dB corner frequency (TIA bandwidth) */
    bool calCompleted;                                                      /*!< Calibration ran to completion */
    bool calConverged;                                                      /*!< Calibration converged */
    uint16_t beforeCalC1[ADI_ADRV910X_TIA_CUTOFF_NUM_LPF_VALS];             /*!< C1 capacitor code before calibration, per LPF gain */
    uint16_t afterCalC1[ADI_ADRV910X_TIA_CUTOFF_NUM_LPF_VALS];              /*!< C1 capacitor code after calibration, per LPF gain */
    float beforeCalPowerDiffC1[ADI_ADRV910X_TIA_CUTOFF_NUM_LPF_VALS];       /*!< Power difference at C1 before calibration, per LPF gain */
    float afterCalPowerDiffC1[ADI_ADRV910X_TIA_CUTOFF_NUM_LPF_VALS];        /*!< Power difference at C1 after calibration, per LPF gain */
} adi_adrv910x_RxTiaCutoffInitStatus_t;

/**
 * \brief Rx tone calibration init calibration status for a single profile
 */
typedef struct adi_adrv910x_RxTcalInitStatus
{
    uint32_t errorCode;                                                     /*!< Cal error code (0 = no error) */
    bool calCompleted;                                                      /*!< Tone calibration ran to completion */
    bool loDelayCalCompleted[ADI_ADRV910X_RX_LODELAY_NUM_PLL];              /*!< LO delay calibration completion per PLL */
    uint32_t busyFlag;                                                      /*!< Cal busy flag */
    uint32_t tonePen;                                                       /*!< Positive tone enable word */
    uint32_t toneNen;                                                       /*!< Negative tone enable word */
    uint32_t rxGainIndex;                                                   /*!< Rx gain index used for the measurement */
    int32_t loc;                                                            /*!< Leakage of carrier estimate */
    int32_t hd[ADI_ADRV910X_TCAL_HD_LEN];                                   /*!< Harmonic distortion bins */
    int32_t cfirAllPass;                                                    /*!< Correction FIR all-pass indicator */
    float cfirCoeffRe[ADI_ADRV910X_RXQEC_CFIR_NTAPS];                       /*!< Correction FIR real coefficients */
    float cfirCoeffIm[ADI_ADRV910X_RXQEC_CFIR_NTAPS];                       /*!< Correction FIR imaginary coefficients */
    uint8_t loDelayCalIq[ADI_ADRV910X_RX_LODELAY_LO_TABLE_SIZE][ADI_ADRV910X_RX_LODELAY_NUM_PLL][ADI_ADRV910X_RX_LODELAY_GAIN_TABLE_SIZE][ADI_ADRV910X_LO_DELAY_NUM]; /*!< Calibrated LO delay per LO, PLL, gain and I/Q */
    uint8_t freqId;                                                         /*!< Frequency identifier of the reported profile */
} adi_adrv910x_RxTcalInitStatus_t;

/**
 * \brief One I/Q DC correction word pair
 */
typedef struct adi_adrv910x_RfdcCorrWords
{
    uint32_t dcCorrWordI;                                       /*!< I channel DC correction word */
    uint32_t dcCorrWordQ;                                       /*!< Q channel DC correction word */
} adi_adrv910x_RfdcCorrWords_t;

/**
 * \brief RFDC correction words for one channel, frequency region and LO
 */
typedef struct adi_adrv910x_RfdcChCorrWords
{
    adi_adrv910x_RfdcCorrWords_t rxCorrWords[ADI_ADRV910X_RFDC_NUM_RX_GAINS];           /*!< Correction words per Rx gain */
    adi_adrv910x_RfdcCorrWords_t orxElbCorrWords[ADI_ADRV910X_RFDC_NUM_ORX_FE_GAINS];   /*!< Correction words per ORx/ELB front end gain */
    adi_adrv910x_RfdcCorrWords_t lpbkCorrWords[ADI_ADRV910X_RFDC_NUM_LPBK_GAINS];       /*!< Correction words per loopback gain */
} adi_adrv910x_RfdcChCorrWords_t;

/**
 * \brief Rx RF DC offset init calibration status
 */
typedef struct adi_adrv910x_RxRfdcOffsetInitStatus
{
    adi_adrv910x_RfdcChCorrWords_t corrWords[ADI_ADRV910X_CAL_STATUS_NUM_CHANNELS][ADI_ADRV910X_RFDC_NUM_FREQ_REGIONS][ADI_ADRV910X_RFDC_NUM_LOS]; /*!< Correction words per channel, frequency region and LO */
    uint8_t rxInitDone[ADI_ADRV910X_CAL_STATUS_NUM_CHANNELS];       /*!< Rx path initialisation complete flags */
    uint8_t orxInitDone[ADI_ADRV910X_CAL_STATUS_NUM_CHANNELS];      /*!< ORx path initialisation complete flags */
    uint8_t elbInitDone[ADI_ADRV910X_CAL_STATUS_NUM_CHANNELS];      /*!< External loopback initialisation complete flags */
    uint8_t ilbInitDone[ADI_ADRV910X_CAL_STATUS_NUM_CHANNELS];      /*!< Internal loopback initialisation complete flags */
} adi_adrv910x_RxRfdcOffsetInitStatus_t;

/**
 * \brief DMR path delay init calibration status
 */
typedef struct adi_adrv910x_RxDmrPathDelayInitStatus
{
    uint16_t pathDelay;                                         /*!< Measured DMR path delay */
    uint8_t syncIndex;                                          /*!< Sync index the delay was measured against */
} adi_adrv910x_RxDmrPathDelayInitStatus_t;

/**
 * \brief Tx DPD tracking calibration status
 */
typedef struct adi_adrv910x_TxDpdTrackingStatus
{
    uint32_t numberOfSuccessfulIterations;                      /*!< Successful DPD update count */
    uint32_t numberOfCaptures;                                  /*!< Capture count */
    uint8_t normalRate;                                         /*!< Normal rate indicator */
    uint8_t ilbIntegerDelay;                                    /*!< Internal loopback integer delay */
    uint8_t ilbFractionalDelay;                                 /*!< Internal loopback fractional delay */
    uint8_t elbIntegerDelay;                                    /*!< External loopback integer delay */
    uint8_t elbFractionalDelay;                                 /*!< External loopback fractional delay */
} adi_adrv910x_TxDpdTrackingStatus_t;

/**
 * \brief Rx HD2 tracking calibration status
 */
typedef struct adi_adrv910x_RxHd2TrackingStatus
{
    adi_adrv910x_CalStatusHdr_t hdr;                            /*!< Common calibration status header */
} adi_adrv910x_RxHd2TrackingStatus_t;

/**
 * \brief Tx QEC tracking calibration status
 */
typedef struct adi_adrv910x_TxQecTrackingStatus
{
    adi_adrv910x_CalStatusHdr_t hdr;      /*!< Common calibration status header */
    uint8_t suppressedOverFreq;           /*!< 1 = TxQEC tracking suppressed (carrier above offset-LO disable freq) */
} adi_adrv910x_TxQecTrackingStatus_t;

/**
 * \brief Error and iteration accumulators for one wideband poly scope
 */
typedef struct adi_adrv910x_WbPolyStatusCell
{
    uint32_t iterCount;                                         /*!< Iterations accumulated in this scope */
    uint32_t errorFlags;                                        /*!< Accumulated error flags */
    uint32_t errorCount;                                        /*!< Accumulated error count */
    uint32_t selfcheckErrorCount;                               /*!< Self check error count */
    uint32_t selfcheckIrrDbMin;                                 /*!< Minimum self check image rejection in dB */
    uint32_t selfcheckIrrDbMax;                                 /*!< Maximum self check image rejection in dB */
} adi_adrv910x_WbPolyStatusCell_t;

/**
 * \brief Results of the last wideband poly analysis
 */
typedef struct adi_adrv910x_WbPolyAlgoStatus
{
    uint32_t powerProfile;                                      /*!< Detected power profile */
    uint32_t sigBins;                                           /*!< Signal bins used */
    uint32_t posSigBins;                                        /*!< Positive frequency signal bins */
    uint32_t negSigBins;                                        /*!< Negative frequency signal bins */
    uint32_t equSigBins;                                        /*!< Bins with equal positive and negative energy */
    uint32_t peakPowBins;                                       /*!< Peak power bins */
    uint32_t peakPosBins;                                       /*!< Peak positive frequency bins */
    uint32_t peakNegBins;                                       /*!< Peak negative frequency bins */
    uint32_t algo;                                              /*!< Algorithm variant selected */
    uint32_t loEstMode;                                         /*!< LO estimation mode selected */
} adi_adrv910x_WbPolyAlgoStatus_t;

/**
 * \brief Rx wideband poly tracking calibration status
 */
typedef struct adi_adrv910x_RxWbPolyTrackingStatus
{
    uint32_t errorCode;                                         /*!< Cal error code (0 = no error) */
    uint32_t percentComplete;                                   /*!< Percentage of data collected (0-100) */
    uint32_t selfcheckIrrDb;                                    /*!< Self check image rejection in dB */
    uint32_t iterCount;                                         /*!< Iterations performed */
    uint32_t updateCount;                                       /*!< Actuator updates performed */
    adi_adrv910x_WbPolyStatusCell_t curr;                       /*!< Accumulators for the current iteration */
    adi_adrv910x_WbPolyStatusCell_t cycle;                      /*!< Accumulators for the current cycle */
    adi_adrv910x_WbPolyStatusCell_t radioOn;                    /*!< Accumulators since the last radio on */
    adi_adrv910x_WbPolyStatusCell_t all;                        /*!< Accumulators since boot */
    adi_adrv910x_WbPolyAlgoStatus_t algoStatus;                 /*!< Results of the last analysis */
} adi_adrv910x_RxWbPolyTrackingStatus_t;

/*============================================================================
 * CalStatus container
 *===========================================================================*/

/**
 * \brief Calibration status returned by adi_adrv910x_cals_CalStatus_Get()
 *
 * Only the member matching \c sel holds valid data; the remaining members are
 * zeroed. \c sel echoes the selector passed to the get API.
 *
 * \note Deliberately a flat struct, not a union of these members: the webrpcgen
 * tool that generates the server shims and C# client bindings from this header
 * cannot marshal unions ("Unhandled member type").
 */
typedef struct adi_adrv910x_CalStatus
{
    adi_adrv910x_CalStatusSel_e sel;                            /*!< Calibration whose status is populated below */
    adi_adrv910x_TxQecInitStatus_t txQecInit;                   /*!< Valid for #ADI_ADRV910X_CAL_STATUS_TX_QEC_INIT */
    adi_adrv910x_TxLolInitStatus_t txLolInit;                   /*!< Valid for #ADI_ADRV910X_CAL_STATUS_TX_LOL_INIT */
    adi_adrv910x_TxBbafInitStatus_t txBbafInit;                 /*!< Valid for #ADI_ADRV910X_CAL_STATUS_TX_BBAF_INIT */
    adi_adrv910x_TxBbafGdInitStatus_t txBbafGdInit;             /*!< Valid for #ADI_ADRV910X_CAL_STATUS_TX_BBAF_GD_INIT */
    adi_adrv910x_RxHpAdcRcInitStatus_t rxHpAdcRcInit;           /*!< Valid for #ADI_ADRV910X_CAL_STATUS_RX_HPADC_RC_INIT */
    adi_adrv910x_RxHpAdcFlashInitStatus_t rxHpAdcFlashInit;     /*!< Valid for #ADI_ADRV910X_CAL_STATUS_RX_HPADC_FLASH_INIT */
    adi_adrv910x_RxTiaCutoffInitStatus_t rxTiaCutoffInit;       /*!< Valid for #ADI_ADRV910X_CAL_STATUS_RX_TIA_CUTOFF_INIT */
    adi_adrv910x_RxTcalInitStatus_t rxTcalInit;                 /*!< Valid for #ADI_ADRV910X_CAL_STATUS_RX_TCAL_INIT */
    adi_adrv910x_RxRfdcOffsetInitStatus_t rxRfdcOffsetInit;     /*!< Valid for #ADI_ADRV910X_CAL_STATUS_RX_RFDC_OFFSET_INIT */
    adi_adrv910x_RxDmrPathDelayInitStatus_t rxDmrPathDelayInit; /*!< Valid for #ADI_ADRV910X_CAL_STATUS_RX_DMR_PATH_DELAY_INIT */
    adi_adrv910x_TxDpdTrackingStatus_t txDpdTracking;           /*!< Valid for #ADI_ADRV910X_CAL_STATUS_TX_DPD_TRACKING */
    adi_adrv910x_RxHd2TrackingStatus_t rxHd2Tracking;           /*!< Valid for #ADI_ADRV910X_CAL_STATUS_RX_HD2_TRACKING */
    adi_adrv910x_RxWbPolyTrackingStatus_t rxWbPolyTracking;     /*!< Valid for #ADI_ADRV910X_CAL_STATUS_RX_WB_POLY_TRACKING */
    adi_adrv910x_TxQecTrackingStatus_t txQecTracking;           /*!< Valid for #ADI_ADRV910X_CAL_STATUS_TX_QEC_TRACKING */
} adi_adrv910x_CalStatus_t;

#endif /* _ADI_ADRV910X_CALS_TYPES_H_ */
