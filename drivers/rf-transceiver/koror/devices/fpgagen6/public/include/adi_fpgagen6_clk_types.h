/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_fpgagen6_clk_types.h
* \brief Contains FPGAGEN6 API clock structure definitions and macros
*
* FPGA API Version: 2.9.0.4
*
*/


#ifndef _ADI_FPGAGEN6_CLK_TYPES_H_
#define _ADI_FPGAGEN6_CLK_TYPES_H_

#include "adi_fpgagen6_data_interface_types.h"

#define ADI_FPGAGEN6_NUM_MMCM_OUTPUT_CLOCKS (7U)
#define ADI_FPGAGEN6_MMCM_LOCK_TIMEOUT (5U)
#define ADI_FPGAGEN6_PHY_NUM_CHANNELS (8U)
#define ADI_FPGAGEN6_QPLL0_MIN_KHZ (9800000U)
#define ADI_FPGAGEN6_QPLL0_MAX_KHZ (16375000U)
#define ADI_FPGAGEN6_QPLL1_MIN_KHZ (8000000U)
#define ADI_FPGAGEN6_QPLL1_MAX_KHZ (13000000U)
#define ADI_FPGAGEN6_PHY_QUAD_COUNT (2U)
#define ADI_FPGAGEN6_QPLL_DDC_MULTIPLIER (2U)
#define ADI_FPGAGEN6_QPLL_LOCK_TIMEOUT (5U)

typedef enum adi_fpgagen6_MmcmSel
{
    ADI_FPGAGEN6_CORE_MMCM,
    ADI_FPGAGEN6_TRANSPORT_MMCM
} adi_fpgagen6_MmcmSel_e;

typedef enum adi_fpgagen6_Qpll_Quad
{
    ADI_FPGAGEN6_QPLL_QUAD0 = 0x1U,
    ADI_FPGAGEN6_QPLL_QUAD1 = 0x2U,
    ADI_FPGAGEN6_QPLL_QUAD_ALL = 0x3U,
} adi_fpgagen6_Qpll_Quad_e;

/**
* \brief Enumerated list of clk sources available to the Serializer/deserializer 
*/
typedef enum adi_fpgagen6_QpllClkSel
{
    ADI_FPGAGEN6_QPLL_GTREFCLK0      = 0x1U,
    ADI_FPGAGEN6_QPLL_GTREFCLK1      = 0x2U,
    ADI_FPGAGEN6_QPLL_GTNORTHREFCLK0 = 0x3U,
    ADI_FPGAGEN6_QPLL_GTNORTHREFCLK1 = 0x4U,
    ADI_FPGAGEN6_QPLL_GTSOUTHREFCLK0 = 0x5U,
    ADI_FPGAGEN6_QPLL_GTSOUTHREFCLK1 = 0x6U,
    ADI_FPGAGEN6_QPLL_GTGREFCLK      = 0x7U
} adi_fpgagen6_QpllClkSel_e;

/**
*  \brief Data structure to hold the MMCM configuration settings
*/
typedef struct adi_fpgagen6_MmcmCfg
{
    uint8_t  mVal;                                                      /*! MMCM Multiplier. This value is generated based on the given inputs, do not modify */ 
    uint8_t  dVal;                                                      /*! MMCM Divider. This value is generated based on the given inputs, do not modify */ 
    uint8_t  outputClkDiv[ADI_FPGAGEN6_NUM_MMCM_OUTPUT_CLOCKS];         /*! MMCM Channel Dividers. This value is generated based on the given inputs, do not modify */ 
    uint32_t outputClkFreq_kHz[ADI_FPGAGEN6_NUM_MMCM_OUTPUT_CLOCKS];    /*! Desired Channel frequencies. This value must be populated with the desired value prior to adi_fpgagen6_MmcmSettingsConfigure */ 
} adi_fpgagen6_MmcmCfg_t;

/**
*  \brief Data structure to hold the qpll settings that are based on clock frequencies
*/
typedef struct adi_fpgagen6_QpllClockDependencies
{
    uint32_t                    qpllVco_kHz;        /*! Actual Vco frequency  */
    uint32_t                    qpllVcoOut_kHz;     /*! Frequency seen at output of Qpll, ie after the divider if applicable, see qpllOutrateDivEn  */
    adi_fpgagen6_LaneClkSel_e   laneClkSel;         /*! GTX QPLL upper or lower band VCO select,  */
    uint32_t                    laneRate_kHz;       /*! The lane rate used to calculate these settings. Used for writing DRP registers. */
    uint16_t                    mVal;               /*! reference clock divisor value,  */
    uint16_t                    nVal;               /*! loop divisor value,  */
    uint16_t                    qpllClkOutRate;     /*! 0: Out freq = Vco / 2; 1: Out freq = Vco */
} adi_fpgagen6_QpllClockDependencies_t;

/**
*  \brief Data structure to hold the MMCM configuration settings
*/
typedef struct adi_fpgagen6_QpllCfg
{
    uint32_t                                refClk_kHz;                                 /*! The recClk used to calculate these settings. Used for writing DRP registers.  */
    adi_fpgagen6_QpllClockDependencies_t    qpllClockSettings;                          /*! Holds the settings derived from clock inputs in adi_fpgagen6_QpllSettingsConfigure */
    adi_fpgagen6_QpllClkSel_e               refClkSelect[ADI_FPGAGEN6_PHY_QUAD_COUNT];  /*! GTX QPLL Ref Clk select per quad, ie both Qplls in the quad get this refclk */
} adi_fpgagen6_QpllCfg_t;


#endif  /* _ADI_FPGAGEN6_CLK_TYPES_H_ */
