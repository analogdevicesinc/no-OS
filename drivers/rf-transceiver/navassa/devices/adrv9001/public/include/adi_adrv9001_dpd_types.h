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
                                                     *   ADI_ADRV9001_TRACKING_CAL_TX_DPD may be used to enable DPD */
    adi_adrv9001_DpdAmplifier_e amplifierType;      //!< Type of Power Amplifier
    adi_adrv9001_DpdLutSize_e lutSize;              //!< Bit size of the DPD LUT
    adi_adrv9001_DpdModel_e model;                  //!< DPD Model to be used
    bool changeModelTapOrders;                      //!< Whether to use the Tap_Orders in #modelOrdersForEachTap
    uint32_t modelOrdersForEachTap[4];              /*!< Bitmap for each of the taps in a model to indicate which power
                                                     *   terms are included in the model (and thus the auto-correlation
                                                     *   matrix). Used when #changeModelTapOrders is true */
    uint8_t preLutScale;                            //!< Prescaler for the LUT (U2.1; min = 0; max = 3.5) */
} adi_adrv9001_DpdInitCfg_t;

/**
 * \brief DPD settings that may be configured in the CALIBRATED state
 */
typedef struct adi_adrv9001_DpdCfg
{
    uint32_t numberOfSamples;           //!< Number of samples to use for DPD (min: 3; max: 4096) */
    bool outlierRemovalEnable;          //!< Enable removal of outliers */
    uint32_t outlierRemovalThreshold;   //!< Threshold for outlier removal (U2.30) */

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
    bool immediateLutSwitching;             //!< Whether the LUT switches immediately or at the end of a Tx frame */
    bool useSpecialFrame;                   //!< Whether to only run DPD on a user indicated special frame */
    bool resetLuts;                         //!< Whether to reset LUTs to unity. Always read as 0 and is self-clearing */
} adi_adrv9001_DpdCfg_t;

typedef struct adi_adrv9001_DpdCoefficients
{
    uint8_t region;     //!< The region of the LUT initialization data (valid 0 - 7); ignored on get - returns most recent
    uint8_t coefficients[ADI_ADRV9001_DPD_NUM_COEFFICIENTS];    //!< DPD coefficients
}adi_adrv9001_DpdCoefficients_t;

#endif /* _ADI_ADRV9001_DPD_TYPES_H_ */
