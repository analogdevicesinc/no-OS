/** 
 * \file adrv904x_bf_cduc_hb_dpath_types.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _ADRV904X_BF_CDUC_HB_DPATH_TYPES_H_
#define _ADRV904X_BF_CDUC_HB_DPATH_TYPES_H_

typedef enum adrv904x_BfCducHbDpathChanAddr
{
    ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH    =    0x608B0C00,
    ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_HB_DPATH    =    0x609B0C00,
    ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_HB_DPATH    =    0x60AB0C00,
    ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_HB_DPATH    =    0x60BB0C00,
    ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_HB_DPATH    =    0x60CB0C00,
    ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_HB_DPATH    =    0x60DB0C00,
    ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_HB_DPATH    =    0x60EB0C00,
    ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_HB_DPATH    =    0x60FB0C00
} adrv904x_BfCducHbDpathChanAddr_e;

/** 
 * \brief Enumeration for bandDest
 */

typedef enum adrv904x_Bf_CducHbDpath_BandDest
{
    ADRV904X_BF_CDUC_HB_DPATH_BAND_DEST_ENUM000                         =    0,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_BAND_DEST_ENUM001                         =    1   /*!< No description provided                                                                                             */
} adrv904x_Bf_CducHbDpath_BandDest_e;

/** 
 * \brief Enumeration for cducClkToBand0Rate
 */

typedef enum adrv904x_Bf_CducHbDpath_CducClkToBand0Rate
{
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM000                         =    0,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM001                         =    1,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM002                         =    2,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM003                         =    3,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM004                         =    4,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM005                         =    5,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM006                         =    6,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM007                         =    7,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM008                         =    8,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM009                         =    9,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM010                         =   10,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM011                         =   11,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM012                         =   12,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM013                         =   13,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM014                         =   14,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM015                         =   15,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM016                         =   16,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM017                         =   17,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM018                         =   18,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM019                         =   19,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM020                         =   20,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM021                         =   21,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM022                         =   22,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM023                         =   23,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM024                         =   24,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM025                         =   25,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM026                         =   26,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM027                         =   27,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM028                         =   28,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM029                         =   29,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM030                         =   30,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM031                         =   31,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM032                         =   32,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM033                         =   33,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM034                         =   34,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM035                         =   35,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM036                         =   36,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM037                         =   37,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM038                         =   38,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM039                         =   39,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM040                         =   40,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM041                         =   41,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM042                         =   42,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM043                         =   43,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM044                         =   44,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM045                         =   45,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM046                         =   46,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM047                         =   47,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM048                         =   48,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM049                         =   49,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM050                         =   50,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM051                         =   51,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM052                         =   52,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM053                         =   53,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM054                         =   54,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM055                         =   55,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM056                         =   56,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM057                         =   57,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM058                         =   58,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM059                         =   59,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM060                         =   60,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM061                         =   61,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM062                         =   62,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND0_RATE_ENUM063                         =   63   /*!< No description provided                                                                                             */
} adrv904x_Bf_CducHbDpath_CducClkToBand0Rate_e;

/** 
 * \brief Enumeration for cducClkToBand1Rate
 */

typedef enum adrv904x_Bf_CducHbDpath_CducClkToBand1Rate
{
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM000                         =    0,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM001                         =    1,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM002                         =    2,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM003                         =    3,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM004                         =    4,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM005                         =    5,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM006                         =    6,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM007                         =    7,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM008                         =    8,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM009                         =    9,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM010                         =   10,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM011                         =   11,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM012                         =   12,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM013                         =   13,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM014                         =   14,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM015                         =   15,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM016                         =   16,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM017                         =   17,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM018                         =   18,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM019                         =   19,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM020                         =   20,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM021                         =   21,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM022                         =   22,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM023                         =   23,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM024                         =   24,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM025                         =   25,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM026                         =   26,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM027                         =   27,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM028                         =   28,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM029                         =   29,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM030                         =   30,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM031                         =   31,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM032                         =   32,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM033                         =   33,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM034                         =   34,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM035                         =   35,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM036                         =   36,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM037                         =   37,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM038                         =   38,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM039                         =   39,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM040                         =   40,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM041                         =   41,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM042                         =   42,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM043                         =   43,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM044                         =   44,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM045                         =   45,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM046                         =   46,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM047                         =   47,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM048                         =   48,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM049                         =   49,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM050                         =   50,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM051                         =   51,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM052                         =   52,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM053                         =   53,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM054                         =   54,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM055                         =   55,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM056                         =   56,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM057                         =   57,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM058                         =   58,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM059                         =   59,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM060                         =   60,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM061                         =   61,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM062                         =   62,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_BAND1_RATE_ENUM063                         =   63   /*!< No description provided                                                                                             */
} adrv904x_Bf_CducHbDpath_CducClkToBand1Rate_e;

