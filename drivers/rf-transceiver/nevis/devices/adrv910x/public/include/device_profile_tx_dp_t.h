/*!
 * @brief   TX datapath data types.
 *
 * @details This file provides data type definitions for TX datapath.
 *
 */
/*******************************************************************************
  Copyright(c) 2017-2018 Analog Devices, Inc. All Rights Reserved. This software
  is proprietary & confidential to Analog Devices, Inc. and its licensors. By
  using this software you agree to the terms of the associated Analog Devices
  License Agreement.
 ******************************************************************************/
#ifndef __DEVICE_PROFILE_TX_DP_T_HEADER__
#define __DEVICE_PROFILE_TX_DP_T_HEADER__

#include "adi_device_profile_pack.h"
#include "device_profile_nco_drv_t.h"
#include "device_profile_pfir_wbnb_t.h"
#include "device_profile_channels_t.h"

/*!
 *  @addtogroup tx_datapath
 *  @ingroup drivers
 *  @{
 */

/*! enumerations of the functional interpolation blocks definition */
typedef enum  {
    TX_DP_PREPROC_I_ONLY_BYPASSED        = 0u,     /*!< Selects mode0 for the Tx PreProc, I only mode, PFIR bypass     */
    TX_DP_PREPROC_I_Q                    = 1u,     /*!< Selects mode1 for the Tx PreProc, I and Q PFIR mode            */
    TX_DP_PREPROC_I_ONLY_I_PFIR          = 2u,     /*!< Selects mode2 for the Tx PreProc, I PFIR only mode             */
    TX_DP_PREPROC_I_ONLY_I_PFIR_Q_PFIR   = 3u,     /*!< Selects mode3 for the Tx PreProc, cascaded I and Q PFIR mode   */
    TX_DP_PREPROC_I_ONLY_BYPASSED_PFIR_ENABLED        = 4u,     
    TX_DP_PREPROC_I_Q_PFIR_ENABLED                    = 5u,     
    TX_DP_PREPROC_I_ONLY_I_PFIR_PFIR_ENABLED          = 6u,     
    TX_DP_PREPROC_I_ONLY_I_PFIR_Q_PFIR_PFIR_ENABLED   = 7u,     
} ADI_NEVIS_PACK_ENUM txDpPreProcMode_e;

/*! enumerations of the symbol mapper mode definition */
typedef enum  {
    TX_DP_SYM_MAPPER_DISABLE        = 0u,     /*!< Disable Symbol Mapper    */
    TX_DP_SYM_MAPPER_QPSK           = 1u,     /*!< QPSK Mode            */
    TX_DP_SYM_MAPPER_CQPSK          = 2u,     /*!< CQPSK Mode             */
    TX_DP_SYM_MAPPER_FSK            = 3u,     /*!< FSK Mode   */
} ADI_NEVIS_PACK_ENUM txDpSymbleMapperMode_e;

   
/*! bit mask for PFIR enable flag */
#define BITM_TX_DP_PREPROC_MODE_PFIR_ENABLE 0x4

/*! enumerations of the functional interpolation blocks definition */
typedef enum  {
    TX_DP_IQDMDUC_MODE0   = 0u,     /*!< Selects mode0 for the Tx IqDmDuc, bypass IQ in IQ out  */
    TX_DP_IQDMDUC_MODE1   = 1u,     /*!< Selects mode1 for the Tx IqDmDuc, IQ up-converter DM   */
    TX_DP_IQDMDUC_MODE2   = 2u,     /*!< Selects mode2 for the Tx IqDmDuc, DUC                  */
} ADI_NEVIS_PACK_ENUM txDpIqDmDucMode_e;

/*! Contains MUX and other settings for pre-processor */
ADI_NEVIS_PACK_START
typedef struct{

    /* tx preprocessor symbol map */
    int32_t     symMapSymbol0;               /*!< TX Preprocessor symbol mapping symbol 0, in S3.14 18-bit format */
    int32_t     symMapSymbol1;               /*!< TX Preprocessor symbol mapping symbol 1, in S3.14 18-bit format */
    int32_t     symMapSymbol2;               /*!< TX Preprocessor symbol mapping symbol 2, in S3.14 18-bit format */
    int32_t     symMapSymbol3;               /*!< TX Preprocessor symbol mapping symbol 3, in S3.14 18-bit format */
    
    uint8_t     symMapDivFactor;             /*!< TX Preprocessor symbol mapping division factor. Valid factors are 1, 2, 3, 4, 5 10, 16 and 20. 
                                                  This parameter also enables/disables symbol mapping, i.e. when the ratio is x4, x5, x10, x16 or x20,
                                                  then symbol mapping is enabled. */
    uint8_t     symMapQVal;                  /*!< Q symbol mapping value */
    
    /* Tx pre-processor config parameters that support four modes of operation */
    txDpPreProcMode_e preProcMode;           /*!< TX Preprocessor mode selection */
    
    txDpSymbleMapperMode_e symMapperMode;    /*!< Symbol mapper mode selection */
    
    pfirBank_e txPreProcWbNbPfirIBank;     	/*/< Block #3, TPFIR_I, Dynamic, configurable during dynamic profile switching */
    pfirBank_e txPreProcWbNbPfirQBank;		/*/< Block #5, TPFIR_Q, Dynamic, configurable during dynamic profile switching */
    uint8_t    padding[2u];

} txPreProc_t;
ADI_NEVIS_PACK_FINISH

