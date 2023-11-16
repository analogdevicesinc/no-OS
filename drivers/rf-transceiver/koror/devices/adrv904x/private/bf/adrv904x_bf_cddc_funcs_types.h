/** 
 * \file adrv904x_bf_cddc_funcs_types.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _ADRV904X_BF_CDDC_FUNCS_TYPES_H_
#define _ADRV904X_BF_CDDC_FUNCS_TYPES_H_

typedef enum adrv904x_BfCddcFuncsChanAddr
{
    ADRV904X_BF_SLICE_RX_0__RX_CDDC_RX_CDDC_FUNCS    =    0x600E0E00,
    ADRV904X_BF_SLICE_RX_1__RX_CDDC_RX_CDDC_FUNCS    =    0x601E0E00,
    ADRV904X_BF_SLICE_RX_2__RX_CDDC_RX_CDDC_FUNCS    =    0x602E0E00,
    ADRV904X_BF_SLICE_RX_3__RX_CDDC_RX_CDDC_FUNCS    =    0x603E0E00,
    ADRV904X_BF_SLICE_RX_4__RX_CDDC_RX_CDDC_FUNCS    =    0x604E0E00,
    ADRV904X_BF_SLICE_RX_5__RX_CDDC_RX_CDDC_FUNCS    =    0x605E0E00,
    ADRV904X_BF_SLICE_RX_6__RX_CDDC_RX_CDDC_FUNCS    =    0x606E0E00,
    ADRV904X_BF_SLICE_RX_7__RX_CDDC_RX_CDDC_FUNCS    =    0x607E0E00
} adrv904x_BfCddcFuncsChanAddr_e;

/** 
 * \brief Enumeration for cddcClkDiv
 */

typedef enum adrv904x_Bf_CddcFuncs_CddcClkDiv
{
    ADRV904X_BF_CDDC_FUNCS_CDDC_CLK_DIV_DIV_BY_1                        =    0,  /*!< No description provided */
    ADRV904X_BF_CDDC_FUNCS_CDDC_CLK_DIV_DIV_BY_2                        =    1,  /*!< No description provided */
    ADRV904X_BF_CDDC_FUNCS_CDDC_CLK_DIV_DIV_BY_4                        =    2,  /*!< No description provided */
    ADRV904X_BF_CDDC_FUNCS_CDDC_CLK_DIV_DIV_BY_8                        =    3   /*!< No description provided */
} adrv904x_Bf_CddcFuncs_CddcClkDiv_e;

/** 
 * \brief Enumeration for cddcJesdClkDiv
 */

typedef enum adrv904x_Bf_CddcFuncs_CddcJesdClkDiv
{
    ADRV904X_BF_CDDC_FUNCS_CDDC_JESD_CLK_DIV_DIV_BY_1                        =    0,  /*!< No description provided */
    ADRV904X_BF_CDDC_FUNCS_CDDC_JESD_CLK_DIV_DIV_BY_2                        =    1,  /*!< No description provided */
    ADRV904X_BF_CDDC_FUNCS_CDDC_JESD_CLK_DIV_DIV_BY_4                        =    2,  /*!< No description provided */
    ADRV904X_BF_CDDC_FUNCS_CDDC_JESD_CLK_DIV_DIV_BY_8                        =    3,  /*!< No description provided */
    ADRV904X_BF_CDDC_FUNCS_CDDC_JESD_CLK_DIV_DIV_BY_16                       =    4,  /*!< No description provided */
    ADRV904X_BF_CDDC_FUNCS_CDDC_JESD_CLK_DIV_DIV_BY_32                       =    5,  /*!< No description provided */
    ADRV904X_BF_CDDC_FUNCS_CDDC_JESD_CLK_DIV_DIV_BY_64                       =    6,  /*!< No description provided */
    ADRV904X_BF_CDDC_FUNCS_CDDC_JESD_CLK_DIV_DIV_BY_128                      =    7   /*!< No description provided */
} adrv904x_Bf_CddcFuncs_CddcJesdClkDiv_e;

/** 
 * \brief Enumeration for cddcPwrmeasClkDiv
 */

typedef enum adrv904x_Bf_CddcFuncs_CddcPwrmeasClkDiv
{
    ADRV904X_BF_CDDC_FUNCS_CDDC_PWRMEAS_CLK_DIV_DIV_BY_1                        =    0,  /*!< No description provided */
    ADRV904X_BF_CDDC_FUNCS_CDDC_PWRMEAS_CLK_DIV_DIV_BY_2                        =    1,  /*!< No description provided */
    ADRV904X_BF_CDDC_FUNCS_CDDC_PWRMEAS_CLK_DIV_DIV_BY_4                        =    2,  /*!< No description provided */
    ADRV904X_BF_CDDC_FUNCS_CDDC_PWRMEAS_CLK_DIV_DIV_BY_8                        =    3,  /*!< No description provided */
    ADRV904X_BF_CDDC_FUNCS_CDDC_PWRMEAS_CLK_DIV_DIV_BY_16                       =    4,  /*!< No description provided */
    ADRV904X_BF_CDDC_FUNCS_CDDC_PWRMEAS_CLK_DIV_DIV_BY_32                       =    5,  /*!< No description provided */
    ADRV904X_BF_CDDC_FUNCS_CDDC_PWRMEAS_CLK_DIV_DIV_BY_64                       =    6,  /*!< No description provided */
    ADRV904X_BF_CDDC_FUNCS_CDDC_PWRMEAS_CLK_DIV_DIV_BY_128                      =    7   /*!< No description provided */
} adrv904x_Bf_CddcFuncs_CddcPwrmeasClkDiv_e;

#endif // _ADRV904X_BF_CDDC_FUNCS_TYPES_H_

