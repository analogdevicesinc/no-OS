/**
 * Copyright 2020 - 2021 Analog Devices Inc.
 * Released under the ADRV904X Software license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adi_adrv904x_dfe_svc_dfe_pwr_mtr_t.h
 * \brief DFE power meter service module definitions and types
 *
 * DFE SDK API Version: 2.9.0.4
 */

#ifndef ADRV904X_DFE_SVC_DFE_PWR_MTR_T_H_
#define ADRV904X_DFE_SVC_DFE_PWR_MTR_T_H_

#include <stdint.h>
#include <stdbool.h>
#include "adi_adrv904x_platform_pack.h"

/**
 * \brief Enumerated list of options to select the TSSI power meter
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_TSSI_TX_1,                                          /*!< Tx TSSI 1 power meter */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_TSSI_TX_2,                                          /*!< Tx TSSI 2 power meter */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_TSSI_ORX,                                           /*!< ORx TSSI power meter  */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_TSSI_CDUC_1,                                        /*!< Tx CDUC TSSI 1 power meter */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_TSSI_CDUC_2,                                        /*!< Tx CDUC TSSI 2 power meter */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_TSSI_CDUC_3,                                        /*!< Tx CDUC TSSI 3 power meter */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_TSSI_CDUC_4,                                        /*!< Tx CDUC TSSI 4 power meter */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_TSSI_CDUC_5,                                        /*!< Tx CDUC TSSI 5 power meter */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_TSSI_CDUC_6,                                        /*!< Tx CDUC TSSI 6 power meter */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_TSSI_CDUC_7,                                        /*!< Tx CDUC TSSI 7 power meter */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_TSSI_CDUC_8,                                        /*!< Tx CDUC TSSI 8 power meter */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_NUM_TSSI_METERS,                                    /*!< number of TSSI meters */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_NUM_CLGC_METERS = ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_TSSI_ORX + 1, /*!< Number of TSSI meters for CLGC power measurements */
} adi_adrv904x_DfeSvcDfePwrMtrTssi_e;

/**
 * \brief Enumerated list of options to select the RSSI power meter
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_RSSI_RX_DDC_0,             /*!< Rx DDC RSSI 0 power meter */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_RSSI_RX_DDC_1,             /*!< Rx DDC RSSI 1 power meter */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_RSSI_CDDC_1,               /*!< Rx CDDC RSSI 1 power meter */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_RSSI_CDDC_2,               /*!< Rx CDDC RSSI 2 power meter */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_RSSI_CDDC_3,               /*!< Rx CDDC RSSI 3 power meter */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_RSSI_CDDC_4,               /*!< Rx CDDC RSSI 4 power meter */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_RSSI_CDDC_5,               /*!< Rx CDDC RSSI 5 power meter */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_RSSI_CDDC_6,               /*!< Rx CDDC RSSI 6 power meter */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_RSSI_CDDC_7,               /*!< Rx CDDC RSSI 7 power meter */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_RSSI_CDDC_8,               /*!< Rx CDDC RSSI 8 power meter */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_NUM_RSSI_METERS,           /*!< number of RSSI meters */
} adi_adrv904x_DfeSvcDfePwrMtrRssi_e;

/**
 * \brief Enumerated list of options to select the TX TSSI power meter input source
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_PRE_DPD_HB1_OUT,           /*!< 0: TSSI 1: pre-DPD HB1 output */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_PRE_DPD_HB2_OUT_AFTER_HC,  /*!< 1: TSSI 1: pre-DPD HB2 output after hard clipper */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_CFR_IN,                    /*!< 2: TSSI 2: CFR input */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_CFR_OUT,                   /*!< 3: TSSI 2: CFR output */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_POST_DPD_HB1_OUT,          /*!< 4: TSSI 2: post-DPD HB1 output */
    ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_TSSI_NUM_SOURCES           /*!< 5: number of Tx sources */
} adi_adrv904x_DfeSvcDfePwrMtrTssiSrc_e;

