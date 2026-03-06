/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * \file adi_adrv903x_lo_types.h
 * 
 * \brief   Contains ADRV903X LO data structures.
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef __ADRV903X_LO_TYPES_H__
#define __ADRV903X_LO_TYPES_H__

#include "adi_adrv903x_platform_pack.h"

/**
 *  \brief Enum of LO selections
 */
typedef enum adi_adrv903x_LoName
{
    ADI_ADRV903X_LO0 = 0,         /*!< Selects LO0 for Rx and Tx */
    ADI_ADRV903X_LO1,             /*!< Selects LO1 for Rx and Tx */
} adi_adrv903x_LoName_e;

/**
*  \brief Enum of LO Options selections. Feature is not implemented. Use 0 as default. Users are responsible for changing Band NCOs if changing RF LO.
*/
typedef enum adi_adrv903x_LoOption
{
    ADI_ADRV903X_NCO_NO_OPTION_SELECTED  = 0x00,     /*!< NCO auto-update disabled - Don't re-program NCOs when programming LO */
    ADI_ADRV903X_NCO_AUTO_UPDATE_DISABLE = 0x01      /*!< Selects the NCO auto update disable */
} adi_adrv903x_LoOption_e;

/**
 *  \brief Enum of PLL selections
 */
typedef enum adi_adrv903x_Pll
{
    ADI_ADRV903X_RF0_PLL = 0,         /*!< Selects RF0 PLL */
    ADI_ADRV903X_RF1_PLL,             /*!< Selects RF1 PLL */
    ADI_ADRV903X_CLK_PLL,             /*!< Selects Clk PLL */
    ADI_ADRV903X_SERDES_PLL           /*!< Selects Serdes PLL */
} adi_adrv903x_Pll_e;

#endif /* __ADRV903X_LO_TYPES_H__ */

