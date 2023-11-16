/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adi_fpgagen6_utilities_types.h
 * \brief Contains FPGA utilities enum and struct definitions
 *
 * FPGA API Version: 2.9.0.4
 */

#ifndef _ADI_FPGAGEN6_UTILITIES_TYPES_H_
#define _ADI_FPGAGEN6_UTILITIES_TYPES_H_

#include "adi_fpgagen6_data_interface_types.h"

#define ADI_FPGAGEN6_NUM_SAMPLE_XBAR                64U         /*!< Max number of converters to look at when deciding interleave mode */
#define ADI_FPGAGEN6_NUM_SAMPLE_XBAR_LS             32U         /*!< Max number of converters to look at when deciding interleave mode for link sharing configs */
#define ADI_FPGAGEN6_SAMPLE_XBAR_INVALID            128U        /*!< Invalid converter value */

typedef struct adi_fpgagen6_CommonLinkCfg
{
    uint32_t                iqRate_kHz;
    uint8_t                 lanesEnabled;
    uint8_t                 scramblingEn;                                       /*!< Scrambling Enable */
    uint16_t                jesd204K;                                           /*!< K */
    uint8_t                 jesd204E;                                           /*!< E */
    uint8_t                 jesd204M;                                           /*!< M */
    uint8_t                 jesd204S;                                           /*!< S */
    uint8_t                 jesd204Np;                                          /*!< Np */
    uint8_t                 jesd204F;                                           /*!< F */
    uint8_t                 jesd204N;                                           /*!< N */
    uint8_t                 jesd204Cs;                                          /*!< Cs */
    uint8_t                 jesdDid;                                            /*!< Device ID */
    uint8_t                 jesdBid;                                            /*!< Bank ID */
    uint8_t                 jesdLid[ADI_FPGAGEN6_LANES_PHYSICAL_NUM];           /*!< Lane ID */
    uint8_t                 jesdSampleXBar[ADI_FPGAGEN6_NUM_SAMPLE_XBAR];         /*!< Lane XBar */
    
    /* Link sharing parameters. This is only valid for Deframers 0 & 1 */
    uint8_t                 linkSharingEn;
    uint8_t                 linkSharingNp;
    uint16_t                linkSharingK;
    uint8_t                 linkSharingS;
    uint8_t                 linkSharingM;
    uint8_t                 linkSharingF;
    uint8_t                 linkSharingSampleXBar[ADI_FPGAGEN6_NUM_SAMPLE_XBAR_LS];
    
    uint8_t                 deviceLaneXbar[ADI_FPGAGEN6_LANES_PHYSICAL_NUM];    /*!< When a bit is set to 1, the corresponding serdes lane is used for that link */
} adi_fpgagen6_CommonLinkCfg_t;

typedef struct adi_fpgagen6_DeframerDataLinkCfg
{
    adi_fpgagen6_CommonLinkCfg_t    dataLink;                                       /*!<  Common Link Config Data */
} adi_fpgagen6_DeframerDataLinkCfg_t;

typedef struct adi_fpgagen6_FramerDataLinkCfg
{
    adi_fpgagen6_CommonLinkCfg_t    dataLink;   /*!< Common Link Config Data */
} adi_fpgagen6_FramerDataLinkCfg_t;

typedef struct adi_fpgagen6_FpgaCfg
{
    adi_fpgagen6_DeframerDataLinkCfg_t  deframerlink[ADI_FPGAGEN6_MAX_DEFRAMERS];   /*!< Deframer Link Configs */
    adi_fpgagen6_FramerDataLinkCfg_t    framerlink[ADI_FPGAGEN6_MAX_FRAMERS];       /*!< Framer Link Configs */
    uint32_t                            deviceClk_kHz;                              /*!< Device Clock (kHz) */
    uint8_t                             jesd204CEn;                                 /*!< Flag to specify JESD mode. 1 = 204C, 0 = 204B */
} adi_fpgagen6_FpgaCfg_t;

#endif /* _ADI_FPGAGEN6_UTILITIES_TYPES_H_ */
