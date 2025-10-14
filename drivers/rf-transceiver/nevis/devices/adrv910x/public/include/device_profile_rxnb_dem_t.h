/*!
 * @brief:   RXNB Demodulation data types
 *
 * @details: This file provides data type definitions for RXNB demodulation
 */
/*******************************************************************************
  Copyright(c) 2017-2018 Analog Devices, Inc. All Rights Reserved. This software
  is proprietary & confidential to Analog Devices, Inc. and its licensors. By
  using this software you agree to the terms of the associated Analog Devices
  License Agreement.
 ******************************************************************************/

#ifndef __DEVICE_PROFILE_RXNB_DEM_T_H__
#define __DEVICE_PROFILE_RXNB_DEM_T_H__

#include "adi_device_profile_pack.h"
#include "device_profile_nco_drv_t.h"
#include "device_profile_pfir_wbnb_t.h"
//#include "si_errata_nav.h"

/**
 *  @addtogroup rxnb_dem
 *  @ingroup drivers
 *  @{
 */

/*! Enum for RX WbNb Compensation PFIR input select */
typedef enum
{
    RPFIRIN_MUX_INT_IN = 0u,
    RPFIRIN_MUX_EXT_IN = 1u,
} ADI_NEVIS_PACK_ENUM rpFirInMuxSel_e;

/*! Enum for RX dpinfifo input data select */
typedef enum
{
    DPINFIFOINPUTSEL_DP  = 0u,                      /*!< select data from datapath */
    DPINFIFOINPUTSEL_SPI = 1u,                      /*!< select test pattern from SPI  */
} ADI_NEVIS_PACK_ENUM dpInFifoInputSel_e;

/*! Enum for RX dpinfifo mode settings */
typedef enum
{
    DPINFIFOMODE_DETECTING = 0u,                    /*!< In detecting mode */
    DPINFIFOMODE_DETECTED  = 1u,                    /*!< In detected mode  */
} ADI_NEVIS_PACK_ENUM dpInFifoMode_e;

/*! Enum for rxout select settings */
typedef enum
{
    RXOUTSEL_IQ    = 0u,                            /*!< IQ out */
    RXOUTSEL_FMDEM = 1u,                            /*!< FM demodulation out */
    RXOUTSEL_PFIR  = 2u,                            /*!< PFIR out */
} ADI_NEVIS_PACK_ENUM rxOutSel_e;

/*! Enum for down sampling select settings*/
typedef enum 
{
    NO_DN_SAMPLE    = 0u,                        /*!< 00 : No down sampling */
    DN_SAMPLE_2 = 1u,                            /*!< Downsample by 2 */
    DN_SAMPLE_4  = 2u,                           /*!< Downsample by 4 */
} ADI_NEVIS_PACK_ENUM dnSampleSel_e;

/*! Enum for RX round output mode */
typedef enum
{
    RXROUNDMODE_IQ = 0u,                            /*!< IQ output */
    RXROUNDMODE_I  = 1u,                            /*!< I only output */
} ADI_NEVIS_PACK_ENUM rxRoundMode_e;

/*! Enum for rx output select */
typedef enum
{
    DPARMSEL_DP  = 0u,                              /*!< select datapath output */
    DPARMSEL_ARM = 1u,                              /*!< select arm output */
} ADI_NEVIS_PACK_ENUM dpArmSel_e;

typedef enum
{
    GSOUT_MUX_GS_OUTPUT = 0u,
    GSOUT_MUX_BYPASS    = 1u,
} ADI_NEVIS_PACK_ENUM gsOutMuxSel_e;


#define FD_MAX_FREQ_DISC_THRESHOLD 0xFFFFFFFFu

typedef enum 
{
    FD_DIFF_ARC = 0u,
    FD_ARC_DIFF = 1u,
    FD_PH_DIFF  = 2u,
} ADI_NEVIS_PACK_ENUM freqDiscriminatorMode_e;

/*! Enumeration for Freq discriminator selection*/
typedef enum
{
    FREQ_DISC = 0u,             /*!< The default frequency discriminator   */
    FREQ_DISC_DP2 = 1u,         /*!< The default frequency discriminator in DP2   */
    NUM_RX_FREQDISC = 2u,
} ADI_NEVIS_PACK_ENUM FreqDiscNum_e; //Initially TO BE REMOVED, but actually no

typedef enum
{
//    ONE_PIPE_DATA_DELAY, Invalid setting for FD
      
    ONE_SAMPLE_DIFFERENCE   = 1u,
    TWO_SAMPLE_DIFFERENCE = 2u,
    THREE_SAMPLE_DIFFERENCE  = 3u,
    FOUR_SAMPLE_DIFFERENCE  = 4u,
    FIVE_SAMPLE_DIFFERENCE   = 5u,
    SIX_SAMPLE_DIFFERENCE   = 8u,  /*!<  3LSBs: 0 = 6 sample difference... 4 = 10 sample difference */
    SEVEN_SAMPLE_DIFFERENCE = 9u,
    EIGHT_SAMPLE_DIFFERENCE  = 10u,    
    NINE_SAMPLE_DIFFERENCE  = 11u,
    TEN_SAMPLE_DIFFERENCE   = 12u,    
} ADI_NEVIS_PACK_ENUM freqDiscriminatorInSel_e;

/*! Enumeration for formatting selection*/
typedef enum
{
    DP1_IQ22,                   /*!< DP1: I:22, Q22  */
    DP2_IQ22,                   /*!< DP2: I:22, Q22  */
    DP1_IQ16,                   /*!< DP1: I:16, Q16  */
    DP2_IQ16,
} ADI_NEVIS_PACK_ENUM formatingSel_e;

