/*!
 * @file      device_profile_algo_dp_t.h
 *
 * @brief     data structure data type for algorithm data path
 *
 * @details   
 */
/*******************************************************************************
  Copyright(c) 2017-2018 Analog Devices, Inc. All Rights Reserved. This software
  is proprietary & confidential to Analog Devices, Inc. and its licensors. By
  using this software you agree to the terms of the associated Analog Devices
  License Agreement.
 ******************************************************************************/
   

#ifndef __DEVICE_PROFILE_ALGO_DP_T_HEADER__
#define __DEVICE_PROFILE_ALGO_DP_T_HEADER__

#include <stdint.h>
#include "adi_device_profile_pack.h"
#include "device_profile_nco_drv_t.h"
#include "device_profile_phase_sync_nco_t.h"
#include "device_profile_pfir_mag_t.h"
#include "device_profile_pfir_interp_t.h"
#include "device_profile_logen_t.h"

/**
 *  @addtogroup algo_datapath
 *  @ingroup drivers
 *  @{
 */

/*! Clock connection point enum */
typedef enum{
    CLK184M = 0u,   /* TC/WBDPDI_0 */
    CLK_RB_NB = 0u, /* Rx magComp/BBDC NB RB clk */
    CLK46_61M = 1u, /* TA/WBDPDI_2 */
    CLK2304K = 2u,  /* NBDPDI_0 / NBOBS_0*/
    CLK1152K = 3u,  /* NBDPDI_1 / NBOBS_1 */
    SERIAL_INF_CLK = 3u, 
    CLK576K = 4u,   /* NBDPDI_2 / NBOBS_2 */ 
    CLK1920K = 5u,  /* NBDPDI_0A */
    CLK960K = 6u,   /* NBDPDI_1A */ 
    MAX_NUM_CLK = 7u,
    IN_0 = 7u, 
} ADI_NEVIS_PACK_ENUM clkSel_e;

/*! Clock connection point enum for NB*/
typedef enum{
    CLK_NB_N3_TA = 0u,   /* N3(TA) */
    CLK_NB_TX0 = 1u, /* TX0 */
    CLK_NB_TX1 = 2u,
    CLK_NB_TX2 = 3u,
    CLK_NB_TX3 = 4u,
    CLK_NB_TX4 = 5u,
    CLK_NB_TX5 = 6u,
    CLK_NB_GRP11_TG = 0u,   /* Group 11 TG */
    CLK_NB_GRP11_N3_TA = 1u,   /* Group 11 N3(TA) */
    CLK_NB_GRP11_TX0 = 2u, /* Group 11 TX0 */
    CLK_NB_GRP11_TX1 = 3u,
    CLK_NB_GRP11_TX2 = 4u,
    CLK_NB_GRP11_TX3 = 5u,
    CLK_NB_GRP11_TX4 = 6u,
    CLK_NB_GRP11_TX5 = 7u,
} ADI_NEVIS_PACK_ENUM clkSelNb_e;

/*! Clock enum in KHz*/
typedef enum{
    CLK_184320,  
    CLK_61440,   
    CLK_46080,
    CLK_30720,
    CLK_2304,  
    CLK_1920,  
    CLK_1152,  
    CLK_960,   
    CLK_576,   
} ADI_NEVIS_PACK_ENUM clkRate_e;

/*Algo Enable/Disable defines */
typedef enum {
    ALGO_DP_DISABLE = 0,
    ALGO_DP_ENABLE,
} ADI_NEVIS_PACK_ENUM algoEnable_e; 

/*RX NB WB defines */
typedef enum {
    WB = 0,
    NB,    
} ADI_NEVIS_PACK_ENUM nbWbSel_e; 

typedef enum {
    RM_RN = 0,
    RH_RI,    
} ADI_NEVIS_PACK_ENUM gCompClk_e; 

typedef enum {
    SLICER_IN = 0,
    SLICER_OUT,    
} ADI_NEVIS_PACK_ENUM rssiSel_e; 

typedef enum {
    NB_DP1 = 0,
    NB_DP2,    
} ADI_NEVIS_PACK_ENUM demodRssiSel_e;

/*! RX data path conectivity for Gain Compensation Slicer block */