/*! Contains interpolation settings for txnb_int_top (aka TXnb1_int_top) block */
ADI_NEVIS_PACK_START
typedef struct{
    uint8_t interpBy2Blk20En;  /*!< Setting for block #20. INT2_6, interpolation by 2 */
    uint8_t interpBy2Blk18En;  /*!< Setting for block #18. INT2_5, interpolation by 2 */
    uint8_t interpBy2Blk16En;  /*!< Setting for block #16. INT2_4, interpolation by 2 */
    uint8_t interpBy2Blk14En;  /*!< Setting for block #14. INT2_3, interpolation by 2 */
    uint8_t interpBy2Blk12En;  /*!< Setting for block #12. INT2_2, interpolation by 2 */
    uint8_t interpBy3Blk10En;  /*!< Setting for block #10. INT3,   interpolation by 3 */
    uint8_t interpBy2Blk8En;   /*!< Setting for block #8.  INT2_1, interpolation by 2 */

    uint8_t scicBlk32En;          /*!< Enable/Disable block #32. TSCIC */
    uint8_t scicBlk32DivFactor;   /*!< TSCIC block #32 division factor, 2-10 */
    
    uint8_t padding[3u];

} txNbIntTop_t;
ADI_NEVIS_PACK_FINISH

/*! Contains interpolation settings for NEW NB txnb_int_top (TXnb2_int_top) block */
ADI_NEVIS_PACK_START
typedef struct{
    uint8_t interpBy5_2En;  /*!< Setting for INT5_2, interpolation by 5 */
    uint8_t interpBy5_1En;  /*!< Setting for INT5_1, interpolation by 5 */
    uint8_t interpBy3_1En;  /*!< Setting for INT3_1, interpolation by 3 */ // the DP @ visio doc says 3_1 but yoda says 3 (tx_nb_dp_int3_en) ?
    uint8_t interpBy2_6En;  /*!< Setting for INT2_6, interpolation by 2 */
    uint8_t interpBy2_5En;  /*!< Setting for INT2_5 interpolation by 2 */
    uint8_t interpBy2_4En;  /*!< Setting for INT2_4, interpolation by 2 */
    uint8_t interpBy2_3En;  /*!< Setting for INT2_3 interpolation by 2 */
    uint8_t interpBy2_2En;  /*!< Setting for INT2_2,   interpolation by 2 */
    uint8_t interpBy2_1En;   /*!< Setting for INT2_1, interpolation by 2 */

    uint8_t scicNbBlk32En;          /*!< Enable/Disable block #32. TSCIC */
    uint8_t scicNbBlk32DivFactor;   /*!< TSCIC block #32 division factor, 2-10 */
    
    uint8_t padding;

} txNbIntTopNew_t;
ADI_NEVIS_PACK_FINISH

/*! Contains interpolation settings for txwb_int_top (aka TXnb1_int_top) block */
ADI_NEVIS_PACK_START
typedef struct{
    uint8_t interpBy2Blk30En;     /*!< Setting for block #30. INT2_5, interpolation by 2 */
    uint8_t interpBy2Blk28En;     /*!< Setting for block #28. INT2_4, interpolation by 2 */
    uint8_t interpBy2Blk26En;     /*!< Setting for block #26. INT2_3, interpolation by 2 */
    uint8_t interpBy2Blk24En;     /*!< Setting for block #24. INT2_2, interpolation by 2 */
    uint8_t interpBy2Blk22En;     /*!< Setting for block #22. INT2_1, interpolation by 2 */
    uint8_t wbLpfBlk22p1En;       /*!< Setting for block #22.1 WBLPF, interpolation by 2 */
    
    uint8_t padding[2u];
} txWbIntTop_t;
ADI_NEVIS_PACK_FINISH

