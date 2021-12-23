/**
 * \file
 * \brief Contains ADRV9001 DPD data types
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2019 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_DPD_TYPES_H_
#define _ADI_ADRV9001_DPD_TYPES_H_

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#define ADI_ADRV9001_DPD_NUM_COEFFICIENTS 208

#define INST_TX1_DPD_TOP (0X55000000U) /* tx1_dpd_top: */
#define INST_TX2_DPD_TOP (0X75000000U) /* tx2_dpd_top: */

#define INCR_IDX_NVS_AHB_TX_DPD_TOP_DPD_TOP_TX_I_N              (0X00004U)    /* Array Unit Address Increment Value */
#define INCR_IDX_NVS_AHB_TX_DPD_TOP_DPD_TOP_TX_Q_N              (0X00004U)    /* Array Unit Address Increment Value */
#define INCR_IDX_NVS_AHB_TX_DPD_TOP_DPD_TOP_RX_I_N              (0X00004U)    /* Array Unit Address Increment Value */
#define INCR_IDX_NVS_AHB_TX_DPD_TOP_DPD_TOP_RX_Q_N              (0X00004U)    /* Array Unit Address Increment Value */

#define IDX_NVS_AHB_TX_DPD_TOP_DPD_TOP_TX_I_N(index)            ( (0X00000U)+(index)*(INCR_IDX_NVS_AHB_TX_DPD_TOP_DPD_TOP_TX_I_N) )    /* No description provided */
#define IDX_NVS_AHB_TX_DPD_TOP_DPD_TOP_TX_Q_N(index)            ( (0X04000U)+(index)*(INCR_IDX_NVS_AHB_TX_DPD_TOP_DPD_TOP_TX_Q_N) )    /* No description provided */
#define IDX_NVS_AHB_TX_DPD_TOP_DPD_TOP_RX_I_N(index)            ( (0X08000U)+(index)*(INCR_IDX_NVS_AHB_TX_DPD_TOP_DPD_TOP_RX_I_N) )    /* No description provided */
#define IDX_NVS_AHB_TX_DPD_TOP_DPD_TOP_RX_Q_N(index)            ( (0X0C000U)+(index)*(INCR_IDX_NVS_AHB_TX_DPD_TOP_DPD_TOP_RX_Q_N) )    /* No description provided */

#define pREG_NVS_AHB_TX_DPD_TOP_DPD_TOP_TX_I_N(base,index)      ((base) + IDX_NVS_AHB_TX_DPD_TOP_DPD_TOP_TX_I_N(index))
#define pREG_NVS_AHB_TX_DPD_TOP_DPD_TOP_TX_Q_N(base,index)      ((base) + IDX_NVS_AHB_TX_DPD_TOP_DPD_TOP_TX_Q_N(index))
#define pREG_NVS_AHB_TX_DPD_TOP_DPD_TOP_RX_I_N(base,index)      ((base) + IDX_NVS_AHB_TX_DPD_TOP_DPD_TOP_RX_I_N(index))
#define pREG_NVS_AHB_TX_DPD_TOP_DPD_TOP_RX_Q_N(base,index)      ((base) + IDX_NVS_AHB_TX_DPD_TOP_DPD_TOP_RX_Q_N(index))

/**
 * \brief Supported DPD amplifier types
 */
typedef enum adi_adrv9001_DpdAmplifier
{
    ADI_ADRV9001_DPDAMPLIFIER_NONE,
    ADI_ADRV9001_DPDAMPLIFIER_DEFAULT,
    ADI_ADRV9001_DPDAMPLIFIER_GAN
} adi_adrv9001_DpdAmplifier_e;

/**
 * \brief Supported DPD LUT sizes
 */
typedef enum adi_adrv9001_DpdLutSize
{
    ADI_ADRV9001_DPDLUTSIZE_256,
    ADI_ADRV9001_DPDLUTSIZE_512
} adi_adrv9001_DpdLutSize_e;

/**
 * \brief Supported DPD models
 */
typedef enum adi_adrv9001_DpdModel
{
    ADI_ADRV9001_DPDMODEL_0 = 0,
    ADI_ADRV9001_DPDMODEL_1 = 1,
    ADI_ADRV9001_DPDMODEL_3 = 3,
    ADI_ADRV9001_DPDMODEL_4 = 4
} adi_adrv9001_DpdModel_e;

/**
 * \brief DPD settings that must be configured in the STANDBY state
 */