/** 
 * \brief Enumeration for cducClkToCarrierRate
 */

typedef enum adrv904x_Bf_CducHbDpath_CducClkToCarrierRate
{
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM000                         =    0,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM001                         =    1,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM002                         =    2,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM003                         =    3,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM004                         =    4,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM005                         =    5,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM006                         =    6,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM007                         =    7,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM008                         =    8,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM009                         =    9,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM010                         =   10,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM011                         =   11,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM012                         =   12,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM013                         =   13,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM014                         =   14,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM015                         =   15,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM016                         =   16,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM017                         =   17,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM018                         =   18,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM019                         =   19,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM020                         =   20,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM021                         =   21,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM022                         =   22,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM023                         =   23,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM024                         =   24,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM025                         =   25,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM026                         =   26,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM027                         =   27,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM028                         =   28,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM029                         =   29,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM030                         =   30,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM031                         =   31,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM032                         =   32,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM033                         =   33,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM034                         =   34,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM035                         =   35,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM036                         =   36,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM037                         =   37,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM038                         =   38,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM039                         =   39,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM040                         =   40,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM041                         =   41,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM042                         =   42,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM043                         =   43,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM044                         =   44,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM045                         =   45,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM046                         =   46,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM047                         =   47,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM048                         =   48,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM049                         =   49,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM050                         =   50,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM051                         =   51,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM052                         =   52,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM053                         =   53,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM054                         =   54,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM055                         =   55,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM056                         =   56,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM057                         =   57,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM058                         =   58,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM059                         =   59,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM060                         =   60,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM061                         =   61,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM062                         =   62,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM063                         =   63,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM064                         =   64,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM065                         =   65,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM066                         =   66,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM067                         =   67,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM068                         =   68,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM069                         =   69,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM070                         =   70,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM071                         =   71,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM072                         =   72,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM073                         =   73,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM074                         =   74,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM075                         =   75,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM076                         =   76,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM077                         =   77,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM078                         =   78,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM079                         =   79,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM080                         =   80,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM081                         =   81,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM082                         =   82,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM083                         =   83,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM084                         =   84,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM085                         =   85,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM086                         =   86,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM087                         =   87,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM088                         =   88,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM089                         =   89,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM090                         =   90,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM091                         =   91,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM092                         =   92,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM093                         =   93,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM094                         =   94,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM095                         =   95,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM096                         =   96,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM097                         =   97,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM098                         =   98,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM099                         =   99,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM100                         =  100,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM101                         =  101,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM102                         =  102,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM103                         =  103,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM104                         =  104,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM105                         =  105,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM106                         =  106,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM107                         =  107,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM108                         =  108,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM109                         =  109,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM110                         =  110,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM111                         =  111,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM112                         =  112,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM113                         =  113,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM114                         =  114,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM115                         =  115,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM116                         =  116,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM117                         =  117,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM118                         =  118,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM119                         =  119,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM120                         =  120,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM121                         =  121,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM122                         =  122,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM123                         =  123,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM124                         =  124,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM125                         =  125,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM126                         =  126,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM127                         =  127,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM128                         =  128,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM129                         =  129,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM130                         =  130,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM131                         =  131,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM132                         =  132,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM133                         =  133,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM134                         =  134,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM135                         =  135,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM136                         =  136,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM137                         =  137,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM138                         =  138,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM139                         =  139,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM140                         =  140,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM141                         =  141,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM142                         =  142,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM143                         =  143,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM144                         =  144,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM145                         =  145,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM146                         =  146,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM147                         =  147,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM148                         =  148,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM149                         =  149,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM150                         =  150,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM151                         =  151,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM152                         =  152,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM153                         =  153,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM154                         =  154,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM155                         =  155,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM156                         =  156,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM157                         =  157,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM158                         =  158,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM159                         =  159,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM160                         =  160,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM161                         =  161,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM162                         =  162,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM163                         =  163,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM164                         =  164,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM165                         =  165,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM166                         =  166,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM167                         =  167,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM168                         =  168,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM169                         =  169,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM170                         =  170,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM171                         =  171,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM172                         =  172,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM173                         =  173,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM174                         =  174,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM175                         =  175,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM176                         =  176,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM177                         =  177,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM178                         =  178,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM179                         =  179,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM180                         =  180,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM181                         =  181,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM182                         =  182,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM183                         =  183,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM184                         =  184,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM185                         =  185,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM186                         =  186,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM187                         =  187,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM188                         =  188,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM189                         =  189,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM190                         =  190,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM191                         =  191,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM192                         =  192,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM193                         =  193,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM194                         =  194,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM195                         =  195,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM196                         =  196,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM197                         =  197,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM198                         =  198,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM199                         =  199,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM200                         =  200,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM201                         =  201,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM202                         =  202,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM203                         =  203,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM204                         =  204,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM205                         =  205,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM206                         =  206,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM207                         =  207,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM208                         =  208,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM209                         =  209,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM210                         =  210,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM211                         =  211,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM212                         =  212,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM213                         =  213,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM214                         =  214,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM215                         =  215,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM216                         =  216,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM217                         =  217,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM218                         =  218,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM219                         =  219,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM220                         =  220,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM221                         =  221,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM222                         =  222,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM223                         =  223,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM224                         =  224,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM225                         =  225,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM226                         =  226,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM227                         =  227,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM228                         =  228,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM229                         =  229,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM230                         =  230,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM231                         =  231,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM232                         =  232,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM233                         =  233,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM234                         =  234,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM235                         =  235,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM236                         =  236,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM237                         =  237,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM238                         =  238,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM239                         =  239,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM240                         =  240,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM241                         =  241,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM242                         =  242,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM243                         =  243,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM244                         =  244,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM245                         =  245,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM246                         =  246,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM247                         =  247,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM248                         =  248,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM249                         =  249,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM250                         =  250,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM251                         =  251,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM252                         =  252,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM253                         =  253,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM254                         =  254,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_CDUC_CLK_TO_CARRIER_RATE_ENUM255                         =  255   /*!< No description provided                                                                                             */
} adrv904x_Bf_CducHbDpath_CducClkToCarrierRate_e;

