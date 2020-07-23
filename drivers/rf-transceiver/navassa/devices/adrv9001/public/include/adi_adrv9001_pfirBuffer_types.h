/**
 * \file
 * \brief Type definitions for ADRV9001 PFIR buffers
 * \copyright Analog Devices Inc. 2019. All rights reserved.
 * Released under the ADRV9001 API license, for more information see "LICENSE.txt" in the SDK
 */

#ifndef _ADI_ADRV9001_PFIRBUFFER_TYPES_H_
#define _ADI_ADRV9001_PFIRBUFFER_TYPES_H_

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

#include "adi_adrv9001_defines.h"

#define ADI_ADRV9001_NUM_GAIN_COMP9						6U
#define ADI_ADRV9001_MAG_COMP_NB_PFIR_COEFS_MAX_SIZE	13
#define ADI_ADRV9001_MAG_COMP_PFIR_COEFS_MAX_SIZE		21
#define ADI_ADRV9001_WB_NB_PFIR_COEFS_MAX_SIZE			128

/**
 *  \brief Enumerations to determine symmetricity of PFIR coefficients
 */
typedef enum adi_adrv9001_PfirSymmetric
{
    ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC = 0u, /*!< Coefficients are expected to be non-symmetric */
    ADI_ADRV9001_PFIR_COEF_SYMMETRIC = 1u,     /*!< Coefficients are expected to be symmetric */
} adi_adrv9001_PfirSymmetric_e;

/**
 *  \brief Enumerations to determine number of PFIR filter taps
 */
typedef enum adi_adrv9001_PfirNumTaps
{
    ADI_ADRV9001_PFIR_32_TAPS = 0u,     /*!<  32 taps PFIR */
    ADI_ADRV9001_PFIR_64_TAPS = 1u,     /*!<  64 taps PFIR */
    ADI_ADRV9001_PFIR_96_TAPS = 2u,     /*!<  96 taps PFIR */
    ADI_ADRV9001_PFIR_128_TAPS = 3u,    /*!< 128 taps PFIR */
    ADI_ADRV9001_PFIR_TAPS_MAX_ID = 3u, /*!< PFIR taps max ID */
} adi_adrv9001_PfirNumTaps_e;

/**
 *  \brief Enumerations for PFIR gain
 */
typedef enum adi_adrv9001_PfirGain
{
    ADI_ADRV9001_PFIR_GAIN_NEG_12_DB = 0u,   /*!< -12dB */
    ADI_ADRV9001_PFIR_GAIN_NEG_6_DB = 1u,    /*!< - 6dB */
    ADI_ADRV9001_PFIR_GAIN_ZERO_DB = 2u,     /*!<   0dB */
    ADI_ADRV9001_PFIR_GAIN_POS_6_DB = 3u,    /*!< + 6dB */
    ADI_ADRV9001_PFIR_GAIN_POS_9_54_DB = 4u, /*!< + 9.54dB */
    ADI_ADRV9001_PFIR_GAIN_POS_12_DB = 5u,   /*!< +12dB */
    ADI_ADRV9001_PFIR_GAIN_POS_14_DB = 6u,   /*!< +14dB */
    ADI_ADRV9001_PFIR_GAIN_POS_20_DB = 7u,   /*!< +20dB */
    ADI_ADRV9001_RX_MAX = 7u,

    ADI_ADRV9001_PFIR_GAIN_PLUS_24DB = 8u, /*!< +24dB */
    ADI_ADRV9001_PFIR_GAIN_PLUS_26DB = 9u, /*!< +26dB */
    ADI_ADRV9001_PFIR_GAIN_MAX = 9u,
} adi_adrv9001_PfirGain_e;

/**
* \brief Data structure to hold WB/NB compensation PFIR structure
*/
typedef struct adi_adrv9001_PfirWbNbBuffer
{
    uint8_t numCoeff;                                             /*!< number of coefficients */
    adi_adrv9001_PfirSymmetric_e symmetricSel;                    /*!< symmetric selection */
    adi_adrv9001_PfirNumTaps_e tapsSel;                           /*!< taps selection */
    adi_adrv9001_PfirGain_e gainSel;                              /*!< gain selection */
    int32_t coefficients[ADI_ADRV9001_WB_NB_PFIR_COEFS_MAX_SIZE]; /*!< coefficients */
} adi_adrv9001_PfirWbNbBuffer_t;

/**
* \brief Data structure to hold RX NB pulse shaping RFIR Buffer structure
*/
typedef struct adi_adrv9001_PfirPulseBuffer_t
{
    uint8_t numCoeff;                                             /*!< number of coefficients */
    adi_adrv9001_PfirSymmetric_e symmetricSel;                    /*!< symmetric selection */
    uint8_t taps;                                                 /*!< taps in number */
    adi_adrv9001_PfirGain_e gainSel;                              /*!< gain selection */
    int32_t coefficients[ADI_ADRV9001_WB_NB_PFIR_COEFS_MAX_SIZE]; /*!< coefficients   */
} adi_adrv9001_PfirPulseBuffer_t;

/**
* \brief Data structure to hold RX Low/High TIA Bandwidth HP/LP ADC PFIR Buffer structure
*/
typedef struct adi_adrv9001_PfirMag21Buffer
{
    uint8_t numCoeff;
    int32_t coefficients[ADI_ADRV9001_MAG_COMP_PFIR_COEFS_MAX_SIZE];
} adi_adrv9001_PfirMag21Buffer_t;