/**
 * \brief Typedefs for TSSI done registered callback functions
 *
 * \param [in] chanNum           - channel number
 * \param [in] pwrTssiMeasLinear - power measurement results for ORX and Tx TSSIs (linear)
 * \param [in] pwrTxTssiSuccessfulFlags - Successful flags bitmask b10:Tx TSSI1, b9:Tx TSSI2, b8:Orx TSSI, b7-0: Carrier TSSI-8
 */
typedef void (*adi_adrv904x_DfeSvcTssiPwrMtrDoneCallback_t)(const uint8_t chanNum,
                                                 uint64_t pwrTssiMeasLinear[ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_NUM_TSSI_METERS],
                                                 uint32_t pwrTxTssiSuccessfulFlags);

/**
 * \brief Typedefs for RSSI done registered callback functions
 *
 * \param [in] chanNum           - channel number
 * \param [in] pwrRssiMeasLinear - power measurement results for Rx RSSIs (linear)
 * \param [in] pwrRxRssiSuccessfulFlags - Successful flags bitmask b9:Rx Band0 RSSI, b8: Rx Band1 RSSI, b7-0: Carrier RSSI-8
 */
typedef void (*adi_adrv904x_DfeSvcRssiPwrMtrDoneCallback_t)(const uint8_t chanNum,
                                                 uint64_t pwrRssiMeasLinear[ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_NUM_RSSI_METERS],
                                                 uint32_t pwrRxRssiSuccessfulFlags);