typedef struct adi_adrv9001_DpdInitCfg
{
    bool enable;                                    /*!< When true, the DPD actuator is placed in the datapath and
                                                     *   ADI_ADRV9001_TRACKING_CAL_TX_DPD_CLGC may be used to enable DPD */
    adi_adrv9001_DpdAmplifier_e amplifierType;      //!< Type of Power Amplifier
    adi_adrv9001_DpdLutSize_e lutSize;              //!< Bit size of the DPD LUT
    adi_adrv9001_DpdModel_e model;                  //!< DPD Model to be used
    bool changeModelTapOrders;                      //!< Whether to use the Tap_Orders in #modelOrdersForEachTap
    uint32_t modelOrdersForEachTap[4];              /*!< Bitmap for each of the taps in a model to indicate which power
                                                     *   terms are included in the model (and thus the auto-correlation
                                                     *   matrix). Used when #changeModelTapOrders is true */
    uint8_t preLutScale;                            //!< Prescaler for the LUT (U2.2; min = 0; max = 3.75) */
    uint8_t clgcEnable;                             //!< CLGC Enable */
} adi_adrv9001_DpdInitCfg_t;

/**
 * \brief DPD settings that may be configured in the CALIBRATED state
 */
typedef struct adi_adrv9001_DpdCfg
{
    uint32_t numberOfSamples;           //!< Number of samples to use for DPD (min: 3; max: 4096) */

    /** Additional scaling on the 2nd and higher order power terms.
     * Used to keep the nominal magnitude of each term about the same
     */
    uint32_t additionalPowerScale;

    /** Tx signal power for the lower threshold for Rx/Tx data alignment (U2.30; min = 0; max = 1.0).
     * Where power \f$p(n) = x(n) x^*(n)\f$ and \f$x(n)\f$ is the complex signal data.
     * \note May read back a slightly different value than written due to floating point conversion error
     */
    uint32_t rxTxNormalizationLowerThreshold;

    /** Tx signal power for the upper threshold for Rx/Tx data alignment (U2.30; min = 0; max = 1.0).
     * Where power \f$p(n) = x(n) x^*(n)\f$ and \f$x(n)\f$ is the complex signal data.
     * \note May read back a slightly different value than written due to floating point conversion error
     */
    uint32_t rxTxNormalizationUpperThreshold;
    
    uint32_t detectionPowerThreshold;   //!< Signal power for the power threshold (U1.31) */
    uint32_t detectionPeakThreshold;    //!< Signal power for the peak threshold (U1.31) */
    
    /** If the number of points below the detectionPowerThreshold exceeds this number, the capture is discarded.
     * To disable, set to 4096
     */
    uint16_t countsLessThanPowerThreshold;
    
    /** If the number of points above the detectionPeakThreshold is less than this number, the capture is discarded.
     * To disable, set to 0
     */
    uint16_t countsGreaterThanPeakThreshold;
    
    bool immediateLutSwitching;             //!< Whether the LUT switches immediately or at the end of a Tx frame */
    bool useSpecialFrame;                   //!< Whether to only run DPD on a user indicated special frame */
    bool resetLuts;                         //!< Whether to reset LUTs to unity. Always read as 0 and is self-clearing */
    uint32_t timeFilterCoefficient;         /*!< Time filter coefficient in U1.31 format */
    uint32_t dpdSamplingRate_Hz;            /*!< sampling rate in Hz for the DPD actuator and capture.
                                                 'dpdSamplingRate_Hz' is read only and is ignored in adi_adrv9001_dpd_Configure() */
	uint8_t clgcLoopOpen;                   /*!< If true, the loop is open and the TX attenuators are not updated.  Used to measure a target gain. */
	int32_t clgcGainTarget_HundredthdB;     /*!< Sent in 1/100 dB. */
	uint32_t clgcFilterAlpha;               /*!< filter coefficient for the filtered  gain values. */
	int32_t clgcLastGain_HundredthdB;       /*!< last gain.  Only valid during Get. */
	int32_t clgcFilteredGain_HundredthdB;   /*!< filtered gain.  Only valid during Get. */
} adi_adrv9001_DpdCfg_t;

typedef struct adi_adrv9001_DpdCoefficients
{
    uint8_t region;     //!< The region of the LUT initialization data (valid 0 - 7)
    uint8_t coefficients[ADI_ADRV9001_DPD_NUM_COEFFICIENTS];    //!< DPD coefficients
}adi_adrv9001_DpdCoefficients_t;

typedef struct adi_adrv9001_DpdFhRegions
{
    uint64_t startFrequency_Hz; //!< Carrier frequency greater than or equal to this is included in the region
    uint64_t endFrequency_Hz;   //!< Carrier frequency less than this is included in the region
} adi_adrv9001_DpdFhRegions_t;

#endif /* _ADI_ADRV9001_DPD_TYPES_H_ */
