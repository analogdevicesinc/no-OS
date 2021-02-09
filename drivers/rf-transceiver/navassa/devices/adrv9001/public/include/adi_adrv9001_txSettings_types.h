/**
 * \file
 * \brief Type definitions for ADRV9001 Tx settings
 * \copyright Analog Devices Inc. 2019. All rights reserved.
 * Released under the ADRV9001 API license, for more information see "LICENSE.txt" in the SDK
 */

#ifndef _ADI_ADRV9001_TXSETTINGS_TYPES_H_
#define _ADI_ADRV9001_TXSETTINGS_TYPES_H_

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

#include "adi_adrv9001_rxSettings_types.h"

/**
 * \brief Enumeration of the Tx signal type
 */
typedef enum adi_adrv9001_TxSignalType
{
    ADI_ADRV9001_TX_IQ            = 0,
    ADI_ADRV9001_TX_IQ_FM_FSK     = 1,
    ADI_ADRV9001_TX_DIRECT_FM_FSK = 2
} adi_adrv9001_TxSignalType_e;

/**
 * \brief Enumerations of the Tx Data Path pre-processor Mode
 */
typedef enum adi_adrv9001_TxDpPreProcMode
{
    ADI_ADRV9001_TX_DP_PREPROC_I_ONLY_DATA_BYPASS_PFIRS  = 0,  /*!< I-only data, bypass both PFIRs */
    ADI_ADRV9001_TX_DP_PREPROC_IQ_DATA_WITH_PFIRS        = 1,  /*!< I/Q data, insert both PFIRs */
    ADI_ADRV9001_TX_DP_PREPROC_I_ONLY_DATA_WITH_PFIR     = 2,  /*!< I-only data, insert I only PFIR */
    ADI_ADRV9001_TX_DP_PREPROC_I_ONLY_DATA_CASCADE_PFIRS = 3,  /*!< I-only data, cascade I and Q PFIRS (256-tap total) */
    ADI_ADRV9001_TX_DP_PREPROC_ENABLE_PFIRS              = 0x4 /*!< Mask bit to enable PFIRs */
} adi_adrv9001_TxDpPreProcMode_e;

/**
 * \brief Data structure to hold ADRV9001 TX pre-processor block
 */
typedef struct adi_adrv9001_TxPreProc
{
    /* tx pre-processor symbol map */
    uint32_t    txPreProcSymbol0;         /*!< TX Preprocessor symbol mapping symbol 0, in S3.14 18-bit format */
    uint32_t    txPreProcSymbol1;         /*!< TX Preprocessor symbol mapping symbol 1, in S3.14 18-bit format */
    uint32_t    txPreProcSymbol2;         /*!< TX Preprocessor symbol mapping symbol 2, in S3.14 18-bit format */
    uint32_t    txPreProcSymbol3;         /*!< TX Preprocessor symbol mapping symbol 3, in S3.14 18-bit format */
    uint8_t     txPreProcSymMapDivFactor; /*!< Division factor that controls the symbol mapping. 5 bit (supports 1, 2, 3, 4, 5, 19, 16, 20)*/

    /* Tx pre-processor config parameters that support four modes of operation */
    adi_adrv9001_TxDpPreProcMode_e txPreProcMode; /*!< TX_DP_PREPROC_MODE0 - bypass mode,
                                                       TX_DP_PREPROC_MODE1 - I and Q PFIR mode,
                                                       TX_DP_PREPROC_MODE2 - I PFIR only mode,
                                                       TX_DP_PREPROC_MODE3 - Cascaded I and Q PFIR mode
                                                       STATIC, not configurable on the fly, BBIC to determine based on usecase */

    adi_adrv9001_PfirBank_e txPreProcWbNbPfirIBankSel; /*!< Config of Preproc PFIR I: Block #3, TPFIR_I, Dynamic, configurable during dynamic profile switching */
    adi_adrv9001_PfirBank_e txPreProcWbNbPfirQBankSel; /*!< Config of Preproc PFIR Q: Block #5, TPFIR_Q, Dynamic, configurable during dynamic profile switching */
} adi_adrv9001_TxPreProc_t;

/**
 * \brief Data structure to hold ADRV9001 Tx Narrowband interpolation top block
 */