ADI_NEVIS_PACK_START
typedef struct {    
    algoEnable_e gainCompClkEnable;     /*!< Enable Gain Compensation clocks */
    gCompClk_e gainCompClkSel;          /*!< Gain Compensation clock selection: RM_RN / RH_RI */
    nbWbSel_e nbOrWb_gainCompIn;        /*!< NB/WB to select gain_comp_in_sel */
    nbWbSel_e nbOrWb_slicerIn;          /*!< NB/WB to select slicer_in_sel */
    algoEnable_e slicerRssiClkEnable;   /*!< Enable Slicer and RSSi clocks */
    rssiSel_e dcsgPmDsel;               /*!< Selects input/output of slicer to RSSI */
    demodRssiSel_e demodRssiSel;        /*!< Selects demod input to RSSi - RXNB only */
} rxAlgDpGainCompSlicer_t;
ADI_NEVIS_PACK_FINISH

/*! RX mux201 Selection */
typedef enum {
    MAG_COMP_BYPASS,
    MAG_COMP,    
} ADI_NEVIS_PACK_ENUM mux201Sel_e; 

/*! RX mux202 Selection */
typedef enum {
    BBDC_BYPASS,
    BBDC_COR,
} ADI_NEVIS_PACK_ENUM mux202Sel_e;

/*! RX mux501 Selection */
typedef enum {
    GAIN_COMP_DP,
    MAG_COMP_PFIR_21
} ADI_NEVIS_PACK_ENUM mux501Sel_e;

/*! RX mux502 Selection */
typedef enum {
    M501_OUTPUT,
    BBDC
} ADI_NEVIS_PACK_ENUM mux502Sel_e;

typedef enum {
    RC2_202B,
    RD_202B,
    RJ_202B,
} ADI_NEVIS_PACK_ENUM mux202bSel_e; 

typedef enum {
    NBFIC_BYPASS,
    NBFIC_205,    
} ADI_NEVIS_PACK_ENUM mux205Sel_e; 

typedef enum {
    QEC_CORR_BYPASS,
    QEC_CORR,    
} ADI_NEVIS_PACK_ENUM mux206Sel_e; 

typedef enum {
    M206_OUTPUT,
    RC2_207,    
} ADI_NEVIS_PACK_ENUM mux207Sel_e; 

typedef enum {
    BYPASS_9_1,
    RC2_9_1,    
} ADI_NEVIS_PACK_ENUM mux91Sel_e;

typedef enum {
    BYPASS_QEC_DDC,
    QEC_OUT,
    DDC_OUT,
} ADI_NEVIS_PACK_ENUM qecDdcSel_e; 

typedef enum {
    BYPASS_20_1,
    RXQEC_OUT,    
} ADI_NEVIS_PACK_ENUM mux20p1Sel_e;

typedef enum {
    RH_203,
    RY_203,    
} ADI_NEVIS_PACK_ENUM mux203Sel_e; 

typedef enum {
    M202B_OUT,
    M203_OUT,    
} ADI_NEVIS_PACK_ENUM mux204Sel_e; 

typedef enum {
    M207_OUTPUT,
    RC1_206,    
} ADI_NEVIS_PACK_ENUM mux208Sel_e; 

typedef enum {
    M202B_OUTUT,
    RZ_OUT,    
} ADI_NEVIS_PACK_ENUM mux209Sel_e; 

typedef enum {
    M205_OUT,
    RZ_OUTPUT,
} ADI_NEVIS_PACK_ENUM mux210Sel_e; 

typedef enum {
    M504SEL_M505_OUT,
    M504SEL_M502_OUT
} ADI_NEVIS_PACK_ENUM mux504Sel_e;

typedef enum {
    M505SEL_M502_OUT,
    NBFIC_OUTPUT
} ADI_NEVIS_PACK_ENUM mux505Sel_e;

typedef enum {
    M506SEL_M505_OUT,
    HD2_RXQEC_OUTPUT
} ADI_NEVIS_PACK_ENUM mux506Sel_e;