/**
* \brief Data structure to hold TX/RX Magnitude Compensation PFIR for NB
*/
typedef struct
{
    uint8_t numCoeff;
    int32_t coefficients[ADI_ADRV9001_MAG_COMP_NB_PFIR_COEFS_MAX_SIZE];
} adi_adrv9001_PfirMag13Buffer_t;

/*! PFIR coefficent buffer */
typedef struct adi_adrv9001_PfirBuffer
{
    /*!< During dynamic profile switching, the first two types of PFIRs, RXWbNb and TXWbNbPulShp, would be reloaded for each
     *   profile before the switch */
    /*!< RX WB/NB Compensation PFIR (Channel Filter) coefficient Bank A/B/C/D  in rxnb_dem, block30 */
    adi_adrv9001_PfirWbNbBuffer_t pfirRxWbNbChFilterCoeff_A;
    adi_adrv9001_PfirWbNbBuffer_t pfirRxWbNbChFilterCoeff_B;
    adi_adrv9001_PfirWbNbBuffer_t pfirRxWbNbChFilterCoeff_C;
    adi_adrv9001_PfirWbNbBuffer_t pfirRxWbNbChFilterCoeff_D;

    /*!< TX WB/NB Preprocessing pulse shaping PFIR coefficient Bank A/B/C/D */
    adi_adrv9001_PfirWbNbBuffer_t pfirTxWbNbPulShpCoeff_A;
    adi_adrv9001_PfirWbNbBuffer_t pfirTxWbNbPulShpCoeff_B;
    adi_adrv9001_PfirWbNbBuffer_t pfirTxWbNbPulShpCoeff_C;
    adi_adrv9001_PfirWbNbBuffer_t pfirTxWbNbPulShpCoeff_D;

    /*!< RX NB Pulse Shaping pFIR  128 taps CH1/2 in rxnb_dem */
    adi_adrv9001_PfirPulseBuffer_t pfirRxNbPulShp[ADI_ADRV9001_MAX_RX_ONLY];

    /*!< Channel 1/2 Low/High TIA Bandwidth HP/LP ADC */
    adi_adrv9001_PfirMag21Buffer_t pfirRxMagLowTiaLowSRHp[ADI_ADRV9001_MAX_RX_ONLY];
    adi_adrv9001_PfirMag21Buffer_t pfirRxMagLowTiaHighSRHp[ADI_ADRV9001_MAX_RX_ONLY];
    adi_adrv9001_PfirMag21Buffer_t pfirRxMagHighTiaHighSRHp[ADI_ADRV9001_MAX_RX_ONLY];
    adi_adrv9001_PfirMag21Buffer_t pfirRxMagLowTiaLowSRLp[ADI_ADRV9001_MAX_RX_ONLY];
    adi_adrv9001_PfirMag21Buffer_t pfirRxMagLowTiaHighSRLp[ADI_ADRV9001_MAX_RX_ONLY];
    adi_adrv9001_PfirMag21Buffer_t pfirRxMagHighTiaHighSRLp[ADI_ADRV9001_MAX_RX_ONLY];

    /*!< TX Magnitude Compensation PFIR 21 taps */
    adi_adrv9001_PfirMag21Buffer_t pfirTxMagComp1;
    adi_adrv9001_PfirMag21Buffer_t pfirTxMagComp2;

    /*!< TX/RX Magnitude Compensation PFIR for NB */
    adi_adrv9001_PfirMag13Buffer_t pfirTxMagCompNb[ADI_ADRV9001_MAX_TXCHANNELS];
    adi_adrv9001_PfirMag13Buffer_t pfirRxMagCompNb[ADI_ADRV9001_MAX_RX_ONLY];
} adi_adrv9001_PfirBuffer_t;

/**
 *  \brief Enumerations of PFIR bank selection
 */
typedef enum adi_adrv9001_PfirBank
{
    ADI_ADRV9001_PFIR_BANK_A = 0u, /*!< PFIR bank A */
    ADI_ADRV9001_PFIR_BANK_B = 1u, /*!< PFIR bank B */
    ADI_ADRV9001_PFIR_BANK_C = 2u, /*!< PFIR bank C */
    ADI_ADRV9001_PFIR_BANK_D = 3u, /*!< PFIR bank D */
} adi_adrv9001_PfirBank_e;

/**
 * \brief Data structure to hold PFI coefficients
 */
typedef struct adi_adrv9001_PfirCoeff
{
    uint32_t numCoeff; /*!< Number of Coefficients */
} adi_adrv9001_PfirCoeff_t;

/* TODO: With new PFIR structure change, this structure is not used anywhere now.
         But according to Jim Bush, this structure will be used in dynamic profile switching */
/**
 * \brief Data structure to hold PfirWbNbConfig
 */
typedef struct adi_adrv9001_PfirWbNbConfig
{
    adi_adrv9001_PfirBank_e      bankSel;		/*!< bank: PFIR_BANK_A = 0u (bank A), PFIR_BANK_B = 1u (bank B),
                                                           PFIR_BANK_C = 2u (bank C), PFIR_BANK_D = 3u (bank D) */
    adi_adrv9001_PfirSymmetric_e symmetricSel;	/*!< PFIR coefficient symmetricity */
    adi_adrv9001_PfirNumTaps_e   tapsSel;		/*!< Number of taps */
    adi_adrv9001_PfirGain_e      gainSel;		/*!< gain in dB */
    adi_adrv9001_PfirCoeff_t     coeff;         /*!< PFIR coefficients */
} adi_adrv9001_PfirWbNbConfig_t;

#endif /* _ADI_ADRV9001_PFIRBUFFER_TYPES_H_ */