/**
 * \brief Data structure to hold TSSI power meter configuration parameters
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeSvcDfePwrMtrTssiCfg
{
    uint8_t  resetAtGainChange;                      /*!< measurement reset at gain change, NA for ORx TSSI (deprecated and unused) */
    uint8_t  sourceSelect;                           /*!< defined in adi_adrv904x_DfeSvcDfePwrMtrTssiSrc_e, NA for ORx TSSI (Only used in ConfigGet) */
    uint8_t  radioSeqPinModeEnable;                  /*!< radio seq control, radio_seq_sel to be >= 1 for ORx */
    uint8_t  opMode;                                 /*!< 1: power of 2 meas specified by duration0, 0: non-power of 2 */
    uint8_t  measMode;                               /*!< 0:immediate, 1:Tx on start delay, 2:Tx on stat and wait delay */
    uint16_t startDelay;                             /*!< delay meas from Tx on edge for meas mode 1 and 2, range: 1 - 0x1FFF */
    uint16_t contDelay;                              /*!< similar to startDelay, but for 2nd TDD on duration and beyond, range: 1 - 0x3FFF */
    uint32_t waitDelay;                              /*!< At end of meas, delay the next meas start in FDD/mode 2/manual, range: 1 - 0xFFFFFF */
    uint8_t  fddPinMode;                             /*!< For FDD only, trigger meas start using a GPIO */
    uint8_t  duration0;                              /*!< meas duration0 */
    uint8_t  duration1;                              /*!< meas duration1 */
    uint8_t  duration2;                              /*!< meas duration2 */
    uint8_t  duration3;                              /*!< meas duration3 */
    uint16_t mulFactor0;                             /*!< non-power of 2 mode, the weight of power calculated for duration 0, range:0-0x3FF */
    uint16_t mulFactor1;                             /*!< non-power of 2 mode, the weight of power calculated for duration 1, range:0-0x3FF */
    uint16_t mulFactor2;                             /*!< non-power of 2 mode, the weight of power calculated for duration 2, range:0-0x3FF */
    uint16_t mulFactor3;                             /*!< non-power of 2 mode, the weight of power calculated for duration 3, range:0-0x3FF */
    uint8_t  setLinearThreshold;                     /*!< in linear mode, set the threshold (0 or 1) */
    uint32_t linearThreshold;                        /*!< linear threshold (0 - 0xFFFFFFFF) */
    uint8_t  enabled;                                /*!< measurement is enabled (only applies to get configuration and when making Tx pwr measurement
                                                      *   requests through capture service)	*/
    uint32_t debug;                                  /*!< For ADI private tests */
    uint32_t captureTimeUsec;                        /*!< the expected time (usec) it takes to perform a power measurement */
} adi_adrv904x_DfeSvcDfePwrMtrTssiCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold RSSI (DDC/CDDC) power meter configuration parameters
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeSvcDfePwrMtrRssiCfg
{
    uint8_t  enableGainCalc;                         /*!< include compensation for the gain */
    uint8_t  gaintableStepSize;                      /*!< gain table step size. NA for CDDC RSSI blocks */
    uint8_t  resetAtGainChange;                      /*!< measurement reset at gain change */
    uint8_t  radioSeqPinModeEnable;                  /*!< radio seq control, radio_seq_sel. NA for DDC RSSI blocks */
    uint8_t  opMode;                                 /*!< 1: power of 2 meas specified by duration0, 0: non-power of 2 */
    uint8_t  measMode;                               /*!< 0:immediate, 1:Rx on start delay, 2:Rx on stat and wait delay */
    uint16_t startDelay;                             /*!< delay meas from Rx on edge for meas mode 1 and 2, range: 1 - 0x1FFF */
    uint16_t contDelay;                              /*!< similar to startDelay, but for 2nd TDD on duration and beyond, range: 1 - 0x3FFF */
    uint32_t waitDelay;                              /*!< At end of meas, delay the next meas start in FDD/mode 2/manual, range: 1 - 0xFFFFFF */
    uint8_t  fddPinMode;                             /*!< For FDD only, trigger meas start using a GPIO */
    uint8_t  duration0;                              /*!< meas duration0 */
    uint8_t  duration1;                              /*!< meas duration1 */
    uint8_t  duration2;                              /*!< meas duration2 */
    uint8_t  duration3;                              /*!< meas duration3 */
    uint16_t mulFactor0;                             /*!< non-power of 2 mode, the weight of power calculated for duration 0 , range:0-0xFF for Band RSSI, 0-0x3FF for carrier RSSI */
    uint16_t mulFactor1;                             /*!< non-power of 2 mode, the weight of power calculated for duration 1 , range:0-0xFF for Band RSSI, 0-0x3FF for carrier RSSI */
    uint16_t mulFactor2;                             /*!< non-power of 2 mode, the weight of power calculated for duration 2 , range:0-0xFF for Band RSSI, 0-0x3FF for carrier RSSI */
    uint16_t mulFactor3;                             /*!< non-power of 2 mode, the weight of power calculated for duration 3 , range:0-0xFF for Band RSSI, 0-0x3FF for carrier RSSI */
    uint8_t  enabled;                                /*!< measurement is enabled (only applies to get configuration and when making Tx pwr measurement
                                                      *   requests through capture service)	*/
    uint32_t debug;                                  /*!< For ADI private tests */
    uint32_t captureTimeUsec;                        /*!< the expected time (usec) it takes to perform a power measurement */
} adi_adrv904x_DfeSvcDfePwrMtrRssiCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold the power measurement results from an RSSI carrier measurement
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeSvcDfePwrMtrRssiMeasResult
{
    uint8_t  powerMeasDb;     /*!< Power readback (dB) in 0.25 resolution */
    uint64_t powerMeasLinear; /*!< Power readback (36 bits linear) */
} adi_adrv904x_DfeSvcDfePwrMtrRssiMeasResult_t;
ADI_ADRV904X_PACK_FINISH