/*! RX data path conectivity for RX QEC block */
ADI_NEVIS_PACK_START
typedef struct {    
    mux202bSel_e        mux202b;                /*!< Selects RD, RJ or RC2 */
    mux205Sel_e         mux205;                 /*!< NBFIC or bypass */
    mux206Sel_e         mux206;                 /*!< RXQEC Correlator or bypass */
    mux207Sel_e         mux207;                 /*!< RXQEC or bypass */
    mux208Sel_e         mux208;                 /*!< 206 or 207 */
    mux209Sel_e         mux209;                 /*!< 202B or RZ */
    mux210Sel_e         mux210;                 /*!< 205 or RZ */
    mux505Sel_e         mux505;                 /*!< 502 or NBFIC */
    mux506Sel_e         mux506;                 /*!< 505 or HD2/RXQEC */
    algoEnable_e        rxqecClkEn;             /*!< Enable 184M to rxqec: bit field rx_alg_rxqec_en */
    mux91Sel_e          rxDdcDin1Sel;           /*!< mux 9.1 */
    qecDdcSel_e         mux102_rxqecMuxSel[NUM_LO_TYPES];     /*!< bypass/ddc/qec */
    mux20p1Sel_e        rxDdcDin2Sel;           /*!< mux 20.1 */
    qecDdcSel_e         mux101_wbagcMux[NUM_LO_TYPES];        /*!< bypass/ddc/qec */
    clkRate_e           sampleRate;             /*!< Sample rate RX QEC is connected to */
    int8_t              corrRdyGenOnTime;       /*!< (why signed int?) data gen ready clocks Iready on (sets clock rate of data into RXQEC block) */
    uint16_t            corrRdyGenOffTime;      /*!< data gen ready clocks Iready off (sets clock rate of data into RXQEC block) */
}rxAlgDpQec_t;
ADI_NEVIS_PACK_FINISH

/*! RX data path conectivity for RX QEC OBS block */
ADI_NEVIS_PACK_START
typedef struct {    
    mux203Sel_e         mux203;                 /*!< Selects Selects RD/RB which is the correction path or alternate path Mux */
    mux204Sel_e         mux204;                 /*!< Select which alternate path RH (RPFIR output) or RY (TIA Fine Tune at 30.72 MHz */    
    mux504Sel_e         mux504;                 /*!< 505 or 502 */
    
    int8_t              obsRdyGenOnTime;        /*!< (why signed int?) data gen ready clocks Iready on (sets clock rate of data into RXQEC block) */
    uint16_t            obsRdyGenOffTime;       /*!< data gen ready clocks Iready off (sets clock rate of data into RXQEC block) */
}rxAlgDpQecObs_t;
ADI_NEVIS_PACK_FINISH

/*! TX data path conectivity for TX DPD Actuator block */

/*! TX mux402 Selection */
typedef enum {
    FROM_DPD_ACT,
    FROM401,
    FROM406,
} ADI_NEVIS_PACK_ENUM mux402Sel_e; 

/*! TX mux402A Selection */
typedef enum {
    FROM_0,
    FROM_TB2,
    FROM_NBDPDI_0,
} ADI_NEVIS_PACK_ENUM mux402ASel_e; 

/*! TX mux402B Selection */
typedef enum {
    FROM_TC,
    FROM_TA,
    FROM402A_402B,
} ADI_NEVIS_PACK_ENUM mux402BSel_e; 

/*! TX mux403 Selection */
typedef enum {
    FROM_LOL,
    FROM402C,    
    FROM402A_403,
} ADI_NEVIS_PACK_ENUM mux403Sel_e; 

typedef enum {
    FROM_402,
    PN_A,
} ADI_NEVIS_PACK_ENUM mux401CSel_e; 

typedef enum {
    ADD_ZERO,
    ADD_PN,
} ADI_NEVIS_PACK_ENUM pnAdder_e; 


ADI_NEVIS_PACK_START
typedef struct {    
    clkSel_e mux401InputSel;     /* 0, 576k, 960k, 1.1M, 1.92M, 2.3M, 61.44M, 184M */
    clkSelNb_e mux401CNbInputSel; /*NB 401C input select */
    algoEnable_e clkGrp8Ena;
    mux402Sel_e mux402Sel;     /*  Two bits to enable 401 to 402 or DPD actuator to 402 */
    mux401CSel_e mux401CPnAdderBypass;  /* bypass PN adder A or not */
    pnAdder_e pnAdderAAddNonZero;  /* 0: adding zero, 1: adding PN */
} txAlgDpDpdActuator_t;
ADI_NEVIS_PACK_FINISH

typedef enum {
    TXLOL_PNGEN,
    CHEST_PNGEN,
} ADI_NEVIS_PACK_ENUM mux401BSel_e; 

ADI_NEVIS_PACK_START
typedef struct {    
    clkSel_e clkSel;     /* 0, 576k, 960k, 1.1M, 1.92M, 2.3M, 61.44M, 184M */
    clkSelNb_e clkNbSel;
    algoEnable_e clkGrp3Ena;    
    algoEnable_e clkGrp7Ena;  
    mux401BSel_e mux401bPnSel;      /* Selects PN Gen from in TXLOL or Ch Est */
} txAlgDpPnGen_t;
ADI_NEVIS_PACK_FINISH