/** 
 * \brief Enumeration for downsamplerRatio
 */

typedef enum adrv904x_Bf_CducHbDpath_DownsamplerRatio
{
    ADRV904X_BF_CDUC_HB_DPATH_DOWNSAMPLER_RATIO_ENUM000                         =    0,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_DOWNSAMPLER_RATIO_ENUM001                         =    1,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_DOWNSAMPLER_RATIO_ENUM002                         =    2,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_DOWNSAMPLER_RATIO_ENUM003                         =    3,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_DOWNSAMPLER_RATIO_ENUM004                         =    4,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_DOWNSAMPLER_RATIO_ENUM005                         =    5   /*!< No description provided                                                                                             */
} adrv904x_Bf_CducHbDpath_DownsamplerRatio_e;

/** 
 * \brief Enumeration for intRate
 */

typedef enum adrv904x_Bf_CducHbDpath_IntRate
{
    ADRV904X_BF_CDUC_HB_DPATH_INT_RATE_ENUM000                         =    0,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_INT_RATE_ENUM001                         =    1,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_INT_RATE_ENUM002                         =    2,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_INT_RATE_ENUM003                         =    3,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_INT_RATE_ENUM004                         =    4,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_HB_DPATH_INT_RATE_ENUM005                         =    5   /*!< No description provided                                                                                             */
} adrv904x_Bf_CducHbDpath_IntRate_e;

#endif // _ADRV904X_BF_CDUC_HB_DPATH_TYPES_H_