/*! Contains interpolation settings for tx_int_top block shared b/w WB and New NB */
ADI_NEVIS_PACK_START
typedef struct{
    uint8_t interpBy3Blk44p1En; /*!< Setting for block #44.1. INT3_50, interpolation by 3 */
    uint8_t sinc3Blk44En;       /*!< Setting for block #44. SINC3TX, interpolation by 3 */
    uint8_t sinc2Blk42En;       /*!< Setting for block #42. SINC2, interpolation by 2 */
    uint8_t interpBy3Blk40En;   /*!< WB Setting (txMode=1) for block #40. INT3, interpolation by 3 */
    uint8_t interpBy3Blk40p1En; /*!< NB Setting (txMode=0) for block #40.1. INT3, interpolation by 3 */    
    uint8_t interpBy2Blk38En;   /*!< Setting for block #38. INT2_2, interpolation by 2 */
    uint8_t interpBy2Blk36En;   /*!< Setting for block #36. INT2_1, interpolation by 2 */
    uint8_t interpBy2BlkEn;     /*!< Setting for block INT2, interpolation by 2, Only in NB(txmode=0) */
} txIntTop_t;
ADI_NEVIS_PACK_FINISH

/*! Contains freq_dev settings for tx_int_top block */
ADI_NEVIS_PACK_START
typedef struct{

    /* Frequency dev mapper. All parameters are STATIC, not configurable on the fly, what/who/when are these parameters determined */
    uint32_t rrc2Frac;         /*!< Frequency Deviation */
    uint32_t mpll;             /*!< Frequency fraction multiplier */
    uint32_t nchLsw;           /*!< Frequency offset. Contains lest significant word, i.e Bit[0:31] */
    uint8_t  nchMsb;           /*!< Frequency offset. Contains most significant 3 bits, i.e Bit[32:34] */
    uint8_t  freqDevMapEn;     /*!< Enable/disable block #47 Freq_Dev Mapper. */
    uint8_t  txRoundEn;        /*!< Enable/disable tx round block #46. */
    uint8_t  padding[1u];

} txIntTopFreqDevMap_t;
ADI_NEVIS_PACK_FINISH


/*! Contains settings for IQDM block */
ADI_NEVIS_PACK_START
typedef struct{

    /* Tx IQ DM DUC config parameters that support three modes of operation */
    txDpIqDmDucMode_e iqdmDucMode;           /*!< Select IQDM-DUC block mode */

    uint8_t  padding[3u];

    /* Parameters for TX_DP_IQDMDUC_MODE1 IqDm mode */
    uint32_t iqdmDev;                        /*!< IQDM frequency deviation factor */
    uint32_t iqdmDevOffset;                  /*!< IQDM frequency offset */
    uint32_t iqdmScalar;                     /*!< IQDM scalar */
    uint32_t iqdmThreshold;                  /*!< IQDM threshold */

    /* Parameters for IQDM DUC NCO */
    ncoDpConfig_t iqdmNco;                   /*!< IQDM NCO */
}txIqdmDuc_t;
ADI_NEVIS_PACK_FINISH


/*! Public TX Channel profile data structure. Visible from BBIC */
ADI_NEVIS_PACK_START
typedef struct {

    /*! pre_proc block settings */
    txPreProc_t      preProc;

    /*! wb_int_top block settings */
    txWbIntTop_t     wbIntTop;

    /*! nb_int_top block settings */
    txNbIntTop_t     nbIntTop;

    /*! int_top block settings */
    txIntTop_t       intTop;
    txIntTopFreqDevMap_t  intTopFreqDevMap;

    /*! iqdm_duc block settings */
    txIqdmDuc_t      iqdmDuc;

}txDpProfile_t;
ADI_NEVIS_PACK_FINISH

/*! Public TX Channel profile data structure. Visible from BBIC */
ADI_NEVIS_PACK_START
typedef struct {
    /*! pre_proc block settings */
    txPreProc_t      preProc;
    /*! new NB nb_int_top block settings */
    txNbIntTopNew_t     nbIntTopNew;  
    /*! int_top block settings */
    txIntTop_t       intTop;
    txIntTopFreqDevMap_t  intTopFreqDevMap;
    /*! iqdm_duc block settings */
    txIqdmDuc_t      iqdmDuc;
}txNbDpProfile_t;
ADI_NEVIS_PACK_FINISH

/*! Config for TX internal tone generation */
ADI_NEVIS_PACK_START
typedef struct {
    bool           isEnabled;  /* TX internal tone generation is enabled when set   */
    uint8_t        gaindB;     /* Amplitude of the tone. 0 = 0dB, 1 = 6dB, 2 = 12dB */
    int32_t        freqHz;     /* Frequency of the tone                             */
}txIntToneGenCfg_t;
ADI_NEVIS_PACK_FINISH

/*! @} */
#endif /* __DEVICE_PROFILE_TX_DP_T_HEADER__ */