typedef struct adi_adrv9001_TxNbIntTop
{
    uint8_t txInterpBy2Blk20En;   /*!< Tx Interpolator2 Narrowband Top block 20. Bit field tx_dp_int2_20_en */
    uint8_t txInterpBy2Blk18En;   /*!< Tx Interpolator3 Narrowband Top block 18. Bit field tx_dp_int2_18_en */
    uint8_t txInterpBy2Blk16En;   /*!< Tx Interpolator2 Narrowband Top block 16. Bit field tx_dp_int2_16_en */
    uint8_t txInterpBy2Blk14En;   /*!< Tx Interpolator2 Narrowband Top block 14. Bit field tx_dp_int2_14_en */
    uint8_t txInterpBy2Blk12En;   /*!< Tx Interpolator2 Narrowband Top block 12. Bit field tx_dp_int2_12_en */
    uint8_t txInterpBy3Blk10En;   /*!< Tx Interpolator3 Narrowband Top block 10. Bit field tx_dp_int3_10_en */
    uint8_t txInterpBy2Blk8En;    /*!< Tx Interpolator2 Narrowband Top block 8. Bit field tx_dp_int2_8_en */

    uint8_t txScicBlk32En;        /*!< Tx TSCIC Narrowband Top block 32. Bit field tx_dp_tscic_32_en */
    uint8_t txScicBlk32DivFactor; /*!< Tx TSCIC Narrowband Top block 32. Bit field tx_dp_tscic_32_div_factor */
} adi_adrv9001_TxNbIntTop_t;

/**
 * \brief Data structure to hold ADRV9001 Tx Wideband interpolation top block
 */
typedef struct adi_adrv9001_TxWbIntTop
{
    uint8_t txInterpBy2Blk30En; /*!< Tx Interpolator2 Wideband Top block 30. Bit field tx_dp_int2_30_en */
    uint8_t txInterpBy2Blk28En; /*!< Tx Interpolator2 Wideband Top block 28. Bit field tx_dp_int2_28_en */
    uint8_t txInterpBy2Blk26En; /*!< Tx Interpolator2 Wideband Top block 26. Bit field tx_dp_int2_26_en */
    uint8_t txInterpBy2Blk24En; /*!< Tx Interpolator2 Wideband Top block 24. Bit field tx_dp_int2_24_en*/
    uint8_t txInterpBy2Blk22En; /*!< Tx Interpolator2 Wideband Top block 22. Bit field tx_dp_int2_22_en*/
    uint8_t txWbLpfBlk22p1En;   /*!< Tx Wideband LPF Wideband Top block 22.1. Bit field tx_dp_int2_22_1_en */
} adi_adrv9001_TxWbIntTop_t;

/**
 * \brief Data structure to hold ADRV9001 Tx interpolation top block
 */
typedef struct adi_adrv9001_TxIntTop
{
    uint8_t interpBy3Blk44p1En; /*!< Tx Interpolator3 Top block 44.1. Bit field tx_dp_int3_44_1_en */
    uint8_t sinc3Blk44En;       /*!< Tx Sinc3 Top block 44. Bit field tx_dp_sinc3_44_en*/
    uint8_t sinc2Blk42En;       /*!< Tx Sinc2 Top block 42. Bit field tx_dp_int2_42_en*/
    uint8_t interpBy3Blk40En;   /*!< Tx Interpolator3 Top block 40. Bit field tx_dp_int3_40_en*/
    uint8_t interpBy2Blk38En;   /*!< Tx Interpolator2 Top block 38. Bit field tx_dp_int2_38_en */
    uint8_t interpBy2Blk36En;   /*!< Tx Interpolator2 Top block 36. Bit field tx_dp_int2_36_en*/
} adi_adrv9001_TxIntTop_t;

/**
 * \brief Data structure to hold ADRV9001 TX Interpolation Top frequency device mapper block
 */
typedef struct adi_adrv9001_TxIntTopFreqDevMap
{
    /* Frequency dev mapper */
    uint32_t rrc2Frac;		/*!< Frequency Deviation */
    uint32_t mpll;			/*!< Frequency fraction multiplier */
    uint32_t nchLsw;		/*!< Frequency offset. Contains least significant word, i.e Bit[0:31] */
    uint8_t  nchMsb;		/*!< Frequency offset. Contains most significant 3 bits, i.e Bit[32:34] */
    uint8_t  freqDevMapEn;	/*!< Enable/disable block #47 Freq_Dev Mapper. */
    uint8_t  txRoundEn;		/*!< Enable/disable tx round block #46. */
} adi_adrv9001_TxIntTopFreqDevMap_t;

/**
 * \brief Enumerations of the Tx Data Path IQDMDUC Mode
 */
typedef enum adi_adrv9001_TxDpIqDmDucMode
{
    ADI_ADRV9001_TX_DP_IQDMDUC_MODE0 = 0u, /*!< Selects mode0 for the Tx IqDmDuc, bypass IQ in IQ out  */
    ADI_ADRV9001_TX_DP_IQDMDUC_MODE1 = 1u, /*!< Selects mode1 for the Tx IqDmDuc, IQ DM                */
    ADI_ADRV9001_TX_DP_IQDMDUC_MODE2 = 2u  /*!< Selects mode2 for the Tx IqDmDuc, DUC                  */
} adi_adrv9001_TxDpIqDmDucMode_e;

/**
 * \brief Data structure to hold ADRV9001 TX IQDM block
 */
