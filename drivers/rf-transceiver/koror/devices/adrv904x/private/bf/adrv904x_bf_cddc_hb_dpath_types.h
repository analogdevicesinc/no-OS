/** 
 * \file adrv904x_bf_cddc_hb_dpath_types.h Automatically generated file with generator ver 0.0.16.0.
 * 
 * \brief Contains BitField functions to support ADRV904X transceiver device.
 * 
 * ADRV904X BITFIELD VERSION: 0.0.0.11
 * 
 * Disclaimer Legal Disclaimer
 * 
 * Copyright 2015 - 2021 Analog Devices Inc.
 * 
 * Released under the ADRV904X API license, for more information see the "LICENSE.PDF" file in this zip file.
 */

#ifndef _ADRV904X_BF_CDDC_HB_DPATH_TYPES_H_
#define _ADRV904X_BF_CDDC_HB_DPATH_TYPES_H_

typedef enum adrv904x_BfCddcHbDpathChanAddr
{
    ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_HB_DPATH    =    0x600E0C00,
    ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_HB_DPATH    =    0x601E0C00,
    ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_HB_DPATH    =    0x602E0C00,
    ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_HB_DPATH    =    0x603E0C00,
    ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_HB_DPATH    =    0x604E0C00,
    ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_HB_DPATH    =    0x605E0C00,
    ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_HB_DPATH    =    0x606E0C00,
    ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_HB_DPATH    =    0x607E0C00
} adrv904x_BfCddcHbDpathChanAddr_e;

/** 
 * \brief Enumeration for bandSource
 */

typedef enum adrv904x_Bf_CddcHbDpath_BandSource
{
    ADRV904X_BF_CDDC_HB_DPATH_BAND_SOURCE_ENUM000                         =    0,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_BAND_SOURCE_ENUM001                         =    1   /*!< No description provided */
} adrv904x_Bf_CddcHbDpath_BandSource_e;

/** 
 * \brief Enumeration for cddcClkToBand0Rate
 */

typedef enum adrv904x_Bf_CddcHbDpath_CddcClkToBand0Rate
{
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM000                         =    0,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM001                         =    1,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM002                         =    2,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM003                         =    3,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM004                         =    4,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM005                         =    5,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM006                         =    6,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM007                         =    7,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM008                         =    8,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM009                         =    9,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM010                         =   10,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM011                         =   11,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM012                         =   12,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM013                         =   13,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM014                         =   14,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM015                         =   15,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM016                         =   16,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM017                         =   17,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM018                         =   18,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM019                         =   19,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM020                         =   20,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM021                         =   21,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM022                         =   22,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM023                         =   23,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM024                         =   24,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM025                         =   25,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM026                         =   26,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM027                         =   27,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM028                         =   28,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM029                         =   29,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM030                         =   30,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM031                         =   31,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM032                         =   32,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM033                         =   33,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM034                         =   34,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM035                         =   35,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM036                         =   36,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM037                         =   37,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM038                         =   38,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM039                         =   39,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM040                         =   40,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM041                         =   41,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM042                         =   42,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM043                         =   43,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM044                         =   44,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM045                         =   45,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM046                         =   46,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM047                         =   47,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM048                         =   48,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM049                         =   49,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM050                         =   50,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM051                         =   51,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM052                         =   52,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM053                         =   53,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM054                         =   54,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM055                         =   55,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM056                         =   56,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM057                         =   57,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM058                         =   58,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM059                         =   59,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM060                         =   60,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM061                         =   61,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM062                         =   62,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND0_RATE_ENUM063                         =   63   /*!< No description provided */
} adrv904x_Bf_CddcHbDpath_CddcClkToBand0Rate_e;

/** 
 * \brief Enumeration for cddcClkToBand1Rate
 */

typedef enum adrv904x_Bf_CddcHbDpath_CddcClkToBand1Rate
{
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM000                         =    0,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM001                         =    1,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM002                         =    2,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM003                         =    3,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM004                         =    4,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM005                         =    5,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM006                         =    6,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM007                         =    7,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM008                         =    8,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM009                         =    9,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM010                         =   10,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM011                         =   11,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM012                         =   12,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM013                         =   13,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM014                         =   14,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM015                         =   15,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM016                         =   16,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM017                         =   17,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM018                         =   18,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM019                         =   19,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM020                         =   20,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM021                         =   21,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM022                         =   22,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM023                         =   23,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM024                         =   24,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM025                         =   25,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM026                         =   26,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM027                         =   27,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM028                         =   28,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM029                         =   29,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM030                         =   30,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM031                         =   31,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM032                         =   32,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM033                         =   33,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM034                         =   34,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM035                         =   35,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM036                         =   36,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM037                         =   37,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM038                         =   38,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM039                         =   39,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM040                         =   40,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM041                         =   41,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM042                         =   42,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM043                         =   43,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM044                         =   44,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM045                         =   45,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM046                         =   46,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM047                         =   47,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM048                         =   48,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM049                         =   49,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM050                         =   50,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM051                         =   51,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM052                         =   52,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM053                         =   53,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM054                         =   54,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM055                         =   55,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM056                         =   56,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM057                         =   57,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM058                         =   58,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM059                         =   59,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM060                         =   60,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM061                         =   61,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM062                         =   62,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_CDDC_CLK_TO_BAND1_RATE_ENUM063                         =   63   /*!< No description provided */
} adrv904x_Bf_CddcHbDpath_CddcClkToBand1Rate_e;

/** 
 * \brief Enumeration for decRate
 */

typedef enum adrv904x_Bf_CddcHbDpath_DecRate
{
    ADRV904X_BF_CDDC_HB_DPATH_DEC_RATE_ENUM000                         =    0,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_DEC_RATE_ENUM001                         =    1,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_DEC_RATE_ENUM002                         =    2,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_DEC_RATE_ENUM003                         =    3,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_DEC_RATE_ENUM004                         =    4,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_DEC_RATE_ENUM005                         =    5   /*!< No description provided */
} adrv904x_Bf_CddcHbDpath_DecRate_e;

/** 
 * \brief Enumeration for upsamplerRatio
 */

typedef enum adrv904x_Bf_CddcHbDpath_UpsamplerRatio
{
    ADRV904X_BF_CDDC_HB_DPATH_UPSAMPLER_RATIO_ENUM000                         =    0,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_UPSAMPLER_RATIO_ENUM001                         =    1,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_UPSAMPLER_RATIO_ENUM002                         =    2,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_UPSAMPLER_RATIO_ENUM003                         =    3,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_UPSAMPLER_RATIO_ENUM004                         =    4,  /*!< No description provided */
    ADRV904X_BF_CDDC_HB_DPATH_UPSAMPLER_RATIO_ENUM005                         =    5   /*!< No description provided */
} adrv904x_Bf_CddcHbDpath_UpsamplerRatio_e;

#endif // _ADRV904X_BF_CDDC_HB_DPATH_TYPES_H_