typedef enum {
    SIGNAL_NCO1,
    SIGNAL_NCO2,
} ADI_NEVIS_PACK_ENUM mux406Sel_e; 

typedef enum {
    FROM407,
    FROM401B_TPN,
} ADI_NEVIS_PACK_ENUM mux417Sel_e;

typedef enum {
    FROM417_TO_416A,
    FROM401_TO_416A,
} ADI_NEVIS_PACK_ENUM mux416aSel_e;

typedef enum {
    FROM401C_TO_416B,
    FROM401_TO_416B,
} ADI_NEVIS_PACK_ENUM mux416bSel_e;

typedef enum {
    FROM414,
    TXLOL_ACTB_SEL,
} ADI_NEVIS_PACK_ENUM mux418Sel_e;

typedef enum {
    NCO_2_Q_SEL,
    NCO_2_I_SEL,
} ADI_NEVIS_PACK_ENUM mux406bSel_e;

typedef enum {
    TXBBF,
    SERIAL_INF,
    TXLOL,
} ADI_NEVIS_PACK_ENUM srcNcoMode_e; 

typedef enum {
    AMP_INC_0DB,
	AMP_INC_6DB,
	AMP_INC_12DB,
} ADI_NEVIS_PACK_ENUM ncoMcsAdjAmp_e;


/*! TX data path conectivity for TX DPD Actuator block */
ADI_NEVIS_PACK_START
typedef struct {    
    mux406Sel_e mux406Sel;     /* Selects NCO1/NCO */
    mux406bSel_e mux406bSel;   /* Selects NCO2 I or Q */
    ncoDpConfig_t srcNco1;
    ncoMcsAdjAmp_e nco1AdjAmp1;
    ncoMcsAdjAmp_e nco1AdjAmp2;
    bool gainDelaySel;
    bbfNcoConfig_t srcNco2;
    srcNcoMode_e  mode;
    clkSel_e clkSel;
    clkSelNb_e clkNbSel;
    txSrcNcoPhaseOffsetDc_t phaseOffsetDcConfig;
} txAlgDpDataSrcNco12_t;
ADI_NEVIS_PACK_FINISH

typedef enum {
    FROM_402A,
    PN_C,
} ADI_NEVIS_PACK_ENUM mux402cSel_e; 

typedef enum {
    FROM_403,
    TX_MAGCOMP_403B,
} ADI_NEVIS_PACK_ENUM mux403bSel_e; 

typedef enum {
    TB2_404,
    TB4_404,
} ADI_NEVIS_PACK_ENUM mux404Sel_e; 

typedef enum {
    M402C_407,  
    TB4_407,
} ADI_NEVIS_PACK_ENUM mux407Sel_e;

typedef enum {
    FROM_404_404B,
    TH_404B,
} ADI_NEVIS_PACK_ENUM mux404BSel_e;

typedef enum {
    FROM_404_410C,
    TG_410C
} ADI_NEVIS_PACK_ENUM mux410CSel_e;

typedef enum {
    TX_MAGCOMP_410B,
    TG_410B,
} ADI_NEVIS_PACK_ENUM mux410BSel_e; 

typedef enum {
    M410B_410,
    ATTEN_410,    
} ADI_NEVIS_PACK_ENUM mux410Sel_e; 

typedef enum {
    M410B_411,
    SLEWRATE_411,
} ADI_NEVIS_PACK_ENUM mux411Sel_e; 

typedef enum {
    ATTEN_412,
    SLEWRATE_412,
} ADI_NEVIS_PACK_ENUM mux412Sel_e; 

typedef enum {
    ATTEN_413,
    M411_413,
} ADI_NEVIS_PACK_ENUM mux413Sel_e; 

typedef enum {
    M412_414,
    CPLX_GAIN,
} ADI_NEVIS_PACK_ENUM mux414Sel_e; 

typedef enum {
    ATTEN_415,
    SLEWRATE_415,
    M410B_415
} ADI_NEVIS_PACK_ENUM mux415Sel_e; 

typedef enum {
    NCO_INTERP_PFIR_A,
    NCO_INTERP_PFIR_B,
    NCO_INTERP_PFIR_C,
} ADI_NEVIS_PACK_ENUM ncoInterp_e;