typedef struct adi_adrv9001_TxIqdmDuc
{
    /* Tx IQ DM DUC config parameters that support three modes of operation */
    adi_adrv9001_TxDpIqDmDucMode_e iqdmDucMode; /*!< TX_DP_IQDMDUC_MODE0 - bypass IQ in IQ out, TX_DP_IQDMDUC_MODE1 - IQ DM, TX_DP_IQDMDUC_MODE2 - DUC */

    /* Parameters for TX_DP_IQDMDUC_MODE1 IqDm mode */
    uint32_t iqdmDev;       /*!< Parameters for TX_DP_IQDMDUC_MODE1 Iqdm mode */
    uint32_t iqdmDevOffset; /*!< Parameters for TX_DP_IQDMDUC_MODE1 Iqdm mode */
    uint32_t iqdmScalar;    /*!< Parameters for TX_DP_IQDMDUC_MODE1 Iqdm mode */
    uint32_t iqdmThreshold; /*!< Parameters for TX_DP_IQDMDUC_MODE1 Iqdm mode */

    adi_adrv9001_NcoDpConfig_t iqdmNco; /*!< Parameters for IQDM NCO */
} adi_adrv9001_TxIqdmDuc_t;

/**
 * \brief Data structure to hold ADRV9001 TX Channel profile data
 */
typedef struct adi_adrv9001_TxDpProfile
{
    adi_adrv9001_TxPreProc_t           txPreProc;          /*!< Tx pre processor block (Static Settings) */
    adi_adrv9001_TxWbIntTop_t          txWbIntTop;         /*!< txwb_int_top (Wideband Top) block interpolater configs (Dynamic Settings)*/
    adi_adrv9001_TxNbIntTop_t          txNbIntTop;         /*!< txnb_int_top (Narrowband Top) block interpolater configs (Dynamic Settings) */
    adi_adrv9001_TxIntTop_t            txIntTop;           /*!< tx_int_top (TOP) block interpolater configs (Dynamic Settings) */
    adi_adrv9001_TxIntTopFreqDevMap_t  txIntTopFreqDevMap; /*!< tx_int_top (TOP) block frequency dev map configs (Static Settings) */
    adi_adrv9001_TxIqdmDuc_t           txIqdmDuc;          /*!< tx_iqdm_duc block configs (Static Settings) */
} adi_adrv9001_TxDpProfile_t;

/**
 *  \brief Data structure to hold ADRV9001 Tx Profile Configuration settings
 */
typedef struct adi_adrv9001_TxProfile
{
    uint32_t primarySigBandwidth_Hz;              /*!< Tx primary signal BW in Hz*/
    uint32_t txInputRate_Hz;                      /*!< Tx input data rate in Hz */
    uint32_t txInterfaceSampleRate_Hz;			  /*!< TX sample rate at serial interface */
    int32_t  txOffsetLo_kHz;                      /*!< Frequency of LO relative to carrier */
    uint32_t validDataDelay;					  /*!< Valid data delay relative to TX Enable by 184MHz clock counter */
    uint32_t txBbf3dBCorner_kHz;                  /*!< Tx BBF 3dB corner in kHz - butterFilterBw */
    adi_adrv9001_TxSignalType_e  outputSignaling; /*!< Output to Analog signal type */
    uint8_t  txPdBiasCurrent;					  /*!< pre-distorter programmable bias current*/
    uint8_t  txPdGainEnable;					  /*!< TX Pre-distortion gain enable */

    uint32_t txPrePdRealPole_kHz;				  /*!< TX Pre-distortion pole */

    uint32_t txPostPdRealPole_kHz;				  /*!< Post-distorter (i.e. interstage) filter Fc  */
    adi_adrv9001_ComponentPowerLevel_e txBbfPower; /*!< Tx baseband filter power mode */
    uint8_t  txExtLoopBackType;					  /*!< 0: No external loopback connect,
                                                   *   1: loopback before PA,
                                                   *   2: loopback after PA. */
    uint8_t  txExtLoopBackForInitCal;			  /*!< 0: ext loop back should not be used for init cal */
    int16_t  txPeakLoopBackPower;                 /*!< Target RMS signal power at input to receiver and its peak-to-average ratio, -40 to +5 dBm */
    uint32_t frequencyDeviation_Hz;               /*!< frequency deviation value in Hz for both FM_IQ and FM_DM.*/
    adi_adrv9001_TxDpProfile_t txDpProfile;       /*!< TX digital data path config */
    adi_adrv9001_SsiConfig_t   txSsiConfig;       /*!< TX Serial data interface config */
} adi_adrv9001_TxProfile_t;

/**
 *  \brief Data structure to hold ADRV9001 Tx Channel configuration settings
 */
typedef struct adi_adrv9001_TxSettings
{
    uint32_t txInitChannelMask;                                            /*!< Tx channel mask of which channels to initialize */
    adi_adrv9001_TxProfile_t txProfile[ADI_ADRV9001_MAX_TXCHANNELS]; /*!< Tx settings per Tx channel */
} adi_adrv9001_TxSettings_t;

#endif /* _ADI_ADRV9001_TXSETTINGS_TYPES_H_ */
