/**
 * \file
 * \brief Contains ADRV9001 init related private types for
 *        adrv9001_init.c 
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2015 - 2018 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADRV9001_INIT_TYPES_H_
#define _ADRV9001_INIT_TYPES_H_

#include "adi_adrv9001.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADRV9001_INITIAL_BRING_UP    1
    
#define ADRV9001_PLL_NUM_BANDS     (53u)
#define ADRV9001_CLK_PLL_MODULUS   (8388593u)
#define ADRV9001_VCO_INIT_DEL      (1.0e-6f)    /* VCO calibration initial delay */
#define ADRV9001_VCO_CAL_WAIT      (100.0e-9f)  /* VCO CAL init wait */
#define ADRV9001_VCO_ALC_WAIT      (0.5e-6f)    /* VCO ALC (Auto Level Control) wait */
#define ADRV9001_VCO_CAL_CLK       (40e6f)      /* VCO Cal Clock */

#define ADRV9001_TEMP_SENSOR_DFL_OFFSET_CODE_VALUE (16u)   /* Default RTL offset code value */
#define ADRV9001_KELVIN_TO_CELSIUS (273u)       /* Kelvin to Celsius conversion */

#define ADRV9001_TCIDAC_125C       (328)
#define ADRV9001_TCIDAC_MAX        (4095)

/* Enumation for Coarse and Fine calibrations */
typedef enum adrv9001_PllVcoCalType
{
    PLL_COARSE_FINE_CAL, /*!< VCO Coarse/Fine Cal  */
    PLL_COARSE_CAL       /*!< VCO Coarse Cal    */    
} adrv9001_PllVcoCalType_e;

typedef enum adrv9001_LfPllMode 
{
    PLL_NORM_MODE = 0x00, /*!< Loopfilter PLL setting In slow mode */
    PLL_FAST_MODE = 0x01  /*!< Loopfilter PLL setting In Fast mode  */
} adrv9001_LfPllMode_e;

typedef enum adrv9001_PhaseTrackMode
{
    PHSYNC_OFF              = 0x00,     /*!< Phase tracking disabled */
    PHSYNC_INIT_ONLY        = 0x01,     /*!< Do only 1 phase init */
    PHSYNC_CONTINUOUS_TRACK = 0x02      /*!< Do only 1 phase init and continuous track update */
} adrv9001_PhaseTrackMode_e;
    
typedef struct adrv9001_TempCoef 
{
    uint16_t    f1_MHz;
    uint16_t    f2_MHz;  
    uint8_t     biasRef;
    uint8_t     biasTcf;
    uint8_t     vcoVarTc;
} adrv9001_TempCoef_t;

typedef struct adrv9001_loopFilterResult 
{
    uint8_t      C1;  /*!< Loopfilter C1 value */
    uint8_t      C2;  /*!< Loopfilter C2 value */
    uint8_t      C3;  /*!< Loopfilter C3 value */
    uint8_t      R1;  /*!< Loopfilter R1 value */
    uint8_t      R3;  /*!< Loopfilter R3 value */
    uint8_t      ICP; /*!< Loopfilter ICP (I (current) Charge Pump) value */
    uint32_t     effectiveLoopBW; /*!< Loopfilter Effective Bandwidth from calculated R/Cs */
} adrv9001_loopFilterResult_t;

/* This structure contains the state of the PLL settings */
typedef struct adrv9001_PllSynthParam
{
    uint64_t      pllFreq_Hz;      /*!< PLL frequency (30MHz~6GHz) */
    uint64_t      vcoFreq_Hz;      /*!< VCO frequency in Hz */
    uint8_t       isPllInUse;      /*!< PLL in use? */
    uint8_t       isPathEnabled;   /*!< Path enabled? */
    uint8_t       refClkDiv;       /*!< Reference clock divider: 1, 2, 4 */
    uint8_t       k1ClkDiv;        /*!< Device clock divider: 2**n (n=0~6) */
    uint32_t      refClockFreq_Hz; /*!< Reference clock (<=307.2MHz) */
    uint8_t       loDiv;           /*!< LO frequency divider (2~1022, step 2) */
    uint8_t       vcoVaractor;     /*!< VCO varactor: vco_var */
    uint32_t      maxCnt;          /*!< Frequency calibration max counter: freq_cal_max_cnt */
    uint8_t       vcoInitDelay;    /*!< VCO calibration initial delay: vco_cal_init_del */
    uint8_t       vcoClkDiv;       /*!< VCO calibration ALC clock divider: vco_cal_alc_clk_div */
    uint8_t       alcWait;         /*!< VCO ALC (Auto Level Control) wait period: vco_cal_alc_wait */
    uint8_t       alcInitWait;     /*!< VCO ALC initial wait period: vco_cal_alc_init_wait */
    uint8_t       fractionalByte0; /*!< PLL fractional */
    uint8_t       fractionalByte1;
    uint8_t       fractionalByte2;
    uint8_t       integerByte0;    /*!< PLL integer */
    uint8_t       integerByte1;
    uint8_t       biasRef;         /*!< VCO bias reference DAC: vco_bias_ref */
    uint8_t       biasTcf;         /*!< VCO bias DAC temperature coefficient: vco_bias_tcf */
    uint8_t       vcoVarTc;        /*!< Number of varactors connected to the output of the VCO temp comp IDAC: vco_var_tc */
    uint8_t       quickFreqCalEn;  /*!< VCO/ALC calibration: quick_freq_cal_en */
    uint8_t       quickFreqCalThreshold; /*!< VCO/ALC calibration: quick_freq_cal_threshold */
    uint8_t       vcoCalMaxCntBandEn;    /*!< VCO/ALC calibration: vcocal_maxcntband_en */
    uint8_t       endVcoCalMaxCntEn;     /*!< VCO/ALC calibration: endvcocal_maxcnt_en */
    uint8_t       vcoFineCalEn;          /*!< VCO/ALC calibration: vco_fine_cal_en */
    uint16_t      temperature;           /*!< Pll temperature */

    uint8_t       bandIndex;             /*!< PLL band index (0~52) */
    uint8_t       feCapIndx;
    uint8_t       locmIndx;
    uint8_t       lohilow;
    uint8_t       toneChannel;           /*!< Tone channel (0,1, 0xFF) */
    uint8_t       isLfUserOverride;
    uint32_t      loopBW;
    uint32_t      phaseMargin;
    uint8_t       voutLvl;
    adrv9001_LfPllMode_e   mode;
    adrv9001_PhaseTrackMode_e    phaseSyncMode;
    adrv9001_loopFilterResult_t  loopFilter;
#if (ADRV9001_INITIAL_BRING_UP == 1)
    uint16_t      dbg_f_alc;
    uint8_t       dbg_f_fineband;
    uint8_t       dbg_f_coarseband;
    uint16_t      dbg_tcidac;
    uint8_t       dbg_f_calbits;
#endif 
} adrv9001_PllSynthParam_t;

/* This data structure holds the rc tune clock scaling parameters. */
typedef struct adrv9001_AdcTunerCalClk
{
    uint8_t  divRatio; /*!< Divide ratio */
    uint8_t  nScale;   /*!< Normalization scale */
} adrv9001_AdcTunerCalClk_t;
    
/* This is a data structure used to hold the capacitance and conductance scaling factors. */
typedef struct adrv9001_AdcTunerResult
{
    uint16_t  sc; /*!< capacitance scaling factor */
    uint16_t  sg; /*!< conductance scaling factor */
} adrv9001_AdcTunerResult_t;
    
#ifdef __cplusplus
}
#endif

#endif