/*
 * \brief Data structure to hold the power measurement results from a TSSI measurement
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeSvcDfePwrMtrTssiMeasResult
{
    uint8_t  powerMeasDb;     /*!< Power readback (dB) in 0.25 resolution */
    uint64_t powerMeasLinear; /*!< Power readback (36 bits linear) */
    uint8_t  thresExceeded;   /*!< TSSI 1 or 2 threshold exceeded. */
} adi_adrv904x_DfeSvcDfePwrMtrTssiMeasResult_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold DPD power meter configuration parameters
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeSvcDfePwrMtrDpdCfg
{
    uint8_t  dpdInOutPwrMeasDuration;                /*!< power measurement duration (range 0 - 24) */
    uint16_t dpdInOutPwrMeasContDlyCntr;             /*!< wait before continuing with power measurement after txon */
    uint8_t  dpdInOutPwrMeasTDDModeEn;               /*!< run power measurement in TDD mode */
    uint8_t  dpdInOutPeakToPowerMode;                /*!< enable peak-to-power mode */
    uint8_t  dpdInOutPwrMeasPause;                   /*!< power measurement is paused (only used in the GetConfig) */
    uint8_t  dpdInOutPwrMeasEn;                      /*!< power measurement is enabled (only used in the GetConfig) */
} adi_adrv904x_DfeSvcDfePwrMtrDpdCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold DPD power meter measurements
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeSvcDfePwrMtrDpdResults
{
    uint8_t pwrDpdIn;                               /*!< pre-DPD power */
    uint8_t peakPwrDpdIn;                           /*!< pre-DPD peak power */
    uint8_t pwrDpdOut;                              /*!< post-DPD power */
    uint8_t peakPwrDpdOut;                          /*!< post-DPD peak power */
} adi_adrv904x_DfeSvcDfePwrMtrDpdResults_t;
ADI_ADRV904X_PACK_FINISH

