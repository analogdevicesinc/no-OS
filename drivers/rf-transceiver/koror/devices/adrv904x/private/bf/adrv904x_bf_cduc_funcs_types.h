/** 
 * \file adrv904x_bf_cduc_funcs_types.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _ADRV904X_BF_CDUC_FUNCS_TYPES_H_
#define _ADRV904X_BF_CDUC_FUNCS_TYPES_H_

typedef enum adrv904x_BfCducFuncsChanAddr
{
    ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_FUNCS    =    0x608B0E00,
    ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_FUNCS    =    0x609B0E00,
    ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_FUNCS    =    0x60AB0E00,
    ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_FUNCS    =    0x60BB0E00,
    ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_FUNCS    =    0x60CB0E00,
    ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_FUNCS    =    0x60DB0E00,
    ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_FUNCS    =    0x60EB0E00,
    ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_FUNCS    =    0x60FB0E00
} adrv904x_BfCducFuncsChanAddr_e;

/** 
 * \brief Enumeration for cducClkDiv
 */

typedef enum adrv904x_Bf_CducFuncs_CducClkDiv
{
    ADRV904X_BF_CDUC_FUNCS_CDUC_CLK_DIV_DIV_BY_1                        =    0,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_FUNCS_CDUC_CLK_DIV_DIV_BY_2                        =    1,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_FUNCS_CDUC_CLK_DIV_DIV_BY_4                        =    2,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_FUNCS_CDUC_CLK_DIV_DIV_BY_8                        =    3   /*!< No description provided                                                                                             */
} adrv904x_Bf_CducFuncs_CducClkDiv_e;

/** 
 * \brief Enumeration for cducJesdClkDiv
 */

typedef enum adrv904x_Bf_CducFuncs_CducJesdClkDiv
{
    ADRV904X_BF_CDUC_FUNCS_CDUC_JESD_CLK_DIV_DIV_BY_1                        =    0,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_FUNCS_CDUC_JESD_CLK_DIV_DIV_BY_2                        =    1,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_FUNCS_CDUC_JESD_CLK_DIV_DIV_BY_4                        =    2,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_FUNCS_CDUC_JESD_CLK_DIV_DIV_BY_8                        =    3,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_FUNCS_CDUC_JESD_CLK_DIV_DIV_BY_16                       =    4,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_FUNCS_CDUC_JESD_CLK_DIV_DIV_BY_32                       =    5,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_FUNCS_CDUC_JESD_CLK_DIV_DIV_BY_64                       =    6,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_FUNCS_CDUC_JESD_CLK_DIV_DIV_BY_128                      =    7   /*!< No description provided                                                                                             */
} adrv904x_Bf_CducFuncs_CducJesdClkDiv_e;

/** 
 * \brief Enumeration for cducPwrmeasClkDiv
 */

typedef enum adrv904x_Bf_CducFuncs_CducPwrmeasClkDiv
{
    ADRV904X_BF_CDUC_FUNCS_CDUC_PWRMEAS_CLK_DIV_DIV_BY_1                        =    0,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_FUNCS_CDUC_PWRMEAS_CLK_DIV_DIV_BY_2                        =    1,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_FUNCS_CDUC_PWRMEAS_CLK_DIV_DIV_BY_4                        =    2,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_FUNCS_CDUC_PWRMEAS_CLK_DIV_DIV_BY_8                        =    3,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_FUNCS_CDUC_PWRMEAS_CLK_DIV_DIV_BY_16                       =    4,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_FUNCS_CDUC_PWRMEAS_CLK_DIV_DIV_BY_32                       =    5,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_FUNCS_CDUC_PWRMEAS_CLK_DIV_DIV_BY_64                       =    6,  /*!< No description provided                                                                                             */
    ADRV904X_BF_CDUC_FUNCS_CDUC_PWRMEAS_CLK_DIV_DIV_BY_128                      =    7   /*!< No description provided                                                                                             */
} adrv904x_Bf_CducFuncs_CducPwrmeasClkDiv_e;

#endif // _ADRV904X_BF_CDUC_FUNCS_TYPES_H_