/*! Enumeration for New NB DEMOD DP1/2 selection*/
typedef enum
{
    NB_DEM_DP1,                 /*!< DP1 on NB DEMOD */
    NB_DEM_DP2,                 /*!< DP2 on NB DEMOD */
    NB_DEM_NUM                  /*!< NUM of NB DEMOD paths */
} ADI_NEVIS_PACK_ENUM nbDemPathSel_e;

/*! dpinfifo configuration structure */
ADI_NEVIS_PACK_START
typedef struct
{
    dpInFifoInputSel_e dpInFifoTestDataSel;
    uint8_t            dpInFifoEn;                  /*!< dpinfifo enable */
    dpInFifoMode_e     dpInFifoMode;
    uint8_t            padding[1u];
    
} dpInFifoConfig_t;
ADI_NEVIS_PACK_FINISH

/*! rxnb nco structure */
ADI_NEVIS_PACK_START
typedef struct
{
    uint8_t            rxNbNcoEn;                   /*!< rxnb nco enable */
    uint8_t            padding[3u];
    ncoDpConfig_t      rxNbNcoConfig;
} rxNbNcoConfig_t;
ADI_NEVIS_PACK_FINISH

/*! rx WbNb compensation PFir structure */
ADI_NEVIS_PACK_START
typedef struct
{
    rpFirInMuxSel_e    rxWbNbCompPFirInMuxSel;
    uint8_t            rxWbNbCompPFirEn;            /*!< rx WbNb compensation PFir enable */
    pfirBank_e 		   rxWbNbCompPFirBankSel;
    uint8_t 		   padding[1u];
} rxWbNbCompPFir_t;
ADI_NEVIS_PACK_FINISH

/*! resampler structure */
ADI_NEVIS_PACK_START
typedef struct
{
    uint8_t            rxReSampEn;                  /*!< resampler enable */   
    uint8_t            padding[3u];    
    uint32_t           reSampPhaseI;               /*!< I channel resampler phase */
    uint32_t           reSampPhaseQ;               /*!< Q channel resampler phase */
} rxReSampConfig_t;
ADI_NEVIS_PACK_FINISH

/*! Enumeration for wide band path select */
typedef enum
{
    NB_PTH = 0,         /*!< Narrow band data path  */
    WB_PTH = 1,         /*!< Wide band data path  */
} ADI_NEVIS_PACK_ENUM wbPthSel_e;

/*! Enumeration for corr fifo delay enable*/
typedef enum
{
    DTH_DLY_EN = 0,                         /*!< Use Navassa data path delay enable for correlator*/  
    PS1_SPI_CORR_DLY_EN = 1,                /*!< PS1 SPI triggers delay enable */  
    PS2_SPI_CORR_DLY_EN = 2,                /*!< PS2 SPI triggers delay enable */  
    PS2_SCHE_CORR_DLY_EN = 3,               /*!< PS2 event scheduler triggers delay enable */  
} ADI_NEVIS_PACK_ENUM corrFifoDelayEn_e;

/*! RX1 rxnbdem structure */
ADI_NEVIS_PACK_START
typedef struct
{
    rxNbNcoConfig_t    rxNbNco;
    rxWbNbCompPFir_t   rxWbNbCompPFir;
    rxReSampConfig_t   reSamp;
    gsOutMuxSel_e      gsOutMuxSel;                 /*!< fic algorithm enable */
    dnSampleSel_e      dnSampleSel;
    rxOutSel_e         rxOutSel;
    rxRoundMode_e      rxRoundMode;
} rxnbDemConfig_t;
ADI_NEVIS_PACK_FINISH

/*! RXNB rxnbdem structure */
ADI_NEVIS_PACK_START
typedef struct
{
    wbPthSel_e         wbPthSel;                /*!< rxnb nbdemod may select wide band data path */
    formatingSel_e     dp3Sel;                  /*!< Format of DP3 DMA to PS2   */
    corrFifoDelayEn_e  corrFifoDelayEn;
    uint8_t            corrSelDp2En;            /*!< Correlator selects DP2  */
    dpInFifoConfig_t   dpInFifo;
    rxNbNcoConfig_t    rxNbNco;
    rxWbNbCompPFir_t   rxWbNbCompPFir[NB_DEM_NUM];          /*!< two instances on DP and DP2*/
    rxReSampConfig_t   reSamp[NB_DEM_NUM];
    gsOutMuxSel_e      gsOutMuxSel[NB_DEM_NUM];                 /*!< fic algorithm enable */
    dnSampleSel_e      dnSampleSel[NB_DEM_NUM];
    uint8_t            FDSampleDelay[NB_DEM_NUM];
    uint8_t            pulPfirDelay[NB_DEM_NUM];
    rxOutSel_e         rxOutSel[NB_DEM_NUM];
    rxRoundMode_e      rxRoundMode[NB_DEM_NUM];
    uint32_t           freqDeviation[NB_DEM_NUM];
    uint32_t           symbolRate[NB_DEM_NUM];
    dpArmSel_e         dpArmSel;
	uint8_t            dpEnabled[NB_DEM_NUM];
    uint8_t            padding[2];
} rxnbDemConfigNb_t;
ADI_NEVIS_PACK_FINISH

/**@}*/
#endif /* __DEVICE_PROFILE_RXNB_DEM_T_H__ */