/**
 *  \brief Enum of Carriers (mask) within a Tx Channel
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_DFE_TX_CARRIER_NONE = 0x00U,
    ADI_ADRV904X_DFE_SVC_DFE_TX_CARRIER_0    = 0x01U,
    ADI_ADRV904X_DFE_SVC_DFE_TX_CARRIER_1    = 0x02U,
    ADI_ADRV904X_DFE_SVC_DFE_TX_CARRIER_2    = 0x04U,
    ADI_ADRV904X_DFE_SVC_DFE_TX_CARRIER_3    = 0x08U,
    ADI_ADRV904X_DFE_SVC_DFE_TX_CARRIER_4    = 0x10U,
    ADI_ADRV904X_DFE_SVC_DFE_TX_CARRIER_5    = 0x20U,
    ADI_ADRV904X_DFE_SVC_DFE_TX_CARRIER_6    = 0x40U,
    ADI_ADRV904X_DFE_SVC_DFE_TX_CARRIER_7    = 0x80U,
    ADI_ADRV904X_DFE_SVC_DFE_TX_CARRIER_ALL  = 0xFFU,
} adi_adrv904x_DfeSvcDfeTxCarrier_e;

/**
 *  \brief Enum of Bands (mask) within a Tx Channel
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_DFE_TX_BAND_NONE = 0x00U,
    ADI_ADRV904X_DFE_SVC_DFE_TX_BAND_0    = 0x01U,
    ADI_ADRV904X_DFE_SVC_DFE_TX_BAND_1    = 0x02U,
    ADI_ADRV904X_DFE_SVC_DFE_TX_BAND_ALL  = 0x03U,
} adi_adrv904x_DfeSvcDfeTxBand_e;

/**
 * \brief Data structure to hold Tx carrier power meter configuration parameters
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeSvcDfeTxCarrierPowerMeasCfg
{
    uint8_t  measDuration;              /*!< Number of samples over which power is
                                         *   averaged 2^(measDuration); Range 0 - 18 */
    uint16_t measContinueDelayDuration; /*!< For TDD enable only.
                                         *   Set the duration to wait before continuing
                                         *   with power measurement after txon goes high */
    uint16_t peakToPowerMode;           /*!< When this mode is enabled, power for a given
                                         *   measurement duration is available in
                                         *   the tx_power bit-field.
                                         *   Also, the largest peak (I^2+Q^2) within that duration
                                         *   is available in the tx_power_largest_peak bit-field. */
    uint32_t inputSelect;               /*!< Selection of the data path point for power calculations.
                                         *   0 = chan filter out, 1 = delay matched out, 2 = multiplier out */
    uint8_t  tddModeEnable;             /*!< 1 = enable TDD mode, 0 = disable TDD mode */

    uint8_t  enable;                    /*!< Power measurement is enabled (used in get config). */
    uint8_t  pause;                     /*!< Power measurement is paused (used in get config). */
} adi_adrv904x_DfeSvcDfeTxCarrierPowerMeasCfg_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold the power measurement results from a Tx carrier
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeSvcDfeTxCarrierPowerMeasResult
{
    int32_t power_mdB;       /*!< Average power calculated for the measurement duration in mdB */

    int32_t largestPeak_mdB; /*!< Highest I^2+Q^2 within a given measurement duration.
                              *   This value is available if the tx_peak_to_power_mode bit-field is set 1.
                              *   in mdB with resolution of 0.25dB. */
} adi_adrv904x_DfeSvcDfeTxCarrierPowerMeasResult_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold the Tx carrier gain control
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeSvcDfeTxCarrierCarrierGain
{
    uint32_t channelMask;  /*!< Mask of Tx Channels for carrier gain control */
    uint32_t carrierMask;  /*!< Carrier mask for N carriers where N = 8. These enable gainReg[n] */

    uint32_t gain_mdB[8];  /*!< Gain in mdB */
} adi_adrv904x_DfeSvcDfeTxCarrierCarrierGain_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold the Tx carrier band attenuation.
 *        Used to ensure that the aggregated carrier output doesn't exceed the digital fullscale.
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeSvcDfeTxCarrierCarrierBandAttn
{
    uint32_t channelMask; /*!< Tx Channel Mask selection (8 channels) */
    uint32_t bandMask;    /*!< Tx Band Mask selection (2 bands) */

    uint32_t attn_mdB[2]; /*!< Carrier attenuation for bands. In mdB.
                           *   API will convert to Fixed-point attenuation in Q0.8 format
                           *   to support -0.03 dB (255) to -infinity for 0. */
} adi_adrv904x_DfeSvcDfeTxCarrierCarrierBandAttn_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold the Post DPD gain parameters
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeSvcDfePostDpdGain
{
    uint8_t magGain;     /*!< Actuator Magnitude Square Input Gain Range 0-4*FS. Saturated to 1x.
                          *   Data Format: Unsigned Int.Frac=2.6. Unity gain=0x40.*/

    uint8_t gmpOutShift; /*!< Actuator Output Shift for gmp terms Range 0-15. Add one to double output gain.
                          *   Output is left shifted to accommodate output normalization.
                          *   Default shift is configured for unity coefficient sum of 0x2000 and
                          *   2 bits of headroom for individual coefficient. When large number of terms
                          *   are used resulting in bit growth, the output gain can be decreased (right shift)
                          *   to the correct the overall gain. Max dynamic range at out_shift=0, supporting
                          *   all LUTs active and data at maximum. */

    uint8_t ddrOutShift; /*!< Actuator Output Shift for ddr terms Range 0-15. Add one to double output gain.
                          *   Output is left shifted to accommodate output normalization.
                          *   Default shift is configured for unity coefficient sum of 0x2000 and
                          *   2 bits of headroom for individual coefficient. When large number of terms
                          *   are used resulting in bit growth, the output gain can be decreased (right shift)
                          *   to the correct the overall gain. Max dynamic range at out_shift=0, supporting
                          *   all LUTs active and data at maximum. */
} adi_adrv904x_DfeSvcDfePostDpdGain_t;
ADI_ADRV904X_PACK_FINISH

/**
 * \brief Data structure to hold the Tx carrier error status and error mask bits for a Tx channel
 */
    ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_DfeSvcDfeTxCarrierErrorStatusMask
{
    uint8_t carrFilterError; /*!< Indicates over/underflow in channel filter carrier X (8 bit mask) */
    uint8_t hbError;         /*!< Indicates over/underflow in CDUC HB filter (1 bit) */
    uint8_t carrGainError;   /*!< Indicates over/underflow in carrier gain X (8 bit mask). */
    uint8_t attenBandError;  /*!< Indicates over/underflow in CDUC HD filter attenuation for bandX (2 bit mask) */
} adi_adrv904x_DfeSvcDfeTxCarrierErrorStatusMask_t;
ADI_ADRV904X_PACK_FINISH

#endif  /* ADRV904X_DFE_SVC_DFE_PWR_MTR_T_H_ */