typedef enum {
    PFIR_SEL_I_HOP,
    PFIR_SEL_CURRENT,
    PFIR_SEL_NEXT,
} ADI_NEVIS_PACK_ENUM pfir24Hop_e;

/*! TX data path conectivity for TXQEC LOL NCO_PFIR block */
ADI_NEVIS_PACK_START
typedef struct {
    ncoInterp_e whichOne;
    algoEnable_e ncoEnable;     /* nco Enable a/b/c to enable NCO0/1/2 */
    phaseSyncNcoConfig_t ncoConfig;   /* NCO configuration for NCO0/1/2*/
    bool usePFir;    /* use_pfir_a/b/c = 1: PFir with NCO0 enable while phase Interp disable */
    bool ncoFirst;          /* Swap the order of NCO and phase Interp: 1: NCO first */
    bool bypassIntp;        /* bypass interperlator */
    bool turnOnIntp;        /* turns on interpolator on observation data which goes to the external tx qec module*/
    pfir24Hop_e pfirHopSel;         /* 0: use I_HOP, 1: use current pfir coeff, 2:use next*/
} txAlgDpTxCorrNcoPfir_t;
ADI_NEVIS_PACK_FINISH

typedef enum {
    FIFO_A_407C,
    FIFO_B_407C,    
} ADI_NEVIS_PACK_ENUM mux407cSel_e; 

typedef enum {
    NCO_B_408C,
    NCO_A_408C,
} ADI_NEVIS_PACK_ENUM mux408cSel_e; 

/*! TX data path conectivity for RSSI/Cross correlator CLGC block */
ADI_NEVIS_PACK_START
typedef struct {    
    algoEnable_e clkGrp2Ena;   /*Enable/Disable Group Clock 2 */
    clkSel_e clkSel; /* clock selection for group clock 2 */
    clkSelNb_e clkNbSel; /* clock selection for group clock 2 */
    mux407cSel_e mux407cFifoAB;   /* Select Fifo A/B */
    mux408cSel_e mux408cNcoAB;
} txAlgDpRssiCrCorrClgcSel_t;
ADI_NEVIS_PACK_FINISH

typedef enum {
    FIFO_A,
    FIFO_B
} ADI_NEVIS_PACK_ENUM fifoSel_e; 

typedef enum {
    NCO_A,
    NCO_B
} ADI_NEVIS_PACK_ENUM ncoSel_e; 

typedef enum {
    RXMAG_COMP_409B,
    RAQ409_BYPASS_RXMAG,
} ADI_NEVIS_PACK_ENUM mux409bSel_e;

typedef enum {
    MUX600_RL_NB,
    MUX600_RX0_NB,
    MUX600_RX1_NB,
    MUX600_RX2_NB,
    MUX600_RX3_NB,
    MUX600_RX4_NB,
    MUX600_RX5_NB
} ADI_NEVIS_PACK_ENUM mux600Sel_e;

ADI_NEVIS_PACK_START
typedef struct {
    algoEnable_e clkGrp3Ena;   /*Enable/Disable Group Clock 3 */
    algoEnable_e dpdSampleCaptureEn;
    algoEnable_e txPathDelayCorrEn;
    clkSel_e mux302RaqInputSel;
    fifoSel_e mux407bSel;
    ncoSel_e mux408bSel;
    mux409bSel_e mux409bRxMagCompFir13Sel;     /* Selects RX MagComp FIR13 output and enable clock */
    txAlgDpTxCorrNcoPfir_t  ncoPhIntPfirB;  /* Config of NCO1/Phase Interp/PFIR */
    mux416bSel_e mux416bSel;
    mux600Sel_e mux600Sel;
} txAlgDpPathDelay_t;
ADI_NEVIS_PACK_FINISH

typedef enum {
    BYPASS_TWIN_FIR,
    TWIN_FIR_405,
} ADI_NEVIS_PACK_ENUM mux405aSel_e; 

typedef enum {
    TWIN_FIR_409,
    RAR409,     
} ADI_NEVIS_PACK_ENUM mux409aSel_e; 

