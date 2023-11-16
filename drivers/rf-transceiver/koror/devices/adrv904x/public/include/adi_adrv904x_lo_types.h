/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file adi_adrv904x_lo_types.h
 * 
 * \brief   Contains ADRV904X LO data structures.
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef __ADRV904X_LO_TYPES_H__
#define __ADRV904X_LO_TYPES_H__

#include "adi_adrv904x_platform_pack.h"

/**
 *  \brief Enum of LO selections
 */
typedef enum adi_adrv904x_LoName
{
    ADI_ADRV904X_LO0 = 0,         /*!< Selects LO0 for Rx and Tx */
    ADI_ADRV904X_LO1,             /*!< Selects LO1 for Rx and Tx */
} adi_adrv904x_LoName_e;

/**
*  \brief Enum of LO Options selections. Feature is not implemented. Use 0 as default. Users are responsible for changing Band NCOs if changing RF LO.
*/
typedef enum adi_adrv904x_LoOption
{
    ADI_ADRV904X_NCO_NO_OPTION_SELECTED  = 0x00,     /*!< NCO auto-update disabled - Don't re-program NCOs when programming LO */
    ADI_ADRV904X_NCO_AUTO_UPDATE_DISABLE = 0x01      /*!< Selects the NCO auto update disable */
} adi_adrv904x_LoOption_e;

/**
 *  \brief Enum of PLL selections
 */
typedef enum adi_adrv904x_Pll
{
    ADI_ADRV904X_RF0_PLL = 0,         /*!< Selects RF0 PLL */
    ADI_ADRV904X_RF1_PLL,             /*!< Selects RF1 PLL */
    ADI_ADRV904X_CLK_PLL,             /*!< Selects Clk PLL */
    ADI_ADRV904X_SERDES_PLL           /*!< Selects Serdes PLL */
} adi_adrv904x_Pll_e;

#endif /* __ADRV904X_LO_TYPES_H__ */