/*! TX data path conectivity for TXQEC/LOL correlator block */
ADI_NEVIS_PACK_START
typedef struct {    
    algoEnable_e clkGrp4Ena;   /*Enable/Disable Group Clock 4 */
    clkSel_e mux301RarInputSel;
    mux405aSel_e mux405aTwinFirSel;    /* Also enable the clock */
    fifoSel_e mux407aSel; 
    ncoSel_e mux408aSel; 
    mux409aSel_e mux409aTwinFirSel;     /*  Selects obs twin FIR output and enable the clock */
    txAlgDpTxCorrNcoPfir_t  ncoPhIntPfirA;  /* Config of NCO0/Phase Interp/PFIR */
    algoEnable_e bypassDespreader;     /* enable despreader or not*/
    mux416aSel_e mux416aSel;
    mux417Sel_e mux417Sel;
    mux600Sel_e mux600Sel;
} txAlgDpTxQecLolCorr_t;
ADI_NEVIS_PACK_FINISH

typedef enum {
    TB_NO_CONNECTION,
    TB_2304K,
} ADI_NEVIS_PACK_ENUM tbConnect_e; 

typedef enum {
    TB2_NO_CONNECTION,
    TB2_1152K,
    TB2_576K,
} ADI_NEVIS_PACK_ENUM tb2Connect_e; 

typedef enum {
    TB3_NO_CONNECTION,
    TB3_61440K_46080K,
} ADI_NEVIS_PACK_ENUM tb3Connect_e;

typedef enum {
    TB4_NO_CONNECTION,
    TB4_184320K,
} ADI_NEVIS_PACK_ENUM tb4Connect_e; 

typedef enum {
    TF_NO_CONNECTION,
    CAL_IN,
} ADI_NEVIS_PACK_ENUM tfConnect_e; 

typedef enum {
    TH_NO_CONNECTION,
    TX_ATTEN_IN,
} ADI_NEVIS_PACK_ENUM thConnect_e;

typedef enum {
    TB2_NB_NO_CONNECTION,
    TB2_NB_TX0,
    TB2_NB_TX1,
    TB2_NB_TX2,
    TB2_NB_TX3,
    TB2_NB_TX4,
    TB2_NB_TX5
} ADI_NEVIS_PACK_ENUM tb2NbConnect_e; 

/*! enumerations of the functional interpolation blocks definition */
typedef enum  {
    RX_DDC_MODE_BYPASS            = 0u,     /*!< Selects bypass mode for the RX DDC       */
    RX_DDC_MODE_REGULAR_DDCIN1    = 1u,     /*!< Selects regular NCO IN1 input mode for the RX DDC    */
    RX_DDC_MODE_REGULAR_DDCIN2    = 2u,     /*!< Selects regular NCO IN2 input mode for the RX DDC    */
    RX_DDC_MODE_PHASESYNC_DDCIN1  = 3u,     /*!< Selects phase sync NCO IN1 input mode for the RX DDC */
    RX_DDC_MODE_PHASESYNC_DDCIN2  = 4u,     /*!< Selects phase sync NCO IN2 input mode for the RX DDC */
} ADI_NEVIS_PACK_ENUM rxDdcMode_e;

ADI_NEVIS_PACK_START
typedef struct {
    tbConnect_e tbConnect;
    tb2Connect_e tb2Connect;
    tb3Connect_e tb3Connect;
    tb4Connect_e tb4Connect;
    tfConnect_e tfConnect;
    thConnect_e thConnect;
    tb2NbConnect_e tb2NbConnect;
}txAlgoConnect_t;
ADI_NEVIS_PACK_FINISH

/*! Contains settings for RX DDC block */
ADI_NEVIS_PACK_START
typedef struct{
    rxDdcMode_e           ddcMode;        /*!< RXDDC mode                     */
    uint8_t               selPosJwtOut;   /*!< select positive JWT output     */
    uint8_t               ddcScicEn;      /*!< Set to enable SCIC in RXDDC, block 103 and 104 */
    uint8_t               ddcScicDivFactor;  /*!< RXDDC SCIC division factor. This parameter records
                                                  the actual division factor in the hardware which
                                                  is 2-10. When writing to the SPI register, the 
                                                  value is mapped to 1-9, and the mapping is taken 
                                                  care of by the driver */
    uint8_t               ddcScicLowRippleEn;/*!< Set to enable low ripple mode. */
    uint8_t               padding[3u];	
    ncoDpConfig_t         ddcNco1;        /*!< regular nco                    */
    phaseSyncNcoConfig_t  ddcNco2;        /*!< phase sync nco                 */  
    bool                  phaseSync32BitMode;  /*!< Enable 32 bit mode for NCO2*/
} rxDdc_t;
ADI_NEVIS_PACK_FINISH

/**@}*/

#endif /* __DEVICE_PROFILE_ALGO_DP_T_HEADER__ */
